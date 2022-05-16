#pragma once
#include "typedef.h"
#include "graphics.h"
typedef struct{
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed))RSDPDescriptor;

typedef struct{
 RSDPDescriptor firstPart;
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed))RSDPDescriptor20;


void detect_cores(uint8_t *rsdt);
