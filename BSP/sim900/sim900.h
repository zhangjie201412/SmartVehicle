#ifndef __SIM900_H__
#define __SIM900_H__
#include "bsp.h"
#include "includes.h"

#define RCC_APBxPeriph_SIM900_USART_IO          RCC_APB2Periph_GPIOB
#define RCC_APBxPeriph_SIM900_USART             RCC_APB1Periph_USART3
#define SIM900_USART_TXD				        GPIO_Pin_10
#define SIM900_USART_RXD				        GPIO_Pin_11
#define SIM900_USART_IO                         GPIOB
#define SIM900_USART	                        USART3
#define SIM900_PinRemap                         DISABLE
#define SIM900_USARTAPB                         APB1
//#define SIM900_USART_IRQHandler                 USART3_IRQHandler

#define SIM900_AT_RESP_MQ_POOL_SIZE             1024
#define SIM900_AT_REST_MQ_MSG_SIZE              32
#define SIM900_GSM_CONN_TIMEOUT                 5
#define SIM900_DATA_TIMEOUT                     10
#define SIM900_TCP_CONN_TIMEOUT                 20

typedef void(*RecvFunc)(uint8_t *buf);

enum {
	STATUS_POWERON,
	STATUS_INIT,
	STATUS_CONFIG,
	STATUS_CONNECTING,
	STATUS_CONNECTED,
};

void sim900_init(void);
void sim900_poweron(void);
void send(uint8_t c);
void sim900_write(uint8_t *buf, uint16_t size);
void sim900_connect(void);
int sim900_get_device_id(void);
void sim900_register_recv(RecvFunc func);
#endif
