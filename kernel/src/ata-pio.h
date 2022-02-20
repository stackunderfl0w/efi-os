#pragma once
#include "io.h"
#define ATAPIO_SECTOR_SIZE 512
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

struct atapio_status_byte{
	uint8_t ERR : 3; // Indicates an error occurred.
	uint8_t DRQ : 1; // Set when the drive has PIO data to transfer, or is ready to accept PIO data.
	uint8_t SRV : 1; // Overlapped Mode Service Request.
	uint8_t DF : 1;  // Drive Fault Error
	uint8_t RDY : 1; // Bit is clear when drive is spun down, or after an error. Set otherwise.
	uint8_t BSY : 1; // Indicates the drive is preparing to send/receive data
};

typedef struct atapio_status_byte atapio_status_byte_t;

struct atapio_disk_sector{
	uint8_t bytes[32];
};

typedef struct atapio_disk_sector atapio_disk_sector_t;



void atapio_wait_for_rdy(uint16_t DCR);

void atapio_wait_for_drq(uint16_t DCR);
void atapio_software_reset(uint16_t DCR);


void atapio_read_sectors(uint32_t lba, char sector_count, uint8_t *buffer);

void atapio_write_sectors(uint32_t lba, char sector_count, char *buffer);

void atapio_irq_handler();

void atapio_io_delay();