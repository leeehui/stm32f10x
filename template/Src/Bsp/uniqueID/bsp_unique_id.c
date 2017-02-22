

#include "bsp_unique_id.h"

 uint32_t flash_end_addr = 0;

void get_unique_id(uint32_t* id)
{
	*id = *(volatile uint32_t*)(UNIQUE_ID_BASE);
	*(id+1) = *(volatile uint32_t*)(UNIQUE_ID_BASE+4);
	*(id+2) = *(volatile uint32_t*)(UNIQUE_ID_BASE+8);
}

//void get

void update_flash_end_addr(void)
{
	uint32_t flash_size = *(volatile uint16_t*)(FLASH_SIZE_BASE);
	flash_size = flash_size << 10;
	flash_end_addr = 0x08000000 + flash_size - 1;
	printf("flash_end_addr: 0x%x\n", flash_end_addr);
}

void set_bus_addr(uint16_t addr)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
	uint32_t page_addr = flash_end_addr - PAGE_SIZE + 1;//according to datasheet, pagesize
	FLASH_Status s = FLASH_ErasePage(page_addr);
	FLASH_ProgramHalfWord(flash_end_addr-1, addr);
}

uint16_t get_bus_addr(void)
{
	return *(volatile uint16_t*)(flash_end_addr-1);
}


void flash_test(void)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
	uint32_t page_addr = flash_end_addr - (2<<10) + 1;
	FLASH_Status s = FLASH_ErasePage(page_addr);
	FLASH_ProgramHalfWord(0x0807F800, 0x4321);
	printf("data:0x%x\n", *(volatile uint16_t*)(0x0807F800));
}