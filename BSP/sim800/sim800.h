#ifndef __SIM800_H__
#define __SIM800_H__
#include "includes.h"
#include "sim900.h"

#define RCC_APBxPeriph_SIM800_USART_IO          RCC_APB2Periph_GPIOB
#define RCC_APBxPeriph_SIM800_USART             RCC_APB1Periph_USART3
#define SIM800_USART_TXD				        GPIO_Pin_10
#define SIM800_USART_RXD				        GPIO_Pin_11
#define SIM800_USART_IO                         GPIOB
#define SIM800_USART	                        USART3
#define SIM800_PinRemap                         DISABLE
#define SIM800_USARTAPB                         APB1
#define SIM800_USART_IRQHandler                 USART3_IRQHandler

typedef struct __SIM800_CMD {
    char *cmd;
    uint16_t delay;
} sim800_cmd;

typedef void(*RecvFunc)(uint8_t *buf);

enum StatusType {
    STATE_UNINITED,
    STATE_POWERUP,
    STATE_INITED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_IDLE,
    STATE_SENDING,
    STATE_CMD_BUSY,
    STATE_DATA_BUSY,
};

//public:
void sim800_setup(void);
void sim800_powerup(void);

//private:
bool sim800_send_cmd(const char *cmd, const char *rsp);
bool sim800_connect(const char *host, uint32_t port);
void sim800_send(uint8_t *buf, uint32_t len);
void sim800_init(void);
void sim800_write(uint8_t *buf, uint16_t size);
void sim800_delay(uint32_t ms);
void sim800_lock(void);
void sim800_unlock(void);
bool sim800_down(uint16_t sec);
void sim800_up(void);
void sim800_register_recv(RecvFunc func);

#endif
