

#ifndef BSP_CAN__
#define BSP_CAN__

#include "stm32f10x.h"


/** @addtogroup CAN_Networking
  * @{
  */ 

#define RCC_APB2Periph_GPIO_CAN1    RCC_APB2Periph_GPIOB
#define GPIO_Remapping_CAN1         GPIO_Remap1_CAN1
#define GPIO_CAN1                   GPIOB
#define GPIO_Pin_CAN1_RX            GPIO_Pin_8
#define GPIO_Pin_CAN1_TX            GPIO_Pin_9

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define __CAN1_USED__
/* #define __CAN2_USED__*/

#ifdef  __CAN1_USED__
  #define CANx                       CAN1
  #define GPIO_CAN                   GPIO_CAN1
  #define GPIO_Remapping_CAN         GPIO_Remapping_CAN1
  #define GPIO_CAN                   GPIO_CAN1
  #define GPIO_Pin_CAN_RX            GPIO_Pin_CAN1_RX
  #define GPIO_Pin_CAN_TX            GPIO_Pin_CAN1_TX
#else /*__CAN2_USED__*/
  #define CANx                       CAN2
  #define GPIO_CAN                   GPIO_CAN2
  #define GPIO_Remapping_CAN             GPIO_Remap_CAN2
  #define GPIO_CAN                   GPIO_CAN2
  #define GPIO_Pin_CAN_RX            GPIO_Pin_CAN2_RX
  #define GPIO_Pin_CAN_TX            GPIO_Pin_CAN2_TX
#endif  /* __CAN1_USED__ */


void can_demo(void);
void BSP_CAN_BASE_Config(void);
void BSP_CAN_NVIC_Config(void);
void send_sync_canbus(uint8_t *sync_data);



#endif
