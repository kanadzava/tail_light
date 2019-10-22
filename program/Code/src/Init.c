#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
///////////////////////////////

#include "Init.h"
#define PWM_DUTY_REGISTER_L PWM1L
#define PWM_OUTPUT_ENABLE set_PIO01
#define PWM_OUTPUT_DISABLE clr_PIO01

#define LED_DISABLE			clr_P11
#define LED_ENABLE 			set_P11
#define	POWER_ENABLE 		set_P10
#define POWER_DISABLE 	clr_P10
#define INT1						P00
#define INT2						P01
#define LIGHT_INPUT 		P12


void init (void)
{
	LED_DISABLE;
	Set_All_GPIO_Quasi_Mode;
	P15_Input_Mode; //button
	P10_PushPull_Mode; //power gate
	P11_PushPull_Mode; //led gate

	
	P12_Input_Mode; //light input
	
	//I2C
	P13_OpenDrain_Mode; //SCL
	P14_OpenDrain_Mode; //SDA
	P00_Input_Mode; //INT1
	P01_Input_Mode; //INT2	
	


	/////////////////////////////////
	//Brown Out Detector
	/////////////////////////////////
	set_LPBOD1;
	set_LPBOD0;		//check period 25.6ms
	
	
	
	////////////////////////////////
	//timer0 init 	button timer
	/////////////////////////////////
	set_M0_T0;		// mode1 - 16bit
	set_T0M;			//sys clock
	
	
	////////////////////////////////
	//timer1 init		blink tmer
	/////////////////////////////////
	set_M0_T1;		// mode1 - 16bit
	set_T1M;			//sys clock
	
	////////////////////////////////
	//PWM init
	/////////////////////////////////
	//PWM_OUTPUT_DISABLE; //enable PWM out
	set_CLRPWM;
	while (CLRPWM);
	PWMPL = 100;
	PWMPH = 0;
	PWM_DUTY_REGISTER_L = 100-40;
	set_PWMRUN;

		
}


























