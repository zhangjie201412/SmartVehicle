#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "flexcan.h"
#include "ucos_ii.h"
#include "ringbuffer.h"

static __IO CanRxMsg g_rxMsg[RX_PACKAGE_SIZE];
static __IO uint8_t w_off, r_off;
static __IO CanRxMsg m_rxMsg;
OS_EVENT *lock;
OS_EVENT *mailbox;

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

void flexcan_init(u8 velocity)
{
    INT8U err;
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
    w_off = 0;
    r_off = 0;
    lock = OSMutexCreate(1, &err);
    mailbox = OSMboxCreate((void *)0);
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

void flexcan_send_frame(CanTxMsg *txMsg)
{
    u32 count = 0;
    u8 transmitMailBox;

    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
        count ++;
        if(count > 1000000)
            break;
    }
}

int8_t flexcan_ioctl(uint8_t dir, CanTxMsg *txMsg, uint16_t rxId, uint8_t rxCount)
{
    CanRxMsg *rxMsg;
    INT8U err;
    uint8_t i = 0, j = 0;
    uint8_t ret = 0;
    uint8_t exception_count = 0;

    if(dir & DIR_INPUT) {
        flexcan_filter(rxId, rxId, rxId | 0xff, rxId | 0xff);
    } else {
        flexcan_filter(0x00, 0x00, 0x00ff, 0x00ff);
    }
    if(dir & DIR_OUTPUT) {
        flexcan_send_frame(txMsg);
#ifdef FLEXCAN_DEBUG
        printf("->send %04x ", txMsg->StdId);
        for(i = 0; i < 8; i++) {
            printf("%02x ", txMsg->Data[i]);
        }
        printf("\r\n");
#endif
    }

    if(dir & DIR_INPUT) {
        for(i = 0; i < rxCount; /*i++*/) {
            rxMsg = (CanRxMsg *)OSMboxPend(mailbox,
                    FLEXCAN_TIMEOUT * OS_TICKS_PER_SEC, &err);
            if(err != OS_ERR_TIMEOUT) {
                OSMutexPend(lock, 0, &err);
#ifdef FLEXCAN_DEBUG
                printf("->recv %04x ", rxMsg->StdId);
                for(j = 0; j < 8; j++) {
                    printf("%02x ", rxMsg->Data[j]);
                }
                printf("\r\n");
#endif
                //check if the recv id is real rx id
                if(rxMsg->StdId == rxId) {
                    i ++;
                } else {
                    printf("exception!\r\n");
                    exception_count ++;
                    if(exception_count > 5) {
                        ret = -1;
                        OSMutexPost(lock);
                        break;
                    }
                    OSMutexPost(lock);
                    continue;
                }
                //write can msg
                g_rxMsg[w_off].StdId = rxMsg->StdId;
                g_rxMsg[w_off].DLC = rxMsg->DLC;
                for(j = 0; j < rxMsg->DLC; j++) {
                    g_rxMsg[w_off].Data[j] = rxMsg->Data[j];
                }
                w_off ++;
                if(w_off == RX_PACKAGE_SIZE) {
                    w_off = 0;
                }
                ret ++;
                OSMutexPost(lock);
            } else {
                printk("%s: timeout\r\n", __func__);
                break;
            }
        }
    }

    //filter none of can id
    flexcan_filter(0x00, 0x00, 0x00ff, 0x00ff);

    return ret;
}

uint8_t flexcan_count(void)
{
    if(w_off >= r_off)
        return w_off - r_off;
    else
        return RX_PACKAGE_SIZE - r_off + w_off;
}

CanRxMsg *flexcan_dump(void)
{
    CanRxMsg *msg = NULL;
    INT8U err;

    OSMutexPend(lock, 0, &err);
    if(r_off == w_off) {
        OSMutexPost(lock);
        return NULL;
    }

    msg = &g_rxMsg[r_off ++];
    if(r_off == RX_PACKAGE_SIZE)
        r_off = 0;
    OSMutexPost(lock);

    return msg;
}

void flexcan_reset(void)
{
    INT8U err;
    OSMutexPend(lock, 0, &err);
    w_off = 0;
    r_off = 0;
    OSMutexPost(lock);
}

void flexcan_rx_callack(void)
{
    CAN_Receive(CAN1, CAN_FIFO0, &m_rxMsg);
    OSMboxPost(mailbox, &m_rxMsg);
}
