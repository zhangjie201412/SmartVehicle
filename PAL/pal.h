#ifndef __PAL_H__
#define __PAL_H__

#define NAME_MAX_SIZE       16

typedef enum {
    CONTROL_WINDOW = 0,
    CONTROL_DOOR,
    CONTROL_LIGHT,
    CONTROL_SUNFLOOR,
    CONTROL_TRUNK,
    CONTROL_FINDCAR,
    CONTROL_END,
} EnumControlCapability;

//1. process cmd from server to control obd device
//2. poll message from obd device and upload message to server

typedef struct {

} Pal;

typedef struct {
    uint8_t vehicle_type;
    uint8_t capability[CONTROL_END];
} CtrlCapability;

typedef struct {
    void (*control_window)(uint8 state);
    void (*control_door)(uint8 state);
    void (*control_light)(uint8 state);
    void (*control_sunfloor)(uint8 state);
    void (*control_trunk)(uint8 state);
    void (*control_findcar)(uint8 state);
} DevCtrlOps;

#endif
