#ifndef __toyota_H__
#define __toyota_H__
#include "stm32f10x.h"

void toyota_setup(void);
void toyota_ctrl_window(uint8_t state);
void toyota_ctrl_door(uint8_t state);
void toyota_ctrl_light(uint8_t state);
void toyota_ctrl_sunfloor(uint8_t state);
void toyota_ctrl_trunk(uint8_t state);
void toyota_ctrl_findcar(uint8_t state);
void toyota_keepalive(void);

#endif
