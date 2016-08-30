#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__
#include "stm32f10x.h"
#include "pal.h"
#include "config.h"

#ifdef SERVER_IS_K
    #define KEY_DEVICE_ID       "deviceid"
    #define KEY_MSG_TYPE        "msg_type"
    #define KEY_HEARTBEAT       "heartbeat_count"
    #define KEY_CMD_ID          "cmd_id"
    #define KEY_VEHICLE_TYPE    "model"
    #define KEY_STATUS          "status"
    #define KEY_CMD_TYPE        "cmd_type"
    #define KEY_LNG             "lng"
    #define KEY_LAT             "lat"
#elif defined SERVER_IS_VEHICLE_UNION
    #define KEY_DEVICE_ID       "deviceid"
    #define KEY_MSG_TYPE        "msg_type"
    #define KEY_HEARTBEAT       "heartbeat_count"
    #define KEY_CMD_ID          "cmd_id"
    #define KEY_VEHICLE_TYPE    "model"
    #define KEY_STATUS          "status"
    #define KEY_CMD_TYPE        "cmd_type"
    #define KEY_LNG             "lng"
    #define KEY_LAT             "lat"
#endif

#define LOGIN_DELAYED_TIME          6

#ifdef SERVER_IS_K
    #define MSG_TYPE_HEARTBEAT          0
    #define MSG_TYPE_HEARTBEAT_RSP      1
    #define MSG_TYPE_CTRL               2
    #define MSG_TYPE_CTRL_RSP           3
    #define MSG_TYPE_UPLOAD             4
    #define MSG_TYPE_FAULT_CODE         5
    #define MSG_TYPE_CLEAR_FAULT        6
    #define MSG_TYPE_LOCATION           7
    #define MSG_TYPE_LOGIN              8
    #define MSG_TYPE_LOGIN_RSP          9
    #define MSG_TYPE_VEHICLE_TYPE       10
    #define MSG_TYPE_RETRY              11
#elif defined SERVER_IS_VEHICLE_UNION
    #define MSG_TYPE_HEARTBEAT          2
    #define MSG_TYPE_HEARTBEAT_RSP      2
    #define MSG_TYPE_CTRL               3
    #define MSG_TYPE_CTRL_RSP           4
    #define MSG_TYPE_UPLOAD             0
    #define MSG_TYPE_FAULT_CODE         5
    #define MSG_TYPE_CLEAR_FAULT        6
    /*           UNUSED BELOW          */
    /*                 |               */
    /*                 |               */
    /*                 v               */
    #define MSG_TYPE_LOCATION           7
    #define MSG_TYPE_LOGIN              8
    #define MSG_TYPE_LOGIN_RSP          9
    #define MSG_TYPE_VEHICLE_TYPE       10
    #define MSG_TYPE_RETRY              11
    /*                 ^               */
    /*                 |               */
    /*                 |               */
    /*                                 */
#endif

typedef struct {
    uint8_t id;
    char key[NAME_MAX_SIZE];
} CtrlItem;

typedef struct {
    uint8_t id;
    uint32_t cmd_id;
    uint8_t value;
} CtrlMsg;

void transmit_init(void);

uint8_t isConnected(void);
void recv_callback(uint8_t *buf);
void login(void);
void send_retry(void);
void send_heartbeat(uint8_t count);
void control_rsp(uint32_t cmd_id, uint8_t cmd_type);
void upload_item(UpdateItem *item);
void upload_fault_code(FaultCodeValue *value);
void upload_location(uint32_t longitude, uint32_t latitude);

#endif
