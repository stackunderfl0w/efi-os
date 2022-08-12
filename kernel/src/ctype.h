#include "typedef.h"
static inline bool isupper(char c){
	return((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
static inline bool islower(char c){
	return((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
static inline bool isalpha(char c){
	return ((unsigned char)c|32)-'a' < 26;
}
static inline bool isprint(char c){
	return((c >= 0x20 && c <= 0x7e));
}
static inline bool isdigit(char c){
	return((c >= '0' && c <= '9'));
}