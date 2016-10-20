#include "transmit.h"
#include "sim800.h"
#include "pal.h"
#include "cJSON.h"
#include "malloc.h"
#include "app.h"
#include "gps.h"
#include "config.h"

#define HEARTBEAT_INTERVAL 20

extern uint8_t *deviceid[17];
__IO uint8_t heartbeat = 0;
__IO CtrlMsg ctrlMsg;

uint8_t hb_fail_count = 0;

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
#if 0       //disable
#ifdef PROP_HAS_GPS
    gps_init();
    gps_setup();
#endif
#endif
#ifdef VEHICLE_TYPE_FAKE
    fake_setup();
#elif defined VEHICLE_TYPE_TOYOTA
    toyota_setup();
#elif defined VEHICLE_TYPE_GM
    gm_setup();
#endif
#ifdef PROP_HAS_GPRS
    //init for gprs
    sim800_setup();
    //register callback
    sim800_register_recv(recv_callback);
    //run heart beat thread
    OSTaskCreate(heartbeat_thread, (void *)0,
            &heartbeatTaskStk[TASK_STK_SIZE_COMMON - 1],
            HEARTBEAT_THREAD_PRIO);
    heartbeat = 0;
    connected = FALSE;
#endif
    printf("%s: done\r\n", __func__);
    hb_fail_count = 0;
}

uint8_t isConnected(void)
{
    return connected;
}

void recv_callback(uint8_t *buf)
{
    //1. parse buffer
    //2. send queue cmd to pal layer
    cJSON *json, *item;
    char *tmp;
    uint8_t i = 0;
    int msg_type;
    int cmd_id;
    int ctrl_val;
    uint8_t heartbeat_rsp;

    printf("-> %s: %s\r\n", __func__, buf);
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
#ifdef SERVER_IS_K
            case MSG_TYPE_HEARTBEAT:
                printf("---device offline---\r\n");
                go_reboot();
                break;
#endif
            case MSG_TYPE_HEARTBEAT_RSP:
                item = cJSON_GetObjectItem(json, KEY_HEARTBEAT);
                heartbeat_rsp = item->valueint;
                printf("heartbeat = %d, heartbeat_rsp = %d\r\n",
                        heartbeat, heartbeat_rsp);
                if(((heartbeat - 1) * 2 + 1) == heartbeat_rsp) {
                    //printf("heartbeat!\r\n");
                    connected = TRUE;
                    hb_fail_count = 0;
                    //clear ticks
                    setTicks(0);
                } else {
                    printf("failed to parse heartbeat\r\n");
                    if(hb_fail_count ++ > 5) {
                        go_reboot();
                    }
                }
                break;

            case MSG_TYPE_CTRL:
#ifdef SERVER_IS_K
                item = cJSON_GetObjectItem(json, KEY_CMD_ID);
                cmd_id = item->valueint;
#endif
                for(i = 0; i < CONTROL_END; i++) {
                    tmp = strstr((char *)buf, ctrlTable[i].key) ;
                    if(tmp) {
                        item = cJSON_GetObjectItem(json, ctrlTable[i].key);
#ifdef SERVER_IS_K
                        ctrl_val = item->valueint;
                        printf("cmd: %s, val = %d\r\n", ctrlTable[i].key,
                                ctrl_val);
                        ctrlMsg.id = i;
                        ctrlMsg.cmd_id = cmd_id;
                        ctrlMsg.value = ctrl_val;
#elif defined SERVER_IS_VEHICLE_UNION
                        ctrlMsg.id = i;
                        //ignore cmd id in Vehicle Union server
                        ctrlMsg.cmd_id = 0;
                        if(!strcmp(item->valuestring, "ON")) {
                            ctrlMsg.value = 1;
                        } else if(!strcmp(item->valuestring, "OFF")) {
                            ctrlMsg.value = 0;
                        }
#endif
                        OSQPost(getPalInstance()->mailbox, &ctrlMsg);
                    }
                }
                break;
            case MSG_TYPE_CLEAR_FAULT:
                //clear fault code
                getPalInstance()->ops->clear_fault_code();
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
    uint32_t lng, lat;
    parg = parg;
    //send login message firstly
#ifdef SERVER_IS_K
    printf("login!\r\n");
    login();
#endif
    printf("%s\r\n", __func__);
    OSTimeDlyHMSM(0, 0, LOGIN_DELAYED_TIME, 0);
    for(;;) {
        OSTimeDlyHMSM(0, 0, HEARTBEAT_INTERVAL, 0);
        //send heartbeat
        heartbeat = (heartbeat == 100) ? 0 : heartbeat;
        send_heartbeat(heartbeat ++);
#if 0       //disable location
// the vehicle union server will not receive the gps information
#ifdef SERVER_IS_K
        if(heartbeat % 2 == 0) {
            lng = getLongitude();
            lat = getLatitude();
            if(lng != 0 || lat != 0) {
                upload_location(lng, lat);
            }
        }
#endif
#endif
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
    sim800_send((uint8_t *)out, length);
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
    sim800_send((uint8_t *)out, length);

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
    sim800_send((uint8_t *)out, length);

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
#ifdef SERVER_IS_K
    cJSON_AddNumberToObject(root, KEY_CMD_ID, cmd_id);
#endif
    cJSON_AddNumberToObject(root, KEY_STATUS, 0);
    cJSON_AddStringToObject(root, KEY_CMD_TYPE, ctrlTable[cmd_type].key);

    out = cJSON_Print(root);
    length = strlen(out);
    printf("%s\r\n", out);
    sim800_send((uint8_t *)out, length);

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
    sim800_send((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

void upload_fault_code(FaultCodeValue *value)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;
    char val_buf[100];
    uint8_t i;
    uint8_t index = 0, n = 0;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_FAULT_CODE);
    //make value
    memset(val_buf, 0x00, 100);
    for(i = 0; i < value->count; i++) {
        if(i == 0) {
            n = sprintf(val_buf + index, "%04x", value->code[i]);
            index += n;
        } else {
            n = sprintf(val_buf + index, ",%04x", value->code[i]);
            index += n;
        }
    }
    cJSON_AddStringToObject(root, getFaultCodeKey(value->fault_code), val_buf);

    out = cJSON_Print(root);
    length = strlen(out);
    printf("%s\r\n", out);
    sim800_send((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

void upload_location(uint32_t longitude, uint32_t latitude)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    getDeviceId();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_LOCATION);
    cJSON_AddNumberToObject(root, KEY_LNG, longitude);
    cJSON_AddNumberToObject(root, KEY_LAT, latitude);

    out = cJSON_Print(root);
    length = strlen(out);
    printf("%s\r\n", out);
    sim800_send((uint8_t *)out, length);

    cJSON_Delete(root);
    myfree(out);
}

