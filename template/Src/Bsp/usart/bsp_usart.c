
#include "stm32_includes.h"


char usart2DmaSendBuffer[20];
char usart2DmaReceiveBuffer[20];


void BSP_USART2_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
  
	/* USARTx configured as follow:
				- BaudRate = 115200 baud  
				- Word Length = 8 Bits
				- One Stop Bit
				- No parity
				- Hardware flow control disabled (RTS and CTS signals)
				- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable GPIO clock */
	//PA2/PA3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable UART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	//USART2 485使能
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOA, GPIO_Pin_4);   //输出高电平，使能发送

	/* USART configuration */	
	USART_Init(USART2, &USART_InitStructure);


	//DMA 传送配置
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  

	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

	/* 配置USART2 DMA  发送通道 */
	DMA_DeInit(DMA1_Channel7);  //参考手册147页可知，USART2的 TX 连接到DMA通道7
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);//配置DMA传送的目标地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&usart2DmaSendBuffer); //配置要传送数据的buffer地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//表示向外设传送，表示发送
	DMA_InitStructure.DMA_BufferSize = 20;//缓冲区buffer的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE); 

 	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
 	NVIC_Init(&NVIC_InitStructure);  
	
	
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

	/* 配置USART2 DMA  发送通道 */
	DMA_DeInit(DMA1_Channel6);  //参考手册147页可知，USART2的 TX 连接到DMA通道7
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);//配置DMA传送的目标地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&usart2DmaReceiveBuffer); //配置要传送数据的buffer地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//表示向外设传送，表示发送
	DMA_InitStructure.DMA_BufferSize = 1;//缓冲区buffer的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE); 
	DMA_Cmd(DMA1_Channel6, ENABLE);

 	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;  
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
 	NVIC_Init(&NVIC_InitStructure);  

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);		
}

//usart2 tx dma
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC7) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC7);
	}
}

//usart2 Rx dma
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC6) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC6);
		printf("receive:0x%x\n", usart2DmaReceiveBuffer[0]);
//			DMA_Cmd(DMA1_Channel3, DISABLE);
//			DMA_SetCurrDataCounter(DMA2_Channel3,	1);
//			DMA_Cmd(DMA2_Channel3, ENABLE);
	}

}




