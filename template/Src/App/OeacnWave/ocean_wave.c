
#include "stm32_includes.h"

static uint8_t module_addr = 0;
uint16_t led_changing_ms = 0;
uint8_t delta_green  = 0;
uint8_t delta_red  = 0;
uint8_t delta_blue  = 0;
uint8_t current_green  = 0;
uint8_t current_red  = 0;
uint8_t current_blue  = 0;




#define CMD_DATA 1
#define CMD_SYNC 2




void addrs_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	
	RCC_APB2PeriphClockCmd(ADDR0_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR0_PIN;
	GPIO_Init(ADDR0_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(ADDR1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR1_PIN;
	GPIO_Init(ADDR1_GPIO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(ADDR2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR2_PIN;
	GPIO_Init(ADDR2_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(ADDR3_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR3_PIN;
	GPIO_Init(ADDR3_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(ADDR4_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR4_PIN;
	GPIO_Init(ADDR4_GPIO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(ADDR5_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADDR5_PIN;
	GPIO_Init(ADDR5_GPIO_PORT, &GPIO_InitStructure);		
	
}

//check module_addr after several(10...) minutes
void get_module_addr(void)
{
	uint8_t addr=0;
	addr |= GPIO_ReadInputDataBit(ADDR0_GPIO_PORT, ADDR0_PIN);
	addr |= GPIO_ReadInputDataBit(ADDR1_GPIO_PORT, ADDR1_PIN) << 1;
	addr |= GPIO_ReadInputDataBit(ADDR2_GPIO_PORT, ADDR2_PIN) << 2;
	addr |= GPIO_ReadInputDataBit(ADDR3_GPIO_PORT, ADDR3_PIN) << 3;
	addr |= GPIO_ReadInputDataBit(ADDR4_GPIO_PORT, ADDR4_PIN) << 4;
	addr |= GPIO_ReadInputDataBit(ADDR5_GPIO_PORT, ADDR5_PIN) << 5;
	module_addr = addr;
}


//0 module_addr    
//1 cmd            1 data, 2 check alive
//2 green          
//3 red            
//4 blue           
//5 timeH          
//6 timeL          
//7 XOR            
void process_received_data(uint8_t *data_buffer)
{
	if(data_buffer[0] == module_addr)
	{
		if(data_buffer[1] == CMD_DATA)
		{
			send_sync_canbus(data_buffer);
		}
		else if (data_buffer[1] == CMD_SYNC)
		{
			uint16_t time = (((uint16_t)data_buffer[5]) << 8) + (uint16_t)data_buffer[6];
			led_changing_ms = time;
			if(led_changing_ms > 0)
			{
				//calculate delta grb value
				
			}
			//start timer3
			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

		}
	}
}

void led_changing_once(void)
{
	current_green += delta_green;
	current_red += delta_red;
	current_blue += delta_green;
	ws2812_update_same_grb_armcc(current_green, current_red, current_blue, LED_NUM); 
	if((--led_changing_ms) == 0)
	{
		//disable timer
		TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	}
}

