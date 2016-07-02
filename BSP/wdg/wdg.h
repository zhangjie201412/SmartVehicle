#ifndef __WDG_H__
#define __WDG_H__

#include "stm32f10x.h"

void iwdg_init(uint8_t prep, uint16_t rlr);
void iwdg_feed(void);

void wwdg_init(u8 tr, u8 wr, u32 fprer);
#endif
