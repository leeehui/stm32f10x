
#include "stm32_includes.h"



void KEY_BASE_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable the BUTTON Clock */
	RCC_APB2PeriphClockCmd(KEY_BUTTON_GPIO_CLK, ENABLE);

	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = KEY_BUTTON_PIN;
	GPIO_Init(KEY_BUTTON_GPIO_PORT, &GPIO_InitStructure);

}





