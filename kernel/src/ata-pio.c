#include "ata-pio.h"
/*void atapio_wait_for_rdy(uint16_t DCR){
	printf("rdy");
	uint8_t stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	int att=10000;
	do
	{
		//printf("r");
		stsbyte = inb(DCR);
		if(!--att%100){
			printf("r");
		}
	} while (stsbyte_ptr->BSY && !stsbyte_ptr->RDY);
}
//todo actually fix drq checking
void atapio_wait_for_drq(uint16_t DCR){
	printf("drq");
	uint8_t stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	int att=10000;
	do
	{
		//printf("d");
		stsbyte = inb(DCR);
		att--;
		//if(att%100==0){
			//printf("%u\n",att);
			//sleep(50);
		//}
	} while (!stsbyte_ptr->DRQ&&att);
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
/*
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
}*/
void atapio_bsy_wait()   //Wait for bsy to be 0
{
	while(inb(0x1F7)&ATAPIO_STATUS_BSY);
}
void atapio_drq_wait()  //Wait fot drq to be 1
{
	while(!(inb(0x1F7)&ATAPIO_STATUS_RDY));
}
void atapio_read_sectors(uint32_t lba, char sector_count, uint8_t *buffer)
{

	atapio_bsy_wait();
	outb(0x1F6,0xE0 | ((lba >>24) & 0xF));
	outb(0x1F2,sector_count);
	outb(0x1F3, (uint8_t) lba);
	outb(0x1F4, (uint8_t)(lba >> 8));
	outb(0x1F5, (uint8_t)(lba >> 16)); 
	outb(0x1F7,0x20); //Send the read command

	uint16_t *target = (uint16_t*) buffer;
	for (int j =0;j<sector_count;j++)
	{
		atapio_bsy_wait();
		atapio_drq_wait();
		for(int i=0;i<256;i++)
			target[i] = inw(0x1F0);
		target+=256;
	}
}

void atapio_write_sectors(uint32_t lba, char sector_count, uint8_t *buffer)
{
	atapio_bsy_wait();
	outb(0x1F6,0xE0 | ((lba >>24) & 0xF));
	outb(0x1F2,sector_count);
	outb(0x1F3, (uint8_t) lba);
	outb(0x1F4, (uint8_t)(lba >> 8));
	outb(0x1F5, (uint8_t)(lba >> 16)); 
	outb(0x1F7,0x30); //Send the write command

	uint16_t *target = (uint16_t*) buffer;

	for (int j =0;j<sector_count;j++)
	{
		atapio_bsy_wait();
		atapio_drq_wait();
		for(int i=0;i<256;i++)
		{
			outw(0x1F0, target[i+256*j]);
		}
	}
}

