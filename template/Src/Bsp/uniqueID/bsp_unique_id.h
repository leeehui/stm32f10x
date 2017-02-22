#ifndef BSP_UNIQUE_ID__
#define BSP_UNIQUE_ID__

#include "stm32_includes.h"

#define UNIQUE_ID_BASE 0x1FFFF7E8
//#define UNIQUE_ID_BASE 0x1FFF7A10

#define FLASH_SIZE_BASE 0x1FFFF7E0

#define UNIQUE_ADDR_OFFSET 1   //flash_end_addr-1  to  flash_end_addr


//����STM32F10xϵ�У���ͬ������оƬ��flash page �Ĵ�С��ͬ����������ĺ����ж�
//����1��������һЩоƬ������Flash Size���ͺű�ע�Ĵ�С��ͬ����ȷ��Ӧ�����ĸ���׼��
//����STM32F4ϵ�У�page�ĳ���Sector������ÿ��sector�ĸ����һ��������������
//ͬ�����Ը��ݲ�ͬ��оƬ�ͺţ�ȷ�����һ��sector
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

