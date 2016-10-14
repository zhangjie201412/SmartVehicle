#ifndef __toyota_H__
#define __toyota_H__
#include "stm32f10x.h"

#define SUPPORT_MAX_BYTES       0x40
#define SUPPORT_MAX_ITEMS       0x20

typedef struct __ToyotaSupportItem {
    uint8_t sub_id;
    uint8_t len;
    uint8_t support_raw_bytes[SUPPORT_MAX_BYTES];
} ToyotaSupportItem;

typedef struct __ToyotaSupportList {
    bool support_done;
    ToyotaSupportItem items[SUPPORT_MAX_ITEMS];
} ToyotaSupportList;

void toyota_setup(void);
void toyota_ctrl_window(uint8_t state);
void toyota_ctrl_door(uint8_t state);
void toyota_ctrl_light(uint8_t state);
void toyota_ctrl_sunfloor(uint8_t state);
void toyota_ctrl_trunk(uint8_t state);
void toyota_ctrl_findcar(uint8_t state);
void toyota_keepalive(void);
uint8_t* toyota_data_stream(uint8_t pid, uint8_t *len);
uint8_t toyota_engine_on(void);
void toyota_clear_fault_code(void);
uint32_t *toyota_check_fault_code(uint8_t id, uint8_t *len);
void toyota_get_supported(void);
void toyota_map_support_list(void);

#endif
