#include "pal.h"
#include "stdio.h"
#include "fake.h"
#include "toyota.h"
#include "gm.h"
#include "transmit.h"
#include "wdg.h"
#include "flexcan.h"
#include "m25p16.h"

#define CHECK_ENGINE                    1

#define UPLOAD_THREAD_INTERVAL          60
#define ENG_INTERVAL                    120
#define AT_INTERVAL                     360
#define ABS_INTERVAL                    360
#define BCM_INTERVAL                    60

#define DEVICE_ID_ADDRESS               0x80
#define TRANSMIT_TASK_STK_SIZE          128
OS_STK transmitTaskStk[TRANSMIT_TASK_STK_SIZE];

#define UPLOAD_TASK_STK_SIZE            256
OS_STK uploadTaskStk[UPLOAD_TASK_STK_SIZE];

PidItem pidList[PID_SIZE] =
{
    {ENG_DATA_RPM, "eng_data_rpm", 10},
    {ENG_DATA_VS, "eng_data_vs", ENG_INTERVAL},
    {ENG_DATA_ECT, "eng_data_ect", ENG_INTERVAL},
    {ENG_DATA_IAT, "eng_data_iat", ENG_INTERVAL},
    {ENG_DATA_APP, "eng_data_app", ENG_INTERVAL},
    {ENG_DATA_TP, "eng_data_tp", ENG_INTERVAL},
    {ENG_DATA_ERT, "eng_data_ert", ENG_INTERVAL},
    {ENG_DATA_LOAD, "eng_data_load", ENG_INTERVAL},
    {ENG_DATA_LTFT, "eng_data_ltft", ENG_INTERVAL},
    {ENG_DATA_STFT, "eng_data_stft", ENG_INTERVAL},
    {ENG_DATA_MISFIRE1, "eng_data_misfire1", ENG_INTERVAL},
    {ENG_DATA_MISFIRE2, "eng_data_misfire2", ENG_INTERVAL},
    {ENG_DATA_MISFIRE3, "eng_data_misfire3", ENG_INTERVAL},
    {ENG_DATA_MISFIRE4, "eng_data_misfire4", ENG_INTERVAL},
    {ENG_DATA_MISFIRE5, "eng_data_misfire5", ENG_INTERVAL},
    {ENG_DATA_MISFIRE6, "eng_data_misfire6", ENG_INTERVAL},
    {ENG_DATA_FCLS, "eng_data_fcls", ENG_INTERVAL},
    {ENG_DATA_KEYSTATUS, "eng_data_keystatus", ENG_INTERVAL},
    {ENG_DATA_HO2S1, "eng_data_ho2s1", ENG_INTERVAL},
    {ENG_DATA_HO2S2, "eng_data_ho2s2", ENG_INTERVAL},
    {ENG_DATA_MAP, "eng_data_map", ENG_INTERVAL},
    {ENG_DATA_INJECTPULSE, "eng_data_injectpulse", ENG_INTERVAL},
    {ENG_DATA_OILPRESSURE, "eng_data_oilpressure", ENG_INTERVAL},
    {ENG_DATA_OILLEVELSTATUS, "eng_data_oillevelstatus", ENG_INTERVAL},
    {ENG_DATA_AF, "eng_data_af", ENG_INTERVAL},
    {ENG_DATA_IGTIMING, "eng_data_igtiming", ENG_INTERVAL},
    {ENG_DATA_MAF, "eng_data_maf", ENG_INTERVAL},
    {ENG_DATA_OILLIFE, "eng_data_oillife", ENG_INTERVAL},
    {ENG_DATA_OILTEMP, "eng_data_oiltemp", ENG_INTERVAL},
    {ENG_DATA_FUEL, "eng_data_fuel", ENG_INTERVAL},
    {ENG_DATA_FUELLEVEL, "eng_data_fuellevel", ENG_INTERVAL},
    {ENG_DATA_FUELTANK, "eng_data_fueltank", ENG_INTERVAL},
    {AT_DATA_OILTEMP, "at_data_oiltemp", AT_INTERVAL},
    {ABS_DATA_OILLEVEL, "abs_data_oillevel", ABS_INTERVAL},
    {BCM_DATA_CHARGESTATUS, "bcm_data_chargestatus", BCM_INTERVAL},
    {BCM_DATA_BATTCURRENT, "bcm_data_battcurrent", BCM_INTERVAL},
    {BCM_DATA_BATTSTATUS, "bcm_data_battstatus", BCM_INTERVAL},
    {BCM_DATA_BATTVOLT, "bcm_data_battvolt", BCM_INTERVAL},
    {BCM_DATA_DDA, "bcm_data_dda", BCM_INTERVAL},
    {BCM_DATA_PDA, "bcm_data_pda", BCM_INTERVAL},
    {BCM_DATA_RRDA, "bcm_data_rrda", BCM_INTERVAL},
    {BCM_DATA_LRDA, "bcm_data_lrda", BCM_INTERVAL},
    {BCM_DATA_SUNROOF, "bcm_data_sunroof", BCM_INTERVAL},
    {BCM_DATA_PARKLAMP, "bcm_data_parklamp", BCM_INTERVAL},
    {BCM_DATA_HEADLAMP, "bcm_data_headlamp", BCM_INTERVAL},
    {BCM_DATA_HIGHBEAM, "bcm_data_highbeam", BCM_INTERVAL},
    {BCM_DATA_HAZARD, "bcm_data_hazard", BCM_INTERVAL},
    {BCM_DATA_FRONTFOG, "bcm_data_frontfog", BCM_INTERVAL},
    {BCM_DATA_REARFOG, "bcm_data_rearfog", BCM_INTERVAL},
    {BCM_DATA_LEFTTURN, "bcm_data_leftturn", BCM_INTERVAL},
    {BCM_DATA_RIGHTTURN, "bcm_data_rightturn", BCM_INTERVAL},
    {BCM_DATA_ODO, "bcm_data_odo", BCM_INTERVAL},
    {TPMS_DATA_LFTIREP, "tpms_lftirep", BCM_INTERVAL},
    {TPMS_DATA_RFTIREP, "tpms_rftirep", BCM_INTERVAL},
    {TPMS_DATA_LRTIREP, "tpms_lrtirep", BCM_INTERVAL},
    {TPMS_DATA_RRTIREP, "tpms_rrtirep", BCM_INTERVAL},
};

UpdateItem updateList[PID_SIZE];

FaultCodeItem faultCodeList[FAULT_CODE_SIZE] =
{
    {FAULT_ENGINE_CODE, "powertrain"},
    {FAULT_AT_CODE, "powertrain"},
    {FAULT_ABS_CODE, "classis"},
    {FAULT_SRS_CODE, "body_system"},
    {FAULT_BCM_CODE, "body_system"},
    {FAULT_IPC_CODE, "body_system"},
    {FAULT_EPS_CODE, "chassis"},
    {FAULT_AC_CODE, "body_system"},
    {FAULT_TPMS_CODE, "body_system"},
};

CanTxMsg engineAlive =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void transmit_thread(void *parg);
static void upload_thread(void *unused);
__IO uint8_t deviceid[17];

__IO static Pal mPal;

void pal_init(void)
{
    uint8_t i;

    //create mailbox
    mPal.mailbox = OSMboxCreate((void *)0);

    transmit_init();
    iwdg_init(IWDG_Prescaler_256, 0xfff);
    OSTaskCreate(transmit_thread, (void *)0,
            &transmitTaskStk[TRANSMIT_TASK_STK_SIZE - 1],
            TRANSMIT_TASK_PRIO);
    OSTaskCreate(upload_thread, (void *)0,
            &uploadTaskStk[UPLOAD_TASK_STK_SIZE - 1],
            UPLOAD_TASK_PRIO);
    //clear all update item flag
    for(i = 0;i < PID_SIZE; i++) {
        updateList[i].updated = FALSE;
    }

    //printk("test for fault code\r\n");
    //pal_get_fault_code();
}

static void transmit_thread(void *pargs)
{
    INT8U err;
    CtrlMsg *ctrlMsg;

    pargs = pargs;

    while(1) {
        ctrlMsg = (CtrlMsg *)OSMboxPend(mPal.mailbox, 0, &err);
        printf("id = %d, cmd_id = %d, value = %d\r\n",
                ctrlMsg->id, ctrlMsg->cmd_id, ctrlMsg->value);
        pal_do_bcm(ctrlMsg->id, ctrlMsg->value, ctrlMsg->cmd_id);
    }
}

void pal_do_bcm(uint8_t id, uint8_t val, uint32_t cmd_id)
{
    //printf("%s: ++id = %d, val = %d++\r\n", __func__, id, val);
    switch(id) {
        case CONTROL_IMMOLOCK:
            //lock_can();
            set_immo_lock(val);
            //unlock_can();
            break;
        case CONTROL_WINDOW:
            lock_can();
            mPal.ops->control_window(val);
            unlock_can();
            break;
        case CONTROL_DOOR:
            lock_can();
            mPal.ops->control_door(val);
            unlock_can();
            break;
        case CONTROL_LIGHT:
            lock_can();
            mPal.ops->control_light(val);
            unlock_can();
            break;
        case CONTROL_SUNROOF:
            lock_can();
            mPal.ops->control_sunfloor(val);
            unlock_can();
            break;
        case CONTROL_TRUNK:
            lock_can();
            mPal.ops->control_trunk(val);
            unlock_can();
            break;
        case CONTROL_FINDCAR:
            lock_can();
            mPal.ops->control_findcar(val);
            unlock_can();
            break;
        default:
            printf("invalid cmd id\r\n");
            break;
    }
    //response the cmd_id and cmd_type
    control_rsp(cmd_id, id);
    //printf("%s:----\r\n", __func__);
}

void pal_get_fault_code(void)
{
    uint8_t i, j;
    FaultCodeValue value;
    uint8_t len;
    uint32_t *code;

    for(i = 0; i < FAULT_CODE_SIZE; i++) {
        if(mPal.uploadOps->check_fault_code == NULL)
            break;
        value.fault_code = i;
        memset(value.code, 0x00, FAULT_CODE_MAX_SIZE);
        code = mPal.uploadOps->check_fault_code(i, &len);
        printf("end of check fault code\r\n");
        if(code == NULL) {
            printf("Error: code is null\r\n");
            continue;
        }

        for(j = 0; j < len; j++) {
            value.code[j] = code[j];
        }
        value.count = len;

        printf("%s: %s\t", __func__, getFaultCodeKey(i));
        for(j = 0; j < len; j++) {
            printf("%d(%04x) ", value.code[j], value.code[j]);
        }
        printf("\r\n");
        //upload the fault code
        upload_fault_code(&value);
    }
}

void update_item(uint8_t pid, uint8_t *data, uint8_t len)
{
    uint8_t i;

    updateList[pid].len = len;
    for(i = 0; i < len; i++) {
        updateList[pid].data[i] = data[i];
    }
    updateList[pid].updated = TRUE;
    updateList[pid].spend_time += UPLOAD_THREAD_INTERVAL;
}

void clear_item(uint8_t pid)
{
    updateList[pid].updated = FALSE;
    updateList[pid].spend_time = 0;
}

void upload_server(void)
{
    uint8_t i = 0;

    for(; i < PID_SIZE; i++) {
        if(updateList[i].updated &&
                (updateList[i].spend_time >= pidList[i].interval)) {
            //update item
            updateList[i].pid = i;
            upload_item(&updateList[i]);
            clear_item(i);
        }
    }
}

uint8_t check_engine(void)
{
    CanRxMsg *rxMsg;
    uint8_t ret = -1;
    uint8_t on = FALSE;

    ret = flexcan_ioctl(DIR_BI, &engineAlive,
            0x7e8, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //check if the receive msg type is needed
        //TODO: ???
        if(rxMsg == NULL) {
            on = FALSE;
        } else {
            on = TRUE;
        }
    } else {
        on = FALSE;
    }

    return on;
}

void upload_thread(void *unused)
{
    uint8_t i = 0, j;
    uint8_t *data;
    uint8_t len;
    uint8_t engine_on = TRUE;
    uint8_t last_engine_on = FALSE;

    unused = unused;
    //main thread for upload vehicle data
    for(;;) {
        OSTimeDlyHMSM(0, 0, UPLOAD_THREAD_INTERVAL, 0);
        if(!isConnected())
            continue;
#ifdef CHECK_ENGINE
        //check is engine on
        if(mPal.uploadOps->is_engine_on) {
            lock_can();
            engine_on = mPal.uploadOps->is_engine_on();
            if(engine_on == TRUE && (last_engine_on == FALSE)) {
                printf("---> ENGINE IS ON!! <---\r\n");
                //while engine is on, check if vehicle has fault code
                pal_get_fault_code();
            } else if(engine_on == FALSE && (last_engine_on == TRUE)) {
                printf("---> ENGINE IS OFF!! <---\r\n");
            }
            last_engine_on = engine_on;
            unlock_can();
        }
#endif
        for(i = 0; i < PID_SIZE; i++) {
            //            printf("%s: i = %d\r\n", __func__, i);
#ifdef CHECK_ENGINE
            //if engine is off, skip upload engine related pids
            if(!engine_on) {
                i = (i < ENG_DATA_SIZE) ? ENG_DATA_SIZE : i;
                printf("---> skip ENGINE datas!\r\n");
            }
#endif
            //check the ops pointer
            if(mPal.uploadOps->transfer_data_stream == NULL) {
                printf("transfer_data_stream is NULL\r\n");
                continue;
            }

            len = 0;
            lock_can();
            data = mPal.uploadOps->transfer_data_stream(i, &len);
            unlock_can();
            if(data == NULL) {
                if(len != UNSUPPORTED_LEN) {
                    //if current eng data failed, skip eng datas
                    i = (i < ENG_DATA_SIZE) ? ENG_DATA_SIZE : i;
                    //if current bcm data failed, skip bcm datas
                    //if(i >= BCM_DATA_START)
                    //    break;
                    //i = (i >= BCM_DATA_START) ? 0 : i;
                }
                continue;
            }

            for(j = 0; j < len; j++) {
                printf("%d(%02x) ", data[j], data[j]);
            }
            printf("\r\n");
            update_item(i, data, len);
        }
        upload_server();
    }
}

void lock_can(void)
{}

void unlock_can(void)
{}

const char *getPidKey(uint8_t pid)
{
    return pidList[pid].key;
}

const char *getFaultCodeKey(uint8_t code)
{
    return faultCodeList[code].key;
}

void getDeviceId(void)
{
    //read device id from flash
    flash_page_read(deviceid, DEVICE_ID_ADDRESS, 17);
}

Pal *getPalInstance(void)
{
    return &mPal;
}
