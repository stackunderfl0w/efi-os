#include "circular_buffer.h"
#include "memory.h"

_Atomic _Bool b_true=true;
_Atomic _Bool b_false=false;
_Atomic _Bool expected_false=false;

circular_buffer* cb_init(size_t size, size_t element_size){
    circular_buffer* cb=kmalloc(sizeof(circular_buffer)+(size*element_size));
    cb->element_size=element_size;
    cb->max_size=size;
    cb->size=0;
    cb->data=(void*)(cb+sizeof(circular_buffer));
    _Atomic size_t a_zero=0;
    __atomic_store(&cb->head, &a_zero, __ATOMIC_RELAXED);
    __atomic_store(&cb->tail, &a_zero, __ATOMIC_RELAXED);
    __atomic_store(&cb->locked, &b_false, __ATOMIC_RELAXED);
    return cb;
}
void cb_free(circular_buffer* cb){
    free(cb);
}
size_t cb_push(circular_buffer* cb, const void* items, size_t count){
    //Wait for lock
    while (!__atomic_compare_exchange(&cb->locked, &expected_false, &b_true, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
    _Atomic size_t tail=*(size_t*)&cb->tail;
    size_t items_pushed=0;
    for(size_t i=0;i<count;i++){
        if(cb->size==cb->max_size)
            break;//Buffer full
        //cb->data[tail]=items[i];
        memcpy(cb->data+(tail*cb->element_size),items+i*(cb->element_size),cb->element_size);
        tail=(tail+1)%cb->max_size;
        cb->size++;
        items_pushed++;
    }
    __atomic_store(&cb->tail, &tail, __ATOMIC_RELAXED);
    __atomic_store(&cb->locked, &b_false, __ATOMIC_RELAXED);
    return items_pushed;
}
size_t cb_pop(circular_buffer* cb, void* items, size_t count){
    //Wait for lock
    while (!__atomic_compare_exchange(&cb->locked, &expected_false, &b_true, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
    _Atomic size_t head=*(size_t*)&cb->head;
    size_t items_popped=0;
    for(size_t i=0;i<count;i++){
        if(cb->size==0)
            break;//Buffer empty;

        //items[i]=cb->data[head];
        memcpy(items+(i*cb->element_size),cb->data+(head*cb->element_size),cb->element_size);
        head=(head+1)%cb->max_size;
        cb->size--;
        items_popped++;
    }
    __atomic_store(&cb->head, &head, __ATOMIC_RELAXED);
    __atomic_store(&cb->locked, &b_false, __ATOMIC_RELAXED);
    return items_popped;
}