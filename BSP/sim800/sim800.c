#include <stdio.h>
#include "sim800.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "app.h"
#include "ringbuffer.h"
#include "cJSON.h"

OS_EVENT *mSema;
OS_EVENT *mMutex;
uint8_t mState;
uint8_t mRingBuffer[100];
struct rb mRb;

#define CONFIG_SIZE    4
const char *CONFIG_TABLES[CONFIG_SIZE][2] =
{
    {"AT+CIPMODE=0\r\n", "\r\nOK"},
    {"AT+CGATT?\r\n", "\r\n+CGATT"},
    {"AT+CSTT=\"CMNET\"\r\n", "\r\nOK"},
    {"AT+CIICR\r\n", "\r\nOK"},
};

void sim800_powerup(void)
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

void sim800_setup(void)
{
    cJSON *root;
    char *out;
    uint16_t len;

    uint8_t recv;
    bool ret;
    uint8_t i;
    printf("%s start\r\n", __func__);
    sim800_init();
    sim800_powerup();
    printf("%s done\r\n", __func__);
    sim800_delay(10000);
    sim800_write("ATE0\r\n", 6);
    sim800_write("AT+CREG?\r\n", 10);
    sim800_delay(300);
    mState = STATE_INITED;

    for(i = 0; i < CONFIG_SIZE; i++) {
        ret = sim800_send_cmd(CONFIG_TABLES[i][0],
                CONFIG_TABLES[i][1]);
        if(ret != TRUE) {
            printf("%s: cmd[%s] failed\r\n", __func__,
                    CONFIG_TABLES[i][0]);
        }
    }
    sim800_write("AT+CIFSR\r\n", 10);
    sim800_delay(4000);
    sim800_write("AT+CGATT=1\r\n", 12);
    sim800_delay(100);
    //sim800_write("AT+CGSN\r\n", 9);
    sim800_delay(100);
    rb_clear(&mRb);
    sim800_connect("139.196.153.24", 9999);
    sim800_delay(4000);

    while(1) {
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "deviceid", "8661040278998801");
        cJSON_AddNumberToObject(root, "msg_type", 2);
        cJSON_AddNumberToObject(root, "heartbeat_count", 22);
        out = cJSON_Print(root);
        len = strlen(out);

        sim800_send(out, len);
        cJSON_Delete(root);
        myfree(out);
        sim800_delay(4000);
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            printf("recv = %c\r\n", recv);
        }
    }
}

void SIM800_USART_IRQHandler(void)
{
    uint8_t data = 0;
    static uint8_t buf[4];
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting ++;
    OS_EXIT_CRITICAL();

    if(USART_GetITStatus(SIM800_USART, USART_IT_RXNE)
            != RESET) {
        data = USART_ReceiveData(SIM800_USART);
        //printf("%c", data);
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = data;
        switch(mState) {
            case STATE_UNINITED:
                break;
            case STATE_INITED:
                sim800_lock();
                rb_put(&mRb, &data, 1);
                if(buf[0] == 'O' &&
                        buf[1] == 'K' &&
                        buf[2] == '\r' &&
                        buf[3] == '\n') {
                    //printf("ok!\r\n");
                    sim800_up();
                }
                sim800_unlock();
                break;
            case STATE_IDLE:
                rb_put(&mRb, &data, 1);
                break;
            case STATE_BUSY:
                break;
            default:
                break;
        }
    }

    if(USART_GetITStatus(SIM800_USART, USART_IT_TXE)
            != RESET) {
        USART_ITConfig(SIM800_USART, USART_IT_TXE, DISABLE);
    }
    OSIntExit();
}

bool sim800_send_cmd(const char *cmd, const char *rsp)
{
    uint16_t rspLen;
    uint8_t index = 0, i;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;

    rspLen = strlen(rsp);

    sim800_lock();
    //rb_clear(&mRb);
    sim800_write((uint8_t *)cmd, strlen(cmd));
    ret = sim800_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            //printf("recv = %02x\r\n", recv);
            rx_buf[index ++] = recv;
        }
        for(i = 0; i < rspLen; i++) {
            if(rsp[i] != rx_buf[i]) {
                //printf("%02x - %02x\r\n", rsp[i], rx_buf[i]);
                sim800_unlock();
                return FALSE;
            }
        }
        sim800_unlock();
        return TRUE;
    } else {
        sim800_unlock();
        return FALSE;
    }
}

bool sim800_connect(const char *host, uint32_t port)
{
    int i = 0;
    uint8_t buf[128];

    memset(buf, 0x00, 128);
    snprintf(buf, 128, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n", host, port);
    printf("%s: %s\r\n", __func__, buf);
    sim800_send_cmd(buf, "\r\nCONNECT OK");

    return TRUE;
}

void sim800_send(uint8_t *buf, uint32_t len)
{
    uint8_t endByte[1] = {0x1a};
    uint8_t i = 0;

    mState = STATE_BUSY;
    sim800_write("AT+CIPSEND\r\n",12);
    sim800_delay(100);
    sim800_write(buf, len); 
    sim800_write(endByte, 1);
    mState = STATE_IDLE;
}

void sim800_init(void)
{
    INT8U err;
    u8 data;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (SIM800_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM800_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_SIM800_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM800_USART_IO | RCC_APBxPeriph_SIM800_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (SIM800_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = SIM800_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM800_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SIM800_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM800_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(SIM800_USART,&USART_InitStructure);
    data = data;
    data = SIM800_USART->DR;
    data = SIM800_USART->SR;
    USART_ITConfig(SIM800_USART,USART_IT_RXNE,ENABLE);
    USART_Cmd(SIM800_USART,ENABLE);
    //init var
    rb_init(&mRb, &mRingBuffer[0], sizeof(mRingBuffer));
    mSema = OSSemCreate(0);
    mMutex = OSMutexCreate(10, &err);
    mState = STATE_UNINITED;
}

void sim800_write(uint8_t *buf, uint16_t size)
{

    uint16_t i = 0;
    printf("++\r\n");
    for(; i < size; i++) {
        USART_SendData(SIM800_USART, buf[i]);
        while(USART_GetFlagStatus(SIM800_USART, USART_FLAG_TXE) == RESET)
        {;}
    }
    printf("--\r\n");
}

void sim800_delay(uint32_t ms)
{
    OSTimeDlyHMSM(0, 0, 0, ms);
}

void sim800_lock(void)
{
    INT8U err;
    //OSMutexPend(mMutex, 0, &err);
}

void sim800_unlock(void)
{
    //OSMutexPost(mMutex);
}

bool sim800_down(uint16_t sec)
{
    INT8U err;
    OSSemPend(mSema, OS_TICKS_PER_SEC * sec, &err);
    if(err != OS_NO_ERR) {
        printf("%s: TIMEOUT\r\n", __func__);
        return FALSE;
    } else {
        return TRUE;
    }
}

void sim800_up(void)
{
    OSSemPost(mSema);    
}

