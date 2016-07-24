#include "gm.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"
#include "fake.h"

DevCtrlOps gm_ops;
DevUploadOps gm_upload_ops;

CanTxMsg gm_lamp_on = 
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xae, 0x02, 0x00, 0x00, 0x02, 0x02, 0x00
};

CanTxMsg gm_lamp_off = 
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xAE, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00
};

CanTxMsg gm_door_on[5] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x04, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_door_off[5] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_window_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
    },
};

CanTxMsg gm_window_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
    },
};

CanTxMsg gm_trunk_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x10, 0x10, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_trunk_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_sunfloor_on[3] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_sunfloor_off[3] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_findcar_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x02, 0xF0, 0xF0, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_findcar_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_start_stream =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x03, 0xaa, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00
};

PidSupportItem gmSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, SUPPORTED},
    {ENG_DATA_APP, SUPPORTED},
    {ENG_DATA_TP, SUPPORTED},
    {ENG_DATA_ERT, SUPPORTED},
    {ENG_DATA_LOAD, SUPPORTED},
    {ENG_DATA_LTFT, SUPPORTED},
    {ENG_DATA_STFT, SUPPORTED},
    {ENG_DATA_MISFIRE1, SUPPORTED},
    {ENG_DATA_MISFIRE2, SUPPORTED},
    {ENG_DATA_MISFIRE3, SUPPORTED},
    {ENG_DATA_MISFIRE4, SUPPORTED},
    {ENG_DATA_MISFIRE5, SUPPORTED},
    {ENG_DATA_MISFIRE6, SUPPORTED},
    {ENG_DATA_FCLS, SUPPORTED},
    {ENG_DATA_KEYSTATUS, SUPPORTED},
    {ENG_DATA_HO2S1, SUPPORTED},
    {ENG_DATA_HO2S2, SUPPORTED},
    {ENG_DATA_MAP, SUPPORTED},
    {ENG_DATA_INJECTPULSE, SUPPORTED},
    {ENG_DATA_OILPRESSURE, SUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, SUPPORTED},
    {ENG_DATA_AF, SUPPORTED},
    {ENG_DATA_IGTIMING, SUPPORTED},
    {ENG_DATA_MAF, SUPPORTED},
    {ENG_DATA_OILLIFE, SUPPORTED},
    {ENG_DATA_OILTEMP, SUPPORTED},
    {ENG_DATA_FUEL, SUPPORTED},
    {ENG_DATA_FUELLEVEL, SUPPORTED},
    {ENG_DATA_FUELTANK, UNSUPPORTED},
    {AT_DATA_OILTEMP, UNSUPPORTED},
    {ABS_DATA_OILLEVEL, UNSUPPORTED},
    {BCM_DATA_CHARGESTATUS, UNSUPPORTED},
    {BCM_DATA_BATTCURRENT, UNSUPPORTED},
    {BCM_DATA_BATTSTATUS, UNSUPPORTED},
    {BCM_DATA_BATTVOLT, UNSUPPORTED},
    {BCM_DATA_DDA, UNSUPPORTED},
    {BCM_DATA_PDA, UNSUPPORTED},
    {BCM_DATA_RRDA, UNSUPPORTED},
    {BCM_DATA_LRDA, UNSUPPORTED},
    {BCM_DATA_SUNROOF, UNSUPPORTED},
    {BCM_DATA_PARKLAMP, UNSUPPORTED},
    {BCM_DATA_HEADLAMP, UNSUPPORTED},
    {BCM_DATA_HIGHBEAM, UNSUPPORTED},
    {BCM_DATA_HAZARD, UNSUPPORTED},
    {BCM_DATA_FRONTFOG, UNSUPPORTED},
    {BCM_DATA_REARFOG, UNSUPPORTED},
    {BCM_DATA_LEFTTURN, UNSUPPORTED},
    {BCM_DATA_RIGHTTURN, UNSUPPORTED},
    {BCM_DATA_ODO, UNSUPPORTED},
};

GmDataStream gmDataStream[PID_SIZE] =
{
    //ENG_DATA_RPM
    {
        //pid
        ENG_DATA_RPM,
        //txId
        0x7e0,
        //rxId
        {0x7e8, 0x5e8},
        //data
        {0x04, 0x2c, 0xfe, 0x00, 0x0c, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_VS
    {
        ENG_DATA_VS,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0d, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_ECT
    {
        ENG_DATA_ECT,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x05, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_IAT
    {
        ENG_DATA_IAT,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_APP
    {
        ENG_DATA_APP,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x13, 0x1f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_TP
    {
        ENG_DATA_TP,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0xb1, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_ERT
    {
        ENG_DATA_ERT,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x1f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_LOAD
    {
        ENG_DATA_LOAD,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x04, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_LTFT
    {
        ENG_DATA_LTFT,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x07, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_STFT
    {
        ENG_DATA_STFT,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x06, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE1
    {
        ENG_DATA_MISFIRE1,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x06, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE2
    {
        ENG_DATA_MISFIRE2,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x05, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE3
    {
        ENG_DATA_MISFIRE3,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x07, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE4
    {
        ENG_DATA_MISFIRE4,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x08, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE5
    {
        ENG_DATA_MISFIRE5,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0xea, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE6
    {
        ENG_DATA_MISFIRE6,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0xeb, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FCLS
    {
        ENG_DATA_FCLS,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x31, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_KEYSTATUS
    {
        ENG_DATA_KEYSTATUS,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x20, 0x7e, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_HO2S1
    {
        ENG_DATA_HO2S1,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x23, 0x20, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_HO2S2
    {
        ENG_DATA_HO2S2,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x23, 0x21, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MAP
    {
        ENG_DATA_MAP,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x20, 0x8a, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_INJECTPULSE
    {
        ENG_DATA_INJECTPULSE,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0xc3, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_OILPRESSURE
    {
        ENG_DATA_OILPRESSURE,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x27, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_OILLEVELSTATUS
    {
        ENG_DATA_OILLEVELSTATUS,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x27, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_AF
    {
        ENG_DATA_AF,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x44, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_IGTIMING
    {
        ENG_DATA_IGTIMING,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0e, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MAF
    {
        ENG_DATA_MAF,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x10, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_OILLIFE
    {
        ENG_DATA_OILLIFE,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x02, 0x1a, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        3,
        1,
    },
    //ENG_DATA_OILTEMP
    {
        ENG_DATA_OILTEMP,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x54, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FUEL
    {
        ENG_DATA_FUEL,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x19, 0x40, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELLEVEL,
        0x7e0,
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x13, 0x2a, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
};

CanTxMsg gm_keepalive =
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

__IO uint8_t gm_rx_buf[8];

void gm_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init gm ops
    gm_ops.control_door = gm_ctrl_door;
    gm_ops.control_window = gm_ctrl_window;
    gm_ops.control_sunfloor = gm_ctrl_sunfloor;
    gm_ops.control_light = gm_ctrl_light;
    gm_ops.control_findcar = gm_ctrl_findcar;
    gm_ops.control_trunk = gm_ctrl_trunk;
    gm_ops.clear_fault_code = gm_clear_fault_code;

    //use fake ebod
    gm_upload_ops.transfer_data_stream = gm_data_stream;
    gm_upload_ops.is_engine_on = gm_engine_on;
    pal->ops = &gm_ops;
    pal->uploadOps = &gm_upload_ops;
}

uint8_t gm_engine_on(void)
{
    return check_engine();
}

uint8_t* gm_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i, j;
    uint8_t ret;
    uint8_t valid_len;
    uint8_t valid_index = 0;
    uint8_t data_type;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    //check if this pid supported
    if(gmSupportItems[pid].support != SUPPORTED) {
        return NULL;
    }

    printf("pid: %s\r\n", getPidKey(pid));
    //clear flexcan rx buf
    OSTimeDlyHMSM(0, 0, 0, 300);
    flexcan_reset();
    //clear rx buf
    memset(gm_rx_buf, 0x00, 8);

    valid_len = gmDataStream[pid].valid_len;
    offset = gmDataStream[pid].offset;

    //send pid data
    txMsg.StdId = gmDataStream[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = 8;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = gmDataStream[pid].data[i];
    }
    //get data type
    data_type = gmDataStream[pid].data[1] + 0x40;
    ret = flexcan_ioctl(DIR_BI, &txMsg, gmDataStream[pid].rxId[0], 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        if(rxMsg == NULL) {
            return NULL;
        }
        if(gmDataStream[pid].ds) {
            //check rx data type is ok?
            //if(data_type == rxMsg->Data[1]) {
                //get valid pid callback
                //send start stream
                ret = flexcan_ioctl(DIR_BI, &gm_start_stream,
                        gmDataStream[pid].rxId[1], 1);
                if(ret > 0) {
                    rxMsg = flexcan_dump();
                    if(rxMsg == NULL) {
                        return NULL;
                    }
                    //get valid buf
                    for(i = 0; i < 8; i++) {
                        gm_rx_buf[i] = rxMsg->Data[i];
                    }
                }
            //} else {
            //    printf("Error: not valid rx msg\r\n");
            //    return NULL;
            //}
        } else {
            //get valid buf
            for(i = 0; i < 8; i++) {
                gm_rx_buf[i] = rxMsg->Data[i];
            }
        }
        *len = valid_len;
        return gm_rx_buf + offset;
    } else {
        return NULL;
    }
}

void gm_ctrl_window(uint8_t state)
{
    uint8_t i = 0;

    printf("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_window_on[i]);
            OSTimeDlyHMSM(0, 0, 5, 0);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_window_off[i]);
            OSTimeDlyHMSM(0, 0, 5, 0);
        }
    }
}

void gm_ctrl_door(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame(&gm_door_on[i]);
        }
    } else {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame(&gm_door_off[i]);
        }
    }
    flexcan_send_frame(&gm_keepalive);
}

void gm_ctrl_light(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    flexcan_send_frame(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        flexcan_send_frame(&gm_lamp_on);
    } else {
        flexcan_send_frame(&gm_lamp_off);
    }
    OSTimeDlyHMSM(0, 0, 1, 0);
    flexcan_send_frame(&gm_keepalive);
}

void gm_ctrl_sunfloor(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 3; i ++) {
            flexcan_send_frame(&gm_sunfloor_on[i]);
            OSTimeDlyHMSM(0, 0, 3, 0);
        }
    } else {
        for(i = 0; i < 3; i++) {
            flexcan_send_frame(&gm_sunfloor_off[i]);
            OSTimeDlyHMSM(0, 0, 3, 0);
        }
    }
}

void gm_ctrl_trunk(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame(&gm_trunk_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_trunk_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    }
    flexcan_send_frame(&gm_keepalive);
}

void gm_ctrl_findcar(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame(&gm_findcar_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 200);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_findcar_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 200);
        }
    }
    flexcan_send_frame(&gm_keepalive);
}

void gm_clear_fault_code(void)
{
    printf("-> %s\r\n", __func__);
}
