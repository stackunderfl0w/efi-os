#include "scheduler.h"
#include "thread.h"
#include "loop.h"
#include "pit.h"
#include "ring_buffer.h"
thread *current, *next;
thread *_current, *_next;
thread* third;
uint64_t dummy_stack_ptr;
bool scheduler_inited=false;
ring_buffer_command ring;
//ring_buffer_command threads;

void thread_function(){
	int thread_id = current->tid;
	char arr[16]={'T','h','r','e','a','d',' ','0',10,0};
	arr[7]='0'+thread_id;
	while(1){
		//printf(arr);
		push_string(&ring,arr);
		//printf("%u ",thread_id);
	}
}
void thread_function_2(){
	uint64_t i =0;
	while(1){
		i++;
		//asm ("movq %%rsp, %0" : "=r" (i) );
		//printf("%u ",0);
		uint64_t i;
		//asm ("movq %%rsp, %0" : "=r" (i) );
		printf("thread 2 %u ",i);

		sleep(1);
	}
}
void t3(){
	uint32_t x,y;
	while(1){
		get_cursor_pos(&x, &y);
		move_cursor(20, 0);
		for (int i = 0; i < 10; ++i){
			deletechar();
		}

		uint64_t time=(uint64_t)(TimeSinceBoot*100);
		print(to_string(time));
		printchar(' ');
		print(to_string(x));
		printchar(' ');
		print(to_string(y));

		move_cursor(x, y);
		sleep(50);
	}
}
void print_thread(){
	while (1){
		printf("f");
		while(!ring.empty){
			command cmd = pop_command(&ring);
			printchar(cmd.cmd);
		}
	}
}
thread *threads[3];
uint64_t cur_thread=0;
void thread_function_old(){
	int thread_id = current->tid;
	while(1){
		thread_id = current->tid;
		printf("Thread %u\n", thread_id);
		/*_next = next;
		_current = current;

		// Update "scheduler"
		next = current;
		current = _next;

		// Switch thread
		switch_stack(&_current->stack_ptr, &_next->stack_ptr);*/
		cur_thread++;
		if(cur_thread%2){
			current=threads[0];
			next=threads[1];
		}
		else {
			current=threads[1];
			next=threads[0];		
		}
		switch_stack(&current->stack_ptr, &next->stack_ptr);

	}
}
void thread_function_old2(){
	int thread_id = current->tid;
	while(1){
		printf("Thread 2\n", thread_id);
		/*_next = next;
		_current = current;

		// Update "scheduler"
		next = current;
		current = _next;

		// Switch thread
		switch_stack(&_current->stack_ptr, &_next->stack_ptr);*/
		cur_thread++;
		if(cur_thread%2){
			current=threads[0];
			next=threads[1];
		}
		else {
			current=threads[1];
			next=threads[0];		
		}
		switch_stack(&current->stack_ptr, &next->stack_ptr);

	}
}
void start_scheduler(){
	current = new_thread(thread_function_old);
	next = new_thread(thread_function_old);
	third = new_thread(thread_function);
	threads[0]=current;
	threads[1]=next;
	threads[2]=third;

	ring=new_cmd_buf(1000);

	scheduler_inited=true;
	asm ("sti");
	switch_stack(&dummy_stack_ptr, &current->stack_ptr);
}
void handle_scheduler(){
	if(!scheduler_inited)
		return;
	printf("s");

	/*_next = next;
	_current = current;

	next = current;
	current = _next;*/
	current=threads[cur_thread];
	cur_thread++;
	if(cur_thread>=2){
		cur_thread=0;
	}
	next=threads[cur_thread];
	switch_stack(&current->stack_ptr, &next->stack_ptr);

	/*if(cur_thread%2){
		switch_stack(&threads[0]->stack_ptr, &threads[1]->stack_ptr);
	}
	else {
		switch_stack(&threads[1]->stack_ptr, &threads[0]->stack_ptr);
	}*/

	//switch_stack(&threads[cur_thread%3]->stack_ptr, &threads[(cur_thread+1)%3]->stack_ptr);
}