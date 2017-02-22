/*
 * light weight WS2812 lib - ARM Cortex M0/M0+ version
 *
 * Created: 07.07.2013
 *  Author: Tim (cpldcpu@gmail.com)
 */

#include "stm32_includes.h"


uint32_t led_data_buffer_size = LED_NUM * 3;
uint8_t led_data_buffer[LED_NUM * 3]={0};



/*
* The total length of each bit is 1.25µs (25 cycles @ 20Mhz)
* At 0µs the dataline is pulled high.
* To send a zero the dataline is pulled low after 0.375µs
* To send a one the dataline is pulled low after 0.625µs
*/

//delay 1 nanosecond needs (ws2812_cpuclk/1000)/1000000 cpu clcle
#define ws2812_ctot	(((ws2812_cpuclk/1000)*1250)/1000000) //1.25μs = 1250ns
#define ws2812_t1	(((ws2812_cpuclk/1000)*375 )/1000000)	  // floor
#define ws2812_t2	(((ws2812_cpuclk/1000)*625)/1000000)    // ceil

//ZERO
//  T0______T1       T2        T3
//  |        |                 |
//  |        |_________________|

//ONE
//  T0______T1_______T2        T3
//  |                 |        |
//  |                 |________|

//　T0 : set high, wait "w1" cpu cycles
//  T1 : set low if sending zero, keep unchanged if sending one, wait "w2" cycles
//  T2 : set low unconditionally, wait "w3" cycles
//  T3 : T0

//considering the time that occupied by instructions between T0 and T1(T1 and T2, T2 and T3(T0))
//Note: we assume that the performance is 1 cpu cycle per instruction.
//      according to spec, this cpu has 1.25 DMIPS/MHz (Dhrystone 2.1) performance at 0 wait state memory access.
//      so theoretically less than 1 cpu cycle per instruction.
//      when cpu runs faster, there will be less affect to the timing, and the assumption is acceptable for timing
#define w1 (ws2812_t1-2)
#define w2 (ws2812_t2-ws2812_t1-2)
#define w3 (ws2812_ctot-ws2812_t2-5)

#define ws2812_DEL1 nop;
#define ws2812_DEL2 nop; nop;
#define ws2812_DEL4 ws2812_DEL2; ws2812_DEL2
#define ws2812_DEL8 ws2812_DEL4; ws2812_DEL4
#define ws2812_DEL16 ws2812_DEL8; ws2812_DEL8
#define ws2812_DEL32 ws2812_DEL16; ws2812_DEL16
		

void ws2812_update_one_led(uint32_t which_led, uint8_t *grb)
{
	uint32_t buffer_start = which_led * 3;
	led_data_buffer[buffer_start] = *grb;
	led_data_buffer[buffer_start + 1] = *(uint8_t *)(uintptr_t)(grb + 1);
	led_data_buffer[buffer_start + 2] = *(uint8_t *)(uintptr_t)(grb + 2);
}

void ws2812_update_one_led_array(uint32_t which_led, uint8_t green, uint8_t red, uint8_t blue)
{
	uint32_t buffer_start = which_led * 3;
	led_data_buffer[buffer_start] = green;
	led_data_buffer[buffer_start + 1] = red;
	led_data_buffer[buffer_start + 2] = blue;
}

void ws2812_update_num_led(uint32_t led_start, uint32_t led_num, uint8_t *grb)
{
	for(uint32_t counter = led_start; counter < led_num; counter++)
	{
		ws2812_update_one_led(counter, grb);
	}
}

void ws2812_trigger(void)
{	
	ws2812_sendarray_armcc(led_data_buffer, led_data_buffer_size);	
}

void ws2812_config_gpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(WS2812_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = WS2812_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WS2812_PORT, &GPIO_InitStructure);
}

/*
 * Important Note:
 * 1. Do not use optimization (use default or O(0))
 * 2. lsl can not change C flag, use lsls instead, this is why "bcs" can not compiled when using lsl
 */
void ws2812_sendarray_armcc(uint8_t *data,int datlen)
{
uint32_t maskhi = ws2812_mask_set;
uint32_t masklo = ws2812_mask_clr;
volatile uint32_t *set = ws2812_port_set;
volatile uint32_t *clr = ws2812_port_clr;

	uint32_t i;

	uint32_t curbyte;

	__set_PRIMASK(1);
	while (datlen--) {
		curbyte=*data++;
		__ASM {
			lsl curbyte, #24         //curbyte left shift 24bits
			movs i, #8               //set i to 8
			iloop:                   //main loop
			lsls curbyte, #1         //left shift curbyte by #1, subfix "s" tell processor changing the "C" flag
			str maskhi, [set]        //T0(T3) : set output high   
#if (w1&1)                     		 //nop delay
			ws2812_DEL1
#endif
#if (w1&2)
			ws2812_DEL2
#endif
#if (w1&4)
			ws2812_DEL4
#endif
#if (w1&8)
			ws2812_DEL8
#endif
#if (w1&16)
			ws2812_DEL16
#endif
#if (w1&32)
			ws2812_DEL32
#endif
			bcs aaaa                 //skip set output low if MSB is 1, see "lsls" in <<CortexM3ProgrammingManual>>
			str masklo, [clr]        //T1 : set output low    
			aaaa:		             //nop delay
#if (w2&1)
			ws2812_DEL1
#endif
#if (w2&2)
			ws2812_DEL2
#endif
#if (w2&4)
			ws2812_DEL4
#endif
#if (w2&8)
			ws2812_DEL8
#endif
#if (w2&16)
			ws2812_DEL16
#endif
#if (w2&32)
			ws2812_DEL32
#endif
			subs i, #1               //sub i by 1
			str masklo, [clr]        //T2 : unconditionally set output low, Note: str do not change flags
			beq end                  //if i == 0 jump out iloop
#if (w3&1)
			ws2812_DEL1
#endif
#if (w3&2)
			ws2812_DEL2
#endif
#if (w3&4)
			ws2812_DEL4
#endif
#if (w3&8)
			ws2812_DEL8
#endif
#if (w3&16)
			ws2812_DEL16
#endif
#if (w3&32)
			ws2812_DEL32
#endif
			b iloop                  //if program runs here, unconditionall jump to iloop
			end:
		}
	}	
	__set_PRIMASK(0);
}

static __INLINE void ws2812_one_byte_armcc(uint8_t curbyte)
{
uint32_t maskhi = ws2812_mask_set;
uint32_t masklo = ws2812_mask_clr;
volatile uint32_t *set = ws2812_port_set;
volatile uint32_t *clr = ws2812_port_clr;

uint32_t i;
		__ASM {
			lsl curbyte, #24         //curbyte left shift 24bits
			movs i, #8               //set i to 8
			iloop:                   //main loop
			lsls curbyte, #1         //left shift curbyte by #1, subfix "s" tell processor changing the "C" flag
			str maskhi, [set]        //T0(T3) : set output high   
#if (w1&1)                     		 //nop delay
			ws2812_DEL1
#endif
#if (w1&2)
			ws2812_DEL2
#endif
#if (w1&4)
			ws2812_DEL4
#endif
#if (w1&8)
			ws2812_DEL8
#endif
#if (w1&16)
			ws2812_DEL16
#endif
#if (w1&32)
			ws2812_DEL32
#endif
			bcs aaaa                 //skip set output low if MSB is 1, see "lsls" in <<CortexM3ProgrammingManual>>
			str masklo, [clr]        //T1 : set output low    
			aaaa:		             //nop delay
#if (w2&1)
			ws2812_DEL1
#endif
#if (w2&2)
			ws2812_DEL2
#endif
#if (w2&4)
			ws2812_DEL4
#endif
#if (w2&8)
			ws2812_DEL8
#endif
#if (w2&16)
			ws2812_DEL16
#endif
#if (w2&32)
			ws2812_DEL32
#endif
			subs i, #1               //sub i by 1
			str masklo, [clr]        //T2 : unconditionally set output low, Note: str do not change flags
			beq end                  //if i == 0 jump out iloop
#if (w3&1)
			ws2812_DEL1
#endif
#if (w3&2)
			ws2812_DEL2
#endif
#if (w3&4)
			ws2812_DEL4
#endif
#if (w3&8)
			ws2812_DEL8
#endif
#if (w3&16)
			ws2812_DEL16
#endif
#if (w3&32)
			ws2812_DEL32
#endif
			b iloop                  //if program runs here, unconditionall jump to iloop
			end:
		}

}

void ws2812_update_same_grb_armcc(uint8_t green, uint8_t red, uint8_t blue, int led_nums)
{
	__set_PRIMASK(1);
	while(led_nums--)
	{
		ws2812_one_byte_armcc(green);	
		ws2812_one_byte_armcc(red);	
		ws2812_one_byte_armcc(blue);	
	}
	__set_PRIMASK(0);
}

void led_demo_ocean_wave(void)
{
	uint8_t data[3] = {0,0,255};
	while(1)
	{
		for(int i = 0; i < 255; i++)
		{
			data[2] = i;
			ws2812_update_same_grb_armcc(data[0],data[1],data[2],200);
			delay_ms(3);
		}
		for(int i = 255; i > 0; i--)
		{
			data[2] = i;
			ws2812_update_same_grb_armcc(data[0],data[1],data[2],200);
			delay_ms(3);
		}
	}
}
void led_demo(void)
{
	uint8_t rgb[30*3] = {
							0x0, 0x0, 0x0, 
							0x55, 0x55, 0x55, 
							0xaa, 0xaa, 0xaa, 

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,


							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,
							
							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,


							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF,

							0xFF, 0x0, 0x0,
							0x0, 0xFF, 0x0,
							0x0, 0x0, 0xFF

							};
	while(1)
	{
		for(int i = 0; i < 255; i++)
		{
				rgb[2] = i;
				ws2812_sendarray_armcc(rgb, 30*3);
				delay_ms(3);
		}
		for(int i = 255; i > 0; i--)
		{
				rgb[2] = i;
				ws2812_sendarray_armcc(rgb, 30*3);
				delay_ms(3);
		}
    }
}
//void ws2812_sendarray_gnucc(uint8_t *data,int datlen)
//{
//	uint32_t maskhi = ws2812_mask_set;
//	uint32_t masklo = ws2812_mask_clr;
//	volatile uint32_t *set = ws2812_port_set;
//	volatile uint32_t *clr = ws2812_port_clr;
//	uint32_t i; 
//	uint32_t curbyte;

//	while (datlen--) {
//		curbyte=*data++;

//	__asm volatile(
//			"		lsl %[dat],#24				\n\t"
//			"		movs %[ctr],#8				\n\t"
//			"ilop%=:							\n\t"
//			"		lsl %[dat], #1				\n\t"
//			"		str %[maskhi], [%[set]]		\n\t"
//#if (w1&1)
//			ws2812_DEL1
//#endif
//#if (w1&2)
//			ws2812_DEL2
//#endif
//#if (w1&4)
//			ws2812_DEL4
//#endif
//#if (w1&8)
//			ws2812_DEL8
//#endif
//#if (w1&16)
//			ws2812_DEL16
//#endif
//			"		bcs one%=					\n\t"
//			"		str %[masklo], [%[clr]]		\n\t"
//			"one%=:								\n\t"
//#if (w2&1)
//			ws2812_DEL1
//#endif
//#if (w2&2)
//			ws2812_DEL2
//#endif
//#if (w2&4)
//			ws2812_DEL4
//#endif
//#if (w2&8)
//			ws2812_DEL8
//#endif
//#if (w2&16)
//			ws2812_DEL16
//#endif
//			"		sub %[ctr], #1				\n\t"
//			"		str %[masklo], [%[clr]]		\n\t"
//			"		beq	end%=					\n\t"
//#if (w3&1)
//			ws2812_DEL1
//#endif
//#if (w3&2)
//			ws2812_DEL2
//#endif
//#if (w3&4)
//			ws2812_DEL4
//#endif
//#if (w3&8)
//			ws2812_DEL8
//#endif
//#if (w3&16)
//			ws2812_DEL16
//#endif

//			"		b 	ilop%=					\n\t"
//			"end%=:								\n\t"
//			:	[ctr] "+r" (i)
//			:	[dat] "r" (curbyte), [set] "r" (set), [clr] "r" (clr), [masklo] "r" (masklo), [maskhi] "r" (maskhi)
//			);
//	}
//}

