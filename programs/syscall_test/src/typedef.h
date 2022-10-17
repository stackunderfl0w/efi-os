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

typedef UINT64			EFI_PHYSICAL_ADDRESS;
typedef UINT64			EFI_VIRTUAL_ADDRESS;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
