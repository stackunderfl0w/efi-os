#include "ata-pio.h"
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
void atapio_write_sectors(uint32_t lba, char sector_count, char *buffer){
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