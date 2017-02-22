#ifndef BSP_UNIQUE_ID__
#define BSP_UNIQUE_ID__

#include "stm32_includes.h"

#define UNIQUE_ID_BASE 0x1FFFF7E8
//#define UNIQUE_ID_BASE 0x1FFF7A10

#define FLASH_SIZE_BASE 0x1FFFF7E0

#define UNIQUE_ADDR_OFFSET 1   //flash_end_addr-1  to  flash_end_addr


//对于STM32F10x系列，不同容量的芯片，flash page 的大小不同，根据下面的宏来判断
//问题1，对于有一些芯片读到的Flash Size和型号标注的大小不同，不确定应该用哪个标准？
//对于STM32F4系列，page改成了Sector，并且每个sector的概念都不一样，针对这种情况
//同样可以根据不同的芯片型号，确定最后一个sector
#if defined(STM32F10X_MD) || defined(STM32F10X_LD)
	#define PAGE_SIZE  (1<<10)
#elif defined(STM32F10X_HD) || defined(STM32F10X_CL) || defined(STM32F10X_XL)
	#define PAGE_SIZE  (2<<10)
#endif

void get_unique_id(uint32_t* id);

void update_flash_end_addr(void);

void set_bus_addr(uint16_t addr);

uint16_t get_bus_addr(void);

void flash_test(void);

#endif

