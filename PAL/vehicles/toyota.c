#include "toyota.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"
#include "fake.h"

__IO uint8_t toyota_rx_buf[128];

DevCtrlOps toyota_ops;
DevUploadOps toyota_upload_ops;

CanTxMsg toyota_lamp_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x06, 0x30, 0x15, 0xff, 0x20, 0x00, 0x00
};

CanTxMsg toyota_lamp_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x06, 0x30, 0x15, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_door_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0x00, 0x80, 0x00, 0x00
};

CanTxMsg toyota_door_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0x00, 0x40, 0x00, 0x00
};

CanTxMsg toyota_trunk_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x80, 0x00
};

CanTxMsg toyota_trunk_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x00, 0x00
};

CanTxMsg toyota_sunfloor_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0xad, 0x04, 0x30, 0x01, 0x01, 0x40, 0x00, 0x00
};

CanTxMsg toyota_sunfloor_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0xad, 0x04, 0x30, 0x01, 0x01, 0x80, 0x00, 0x00
};

CanTxMsg toyota_window_on[4] = 
{
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x90, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x91, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x92, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x93, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
};

CanTxMsg toyota_window_off[4] = 
{
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x90, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x91, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x92, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x93, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
};

CanTxMsg toyota_keepalive_normal = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_keepalive_door = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x90, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_continue_package =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x30, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00
};

PidSupportItem toyotaSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, UNSUPPORTED},
    {ENG_DATA_APP, UNSUPPORTED},
    {ENG_DATA_TP, UNSUPPORTED},
    {ENG_DATA_ERT, UNSUPPORTED},
    {ENG_DATA_LOAD, UNSUPPORTED},
    {ENG_DATA_LTFT, UNSUPPORTED},
    {ENG_DATA_STFT, UNSUPPORTED},
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
    {ENG_DATA_OILPRESSURE, SUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, UNSUPPORTED},
    {ENG_DATA_AF, UNSUPPORTED},
    {ENG_DATA_IGTIMING, UNSUPPORTED},
    {ENG_DATA_MAF, UNSUPPORTED},
    {ENG_DATA_OILLIFE, UNSUPPORTED},
    {ENG_DATA_OILTEMP, UNSUPPORTED},
    {ENG_DATA_FUEL, UNSUPPORTED},
};

StdDataStream toyotaStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 2,
    },
    {
        ENG_DATA_VS, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    {
        ENG_DATA_ECT, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
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
        ENG_DATA_OILPRESSURE, 0X7e0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
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

void toyota_keepalive(void)
{
    flexcan_send_frame(&toyota_keepalive_normal);
    flexcan_send_frame(&toyota_keepalive_door);
}

void toyota_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init toyota ops
    toyota_ops.control_door = toyota_ctrl_door;
    toyota_ops.control_window = toyota_ctrl_window;
    toyota_ops.control_sunfloor = toyota_ctrl_sunfloor;
    toyota_ops.control_light = toyota_ctrl_light;
    toyota_ops.control_findcar = toyota_ctrl_findcar;
    toyota_ops.control_trunk = toyota_ctrl_trunk;

    pal->ops = &toyota_ops;
    toyota_upload_ops.transfer_data_stream = toyota_data_stream;
    pal->uploadOps = &toyota_upload_ops;
}

uint8_t* toyota_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i, j;
    uint8_t ret;
    uint8_t valid_len;
    uint8_t offset;
    uint8_t l_bytes = 0;
    uint8_t l_packages = 0;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    //check if this pid supported
    if(toyotaSupportItems[pid].support != SUPPORTED)
        return NULL;

    valid_len = toyotaStdDs[pid].valid_len;
    offset = toyotaStdDs[pid].offset;
    txMsg.StdId = toyotaStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = toyotaStdDs[pid].len;
    printf("send-> ");
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = toyotaStdDs[pid].data[i];
        printf("%02x ", txMsg.Data[i]);
    }
    printf("\r\n");
    ret = flexcan_ioctl(DIR_BI, &txMsg, toyotaStdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        printf("recv-> ");
        for(i = 0; i < 8; i++) {
            toyota_rx_buf[i] = rxMsg->Data[i];
            printf("%02x ", toyota_rx_buf[i]);
        }
        printf("\r\n");
        //check if this recv package is a long package
        if(toyota_rx_buf[0] == 0x10) {
            l_bytes = toyota_rx_buf[1];
            l_packages = 0;
            l_packages = (l_bytes - 6) / 7;
            if((l_bytes - 6) % 7 > 0) {
                l_packages += 1;
            }
            for(i = 2; i < 8; i++) {
                toyota_rx_buf[i - 2] = rxMsg->Data[i];
            }
            //send continue package
            ret = flexcan_ioctl(DIR_BI, &toyota_continue_package,
                    toyotaStdDs[pid].rxId, l_packages);
            if(ret == l_packages) {
                printf("recv packages = %d\r\n", ret);
                for(i = 0;i < ret; i++) {
                    rxMsg = flexcan_dump();
                    for(j = 0; j < 7; j++) {
                        toyota_rx_buf[6 + i * 7 + j] = rxMsg->Data[j + 1];
                        printf("%02x ", rxMsg->Data[j + 1]);
                    }
                    printf("\r\n");
                }
            } else {
                printf("error: ret = %d\r\n", ret);
            }
        }
        //short package
        else {
        }

    } else {
        return NULL;
    }

}

void toyota_ctrl_window(uint8_t state)
{
    uint8_t i = 0;
    if(state) {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    } else {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    }
    for(i = 0; i < 3; i++) {
        toyota_keepalive();
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void toyota_ctrl_door(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_door_on);
    } else {
        flexcan_send_frame(&toyota_door_off);
    }
}

void toyota_ctrl_light(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_lamp_on);
    } else {
        flexcan_send_frame(&toyota_lamp_off);
    }
}

void toyota_ctrl_sunfloor(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_sunfloor_on);
    } else {
        flexcan_send_frame(&toyota_sunfloor_off);
    }
}

void toyota_ctrl_trunk(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_trunk_on);
    } else {
        flexcan_send_frame(&toyota_trunk_off);
    }

}

void toyota_ctrl_findcar(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

