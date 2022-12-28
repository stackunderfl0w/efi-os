#include "ata-pio.h"
#include "io.h"

void atapio_bsy_wait(){   //Wait for bsy to be 0
	while(inb(ATAPIO_REGULAR_STATUS_REGISTER_PORT)&ATAPIO_STATUS_BSY);
}
void atapio_drq_wait(){  //Wait fot drq to be 1
	while(!(inb(ATAPIO_REGULAR_STATUS_REGISTER_PORT)&ATAPIO_STATUS_RDY));
}
void atapio_read_sectors(uint32_t lba, char sector_count, uint8_t *buffer){
	atapio_bsy_wait();
	outb(ATAPIO_DRIVE_REGISTER_PORT,0xE0 | ((lba >>24) & 0xF));
	outb(ATAPIO_SECTOR_COUNT_REGISTER_PORT,sector_count);
	outb(ATAPIO_LBA_LOW_REGISTER_PORT, (uint8_t) lba);
	outb(ATAPIO_LBA_MID_REGISTER_PORT, (uint8_t)(lba >> 8));
	outb(ATAPIO_LBA_HIGH_REGISTER_PORT, (uint8_t)(lba >> 16)); 
	outb(ATAPIO_COMMAND_REGISTER_PORT,ATAPIO_READ); //Send the read command

	uint16_t *target = (uint16_t*) buffer;

	for (int j =0;j<sector_count;j++){
		atapio_bsy_wait();
		atapio_drq_wait();
		for(int i=0;i<256;i++)
			target[i+256*j] = inw(ATAPIO_DATA_REGISTER_PORT);
	}
}

void atapio_write_sectors(uint32_t lba, char sector_count, uint8_t *buffer){
	atapio_bsy_wait();
	outb(ATAPIO_DRIVE_REGISTER_PORT,0xE0 | ((lba >>24) & 0xF));
	outb(ATAPIO_SECTOR_COUNT_REGISTER_PORT,sector_count);
	outb(ATAPIO_LBA_LOW_REGISTER_PORT, (uint8_t) lba);
	outb(ATAPIO_LBA_MID_REGISTER_PORT, (uint8_t)(lba >> 8));
	outb(ATAPIO_LBA_HIGH_REGISTER_PORT, (uint8_t)(lba >> 16)); 
	outb(ATAPIO_COMMAND_REGISTER_PORT,ATAPIO_WRITE); //Send the write command

	uint16_t *target = (uint16_t*) buffer;

	for (int j =0;j<sector_count;j++){
		atapio_bsy_wait();
		atapio_drq_wait();
		for(int i=0;i<256;i++)
			outw(ATAPIO_DATA_REGISTER_PORT, target[i+256*j]);
	}
}

