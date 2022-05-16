#include "scheduler.h"
#include "thread.h"
#include "loop.h"
#include "pit.h"
#include "ring_buffer.h"
thread *current, *previous;
thread *dummy_stack_ptr;
bool scheduler_inited=false;
ring_buffer ring;
//ring_buffer_command threads;
extern void yield();
void thread_function(){
	//yield();
	int thread_id = current->tid;
	char arr[16]={'T','h','r','e','a','d',' ','0',10,0};
	arr[7]='0'+thread_id;
	while(1){
		printf(arr);
		//push_ring_buffer_string(&ring,arr);
		//yield();
	}
}
void t3(){
	char str[128];
	uint32_t x,y;
	while(1){
		//ring_buffer_request_space(&ring,100);
		//command cmd;
		//cmd.cmd=command_save_location;
		//push_ring_buffer(&ring, *(void**)&cmd);
		//asm("cli");
		get_cursor_pos(&x, &y);
		//cmd.cmd=command_jump_location;
		//cmd.x=20;
		//cmd.y=0;
		//push_ring_buffer(&ring, *(void**)&cmd);

		move_cursor(30, 0);

		//printf("%u\n",ring.free);
		//if(ring.free>5000){
		//	asm("cli");
		//	loop();
		//}
		//cmd.cmd=command_deletechar;

		for (int i = 0; i < 10; ++i){
			deletechar();
			//push_ring_buffer(&ring, *(void**)&cmd);
		}
		

		uint64_t time=(uint64_t)(TimeSinceBoot*100);
		//cmd.cmd=command_deletechar;

		printf("%u %u %u",time,x,y);
		//printf(str);
		//push_ring_buffer_string(&ring, str);
		//move_cursor(x, y);
		//cmd.cmd=command_return_location;
		//push_ring_buffer(&ring, *(void**)&cmd);
		//asm("sti");

		sleep(1);
	}
}
void print_thread(){
	uint32_t x=0,y=0;
	while (1){
		while(!ring.empty){
			void* tmp = pop_ring_buffer(&ring);
			command cmd=(*(command*)&tmp);

			switch(cmd.cmd){
				case command_printchar:
					printchar(cmd.character);
					break;
				case command_save_location:
					get_cursor_pos(&x, &y);
					break;
				case command_jump_location:
					move_cursor(cmd.x,cmd.y);
					break;
				case command_return_location:
					move_cursor(x,y);
					break;

			}

		}
		//yield();
		//outer while loop seems to be optimized out if there is nothing here
		busyloop(0);
	}
}
thread *new_threads[3];
int num_threads=3;
uint64_t cur_thread=0;


thread* int_thread;
void start_scheduler(){
	printf("start_scheduler\n");
	new_threads[0]=new_thread(thread_function);
	new_threads[1]=new_thread(t3);
	new_threads[2]=new_thread(print_thread);
	//new_threads[3]=new_thread(thread_function);

	ring=new_cmd_buf(1000);

	scheduler_inited=true;
	asm ("sti");
	loop();

}

bool first=true;
void* get_next_thread(void *stack_ptr){
	if(!scheduler_inited)
		return stack_ptr;
	//return new_threads[0]->RSP;
	if (first){
		first=false;
		current=new_threads[0];
		cur_thread=0;
		return current->RSP;
	}
	current->RSP=stack_ptr;
	previous=new_threads[cur_thread%num_threads];
	cur_thread++;
	current=new_threads[cur_thread%num_threads];
	cur_thread%=num_threads;
	return current->RSP;
}