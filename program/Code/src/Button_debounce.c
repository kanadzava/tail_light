#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
/////////////////////////

#include "Button_debounce.h"

#define CLOCK 10000				//Hz
#define DEBOUNCE_TIME 30 	//ms
#define LONG_PRESS 1000
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1


UINT8 button (bit pin)
{
	static bit old = 0;
	UINT16 timer = 0;
	
	
	if (pin == old)
	return 0;
	
	else if (old == BUTTON_PRESSED && pin == BUTTON_RELEASED)
	{
		timer = timer_button();
		old = BUTTON_RELEASED;
		
		if (timer >= LONG_PRESS)
		return 2;
		
		else if(timer >= DEBOUNCE_TIME)
		return 1;
			
		else 
		return 0;
	}
		
	else
	{	
		old = BUTTON_PRESSED;
		timer_start();
		return 0;
	}
	return 0;
}

bit long_change (bit new_long)
{
	static old_long = 0;
	if (old_long == new_long){}

	else
	{
		old_long = new_long;
		return 1;
	}
	return 0;
}


void timer_start()	//tim0
{
	clr_TR0;
	TH0 = 0;
	TL0 = 0;
	set_TR0;
}


UINT16 timer_button (void)
{
	UINT16 tic = 0;
	UINT16 time = 0;
	clr_TR0;
	tic = (TH0 << 8) + TL0;
	time = tic/(CLOCK/1000);
	return time;
}




















