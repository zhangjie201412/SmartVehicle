#if !defined( _BSP_H )
#define _BSP_H

#include "..\App\includes.h"

typedef enum {
    APB1 = 0,
    APB2
} APBType;

#define RCC_APBxPeriph_PC_USART_IO  RCC_APB2Periph_GPIOA
#define RCC_APBxPeriph_PC_USART		RCC_APB1Periph_USART2
#define PC_USART_TXD				GPIO_Pin_2
#define PC_USART_RXD				GPIO_Pin_3
#define PC_USART_IO					GPIOA
#define PC_USART	                USART2
#define PC_PinRemap					DISABLE
#define PC_USARTAPB					APB1

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
CPU_INT32U  BSP_CPU_ClkFreq (void);
void  OS_CPU_SysTickInit(void);
void BSP_Init(void);
void USART_Config(u32 baud);
void delay(int ms);

#endif
