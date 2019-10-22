#include "main.h"

bit BIT_TMP;
#define PWM_OUTPUT_DISABLE clr_PIO01

void main(void)
{   
	UINT16 VDD; //mV
	static UINT8 state = 0;
	static UINT8 button_state = 0;
	static bit long_press = 0;
	init ();
	VDD = get_VDD();

	
	set_LIRC();
	
	while (1)
	{
		button_state = button(P15);
		if (button_state == 1)
			state++;
		
		if (button_state == 2)
			long_press = ~long_press;
		
		
		if (state > 3)
			power_down();
		
		if (VDD < 3550)
			low_bat_blink();
		
		if (VDD < 3000)//3550
		{
			low_bat_blink();
			power_down();
		}
		
		else
		{
			switch(state)
				{
					case 0:
					{
								UINT8 i = 0;
								PWM_OUTPUT_DISABLE;
								for(i = 0; i < 200; i++);
								init_charge(VDD);
								for(i = 0; i < 200; i++);
								state = 1;
					
					}
					case 1:
						{						
							blink_fast();
							break;
						}
					case 2:
						{
							set_PIO01;
							break;
						}
					case 3:
					{
						power_down();						
					}
				
				}

		}
			
		if (cycles(512) || long_change(long_press))
		{
			set_HIRC();
			VDD = get_VDD();
			set_PWM(VDD, long_press);
			set_LIRC();
		}
		set_WDCLR; //wachdog reset				
	}
}