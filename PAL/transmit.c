#include "transmit.h"
#include "sim900.h"
#include "pal.h"
#include "cJSON.h"
#include "malloc.h"
#include "app.h"
#include "gps.h"

#define HEARTBEAT_INTERVAL 10

extern uint8_t *deviceid[17];
__IO uint8_t heartbeat = 0;
__IO CtrlMsg ctrlMsg;

CtrlItem ctrlTable[CONTROL_END] = {
    {CONTROL_WINDOW, "bcm_fun_window"},
    {CONTROL_DOOR, "bcm_fun_door"},
    {CONTROL_LIGHT, "bcm_fun_lamp"},
    {CONTROL_SUNROOF, "bcm_fun_sunroof"},
    {CONTROL_TRUNK, "bcm_fun_trunk"},
    {CONTROL_FINDCAR, "bcm_fun_findcar"},
    {CONTROL_IMMOLOCK, "bcm_fun_immolock"},
};

OS_STK heartbeatTaskStk[TASK_STK_SIZE_COMMON];
static void heartbeat_thread(void *parg);
static uint8_t connected = FALSE;

void transmit_init(void)
{
    //init for gprs
    sim900_init();
    //connect the server
    sim900_connect();
//    gps_init();
//    gps_setup();
//    gps_test();
    //register callback
    sim900_register_recv(recv_callback);
    //run heart beat thread
    OSTaskCreate(heartbeat_thread, (void *)0,
            &heartbeatTaskStk[TASK_STK_SIZE_COMMON - 1],
            HEARTBEAT_THREAD_PRIO);
    heartbeat = 0;
    connected = FALSE;
}

uint8_t isConnected(void)
{
    return connected;
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
        //send retry cmd
        send_retry();
    } else {
        item = cJSON_GetObjectItem(json, KEY_MSG_TYPE);
        msg_type = item->valueint;
        //printf("msg type = %d\r\n", msg_type);
        switch(msg_type) {
            case MSG_TYPE_HEARTBEAT:
                printf("---device offline---\r\n");
                go_reboot();
                break;
            case MSG_TYPE_HEARTBEAT_RSP:
                item = cJSON_GetObjectItem(json, KEY_HEARTBEAT);
                heartbeat_rsp = item->valueint;
                printf("heartbeat = %d, heartbeat_rsp = %d\r\n",
                        heartbeat, heartbeat_rsp);
                if(((heartbeat - 1) * 2 + 1) == heartbeat_rsp) {
                    //printf("heartbeat!\r\n");
                    connected = TRUE;
                    //clear ticks
                    setTicks(0);
                } else {
                    printf("failed to parse heartbeat\r\n");
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
                break;
            case MSG_TYPE_VEHICLE_TYPE:
                item = cJSON_GetObjectItem(json, KEY_VEHICLE_TYPE);
                if(!strcmp(item->valuestring, "toyota")) {
                    printf("->toyota\r\n");
                    toyota_setup();
                } else if(!strcmp(item->valuestring, "gm")) {
                    printf("->gm\r\n");
                    gm_setup();
                } else {
                    fake_setup();
                }
                break;
        }
    }
    cJSON_Delete(json);

}

static void heartbeat_thread(void *parg)
{
    parg = parg;
    //send login message firstly
    printf("login!\r\n");
    login();
    OSTimeDlyHMSM(0, 0, LOGIN_DELAYED_TIME, 0);
    for(;;) {
        OSTimeDlyHMSM(0, 0, HEARTBEAT_INTERVAL, 0);
        //send heartbeat
        heartbeat = (heartbeat == 100) ? 0 : heartbeat;
        send_heartbeat(heartbeat ++);
    }
}

void login(void)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_LOGIN);

    out = cJSON_Print(root);
    length = strlen(out);
    sim900_write((uint8_t *)out, length);
    cJSON_Delete(root);
    myfree(out);
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

void send_retry(void)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_RETRY);

    out = cJSON_Print(root);
    length = strlen(out);
    //printf("%s\r\n", out);
    sim900_write((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

void control_rsp(uint32_t cmd_id, uint8_t cmd_type)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    printf("-> %s\r\n", __func__);
    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_CTRL_RSP);
    cJSON_AddNumberToObject(root, KEY_CMD_ID, cmd_id);
    cJSON_AddNumberToObject(root, KEY_STATUS, 0);
    cJSON_AddStringToObject(root, KEY_CMD_TYPE, ctrlTable[cmd_type].key);

    out = cJSON_Print(root);
    length = strlen(out);
    printf("%s\r\n", out);
    sim900_write((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

void upload_item(UpdateItem *item)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;
    char val_buf[16];
    uint8_t i;
    uint8_t index = 0, n = 0;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_UPLOAD);
    //make value
    memset(val_buf, 0x00, 16);
    for(i = 0; i < item->len; i++) {
        n = sprintf(val_buf + index, "%02x", item->data[i]);
        index += n;
    }
    cJSON_AddStringToObject(root, getPidKey(item->pid), val_buf);

    //printf("%s: %s\r\n", __func__, getPidKey(item->pid));
    out = cJSON_Print(root);
    length = strlen(out);
    printf("%s\r\n", out);
    sim900_write((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

void upload_fault_code(FaultCodeValue *value)
{

}
