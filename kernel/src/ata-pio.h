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



void atapio_wait_for_rdy(uint16_t DCR){
	uint8_t stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	do
	{
		stsbyte = inb(DCR);
	} while (stsbyte_ptr->BSY && !stsbyte_ptr->RDY);
}

void atapio_wait_for_drq(uint16_t DCR){
	uint8_t stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	do
	{
		stsbyte = inb(DCR);
	} while (!stsbyte_ptr->DRQ);
}
void atapio_software_reset(uint16_t DCR){
	int i;
	outb(DCR, 0x04);
	outb(DCR, 0x00);
	for (i = 0; i > 0; i++)
	{
		inb(DCR);
	}
	atapio_wait_for_rdy(DCR);
}


void atapio_read_sectors(uint32_t lba, char sector_count, uint8_t *buffer){
	outb(ATAPIO_ERR_INFO_PORT, 0x00);
	outb(ATAPIO_SECTOR_COUNT_REGISTER_PORT, (uint8_t)sector_count);
	outb(ATAPIO_LBA_LOW_REGISTER_PORT, (uint8_t)lba);
	outb(ATAPIO_LBA_MID_REGISTER_PORT, (uint8_t)(lba >> 8));
	outb(ATAPIO_LBA_HIGH_REGISTER_PORT, (uint8_t)(lba >> 16));
	// master drive + lba + high lba bits
	outb(ATAPIO_DRIVE_REGISTER_PORT, 0xE0 | 0x40 | ((lba >> 24) & 0x0F));
	outb(ATAPIO_COMMAND_REGISTER_PORT, ATAPIO_READ); //read command
	uint32_t offset = 0, i = 0;
	while (sector_count--){
		atapio_wait_for_rdy(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		// Spun up
		atapio_wait_for_drq(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		// Ready for transfer
		for (i = 0; i < 256; i++)
		{
			*((uint16_t *)buffer + offset + i) = inw(ATAPIO_DATA_REGISTER_PORT);
		}
		offset += 256;
	}
	return;
}

/**
 * @brief ATA write sectors
 * 
 * @param lba linear block addres
 * @param sector_count 
 * @param buffer 
 */
void atapio_write_sectors(int lba, char sector_count, char *buffer){
	outb(ATAPIO_ERR_INFO_PORT, 0x00);
	outb(ATAPIO_ERR_INFO_PORT, 0x00);
	outb(ATAPIO_SECTOR_COUNT_REGISTER_PORT, (uint8_t)sector_count);
	outb(ATAPIO_LBA_LOW_REGISTER_PORT, (uint8_t)lba);
	outb(ATAPIO_LBA_MID_REGISTER_PORT, (uint8_t)(lba >> 8));
	outb(ATAPIO_LBA_HIGH_REGISTER_PORT, (uint8_t)(lba >> 16));
	// master drive + lbal + high lba bits
	outb(ATAPIO_DRIVE_REGISTER_PORT, 0xE0 | 0x40 | ((lba >> 24) & 0x0F));
	outb(ATAPIO_COMMAND_REGISTER_PORT, ATAPIO_WRITE);
	uint32_t offset = 0, i = 0;
	while (sector_count--){
		atapio_wait_for_rdy(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		atapio_wait_for_drq(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		for (i = 0; i < 256; outw(ATAPIO_DATA_REGISTER_PORT, *((uint16_t *)buffer + offset + i++)))
			;
		offset += 256;
	}
	return;
}

void atapio_irq_handler(){
	//PIC_sendEOI(ATAPIO_IRQ_LINE);
	PIC_EndSlave();
}

void atapio_io_delay(){
	uint32_t i;
	for (i = 0; i < 10000; i++)
		io_wait();
}