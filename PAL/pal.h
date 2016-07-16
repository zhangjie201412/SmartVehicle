#ifndef __PAL_H__
#define __PAL_H__
#include "stm32f10x.h"
#include "includes.h"

#define NAME_MAX_SIZE       32

typedef enum {
    CONTROL_WINDOW = 0,
    CONTROL_DOOR,
    CONTROL_LIGHT,
    CONTROL_SUNFLOOR,
    CONTROL_TRUNK,
    CONTROL_FINDCAR,
    CONTROL_IMMOLOCK,
    CONTROL_END,
} EnumControlCapability;

typedef struct {
    uint8_t vehicle_type;
    uint8_t capability[CONTROL_END];
} CtrlCapability;

typedef struct {
    void (*control_window)(uint8_t state);
    void (*control_door)(uint8_t state);
    void (*control_light)(uint8_t state);
    void (*control_sunfloor)(uint8_t state);
    void (*control_trunk)(uint8_t state);
    void (*control_findcar)(uint8_t state);
} DevCtrlOps;

//1. process cmd from server to control obd device
//2. poll message from obd device and upload message to server
typedef struct {
    CtrlCapability capability;
    DevCtrlOps *ops;
    OS_EVENT *mailbox;
} Pal;

void pal_init(void);
void pal_do_bcm(uint8_t id, uint8_t val);
void immolock(uint8_t state);
void set_immo_state(uint8_t state);

void getDeviceId(void);
Pal *getPalInstance(void);

#endif
