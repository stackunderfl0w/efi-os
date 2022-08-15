#include "typedef.h"
static inline int isupper(int c){
	return (uint32_t)c-'A'<26;
}
static inline int islower(int c){
	return (uint32_t)c-'a'<26;
}
static inline int isalpha(int c){
	return ((uint32_t)c|32)-'a' < 26;
}
static inline int isprint(int c){
	return((c >= 0x20 && c <= 0x7e));
}
static inline int isdigit(int c){
	return((c >= '0' && c <= '9'));
}