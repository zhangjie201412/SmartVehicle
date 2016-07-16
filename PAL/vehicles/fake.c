#include "fake.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"

__IO uint8_t g_rx_buf[8];

DevCtrlOps fake_ops;
DevUploadOps fake_upload_ops;
PidSupportItem supportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED}
};

StdDataStream stdDs[PID_SIZE] =
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

    fake_upload_ops.transfer_data_stream = fake_data_stream;

    pal->ops = &fake_ops;
    pal->uploadOps = &fake_upload_ops;
}

uint8_t* fake_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i;
    uint8_t ret;
    uint8_t valid_len;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    printf("-> %s pid = %d\r\n", __func__, pid);
    valid_len = stdDs[pid].valid_len;
    offset = stdDs[pid].offset;
    txMsg.StdId = stdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = stdDs[pid].len;
    printf("send-> ");
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = stdDs[pid].data[i];
        printf("%02x ", txMsg.Data[i]);
    }
    printf("\r\n");
    ret = flexcan_ioctl(DIR_BI, &txMsg, stdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        printf("recv-> ");
        for(i = 0; i < 8; i++) {
            g_rx_buf[i] = rxMsg->Data[i];
            printf("%02x ", g_rx_buf[i]);
        }
        printf("\r\n");
        *len = valid_len;
        return g_rx_buf + offset;
    } else {
        return NULL;
    }
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

