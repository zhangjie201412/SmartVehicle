#include "sim900.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "app.h"
#include "..\..\APP\includes.h"
#include "ringbuffer.h"
#include "cJSON.h"
#include "malloc.h"
#include "m25p16.h"

uint8_t connect_cmd[64] = "AT+CIPSTART=\"TCP\",\"139.224.17.163\",\"8880\"\r\n";
//uint8_t connect_cmd[64] = "AT+CIPSTART=\"TCP\",\"117.81.228.156\",\"9999\"\r\n";
static uint8_t rx_buf[100];
static struct rb rx_rb;
OS_EVENT *rx_semaphore;

#define RECEIVE_TASK_STK_SIZE       1024
OS_STK recvTaskStk[RECEIVE_TASK_STK_SIZE];

#define FAKE_TASK_STK_SIZE          128
OS_STK fakeTaskStk[FAKE_TASK_STK_SIZE];
static void fake_thread(void *parg);

#define BUF_LEN		128

__IO char rx_buffer[BUF_LEN];
__IO int rx_offset;

static void thread_sim900_rx_entry(void *parg);

static uint8_t status = STATUS_POWERON;

RecvFunc recvFunc;
void sim900_register_recv(RecvFunc func)
{
    recvFunc = func;
}

void clear_rx_buf(void)
{
    rx_offset = 0;
    memset((void *)rx_buffer, 0x00, BUF_LEN);
}

void sim900_poweron(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    OSTimeDlyHMSM(0, 0, 4, 0);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    OSTimeDlyHMSM(0, 0, 4, 0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

static void fake_thread(void *parg)
{
    parg = parg;
    while(1) {
        OSTimeDlyHMSM(0, 0, 2, 0);
        recvFunc("123");
    }
}

void sim900_init(void)
{
    u8 data;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (SIM900_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM900_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_SIM900_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM900_USART_IO | RCC_APBxPeriph_SIM900_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (SIM900_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = SIM900_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM900_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SIM900_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM900_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(SIM900_USART,&USART_InitStructure);
    data = data;
    data = SIM900_USART->DR;
    data = SIM900_USART->SR;
    USART_ITConfig(SIM900_USART,USART_IT_RXNE,ENABLE);
    USART_Cmd(SIM900_USART,ENABLE);

    rb_init(&rx_rb, &rx_buf[0], sizeof(rx_buf));
    rx_semaphore = OSSemCreate(0);

    //setup recieve thread
    OSTaskCreate(thread_sim900_rx_entry, (void *)0,
            &recvTaskStk[RECEIVE_TASK_STK_SIZE - 1],
            SIM900_RX_PRIO);
    status = STATUS_POWERON;

#if 0
    //fake thread to do some emulator
    printk("create fake thread\r\n");
    OSTaskCreate(fake_thread, (void *)0,
            &fakeTaskStk[FAKE_TASK_STK_SIZE - 1],
            FAKE_TASK_PRIO);
#endif
}

void send(uint8_t c)
{
    USART_SendData(SIM900_USART, c);
    while(USART_GetFlagStatus(SIM900_USART, USART_FLAG_TXE) == RESET)
    {;}
}

void sim900_write(uint8_t *buf, uint16_t size)
{
    //INT8U   err;
    uint16_t i = 0;
    //SIM900_LOCK();
    for(; i < size; i++) {
        send(buf[i]);
    }
    //SIM900_UNLOCK();
}

void at(void)
{
    sim900_write("AT\r\n", 4);
}

void clear_buf(void)
{
    uint8_t i;
    for(i = 0; i < BUF_LEN; i++) {
        rx_buffer[i] = 0x00;
    }
    rx_offset = 0;
}

int check_at(void)
{
    char *tmp = NULL;

    tmp = strstr((char *)rx_buffer, "OK");
    if(tmp)
        return 0;
    else
        return -1;
}

int check_connect(void)
{
    char *tmp = NULL;

    tmp = strstr((char *)rx_buffer, "CONNECT");
    if(tmp)
        return 0;
    else
        return -1;
}

void sim900_wait(int ticks)
{
    int wait = ticks;
    while(--wait > 0)
        ;
}

void sim900_connect(void)
{
    uint8_t i = 0;
    uint8_t retry = 3;

    printk("connect...\r\n");

    while(1) {
        switch(status) {
            case STATUS_POWERON:
                sim900_poweron();
                clear_rx_buf();

                printk("wait for at done\r\n");
                for(i = 0; i < retry; i++) {
                    sim900_write("AT\r\n", 4);
                    //sim900_wait(5000000);
                    OSTimeDlyHMSM(0, 0, 0, 400);
                    if(!check_at()) {
                        status = STATUS_CONFIG;
                        break;
                    }
                    clear_rx_buf();
                }
                break;

                //case STATUS_INIT:
            case STATUS_CONFIG:
                printk("wait for config done\r\n");
                OSTimeDlyHMSM(0, 0, 10, 0);
                sim900_write("AT+CREG?\r\n", 10);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CIPMODE=1\r\n", 14);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CGATT?\r\n", 11);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CSTT=\"CMNET\"\r\n", 17);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CIICR\r\n", 10);
                OSTimeDlyHMSM(0, 0, 2, 0);
                printk("try to get ip address\r\n");
                sim900_write("AT+CIFSR\r\n", 10);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CGATT=1\r\n", 12);
                OSTimeDlyHMSM(0, 0, 0, 100);
                sim900_write("AT+CGSN\r\n", 9);
                OSTimeDlyHMSM(0, 0, 0, 100);
                status = STATUS_CONNECTING;
                break;
            case STATUS_CONNECTING:
                printk("wait for connecting...\r\n");
                sim900_write("AT+CIPSTATUS\r\n", 14);
                OSTimeDlyHMSM(0, 0, 2, 0);
                for(i = 0; i < 1; i++) {
                    clear_rx_buf();
                    sim900_write(connect_cmd, strlen((char *)&connect_cmd[0]));
                    OSTimeDlyHMSM(0, 0, 3, 0);
                    if(!check_at())  {
                        status = STATUS_CONNECTED;
                        break;
                    }
                    clear_rx_buf();
                }
                if(i >= 1) {		//retry failed
                    sim900_write("+++\r\n", 5);
                    OSTimeDlyHMSM(0, 0, 1, 0);
                    sim900_write("AT+CIPCLOSE\r\n", 13);
                    OSTimeDlyHMSM(0, 0, 1, 0);
                    sim900_write("AT+CIPSHUT\r\n", 12);
                    OSTimeDlyHMSM(0, 0, 1, 0);
                    status = STATUS_CONFIG;
                }
                break;
            case STATUS_CONNECTED:
                printk("Connect done!\r\n");
                clear_buf();
                return;
            default:
                break;
        }
    }
}

//char text[] = "{\"msg_type\":\"ctr\",\"deviceid\":\"113344\",\"bcm_fun_trunklock\":\"ON\"}";
static void thread_sim900_rx_entry(void *parg)
{
    INT8U err;
    uint8_t start = 0;
    uint8_t thischar = 0;

    parg = parg;

    printf("Enter: %s\r\n", __func__);
    while(1) {
        OSSemPend(rx_semaphore, 0, &err);
        if(err != OS_NO_ERR)
            continue;

        while(rb_get(&rx_rb, &thischar, 1) == TRUE) {
            printf("%c", thischar);
            if(status == STATUS_CONNECTED) {
                if(thischar == '{') {
                    clear_rx_buf();
                    start = 1;
                    rx_buffer[rx_offset ++] = thischar;
                } else if(thischar == '}') {
                    rx_buffer[rx_offset ++] = thischar;
                    start = 0;
                    if(rx_offset >= 10) {
                        if(recvFunc != NULL)
                            recvFunc((uint8_t *)rx_buffer);
                    }
                } else {
                    if(start)
                        rx_buffer[rx_offset ++] = thischar;
                }
            } else		// init part
            {
                rx_buffer[rx_offset ++] = thischar;
            }
        }
    }
}

void SIM900_USART_IRQHandler(void)
{
    uint8_t data = 0;

    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting ++;
    OS_EXIT_CRITICAL();

    if(USART_GetITStatus(SIM900_USART, USART_IT_RXNE)
            != RESET) {
        data = USART_ReceiveData(SIM900_USART);
        rb_put(&rx_rb, &data, 1);
        OSSemPost(rx_semaphore);
    }

    if(USART_GetITStatus(SIM900_USART, USART_IT_TXE)
            != RESET) {
        USART_ITConfig(SIM900_USART, USART_IT_TXE, DISABLE);
    }
    OSIntExit();
}
