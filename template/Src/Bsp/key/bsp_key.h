#ifndef BSP_KEY__
#define BSP_KEY__

#include "stm32_includes.h"

#define KEY_BUTTON_PIN                   GPIO_Pin_15
#define KEY_BUTTON_GPIO_PORT             GPIOB
#define KEY_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOB
#define KEY_BUTTON_EXTI_LINE             EXTI_Line15
#define KEY_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOB
#define KEY_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource15
#define KEY_BUTTON_EXTI_IRQn             EXTI15_10_IRQn


#define KEY_PRESSED     0x01
#define KEY_NOT_PRESSED 0x00



void KEY_BASE_Config(void);


#endif
