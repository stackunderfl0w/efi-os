#include "scheduler.h"
#include "thread.h"
#include "loop.h"
#include "pit.h"
#include "ring_buffer.h"
thread *current, *previous;
thread *dummy_stack_ptr;
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
	current=threads[cur_thread%3];

	int thread_id = current->tid;
	while(1){
		//thread_id = current->tid;
		printf("Thread %u but cur is %u\n", thread_id,current->tid);
		//printf("Thread %u\n", thread_id);
		asm ("sti");

		previous=threads[cur_thread%3];
		cur_thread++;
		current=threads[cur_thread%3];
		cur_thread%=3;

		switch_stack(&previous->stack_ptr, &current->stack_ptr);

	}
}

void start_scheduler(){
	printf("start_scheduler");
	threads[0]=new_thread(thread_function_old);
	threads[1]=new_thread(thread_function_old);
	threads[2]=new_thread(thread_function_old);

	ring=new_cmd_buf(1000);

	scheduler_inited=true;
	//asm ("sti");
	current=threads[0];
	dummy_stack_ptr=(thread*)REQUEST_PAGE;
	switch_stack(&dummy_stack_ptr, &current->stack_ptr);
}
void handle_scheduler(){
	if(!scheduler_inited)
		return;
	printf("s");


	previous=threads[cur_thread%3];
	cur_thread++;
	current=threads[cur_thread%3];
	cur_thread%=3;
	switch_stack(&previous->stack_ptr, &current->stack_ptr);

	//switch_stack(&threads[cur_thread%3]->stack_ptr, &threads[(cur_thread+1)%3]->stack_ptr);
}