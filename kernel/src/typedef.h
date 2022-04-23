#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned char CHAR8;
typedef unsigned char uint8_t;

typedef unsigned short CHAR16;
typedef unsigned short uint16_t;

typedef unsigned int uint32_t;
typedef unsigned int UINT32;

typedef unsigned long long UINTN;
typedef unsigned long long UINT64;
typedef signed long long ssize_t;


typedef UINT64			EFI_PHYSICAL_ADDRESS;
typedef UINT64			EFI_VIRTUAL_ADDRESS;


void *
memset (void *dest, int val, size_t len);
void*
memcpy (void *dest, const void *src, size_t len);
wchar_t *
wmemset (wchar_t *s, wchar_t c, size_t n);
void* intset(uint32_t *s, const uint32_t c, size_t n);
//char * strstr (const char *haystack, const char *needle);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
uint32_t crc32b(const void* data, size_t length);
