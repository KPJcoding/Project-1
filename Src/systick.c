#include <systick.h>
void systick_delay(uint32_t delay)
{
	SysTick->LOAD = 15999;
	SysTick->VAL=0;
	SysTick->CTRL|=SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	for(int i=0;i<delay;i++)
	{
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)){}
	}
	SysTick->CTRL=0;
}
