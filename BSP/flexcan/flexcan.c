#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "flexcan.h"
#include "ucos_ii.h"
#include "ringbuffer.h"

OS_EVENT *can_rx_sem;
static struct rb can_rx_rb;
static uint8_t can_rx_buf[128];
__IO uint8_t rx_ram[128];

uint8_t trans_count = 0;
//+++error code work arround
uint8_t start_error_code = 0;
//---

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

uint8_t *flexcan_send_frame(CanTxMsg *txMsg)
{
	INT8U os_err;
	u32 count = 0;
    u8 transmitMailBox;
	uint8_t one_byte;
	uint8_t i = 0;

	//rb_init(&can_rx_rb, &can_rx_buf[0], sizeof(can_rx_buf));
    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
		count ++;
		if(count > 1000000)
			break;
	}
	
	//wait for can response
	OSSemPend(can_rx_sem, 2 * OS_TICKS_PER_SEC, &os_err);
	if(os_err == OS_ERR_TIMEOUT) {
        printk("%s: wait for can response timeout[%04x]\r\n",
            __func__, txMsg->StdId);
		return NULL;
    } else {
		//printk("%s: get data\r\n", __func__);
		memset((void *)rx_ram, 0x00, 128);
		i = 0;
		while(rb_get(&can_rx_rb, &one_byte, 1) == TRUE) {
			rx_ram[i++] = one_byte;
			//printk("===[%d] = 0x%02x\r\n", i - 1, one_byte);
		}
		
		return (uint8_t *)&rx_ram;
	}
}

void flexcan_send_frame2(CanTxMsg *txMsg)
{
	u32 count = 0;
    u8 transmitMailBox;

    printf("send id = %04x ext id = %08x\r\n", txMsg->StdId, txMsg->ExtId);
	//rb_init(&can_rx_rb, &can_rx_buf[0], sizeof(can_rx_buf));
    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
		count ++;
		if(count > 1000000)
			break;
	}
}

uint8_t error_code_count = 0;
uint8_t error_code_buf[128];

void flexcan_rx_callack(void)
{
    uint8_t i = 0;
    u8 count = 0;
    CanRxMsg rxMsg;
	u8 len;
    //u16 id;
    CAN_Receive(CAN1, CAN_FIFO0, &rxMsg);
    len = rxMsg.DLC;
    //id = rxMsg.StdId;
	//printk("id = %04x\r\n", id);
#if 0
    printk("-id = %04x, dlc = %d, ", rxMsg.StdId, rxMsg.DLC);
    for(i = 0; i < rxMsg.DLC; i++) {
        printk("%02x  ", rxMsg.Data[i]);
    }
    printk("-\r\n");
#endif
    if(start_error_code) {
        if(rxMsg.Data[0] = 0x81 &&
                rxMsg.Data[1] == 0x00 && rxMsg.Data[2] == 0x00) {
            //error_code end
            count = 2 * error_code_count;
            rb_put(&can_rx_rb, &count, 1);
            rb_put(&can_rx_rb, error_code_buf, count);
    	    OSSemPost(can_rx_sem);
            error_code_count = 0;
        } else {
            error_code_buf[error_code_count * 2] = rxMsg.Data[1];
            error_code_buf[error_code_count * 2 + 1] = rxMsg.Data[2];
            error_code_count++;
        }
    } else {
	    if(trans_count >= 2)
		    return;
    	rb_put(&can_rx_rb, &len, 1);
    	rb_put(&can_rx_rb, rxMsg.Data, len);
    	OSSemPost(can_rx_sem);
    	trans_count ++;
    }
}
