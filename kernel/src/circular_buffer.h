#pragma once
#include "typedef.h"


typedef struct{
    size_t size;
    _Atomic size_t head;
    _Atomic size_t tail;
    uint8_t* data;
    _Atomic bool locked;
}circular_buffer;

circular_buffer* cb_init(size_t size);

void cb_free(circular_buffer* cb);

size_t cb_push(circular_buffer* cb, const uint8_t* items, size_t count);

size_t cb_pop(circular_buffer* cb, uint8_t* items, size_t count);
