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
	ENG_DATA_IAT,
	ENG_DATA_APP,
	ENG_DATA_TP,
	ENG_DATA_ERT,
	ENG_DATA_LOAD,
	ENG_DATA_LTFT,
	ENG_DATA_STFT,
    //11~20
    ENG_DATA_MISFIRE1,
    ENG_DATA_MISFIRE2,
    ENG_DATA_MISFIRE3,
    ENG_DATA_MISFIRE4,
    ENG_DATA_MISFIRE5,
    ENG_DATA_MISFIRE6,
    ENG_DATA_FCLS,
    ENG_DATA_KEYSTATUS,
    ENG_DATA_HO2S1,
    ENG_DATA_HO2S2,
    //21~30
    ENG_DATA_MAP,
    ENG_DATA_INJECTPULSE,
    ENG_DATA_OILPRESSURE,
    ENG_DATA_OILLEVELSTATUS,
    ENG_DATA_AF,
    ENG_DATA_IGTIMING,
    ENG_DATA_MAF,
    ENG_DATA_OILLIFE,
    ENG_DATA_OILTEMP,
    ENG_DATA_FUEL,
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
    uint8_t interval;
} PidItem;

typedef struct {
    uint8_t pid;
    uint8_t support;
} PidSupportItem;

typedef struct {
    uint8_t pid;
    uint8_t data[8];
    uint8_t len;
    uint8_t updated;
    uint8_t spend_time;
} UpdateItem;

typedef struct {
    void (*control_window)(uint8_t state);
    void (*control_door)(uint8_t state);
    void (*control_light)(uint8_t state);
    void (*control_sunfloor)(uint8_t state);
    void (*control_trunk)(uint8_t state);
    void (*control_findcar)(uint8_t state);
} DevCtrlOps;

typedef struct {
    uint8_t *(*transfer_data_stream)(uint8_t pid, uint8_t *len);
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

const char *getPidKey(uint8_t pid);
void getDeviceId(void);
Pal *getPalInstance(void);

#endif
