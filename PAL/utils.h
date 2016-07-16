#ifndef __UTILS_H__
#define __UTILS_H__

#include "stm32f10x.h"

typedef struct {
    uint16_t id;
    uint8_t len;
    uint8_t data[8];
} StdTxMsg;

void sendStdTxMsg(StdTxMsg *msg);

#endif
