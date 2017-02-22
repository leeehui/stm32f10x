/**
  ******************************************************************************
  * @file    CAN/Networking/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "stm32_includes.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */


#define KEY_PRESSED     0x01
#define KEY_NOT_PRESSED 0x00

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void Init_RxMes(CanRxMsg *RxMessage);
void Delay(void);
void delay_ms(uint32_t delay);


/* Private functions ---------------------------------------------------------*/


void RCC_Configuration(void)
{
ErrorStatus HSEStartUpStatus;
//??? RCC?????????
RCC_DeInit();
RCC_HSICmd(ENABLE);
while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
{
}
if(1)
{
FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
FLASH_SetLatency(FLASH_Latency_2);
RCC_HCLKConfig(RCC_SYSCLK_Div1);
RCC_PCLK2Config(RCC_HCLK_Div1);
RCC_PCLK1Config(RCC_HCLK_Div2);
//?? PLL ????????
RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
//?????? PLL,???????:ENABLE??DISABLE
RCC_PLLCmd(ENABLE);//??PLL???????,????????
//????? RCC ??????? ??PLL?????
while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
{
}
//??????(SYSCLK) ??PLL??????
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//??PLL?????????????
// 0x00:HSI ??????
// 0x04:HSE??????
// 0x08:PLL??????
while(RCC_GetSYSCLKSource() != 0x08)
{
}
}
}



/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	//RCC_Configuration();
  delay_ms(500);
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */   

	//CO_TIMConfigTest();
	
	
	KEY_BASE_Config();
	USART1_BASE_Config();	
	BSP_CAN_NVIC_Config();
	BSP_CAN_BASE_Config();		
	ws2812_config_gpio();
	BSP_USART2_Init();

	update_flash_end_addr();
	
	

	set_bus_addr(0xab);
	uint16_t addr = get_bus_addr();
	printf("addr: 0x%x\n", addr);
	
	set_bus_addr(0xabab);
	addr = get_bus_addr();
	printf("addr: 0x%x\n", addr);

	
	uint32_t id[3]={0};
	get_unique_id(id);
	
	printf("USART Printf Example: retarget the C library printf function to the USART\n");
	//led_demo();
	led_demo_ocean_wave();
	//can_demo();
	
	/* Infinite loop */
	while(1)
	{
	}
}






#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif



/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
