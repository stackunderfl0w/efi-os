#pragma once

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef unsigned int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int UINT32;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef unsigned long long uintptr_t;

typedef signed long long ssize_t;
typedef unsigned long long size_t;

typedef short wchar;
typedef _Bool bool;
#define true 1
#define false 0

#define NULL 0

typedef uint64_t		EFI_PHYSICAL_ADDRESS;
typedef uint64_t		EFI_VIRTUAL_ADDRESS;

#define ROUND_4K(x) ((x+4095)&~0xfff)

#define incptr(p, n) ((void *)(((uintptr_t)(p)) + (n)))


void* memset (void *dest, int val, size_t len);
void* memcpy (void *dest, const void *src, size_t len);
wchar* wmemset (wchar *s, wchar c, size_t n);
void* intset(uint32_t *s, const uint32_t c, size_t n);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
uint32_t crc32b(const void* data, size_t length);

int memcmp (const void* str1, const void* str2, size_t count);
void* memmove (void *dest, const void *src, size_t n);
