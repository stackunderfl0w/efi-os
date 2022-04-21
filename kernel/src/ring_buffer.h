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
typedef struct {
	command* buf;
	command* head;
	command* tail;
	command* end;
	uint64_t max; //of the buffer
	uint64_t free;
	bool full;
	bool empty;
}ring_buffer_command;
ring_buffer_command new_cmd_buf(uint64_t size){
	ring_buffer_command ring;
	ring.buf= (command *)(calloc(size * sizeof(command)));
	ring.head=ring.buf;
	ring.tail=ring.buf;
	ring.end=ring.buf+size;
	ring.max=size;
	ring.free=size;
	ring.full=false;
	ring.empty=true;

	return ring;
}
command pop_command(ring_buffer_command *ring){
	command* x = ring->head;
	ring->head++;
	if(ring->head>=ring->end){
		ring->head=ring->buf;
		//printf("head overflow");
	}
	ring->free++;
	ring->empty=ring->head==ring->tail;
	ring->full=false;
	return *x;
}
void push_command(ring_buffer_command *ring, command cmd){
	*ring->tail=cmd;
	ring->tail++;
	if(ring->tail>=ring->end){
		ring->tail=ring->buf;
		//printf("tail overflow");
	}
	ring->free--;
	ring->empty=false;
	ring->full=ring->head==ring->tail;
}
void push_string(ring_buffer_command *ring, char* x){
	volatile uint64_t len=strlen(x);
	if(ring->free<len){
		volatile uint64_t* ptr=&ring->free;
		while(*ptr<len){
			busyloop(100);
		}
	}
	while(*x){
		push_command(ring, (command){(uint8_t)(*x)});
		x++;
	}
}