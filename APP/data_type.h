#ifndef __DATA_TYPE__
#define __DATA_TYPE__

#define TOYOTA                  1
#define SIMULATOR               0

#define DEVICE_ID_ADDRESS       0x80
#define UPDATE_ONCE_MAX         10

#define NAME_LENGTH				32
#define DEVICE_ID				"CU20160305abc"
#define MSG_TYPE_UPLOAD			0
#define MSG_TYPE_UPLOAD_RSP		1
#define MSG_TYPE_HEART_BEAT		2
#define MSG_TYPE_SEND_CMD		3
#define MSG_TYPE_CMD_RSP		4
#define MSG_TYPE_UPLOAD_ERR_CODE 5
#define MSG_TYPE_CMD_CLEAR_CODE 6

enum CONTROL_CMD {
	BCM_FUN_LAMP,
	BCM_FUN_DOOR,
	BCM_FUN_WINDOW,
	BCM_FUN_TRUNK,
	BCM_FUN_SUNROOF,
	BCM_FUN_FINDCAR,
	BCM_END,
};

enum CLEAR_CODE_CMD {
    CLEAR_CODE_ENGINE,
    CLEAR_CODE_AT,
    CLEAR_CODE_ABS,
    CLEAR_CODE_SRS,
    CLEAR_CODE_BCM,
    CLEAR_CODE_IPC,
    CLEAR_CODE_EPS,
    CLEAR_CODE_AC,
    CLEAR_CODE_TPMS,
    CLEAR_CODE_END
};

enum DATA_TYPE {
    //1~10
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
//    ENG_DATA_MISFIRE5,
//    ENG_DATA_MISFIRE6,
    ENG_DATA_FCLS,
//    ENG_DATA_KEYSTATUS,
    ENG_DATA_HO2S1,
    ENG_DATA_HO2S2,
    //21~30
    ENG_DATA_MAP,
//    ENG_DATA_INJECTPULSE,
    ENG_DATA_OILPRESSURE,
   ENG_DATA_OILLEVELSTATUS,
    ENG_DATA_AF,
    ENG_DATA_IGTIMING,
    ENG_DATA_MAF,
//    ENG_DATA_OILLIFE,
    ENG_DATA_OILTEMP,
 //   ENG_DATA_FUEL,

    //31~40
    ENG_DATA_FUELLEVEL,
//    ENG_DATA_FUELTANK,
    AT_DATA_OILTEMP,
//    ABS_DATA_OILLEVEL,
//    BCM_DATA_CHARGESTATUS,
//    BCM_DATA_BATTCURRENT,
//    BCM_DATA_BATTSTATUS,
    BCM_DATA_BATTVOLT,
    BCM_DATA_DDA,

	BCM_DATA_PDA,
    //41~50
	BCM_DATA_RRDA,
	BCM_DATA_LRDA,
	//BCM_DATA_SUNROOF,
	//BCM_DATA_PARKLAMP,
	//BCM_DATA_HEADLAMP,
	//BCM_DATA_HIGHBEAM,
	BCM_DATA_HAZARD,
	BCM_DATA_FRONTFOG,
	BCM_DATA_REARFOG,
	//51~60
	BCM_DATA_LEFTTURN,
	BCM_DATA_RIGHTTURN,
    BCM_DATA_ODO,

	DATA_TYPE_END,
};

enum ERROR_CODE_TYPE {
    ERROR_ENGINE_CODE,
    ERROR_AT_CODE,
    ERROR_ABS_CODE,
    ERROR_SRS_CODE,
    ERROR_BCM_CODE,
    ERROR_IPC_CODE,
    ERROR_EPS_CODE,
    ERROR_AC_CODE,
    ERROR_TPMS_CODE,
//    ERROR_IMMO_CODE,

    ERROR_CODE_END,
};

typedef struct __control {
	int cmd_type;
	char key[NAME_LENGTH];
	uint16_t id;
	uint8_t cmd_count;
	uint8_t cmd_on[8][8];
	uint8_t cmd_off[8][8];
	uint16_t delay;
} ControlCmd;

typedef struct __clear_code {
    int cmd_type;
    char key[NAME_LENGTH];
    uint16_t id;
    uint8_t cmd[8];
    uint16_t delay;
} ClearCode;

typedef enum {
	END = 0,
    TX,
	RX
} TxRxOrder;

typedef struct __data_type {
	int data_type;
	char name[NAME_LENGTH];
    uint16_t tx_id;
    uint16_t rx_id;
	uint8_t offset;
	uint8_t size;
	uint8_t tx[8];
	uint8_t interval;
} DataType;

typedef struct __result {
	int data_type;
	uint8_t count;
	uint8_t val[32];
	uint8_t updated;
	int current;
} Result;

#endif
