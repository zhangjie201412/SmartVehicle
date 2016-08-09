#ifndef __FAKE_H__
#define __FAKE_H__
#include "stm32f10x.h"

void fake_setup(void);
void fake_ctrl_window(uint8_t state);
void fake_ctrl_door(uint8_t state);
void fake_ctrl_light(uint8_t state);
void fake_ctrl_sunfloor(uint8_t state);
void fake_ctrl_trunk(uint8_t state);
void fake_ctrl_findcar(uint8_t state);

uint8_t* fake_data_stream(uint8_t pid, uint8_t *len);
uint32_t *fake_check_fault_code(uint8_t id, uint8_t *len);
uint8_t fake_engine_on(void);
void fake_clear_fault_code(void);
#endif
