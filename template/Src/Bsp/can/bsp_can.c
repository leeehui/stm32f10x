
#include "stm32_includes.h"


CanTxMsg TxMessage;
uint16_t  std_id = 0x321;
uint8_t KeyNumber = 0;



void send_sync_canbus(uint8_t *sync_data)
{
	TxMessage.Data[0] = sync_data[0];
	TxMessage.Data[1] = sync_data[1];
	TxMessage.Data[2] = sync_data[2];
	TxMessage.Data[3] = sync_data[3];
	TxMessage.Data[4] = sync_data[4];
	TxMessage.Data[5] = sync_data[5];
	TxMessage.Data[6] = sync_data[6];
	TxMessage.Data[7] = sync_data[7];
	CAN_Transmit(CANx, &TxMessage);
}


void can_demo(void)
{
	while(1)
	{
		while(GPIO_ReadInputDataBit(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN) == KEY_PRESSED)
		{
			if(KeyNumber == 0x4) 
			{
				KeyNumber = 0x00;
			}
			else
			{
				printf("KeyNumber:%d\n",++KeyNumber);
				TxMessage.Data[0] = KeyNumber;
				CAN_Transmit(CANx, &TxMessage);
				Delay();      
				while(GPIO_ReadInputDataBit(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN) != KEY_NOT_PRESSED)
				{
				}
			}
		}
	}
}


/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void BSP_CAN_BASE_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;

  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
#ifdef  __CAN1_USED__
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN1, ENABLE);
#else /*__CAN2_USED__*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN2, ENABLE);
#endif  /* __CAN1_USED__ */
  /* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_Remapping_CAN , ENABLE);
  
  /* CANx Periph clock enable */
#ifdef  __CAN1_USED__
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#else /*__CAN2_USED__*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
#endif  /* __CAN1_USED__ */
  
  
  /* CAN register init */
  CAN_DeInit(CANx);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 1MBps*/
	//SJW 1-4
	//BS1 1-16
	//BS2 1-8
	//APB1 = 36M
	//CAN Clock = 36/4 = 9M
	//CAN Boad = 9/(SJW+BS1+BS2) = 9/(1+3+5) = 1M
	//CAN Boad = 9/(SJW+BS1+BS2) = 9/(2+10+6) = 512K
	//CAN Clock = 36/18 = 2M	
	//CAN Boad = 2/(SJW+BS1+BS2) = 2/(1+3+4) = 512K

	//CAN Clock = 36/9 = 4M
	//CAN Boad = 4/(SJW+BS1+BS2) = 4/(2+8+6) = 256K

	//CAN Clock = 36/12 = 3M
	//CAN Boad = 3/(SJW+BS1+BS2) = 3/(2+14+8) = 128K

	//CAN Clock = 36/18 = 2M	
	//CAN Boad = 2/(SJW+BS1+BS2) = 2/(2+8+6) = 128K
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
  CAN_InitStructure.CAN_Prescaler = 4;
  CAN_Init(CANx, &CAN_InitStructure);

  /* CAN filter init */
#ifdef  __CAN1_USED__
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else /*__CAN2_USED__*/
  CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif  /* __CAN1_USED__ */
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0;//std_id<<5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x7FF<<5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);

  /* Transmit */
  TxMessage.StdId = 0x321;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 1;
}


/**
  * @brief  Configures the NVIC for CAN.
  * @param  None
  * @retval None
  */
void BSP_CAN_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
#ifndef STM32F10X_CL
#ifdef  __CAN1_USED__
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
#else  /*__CAN2_USED__*/
  /* CAN2 is not implemented in the device */
   #error "CAN2 is implemented only in Connectivity line devices"
#endif /*__CAN1_USED__*/
#else
#ifdef  __CAN1_USED__ 
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#else  /*__CAN2_USED__*/
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
#endif /*__CAN1_USED__*/

#endif /* STM32F10X_CL*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  

}


