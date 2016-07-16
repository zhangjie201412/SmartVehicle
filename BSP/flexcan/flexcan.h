#ifndef __FLEXCAN_H__
#define __FLEXCAN_H__

#define CAN_500K                    4
#define CAN_250K                    8
#define CAN_125K                    16
#define CAN_33K                     60
#define RCC_APBxPeriph_CAN_IO       RCC_APB2Periph_GPIOA
#define CAN_RXD                     GPIO_Pin_11
#define CAN_TXD                     GPIO_Pin_12
#define CAN_IO                      GPIOA
#define CAN_PinRemap                DISABLE
#define flexcan_rx_callack			USB_LP_CAN_RX0_IRQHandler

void flexcan_nvic_init(void);
void flexcan_gpio_init(void);
void flexcan_filter(u32 id1, u32 id2, u32 mid1, u32 mid2);
void flexcan_init(u8 velocity);
uint8_t *flexcan_send_frame(CanTxMsg *txMsg);
void flexcan_send_frame2(CanTxMsg *txMsg);
void flexcan_can_enable(void);

#endif
