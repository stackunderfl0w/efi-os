#include "sorted_list.h"
#include "memory.h"
#include "stdio.h"
#define VECTOR_BASE_CAPACITY 1

sorted_list* create_sorted_list(int (*element_cmp)(void*,void*),int (*search_cmp)(void*,void*)){
	sorted_list* l= kmalloc(sizeof(sorted_list));
	l->capacity=VECTOR_BASE_CAPACITY;
	l->data=(kmalloc(VECTOR_BASE_CAPACITY * sizeof(void*)));
	l->cmp=element_cmp;
	l->search_cmp=search_cmp;
	l->size=0;
	return l;
}
void sorted_list_free(sorted_list* l) {
	free(l->data);
	free(l);
}

void resize_sorted_list(sorted_list* l, uint64_t new_size){
	l->data=krealloc(l->data,sizeof(void*)*new_size);
	l->capacity=new_size;
}

uint64_t sorted_list_search(sorted_list* l, void* search){
	uint64_t min=0,max=l->size,c_idx=min+max/2;
	while (min!=max){
		int x=l->search_cmp(l->data[c_idx],search);
		if(!x)
			return c_idx;
		else if(x<0)
			min=c_idx+1;
		else
			max=c_idx;
		c_idx=(min+max)/2;
	}
	return min;
}

void sorted_list_insert(sorted_list* l, void* element){
	if(l->capacity==(l->size+1)){
		resize_sorted_list(l,l->capacity*2);
	}
	if(l->size==0){
		l->data[l->size++]=element;
		return;
	}
	uint64_t c_idx= sorted_list_search(l,element);
	memmove(&l->data[c_idx]+1,&l->data[c_idx],(sizeof(void*))*(l->size-c_idx));
	l->data[c_idx]=element;
	l->size++;
}
void sorted_list_remove(sorted_list *l, void *element) {
	uint64_t c_idx= sorted_list_search(l,element);
	memmove(&l->data[c_idx],&l->data[c_idx]+1,(sizeof(void*))*(l->size-c_idx));
	l->data[--l->size]=0;
}

void* sorted_list_get(sorted_list* l, void* search){
	uint64_t idx= sorted_list_search(l, search);
	return !l->search_cmp(l->data[idx],search)?l->data[idx]:NULL;
}

