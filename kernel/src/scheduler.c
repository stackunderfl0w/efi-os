#include "scheduler.h"
#include "thread.h"
#include "loop.h"
#include "pit.h"
#include "stdio.h"
#include "memory.h"
#include "fat.h"
#include "graphics.h"
#include "circular_buffer.h"
extern graphics_context* global_context;
extern graphics_context* current_context;
extern int disable_double_buffer;
extern thread *current_thread;

bool scheduler_inited=false;
circular_buffer* thread_pool;

void thread_function(){
	int thread_id = current_thread->tid;
	while(1){
		kprintf("Thread %u %f\n",thread_id,TimeSinceBoot);
		//sleep(10);
		yield();
	}
}
void t3(){
	while(1){
		uint64_t time=(uint64_t)(TimeSinceBoot*100);

		kprintf("\033[s\033[30;0H%u %u %u\033[u",time,30,0);

		//sleep(1);
	}
}
void sacrificial(){
	kprintf("sac thread started");
	return;
}
void vsync(){
	double last_frame[60]={0};
	char st[32]={0};
	double fps=0;
	int count=0;
	while(1){
		if(global_context!=current_context){
			graphics_context* g=current_context;
			for (int i = 59; i > 0; --i){
				last_frame[i]=last_frame[i-1];
			}
			last_frame[0]=TimeSinceBoot;
			swap_buffer(global_context->buf,g->buf);
			int x=global_context->cursor_x,y=global_context->cursor_y;
			global_context->cursor_x=10;
			global_context->cursor_y=global_context->console_height-1;
			count++;
			if (count>10){
				count=0;
				//fps =(1/((last_frame[0]-last_frame[59]))*(60-1));
				fps =1/(last_frame[0]-last_frame[59])*59;
				memset(st,0,32);
				ksprintf(st,"%f",fps);
			}
			print(global_context,st);
			global_context->cursor_x=x;
			global_context->cursor_y=y;
		}
		else{
			draw_mouse(global_context->buf);
		}
		yield();
	}
}

void start_scheduler(){
	asm("cli");
	kprintf("start_scheduler\n");
	thread_pool=cb_init(256,8);
	//new_threads[0] is base thread
	//new_threads[1]=new_thread(thread_function);
	//new_threads[2]=new_thread(thread_function);
	//new_threads[3]=new_thread(thread_function);
	thread* t=new_thread(vsync,NULL);
	cb_push(thread_pool,&t,1);
	// t=new_thread(sacrificial);
	// cb_push(thread_pool,&t,1);

	//new_process("/resources/scrclr.elf", current_context->buf->BaseAddress);

	scheduler_inited=true;
	asm("sti");
	//loop();

}

bool first=true;
void* get_next_thread(void *stack_ptr){
	if(!scheduler_inited)
		return stack_ptr;
	if (first){
		first=false;
		thread* t=kmalloc(sizeof(thread));
		t->tid=0;
		t->state=0;
		current_thread=t;
		//cur_thread=0;
		return stack_ptr;
	}
	current_thread->RSP=stack_ptr;
	if(!current_thread->state&THREAD_DEAD)
		cb_push(thread_pool,&current_thread,1);
	else{
		destroy_thread(current_thread);
	}
	cb_pop(thread_pool,&current_thread,1);

	return current_thread->RSP;
}