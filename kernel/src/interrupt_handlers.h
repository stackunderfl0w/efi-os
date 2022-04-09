#pragma once
#include "typedef.h"
#include "graphics.h"
struct interrupt_frame{
	uint64_t ip;
	uint64_t cs;
	uint64_t flags;
	uint64_t sp;
	uint64_t ss;
	uint64_t err;
	uint64_t pad;
	uint64_t pad2;
};//todo confirm intel manual is not bullshiting me
//struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Keyboard_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Mouse_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void PIT_Handler(struct interrupt_frame* frame);

