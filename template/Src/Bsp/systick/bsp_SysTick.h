#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

//�ڵδ�ʱ���жϷ������е���
void TimingDelay_Decrement(void);

// ��ʼ��ϵͳ�δ�ʱ��
void SysTick_Init(void);


//�ṩ��Ӧ�ó������
void Delay_10ms(__IO u32 nTime);

#endif /* __SYSTICK_H */