#include "misc.h"
#include "delay.h"

static u8 fac_us = 0;
static u16 fac_ms = 0;

void delay_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    //SysTick->CTRL &= 0xfffffffb;
    fac_us = SystemCoreClock / 8000000;
    fac_ms = (u16)fac_us * 1000;
}

void delay_us(u32 us)
{
    u32 temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void delay_ms(u16 ms)
{
    u32 temp;
    SysTick->LOAD = ms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void delay_s(u16 s) {
    u16 i;
    for(i = 0; i < s; i++) {
        delay_ms(1000);
    }
}
