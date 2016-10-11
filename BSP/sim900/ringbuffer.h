#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include "bsp.h"

struct rb {
    uint8_t *buffer_ptr;
    uint16_t buffer_size;
    uint16_t read_index;
    uint16_t write_index;
};

void rb_init(struct rb* rb, uint8_t* pool, uint16_t size);
bool rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length);
bool rb_get(struct rb* rb, uint8_t *ptr, uint16_t length);
bool rb_is_empty(struct rb* rb);
uint16_t rb_get_size(struct rb *rb);
void rb_clear(struct rb* rb);

#endif
