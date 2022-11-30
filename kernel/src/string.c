#include "string.h"
#include "memory.h"

char *strchr(const char *s, int c){
	for (;*s; ++s) {
		if (*s == c)
			return ((char *)s);
	}
	return NULL;
}
char* strstr(char* string, char* substring){
	char *a, *b;
	b = substring;
	if (*b == 0) {
		return string;
	}
	for ( ; *string != 0; string += 1){
	if (*string != *b){
		continue;
	}
	a = string;
	while (1){
		if (*b == 0){
			return string;
		}
		if (*a++ != *b++){
			break;
		}
	}
	b = substring;
	}
	return NULL;
}
char* strrstr(char* haystack, char* needle){
	if (*needle == '\0')
		return (char *) haystack;

	char *result = NULL;
	for (;;) {
		char *p = (char*)(strstr(haystack, needle));
		if (p == NULL)
			break;
		result = p;
		haystack = p + 1;
	}
	return result;
}
int strcmp(const char* str1, const char* str2){
	int index=0;
	while(str1[index]==str2[index]&&str1[index]!=0){
		index++;
	}
	return str1[index]-str2[index];
}
int strcasecmp(const char* str1, const char* str2){
	int index=0;
	while((str1[index]|32)==(str2[index]|32)&&str1[index]!=0){
		index++;
	}
	return str1[index]-str2[index];
}
char** split_string_by_char(char* str, char seperator, int *section_count){
	int index=0;
	int n_index=0;

	int n_count=0;
	while(str[index]){
		if(str[index++]==seperator){
			n_count++;
		}
	}
	
	index=0;
	if(str[0]!=seperator){
		n_count++;
	}
	else{
		index++;
	}
	//kprintf("1:%u 2:%u 3:%u",(uint64_t)n_count,sizeof(char*),strlen(str));
	//kprintf("bytes used: %u\n",((uint64_t)n_count*sizeof(char*)+strlen(str))+1);

	char** sections=calloc(n_count*sizeof(char*)+strlen(str)+1);
	char* start = ((char*)sections)+(n_count*sizeof(char*));

	*section_count=n_count;
	sections[0]=&start[index];

	while(str[index]){
		start[index]=str[index];
		if(str[index]==seperator){
			start[index]=0;
			sections[++n_index]=start+index+1;
		}
		index++;
		
	}
	return sections;
}
uint64_t strlen(char* str){
	uint64_t index=0;
	while(str[index++]);
	return index-1;
}

char* strcpy(char* dest, const char* src){
	char* d=dest;
	while (*src){
		*dest++ = *src++;
	}
	*dest = 0;
	return d;
}
char* strncpy(char* dest, const char* src, size_t n){
	char*d=dest;
	while(n-->0){
		if((*d++=*src++)==0){
			//reached end, fill rest with null;
			while(n--){
				*d++=0;
			}
		}
	}
	return dest;
}