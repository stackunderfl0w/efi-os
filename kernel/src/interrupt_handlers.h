#pragma once
#include "typedef.h"
#include "graphics.h"
/*typedef struct 
{
    uword_t ip;
    uword_t cs;
    uword_t flags;
    uword_t sp;
    uword_t ss;
}interrupt_frame;*/
struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Keyboard_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Mouse_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void PIT_Handler(struct interrupt_frame* frame);

