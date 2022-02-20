#include "typedef.h"
void* memset (void *dest, int val, size_t len){
	unsigned char *ptr = dest;
	while (len-- > 0)
		*ptr++ = val;
	return dest;
}
void* memcpy (void *dest, const void *src, size_t len){
	char *d = dest;
	const char *s = src;
	while (len--)
		*d++ = *s++;
	return dest;
}
wchar_t* wmemset (wchar_t *s, wchar_t c, size_t n){
	wchar_t *wp = s;
	while (n >= 4){
		wp[0] = c;
		wp[1] = c;
		wp[2] = c;
		wp[3] = c;
		wp += 4;
		n -= 4;
	}
	if (n > 0){
		wp[0] = c;
		if (n > 1){
			wp[1] = c;
			if (n > 2)
				wp[2] = c;
		}
	}
	return s;
}
void* intset(uint32_t *s, const uint32_t c, size_t n){
	uint32_t *wp = s;
	while (n>=2){
		wp[0]=c;
		wp[1]=c;
		wp+=2;
		n-=2;
	}
	if (n > 0){
		wp[0] = c;
	}
	return s;
}

const uint32_t Polynomial = 0xEDB88320;
uint32_t crc32b(const void* data, size_t length) {
	uint32_t crc = ~0; // same as previousCrc32 ^ 0xFFFFFFFF
	unsigned char* current = (unsigned char*) data;   
	while (length--){
		crc ^= *current++;
		for (unsigned int j = 0; j < 8; j++){
			if (crc & 1)
				crc = (crc >> 1) ^ Polynomial;
			else
				crc =  crc >> 1;
		}
	}   
	return ~crc; // same as crc ^ 0xFFFFFFFF 
} 