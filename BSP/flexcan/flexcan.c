#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "flexcan.h"
#include "ucos_ii.h"
#include "ringbuffer.h"

OS_EVENT *can_rx_sem;
static struct rb can_rx_rb;
static uint8_t can_rx_buf[100];
__IO uint8_t rx_ram[100];

void flexcan_init(u8 velocity)
{
    CAN_InitTypeDef CAN_InitStructure;

    //init for can
    flexcan_can_enable();
    flexcan_nvic_init();
    flexcan_gpio_init();

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
    CAN_InitStructure.CAN_Prescaler = velocity;
    CAN_Init(CAN1, &CAN_InitStructure);

    can_rx_sem = OSSemCreate(0);
    rb_init(&can_rx_rb, &can_rx_buf[0], sizeof(can_rx_buf));
}

void flexcan_nvic_init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void flexcan_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APBxPeriph_CAN_IO |
            RCC_APB2Periph_AFIO,
            ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,
            ENABLE);
    GPIO_InitStructure.GPIO_Pin = CAN_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_IO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CAN_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_IO, &GPIO_InitStructure);

    if(CAN_PinRemap == ENABLE) {
        GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
    }
}

void flexcan_filter(u32 id1, u32 id2, u32 mid1, u32 mid2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    CAN_FilterInitStructure.CAN_FilterNumber=1;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh=id1<<5;
    CAN_FilterInitStructure.CAN_FilterIdLow=id2<<5;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=mid1<<5;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=mid2<<5;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void flexcan_can_enable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

__IO uint8_t flag;
__IO uint8_t need_recv = FALSE;
__IO uint8_t send_data_save[8];
uint8_t *flexcan_send_frame(CanTxMsg *txMsg)
{
    INT8U os_err;
    u32 count = 0;
    u8 transmitMailBox;
    uint8_t one_byte;
    uint8_t i = 0;

    need_recv = TRUE;
    printk("Send\r\n");
    for(i = 0; i < 8; i++) {
        printk("%02x\t", txMsg->Data[i]);
        send_data_save[i] = txMsg->Data[i];
    }
    printk("\r\n");

    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
        count ++;
        if(count > 1000000)
            break;
    }

    //wait for can response
    OSSemPend(can_rx_sem, 4 * OS_TICKS_PER_SEC, &os_err);
    if(os_err == OS_ERR_TIMEOUT) {
        printk("%s: wait for can[%04x] response timeout\r\n",
                __func__, txMsg->StdId);
        return NULL;
    } else {
        memset((void *)rx_ram, 0x00, 100);
        i = 0;
        while(rb_get(&can_rx_rb, &one_byte, 1) == TRUE) {
            rx_ram[i++] = one_byte;
        }

        printk("Recv\r\n");
        for(i = 0; i < 32; i++) {
            printk("0x%02x\t", rx_ram[i]);
            if((i + 1) % 8 == 0) {
                printk("\r\n");
            }
        }
        printk("\r\n");

        return (uint8_t *)&rx_ram;
    }
}

void flexcan_send_frame2(CanTxMsg *txMsg)
{
    u32 count = 0;
    u8 transmitMailBox;

    need_recv = FALSE;
    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
        count ++;
        if(count > 1000000)
            break;
    }
}

void send_continue_package(void)
{
    CanTxMsg msg =
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x30, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    flexcan_send_frame2(&msg);
    need_recv = TRUE;
}

uint8_t lp_mode_flag = 0;
uint8_t lp_length = 0;
uint8_t lp_index = 0;

void flexcan_rx_callack(void)
{
    CanRxMsg rxMsg;
    u8 len;
    u8 cmd_type;
    u8 valid_index = 0;
    u8 valid_len = 0;
    u8 i = 0;
    u8 sub_id;
    CAN_Receive(CAN1, CAN_FIFO0, &rxMsg);
    len = rxMsg.DLC;
    sub_id = rxMsg.Data[0];

    if(!need_recv)
        return;

    //    if(sub_id == 0x40) {
    if(rxMsg.Data[1] == 0x01 &&
            (rxMsg.Data[2] == 0x7e)) {
        //by pass keep live recv bytes
        return;
    }
    //    }

#if 1
    printk("id = %04x\t", rxMsg.StdId);
    for(i = 0; i < len; i ++) {
        printk(" %02x", rxMsg.Data[i]);
    }
    printk("\r\n");
#endif
    //long package mode
    if(rxMsg.Data[0] == 0x10) {
        //enter long package mode
        lp_mode_flag = 1;
        lp_length = rxMsg.Data[1];
        lp_index = 0;
        rb_put(&can_rx_rb, &lp_length, 1);
        rb_put(&can_rx_rb, rxMsg.Data + 2, 6);
        lp_index = 6;
        //printk("lp length = %d\r\n", lp_length);
        //send continue package
        send_continue_package();
    } else {
        //long package mode
        if(lp_mode_flag)
        {
            for(i = 0; i < 7; i++) {
                if(lp_index < (lp_length - 1)) {
                    rb_put(&can_rx_rb, &(rxMsg.Data[1+ i]), 1);
                    lp_index ++;
                    //printk("lp_index = %d\r\n", lp_index);
                } else {
                    //long package receive done
                    lp_index = 0;
                    lp_mode_flag = 0;
                    rb_put(&can_rx_rb, &(rxMsg.Data[1+ i]), 1);
                    OSSemPost(can_rx_sem);
                    //printk("post\r\n");
                    break;
                }
            }
        } else //normal package
        {
            if(send_data_save[0] > 0x10) {
                //send data has subsystem id
                cmd_type = send_data_save[2] + 0x40;
            } else {
                cmd_type = send_data_save[1] + 0x40;
            }

            for(i = 0; i < len; i++) {
                if(rxMsg.Data[i] == cmd_type) {
                    valid_index = i;
                    break;
                }
            }

            printk("valid_index = %d\r\n", valid_index);
            valid_len = len - valid_index;
            rb_put(&can_rx_rb, &valid_len, 1);
            rb_put(&can_rx_rb, rxMsg.Data + valid_index,
                    valid_len);
            OSSemPost(can_rx_sem);
        }

    }
}
