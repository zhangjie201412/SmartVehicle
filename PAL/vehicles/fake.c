#include "fake.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"

__IO uint8_t fake_rx_buf[8];

DevCtrlOps fake_ops;
DevUploadOps fake_upload_ops;
PidSupportItem fakeSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, UNSUPPORTED},
    {ENG_DATA_APP, UNSUPPORTED},
    {ENG_DATA_TP, UNSUPPORTED},
    {ENG_DATA_ERT, UNSUPPORTED},
    {ENG_DATA_LOAD, SUPPORTED},
    {ENG_DATA_LTFT, SUPPORTED},
    {ENG_DATA_STFT, SUPPORTED},
    {ENG_DATA_MISFIRE1, UNSUPPORTED},
    {ENG_DATA_MISFIRE2, UNSUPPORTED},
    {ENG_DATA_MISFIRE3, UNSUPPORTED},
    {ENG_DATA_MISFIRE4, UNSUPPORTED},
    {ENG_DATA_MISFIRE5, UNSUPPORTED},
    {ENG_DATA_MISFIRE6, UNSUPPORTED},
    {ENG_DATA_FCLS, UNSUPPORTED},
    {ENG_DATA_KEYSTATUS, UNSUPPORTED},
    {ENG_DATA_HO2S1, UNSUPPORTED},
    {ENG_DATA_HO2S2, UNSUPPORTED},
    {ENG_DATA_MAP, UNSUPPORTED},
    {ENG_DATA_INJECTPULSE, UNSUPPORTED},
    {ENG_DATA_OILPRESSURE, UNSUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, UNSUPPORTED},
    {ENG_DATA_AF, UNSUPPORTED},
    {ENG_DATA_IGTIMING, UNSUPPORTED},
    {ENG_DATA_MAF, UNSUPPORTED},
    {ENG_DATA_OILLIFE, UNSUPPORTED},
    {ENG_DATA_OILTEMP, UNSUPPORTED},
    {ENG_DATA_FUEL, UNSUPPORTED},
	{ENG_DATA_FUELLEVEL, UNSUPPORTED},
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

StdDataStream fakeStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0X7df, 8,
        {0x02, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 2,
    },
    {
        ENG_DATA_VS, 0X7df, 8,
        {0x02, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    {
        ENG_DATA_ECT, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //IAT
    {
        ENG_DATA_IAT, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //APP
    {
        ENG_DATA_APP, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //TP
    {
        ENG_DATA_TP, 0X7df, 8,
        {0x02, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 2,
    },
    //ERT
    {
        ENG_DATA_ERT, 0X7df, 8,
        {0x02, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //LOAD
    {
        ENG_DATA_LOAD, 0X7df, 8,
        {0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //LTFT
    {
        ENG_DATA_LTFT, 0X7df, 8,
        {0x02, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //STFT
    {
        ENG_DATA_STFT, 0X7df, 8,
        {0x02, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE1
    {
        ENG_DATA_MISFIRE1, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE2
    {
        ENG_DATA_MISFIRE2, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE3
    {
        ENG_DATA_MISFIRE3, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE4
    {
        ENG_DATA_MISFIRE4, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE5
    {
        ENG_DATA_MISFIRE5, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MISFIRE6
    {
        ENG_DATA_MISFIRE6, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //FCLS
    {
        ENG_DATA_FCLS, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //KEYSTATUS
    {
        ENG_DATA_KEYSTATUS, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //HO2S1
    {
        ENG_DATA_HO2S1, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //HO2S2
    {
        ENG_DATA_HO2S2, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MAP
    {
        ENG_DATA_MAP, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //INJECTPULSE
    {
        ENG_DATA_INJECTPULSE, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //OILPRESSURE
    {
        ENG_DATA_OILPRESSURE, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //OILLEVELSTATUS
    {
        ENG_DATA_OILLEVELSTATUS, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //AF
    {
        ENG_DATA_AF, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //IGTIMING
    {
        ENG_DATA_IGTIMING, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //MAF
    {
        ENG_DATA_MAF, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //OILLIFE
    {
        ENG_DATA_OILLIFE, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //OILTEMP
    {
        ENG_DATA_OILTEMP, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //FUEL
    {
        ENG_DATA_FUEL, 0X7df, 8,
        {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
};

void fake_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init fake ops
    fake_ops.control_door = fake_ctrl_door;
    fake_ops.control_window = fake_ctrl_window;
    fake_ops.control_sunfloor = fake_ctrl_sunfloor;
    fake_ops.control_light = fake_ctrl_light;
    fake_ops.control_findcar = fake_ctrl_findcar;
    fake_ops.control_trunk = fake_ctrl_trunk;
    fake_ops.clear_fault_code = fake_clear_fault_code;

    fake_upload_ops.transfer_data_stream = fake_data_stream;
    fake_upload_ops.is_engine_on = fake_engine_on;
    fake_upload_ops.check_fault_code = fake_check_fault_code;

    pal->ops = &fake_ops;
    pal->uploadOps = &fake_upload_ops;
}

uint8_t fake_engine_on(void)
{
    return check_engine();
}

uint8_t* fake_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i;
    int8_t ret;
    uint8_t valid_len;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    //check if this pid supported
    if(fakeSupportItems[pid].support != SUPPORTED)
        return NULL;

    valid_len = fakeStdDs[pid].valid_len;
    offset = fakeStdDs[pid].offset;
    txMsg.StdId = fakeStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = fakeStdDs[pid].len;
    printf("send-> ");
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = fakeStdDs[pid].data[i];
        printf("%02x ", txMsg.Data[i]);
    }
    printf("\r\n");
    ret = flexcan_ioctl(DIR_BI, &txMsg, fakeStdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        printf("recv-> ");
        for(i = 0; i < 8; i++) {
            fake_rx_buf[i] = rxMsg->Data[i];
            printf("%02x ", fake_rx_buf[i]);
        }
        printf("\r\n");
        *len = valid_len;
        return fake_rx_buf + offset;
    } else {
        return NULL;
    }
}

uint32_t *fake_check_fault_code(uint8_t id, uint8_t *len)
{
    *len = 0;
    return NULL;
}

void fake_ctrl_window(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_door(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_light(uint8_t state)
{
    printf("-> %s\r\n", __func__);

}

void fake_ctrl_sunfloor(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_trunk(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_findcar(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_clear_fault_code(void)
{
    printf("-> %s\r\n", __func__);
}
