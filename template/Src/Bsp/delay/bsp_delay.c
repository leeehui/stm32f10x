

#include "stm32_includes.h"




/**
  * @brief  Delay
  * @param  None
  * @retval None
  */
void Delay(void)
{
  uint16_t nTime = 0x0000;

  for(nTime = 0; nTime <0xFFF; nTime++)
  {
  }
}

void delay_ms(uint32_t delay)
{
	for(int i=0; i<delay; i++)
	{
		for(int j=0; j<13888; j++)
		{}
	}
}







