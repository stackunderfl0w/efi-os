#pragma once
#include "typedef.h"
#include "memory.h"

//char * strstr (const char *haystack, const char *needle);
char *strstr(char* string, char* substring);
char* strrstr(char* haystack, char* needle);
int strcmp(const char* str1, const char* str2);
char** split_string_by_char(char* str, char seperator, int *section_count);

uint64_t strlen(char* str);
char* strcpy(char* dest, const char* src);
char* memmove(char* dest, const char* src, uint64_t len);
