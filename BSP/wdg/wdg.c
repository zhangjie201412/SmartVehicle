#include "wdg.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_wwdg.h"

u8 WWDG_CNT_MAX = 0x7f;
u8 WWDG_CNT = 0x7f;

void iwdg_init(uint8_t prep, uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 		//使能写权限
	IWDG_SetReload(rlr);
	IWDG_SetPrescaler(prep);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void iwdg_feed(void)
{
	IWDG_ReloadCounter();
}

void WWDG_Set_Counter(u8 cnt)
{
	WWDG_Enable(cnt);
}

void WWDG_NVIC_Init()
{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = WWDG_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 2;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

void wwdg_init(u8 tr, u8 wr, u32 fprer)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_CNT = tr & WWDG_CNT_MAX;
	WWDG_SetPrescaler(fprer);
	WWDG_SetWindowValue(wr);
	WWDG_Enable(WWDG_CNT);
	WWDG_ClearFlag();
	WWDG_NVIC_Init();
	WWDG_EnableIT();
}


void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT);
	WWDG_ClearFlag();
}
