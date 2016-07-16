#ifndef __PAL_H__
#define __PAL_H__
#include "stm32f10x.h"
#include "includes.h"

#define NAME_MAX_SIZE       32
#define SUPPORTED           1
#define UNSUPPORTED         0

typedef enum {
    CONTROL_WINDOW = 0,
    CONTROL_DOOR,
    CONTROL_LIGHT,
    CONTROL_SUNFLOOR,
    CONTROL_TRUNK,
    CONTROL_FINDCAR,
    CONTROL_IMMOLOCK,
    CONTROL_END,
} EnumControlCapability;

typedef enum {
    ENG_DATA_RPM,
    ENG_DATA_VS,
    ENG_DATA_ECT,

    //the last one
    PID_SIZE,
} EnumPidType;

typedef struct {
    uint8_t vehicle_type;
    uint8_t capability[CONTROL_END];
} CtrlCapability;

typedef struct {
    uint8_t pid;
    char key[NAME_MAX_SIZE];
} PidItem;

typedef struct {
    uint8_t pid;
    uint8_t support;
} PidSupportItem;

typedef struct {
    void (*control_window)(uint8_t state);
    void (*control_door)(uint8_t state);
    void (*control_light)(uint8_t state);
    void (*control_sunfloor)(uint8_t state);
    void (*control_trunk)(uint8_t state);
    void (*control_findcar)(uint8_t state);
} DevCtrlOps;

typedef struct {
    uint8_t *(*transfer_data_stream)(int pid, uint8_t *len);
} DevUploadOps;

typedef struct {
    uint8_t eng_interval;
    uint8_t at_interval;
    uint8_t abs_interval;
    uint8_t bcm_interval;
    uint8_t ipc_interval;
} DevUploadParam;

typedef struct {
    uint8_t pid;
    uint16_t txId;
    uint8_t len;
    uint8_t data[8];
    uint16_t rxId;
    uint8_t offset;
    uint8_t valid_len;
} StdDataStream;

//1. process cmd from server to control obd device
//2. poll message from obd device and upload message to server
typedef struct {
    CtrlCapability capability;
    DevCtrlOps *ops;
    DevUploadOps *uploadOps;
    PidSupportItem pidSupportList[PID_SIZE];

    OS_EVENT *mailbox;
} Pal;

void pal_init(void);
void pal_do_bcm(uint8_t id, uint8_t val, uint32_t cmd_id);
void immolock(uint8_t state);
void set_immo_state(uint8_t state);

void getDeviceId(void);
Pal *getPalInstance(void);

#endif
