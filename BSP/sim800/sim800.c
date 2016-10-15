#include <stdio.h>
#include "sim800.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "app.h"
#include "ringbuffer.h"
#include "cJSON.h"
#include "config.h"

#ifdef SERVER_IS_K
#define SERV_ADDR   "139.224.17.163"
#define SERV_PORT   8880
#elif defined SERVER_IS_VEHICLE_UNION
#define SERV_ADDR   "139.196.153.24"
#define SERV_PORT   9999
#endif

#define SIM800_CONNECT_RETRY_TIMES  8

#define SIM800_CALLBACK_PRIO        5
#define CALLBACK_TASK_STK_SIZE      256
OS_STK callbackTaskStk[CALLBACK_TASK_STK_SIZE];
static void sim800_callback(void *unused);

OS_EVENT *mSema;
OS_EVENT *mMutex;
OS_EVENT *mSendMutex;
OS_EVENT *mWait;
uint8_t mState;
uint8_t mRingBuffer[256];
struct rb mRb;
RecvFunc sim800_recvFunc;

#define CONFIG_SIZE    4
const char *CONFIG_TABLES[CONFIG_SIZE][2] =
{
    {"AT+CIPMODE=0\r\n", "\r\nOK"},
    {"AT+CGATT?\r\n", "\r\n+CGATT"},
    {"AT+CSTT=\"CMNET\"\r\n", "\r\nOK"},
    {"AT+CIICR\r\n", "\r\nOK"},
};

#define SIM800_CMD_SIZE     8
sim800_cmd mCmds[SIM800_CMD_SIZE] =
{
    {"ATE0\r\n", 100},
    {"AT+CREG?\r\n", 100},
    {"AT+CIPMODE=0\r\n", 100},
    {"AT+CGATT?\r\n", 200},
    {"AT+CSTT=\"CMNET\"\r\n", 200},
    {"AT+CIICR\r\n", 3000},
    {"AT+CIFSR\r\n", 300},
    {"AT+CGATT=1\r\n", 300},
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

void sim800_powerdown(void)
{
    sim800_powerup();
}

void sim800_setup(void)
{
    bool connect_done = FALSE;
    cJSON *root;
    char *out;
    uint16_t len;

    uint8_t retry;
    uint8_t recv;
    bool ret;
    uint8_t i;
    uint8_t signal;
    printf("%s start\r\n", __func__);
    sim800_init();

    retry = 0;
    while(!connect_done) {
        switch(mState) {
            case STATE_UNINITED:
                printf("%s: STATE_UNINITED\r\n", __func__);
                sim800_powerup();
                //wait for module initialization
                sim800_delay(10000);
                mState = STATE_POWERUP;
                if(sim800_send_cmd("AT\r\n", "AT") == TRUE) {
                    mState = STATE_INITED;
                    printf("powerup done!\r\n");
                } else {
                    mState = STATE_UNINITED;
                    printf("powerup failed!\r\n");
                }
                break;
            case STATE_INITED:
                printf("%s: STATE_INITED\r\n", __func__);
                for(i = 0; i < SIM800_CMD_SIZE; i++) {
                    sim800_write(mCmds[i].cmd, strlen(mCmds[i].cmd));
                    sim800_delay(mCmds[i].delay);
                }
                //check signal
                signal = sim800_get_signal();
                if(signal < 5) {
                    //if signal too low power down the gprs module
                    //wait 10 min then re power the module
                    sim800_powerdown();
                    OSTimeDlyHMSM(0, 0, 10, 0);
                    mState = STATE_UNINITED;
                    break;
                }
                rb_clear(&mRb);
                mState = STATE_CONNECTING;
                break;
            case STATE_CONNECTING:
                printf("%s: STATE_CONNECTING\r\n", __func__);
                ret = sim800_connect(SERV_ADDR, SERV_PORT);
                if(ret == TRUE) {
                    //connect done and exit the loop
                    mState = STATE_CONNECTED;
                } else {
                    //re-powerup the module
                    mState = STATE_UNINITED;
                    retry ++;
                }
                break;
            case STATE_CONNECTED:
                printf("%s: STATE_CONNECTED\r\n", __func__);
                connect_done = TRUE;
                mState = STATE_IDLE;
                break;
            default:
                break;
        }
        //if over retry total times, exit the loop
        if(retry > SIM800_CONNECT_RETRY_TIMES) {
            break;
        }
    }

    if(!connect_done) {
        printf("%s: ###connect failed###\r\n", __func__);
        go_reboot();
    }
#if 0
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
    }
#endif
}

static void sim800_callback(void *unused)
{
    INT8U err;

    uint8_t buf[256];
    uint16_t index = 0;
    uint8_t recv;

    unused = unused;

    while(1) {
        index = 0;
        OSSemPend(mWait, 0, &err);
        if(err != OS_NO_ERR)
            continue;

        sim800_lock();
        memset(buf, 0x00, 256);
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            buf[index ++] = recv;
        }
        sim800_unlock();

        if(sim800_recvFunc != NULL) {
            sim800_recvFunc(buf);
        }
    }
}

void SIM800_USART_IRQHandler(void)
{
    uint8_t data = 0;
    static uint8_t buf[4];
    static bool json_start;

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
                break;
            case STATE_SIGNAL:
                sim800_lock();
                rb_put(&mRb, &data, 1);
                if(buf[2] == 'O' &&
                        buf[3] == 'K') {
                    sim800_up();
                }
                sim800_unlock();
                break;
            case STATE_POWERUP:
            case STATE_CONNECTING:
            case STATE_SENDING:
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
                sim800_lock();
                if(data == '{') {
                    json_start = TRUE;
                    rb_clear(&mRb);
                    rb_put(&mRb, &data, 1);
                } else if(data == '}') {
                    rb_put(&mRb, &data, 1);
                    json_start = FALSE;
                    OSSemPost(mWait);
                    //if(sim800_recvFunc != NULL) {
                    //    sim800_recvFunc(mRingBuffer);
                    //}
                } else {
                    if(json_start) {
                        rb_put(&mRb, &data, 1);
                    }
                }
                sim800_unlock();
                break;
            case STATE_CMD_BUSY:
                break;
            case STATE_DATA_BUSY:
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
    rb_clear(&mRb);
    sim800_write((uint8_t *)cmd, strlen(cmd));
    ret = sim800_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
//            printf("recv = %02x %c\r\n", recv, recv);
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
    bool ret;
    int i = 0;
    uint8_t buf[128];
    uint8_t recv;
    uint8_t rx_buf[20];
    uint8_t index = 0;

    memset(buf, 0x00, 128);
    snprintf(buf, 128, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n", host, port);
    printf("%s: %s\r\n", __func__, buf);
    ret = sim800_send_cmd(buf, "\r\nOK");
    if(ret) {
        ret = sim800_down(8);
        if(ret == TRUE) {
            while(!rb_is_empty(&mRb)) {
                rb_get(&mRb, &recv, 1);
                //printf("recv = %02x\r\n", recv);
                rx_buf[index ++] = recv;
            }
            sim800_unlock();
            if(strstr(rx_buf, "CONNECT OK") != NULL) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            sim800_unlock();
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

uint8_t sim800_get_signal(void)
{
    uint8_t signal;
    uint8_t index = 0, i;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;
    uint8_t *s, *p;
    uint8_t buf[4];

    sim800_lock();
    mState = STATE_SIGNAL;
    rb_clear(&mRb);
    sim800_write("AT+CSQ\r\n", 8);
    ret = sim800_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            rx_buf[index ++] = recv;
        }
        if(NULL != (s = strstr(rx_buf, "+CSQ:"))) {
            s = strstr((char *)(s), " ");
            s =s + 1;
            p = strstr((char *)(s), ",");
            if(NULL != s) {
                i = 0;
                while(s < p) {
                    buf[i++] = *(s++);
                }
                buf[i] = '\0';
            }
            signal = atoi(buf);
        }
        sim800_unlock();
    } else {
        sim800_unlock();
        signal = 0;
    }
    mState = STATE_IDLE;
//    printf("signal = %d\r\n", signal);
    return signal;
}

bool sim800_busy = FALSE;

void sim800_wait_busy(void)
{
    while(sim800_busy) {
        printf("busy...\r\n");
        sim800_delay(200);
    }
}

void sim800_send(uint8_t *buf, uint32_t len)
{
    uint8_t endByte[1] = {0x1a};
    uint8_t i = 0;
    INT8U err;
    uint32_t delay = 1000000;

    //sim800_wait_busy();
//    printf("%s:++\r\n", __func__);
    sim800_busy = TRUE;
    OSMutexPend(mSendMutex, 0, &err);

    mState = STATE_DATA_BUSY;
    sim800_write("AT+CIPSEND\r\n",12);
    sim800_delay(100);
    sim800_write(buf, len); 
    mState = STATE_SENDING;
    sim800_write(endByte, 1);
    sim800_delay(200);
    //sim800_send_cmd(endByte, "\r\nSEND OK");
    mState = STATE_IDLE;

    OSMutexPost(mSendMutex);
    sim800_busy = FALSE;
//    printf("%s:--\r\n", __func__);
}


void sim800_register_recv(RecvFunc func)
{
    sim800_recvFunc = func;
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
    mWait = OSSemCreate(0);
    mMutex = OSMutexCreate(10, &err);
    mSendMutex = OSMutexCreate(12, &err);
    mState = STATE_UNINITED;

    OSTaskCreate(sim800_callback, (void *)0,
            &callbackTaskStk[CALLBACK_TASK_STK_SIZE - 1],
            SIM800_CALLBACK_PRIO);
}

void sim800_write(uint8_t *buf, uint16_t size)
{

    uint16_t i = 0;
    for(; i < size; i++) {
        USART_SendData(SIM800_USART, buf[i]);
        while(USART_GetFlagStatus(SIM800_USART, USART_FLAG_TXE) == RESET)
        {;}
    }
}

void sim800_delay(uint32_t ms)
{
    OSTimeDlyHMSM(0, 0, 0, ms);
}

void sim800_lock(void)
{
    INT8U err;
    OSMutexPend(mMutex, 0, &err);
}

void sim800_unlock(void)
{
    OSMutexPost(mMutex);
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

