#pragma once
#include "typedef.h"
#include "memory.h"
#include "graphics.h"
#include "string.h"
#include "loop.h"
///todo make thread safe. not necesary yet till multicore added.
typedef struct {
	union{
		struct{
			uint8_t cmd;
			union {
				char character;
				struct {
					unsigned short x;
					unsigned short y;
				};
			};
		};
		void* ptr;
	};
}command;
#define command_printchar		1
#define command_deletechar		2

#define command_save_location	3
#define command_jump_location	4
#define command_return_location	5
typedef struct {
	void** buf;
	void** head;
	void** tail;
	void** end;
	uint64_t max; //of the buffer
	long free;
	bool full;
	bool empty;
	bool write_lock;
	bool lock;
}ring_buffer;
ring_buffer new_cmd_buf(uint64_t size){
	ring_buffer ring;
	ring.buf= (void **)(calloc(size * sizeof(command)));
	ring.head=ring.buf;
	ring.tail=ring.buf;
	ring.end=ring.buf+size;
	ring.max=size;
	ring.free=size;
	ring.full=false;
	ring.empty=true;
	ring.write_lock=false;
	ring.lock=false;
	return ring;
}
void* pop_ring_buffer(ring_buffer *ring){
	if(ring->empty){
		return NULL;
	}
	volatile bool l=ring->lock;
	while(l=ring->lock);
	ring->lock=true;
	asm("cli");


	void** x = ring->head;
	ring->head++;
	if(ring->head>=ring->end){
		ring->head=ring->buf;
		//printf("head overflow");
	}
	ring->free++;
	//ring->empty=ring->head==ring->tail;
	ring->empty=ring->free==ring->max;

	ring->full=ring->free<1;
	ring->lock=false;
	asm("sti");

	return *x;
}
void push_ring_buffer(ring_buffer *ring, void* cmd){
	if(ring->full){
		return;
	}
	volatile bool l=ring->write_lock;
	while(l=ring->write_lock){
		busyloop(100);
	}
	l=ring->lock;
	while(l=ring->lock);
	ring->lock=true;
	asm("cli");

	*ring->tail=cmd;
	ring->tail++;
	if(ring->tail>=ring->end){
		ring->tail=ring->buf;
		//printf("tail overflow");
	}
	ring->free--;
	if(ring->free<0){
		asm ("cli");

		printf("buffer error");
		loop();

	}
	ring->empty=false;
	//ring->full=ring->head==ring->tail;
	ring->full=ring->free==0;
	ring->lock=false;
	asm("sti");

}
void push_ring_buffer_string(ring_buffer *ring, char* x){
	volatile uint64_t len=strlen(x);
	if(ring->free<len){
		volatile uint64_t* ptr=&ring->free;
		while(*ptr<len){
			busyloop(100);
		}
	}
	while(*x){
		command com;//={(uint8_t)(*x++)};
		com.cmd=command_printchar;
		com.character=(*x++);
		push_ring_buffer(ring, com.ptr);
	}
}
void ring_buffer_request_space(ring_buffer *ring, uint64_t space){
	ring->write_lock=true;
	volatile uint64_t len=space;
	if(ring->free<len){
		volatile uint64_t* ptr=&ring->free;
		while(*ptr<len){
			busyloop(100);
		}
	}
	ring->write_lock=false;
}