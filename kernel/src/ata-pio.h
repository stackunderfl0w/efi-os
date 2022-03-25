#pragma once
#include "io.h"
// ATA bus I/O ports
#define ATAPIO_DATA_REGISTER_PORT 0x1F0
#define ATAPIO_ERR_INFO_PORT 0x1F0
#define ATAPIO_SECTOR_COUNT_REGISTER_PORT 0x1F2
#define ATAPIO_LBA_LOW_REGISTER_PORT 0x1F3
#define ATAPIO_LBA_MID_REGISTER_PORT 0x1F4
#define ATAPIO_LBA_HIGH_REGISTER_PORT 0x1F5
#define ATAPIO_DRIVE_REGISTER_PORT 0x1F6
#define ATAPIO_COMMAND_REGISTER_PORT 0x1F7
#define ATAPIO_REGULAR_STATUS_REGISTER_PORT 0x1F7
// ATA commands
#define ATAPIO_IDENTIFY_DEVICE 0xEC
#define ATAPIO_READ 0x20
#define ATAPIO_WRITE 0x30
// ATA IRQ Line
#define ATAPIO_IRQ_LINE PIC1_OFFSET + 0x0E

//ATA status bits
#define ATAPIO_STATUS_BSY 0x80
#define ATAPIO_STATUS_RDY 0x40
#define ATAPIO_STATUS_DRQ 0x08
#define ATAPIO_STATUS_DF 0x20
#define ATAPIO_STATUS_ERR 0x01

void atapio_read_sectors(uint32_t lba, char sector_count, uint8_t *buffer);

void atapio_write_sectors(uint32_t lba, char sector_count, uint8_t *buffer);

void atapio_irq_handler();

void atapio_io_delay();
