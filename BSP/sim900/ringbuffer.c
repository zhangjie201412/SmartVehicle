#include "ringbuffer.h"
#include <string.h>

void rb_init(struct rb* rb, uint8_t* pool, uint16_t size)
{
    //ASSERT(rb != NULL);
    rb->read_index = 0;
    rb->write_index = 0;
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}

bool rb_is_empty(struct rb* rb)
{
    if(rb->read_index == rb->write_index)
        return TRUE;
    else
        return FALSE;
}

uint16_t rb_get_size(struct rb *rb)
{
    uint16_t size;
    if(rb->read_index > rb->write_index) {
        size = rb->read_index - rb->write_index;
    } else {
        size = rb->buffer_size - rb->write_index + rb->read_index;
    }
    return size;
}

bool rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length)
{
    uint16_t size;
    if(rb->read_index > rb->write_index) {
        size = rb->read_index - rb->write_index;
    } else {
        size = rb->buffer_size - rb->write_index + rb->read_index;
    }
    if(size < length)
        return FALSE;

    if(rb->read_index > rb->write_index) {
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        rb->write_index += length;
    } else {
        size = rb->buffer_size - rb->write_index;
        if(size > length) {
            memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
            rb->write_index += length;
        } else {
            memcpy(&rb->buffer_ptr[rb->write_index], ptr, size);
            memcpy(&rb->buffer_ptr[0], &ptr[size], length - size);
            rb->write_index = length - size;
        }
    }

    return TRUE;
}

bool rb_get(struct rb* rb, uint8_t *ptr, uint16_t length)
{
    uint16_t size;
    if(rb->read_index > rb->write_index)
        size = rb->buffer_size - rb->read_index + rb->write_index;
    else
        size = rb->write_index - rb->read_index;
    if(size < length)
        return FALSE;
    if(rb->read_index > rb->write_index)
    {
        size = rb->buffer_size - rb->read_index;
        if(size > length)
        {
            memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
            rb->read_index += length;
        }
        else
        {
            memcpy(ptr, &rb->buffer_ptr[rb->read_index], size);
            memcpy(&ptr[size], &rb->buffer_ptr[0], length - size);
            rb->read_index = length - size;
        }
    }
    else
    {
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        rb->read_index += length;
    }
    return TRUE;
}

void rb_clear(struct rb* rb)
{
    uint8_t byte;
    while(!rb_is_empty) {
        rb_get(rb, &byte, 1);
    }
//    rb->read_index = 0;
//    rb->write_index = 0;
}
