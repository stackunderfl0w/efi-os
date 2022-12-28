#include "memory.h"
extern graphics_context* global_context;

HEAP_SEG_HEADER* last_hdr;
void* heap_start;
void* heap_end;
void SPLIT_HEAP_SEGMENT(HEAP_SEG_HEADER* seg, uint64_t length){
	HEAP_SEG_HEADER* n_hdr=(HEAP_SEG_HEADER*)((void*)(seg+1)+length);
	n_hdr->next_hdr=seg->next_hdr;
	seg->next_hdr=n_hdr;
	n_hdr->previous_hdr=seg;
	n_hdr->len=seg->len-length-sizeof(HEAP_SEG_HEADER);
	seg->len=length;
	n_hdr->free=true;	
	if(n_hdr->next_hdr!=NULL){
		n_hdr->next_hdr->previous_hdr=n_hdr;
	}
	if(seg==last_hdr){
		last_hdr=n_hdr;
	}
}
void JOIN_HEAP_NEXT_SEGMENT(HEAP_SEG_HEADER* seg){
	HEAP_SEG_HEADER* n_hdr=seg->next_hdr;
	if(n_hdr->next_hdr!=NULL){
		n_hdr->next_hdr->previous_hdr=seg;
	}
	seg->next_hdr=n_hdr->next_hdr;
	seg->len+=n_hdr->len+sizeof(HEAP_SEG_HEADER);
	if(n_hdr->next_hdr==NULL){
		last_hdr=seg;
	}
}
void EXPAND_HEAP(uint64_t size){
	size=(size/0x1000);
	for (int i = 0; i < size; ++i){
		void* tmp=REQUEST_PAGE();
		map_mem(heap_end+(0x1000*i),tmp);
	}
	if (last_hdr->free){
		last_hdr->len+=0x1000*size;
	}
	else{
		print(global_context,"err");
	}
	heap_end+=0x1000*size;
}
void INIT_HEAP(void* adr, uint64_t pages){
	request_mapped_pages(adr,4096*pages);
	heap_start=adr;
	heap_end=adr+(4096*pages);
	last_hdr=adr;
	last_hdr->next_hdr=NULL;
	last_hdr->previous_hdr=NULL;
	last_hdr->free=true;
	last_hdr->len=(4096*pages)-sizeof(HEAP_SEG_HEADER);
}
void* kmalloc(uint64_t size){
	if(!size)
		return NULL;
	HEAP_SEG_HEADER* cur=heap_start;
	while(true){
		if(cur->free){
			if(cur->len>size+sizeof(HEAP_SEG_HEADER)){
				SPLIT_HEAP_SEGMENT(cur,size);
				cur->free=false;
				return (void*)(cur+1);
			}
		}
		if(cur->next_hdr==NULL){
			break;
		}
		cur=cur->next_hdr;
	}
	EXPAND_HEAP(size);
	return kmalloc(size);
}
void* kcalloc(uint64_t size){
	void* adr=kmalloc(size);
	memset (adr,0,size);
	return adr;
}
void free(void* adr){
	HEAP_SEG_HEADER* cur=((HEAP_SEG_HEADER*)adr)-1;
	cur->free=true;
	while(cur->next_hdr!=NULL && cur->next_hdr->free){
		JOIN_HEAP_NEXT_SEGMENT(cur);
	}
	while(cur->previous_hdr!=NULL && cur->previous_hdr->free ){
		cur=cur->previous_hdr;
		JOIN_HEAP_NEXT_SEGMENT(cur);
	}
}
void* krealloc(void* adr, uint64_t size){
	char* loc = kcalloc(size);
	HEAP_SEG_HEADER* cur=((HEAP_SEG_HEADER*)adr)-1;
	memcpy(loc,adr,MIN(size,cur->len));
	free(adr);
	return loc;
}