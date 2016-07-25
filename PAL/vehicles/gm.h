#ifndef __FAKE_H__
#define __FAKE_H__
#include "stm32f10x.h"

typedef struct {
    uint8_t pid;
    uint16_t txId[2];
    uint16_t rxId[2];
    uint8_t data[8];
    uint8_t ds;
    uint8_t offset;
    uint8_t valid_len;
} GmDataStream;

void gm_setup(void);
void gm_ctrl_window(uint8_t state);
void gm_ctrl_door(uint8_t state);
void gm_ctrl_light(uint8_t state);
void gm_ctrl_sunfloor(uint8_t state);
void gm_ctrl_trunk(uint8_t state);
void gm_ctrl_findcar(uint8_t state);
uint8_t* fake_data_stream(uint8_t pid, uint8_t *len);
uint8_t* gm_data_stream(uint8_t pid, uint8_t *len);

uint8_t gm_engine_on(void);
void gm_clear_fault_code(void);
#endif
