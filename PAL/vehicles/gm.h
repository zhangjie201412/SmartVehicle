#ifndef __FAKE_H__
#define __FAKE_H__
#include "stm32f10x.h"

void gm_setup(void);
void gm_ctrl_window(uint8_t state);
void gm_ctrl_door(uint8_t state);
void gm_ctrl_light(uint8_t state);
void gm_ctrl_sunfloor(uint8_t state);
void gm_ctrl_trunk(uint8_t state);
void gm_ctrl_findcar(uint8_t state);

#endif
