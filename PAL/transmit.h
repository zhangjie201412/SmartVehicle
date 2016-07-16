#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__
#include "stm32f10x.h"
#include "pal.h"

#define KEY_DEVICE_ID       "deviceid"
#define KEY_MSG_TYPE        "msg_type"
#define KEY_HEARTBEAT       "heartbeat_count"
#define KEY_CMD_ID          "cmd_id"
#define KEY_VEHICLE_TYPE    "model"
#define KEY_STATUS          "status"
#define KEY_CMD_TYPE        "cmd_type"

#define LOGIN_DELAYED_TIME          6

#define MSG_TYPE_HEARTBEAT          0
#define MSG_TYPE_HEARTBEAT_RSP      1
#define MSG_TYPE_CTRL               2
#define MSG_TYPE_CTRL_RSP           3

#define MSG_TYPE_LOGIN              8
#define MSG_TYPE_LOGIN_RSP          9
#define MSG_TYPE_VEHICLE_TYPE       10

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

void recv_callback(uint8_t *buf);
void login(void);
void send_heartbeat(uint8_t count);
void control_rsp(uint32_t cmd_id, uint8_t cmd_type);

#endif
