#include "transmit.h"
#include "sim900.h"
#include "pal.h"
#include "cJSON.h"
#include "malloc.h"

#define HEARTBEAT_INTERVAL 10

extern uint8_t *deviceid[17];
__IO uint8_t heartbeat = 0;
__IO CtrlMsg ctrlMsg;

CtrlItem ctrlTable[CONTROL_END] = {
    {CONTROL_WINDOW, "bcm_fun_window"},
    {CONTROL_DOOR, "bcm_fun_door"},
    {CONTROL_LIGHT, "bcm_fun_lamp"},
    {CONTROL_SUNFLOOR, "bcm_fun_sunfloor"},
    {CONTROL_TRUNK, "bcm_fun_trunk"},
    {CONTROL_FINDCAR, "bcm_fun_findcar"},
};

OS_STK heartbeatTaskStk[TASK_STK_SIZE_COMMON];
static void heartbeat_thread(void *parg);

void transmit_init(void)
{
    //init for gprs
    sim900_init();
    //connect the server
    sim900_connect();
    //register callback
    sim900_register_recv(recv_callback);
    //run heart beat thread
    OSTaskCreate(heartbeat_thread, (void *)0,
            &heartbeatTaskStk[TASK_STK_SIZE_COMMON - 1],
            HEARTBEAT_THREAD_PRIO);
    heartbeat = 0;
}

void recv_callback(uint8_t *buf)
{
    //1. parse buffer
    //2. send mailbox cmd to pal layer
    //    printf("-> %s: %s\r\n", __func__, buf);
    cJSON *json, *item;
    char *tmp;
    uint8_t i = 0;
    int msg_type;
    int cmd_id;
    int ctrl_val;
    uint8_t heartbeat_rsp;

    json = cJSON_Parse((const char *)buf);
    if(!json) {
        printf("\r\n[%s]\r\n", cJSON_GetErrorPtr());
    } else {
        item = cJSON_GetObjectItem(json, KEY_MSG_TYPE);
        msg_type = item->valueint;
        switch(msg_type) {
            case MSG_TYPE_HEARTBEAT_RSP:
                item = cJSON_GetObjectItem(json, KEY_HEARTBEAT);
                heartbeat_rsp = item->valueint;
                //printf("heartbeat = %d, heartbeat_rsp = %d\r\n",
                //        heartbeat, heartbeat_rsp);
                if(((heartbeat - 1) * 2 + 1) == heartbeat_rsp) {
                    //printf("heartbeat!\r\n");
                } else {
                    printf("fauled to parse heartbeat\r\n");
                }
                break;

            case MSG_TYPE_CTRL:
                item = cJSON_GetObjectItem(json, KEY_CMD_ID);
                cmd_id = item->valueint;
                for(i = 0; i < CONTROL_END; i++) {
                    tmp = strstr((char *)buf, ctrlTable[i].key) ;
                    if(tmp) {
                        item = cJSON_GetObjectItem(json, ctrlTable[i].key);
                        ctrl_val = item->valueint;
                        printf("cmd: %s, val = %d\r\n", ctrlTable[i].key,
                                ctrl_val);
                        ctrlMsg.id = i;
                        ctrlMsg.cmd_id = cmd_id;
                        ctrlMsg.value = ctrl_val;
                        OSMboxPost(getPalInstance()->mailbox, &ctrlMsg);
                    }

                }
        }
    }
    cJSON_Delete(json);

}

static void heartbeat_thread(void *parg)
{
    parg = parg;
    for(;;) {
        OSTimeDlyHMSM(0, 0, HEARTBEAT_INTERVAL, 0);
        //send heartbeat
        send_heartbeat(heartbeat ++);
        heartbeat = (heartbeat == 100) ? 0 : heartbeat;
    }
}

void send_heartbeat(uint8_t count)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_HEARTBEAT);
    cJSON_AddNumberToObject(root, KEY_HEARTBEAT, count);

    out = cJSON_Print(root);
    length = strlen(out);
    //printf("%s\r\n", out);
    sim900_write((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}
