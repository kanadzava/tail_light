#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
///////////////////////////////

#include "functions.h"
#define CLOCK 								10000

#define LOW_BAT_SHORT 				120-50
#define LOW_BAT_LONG 					300-50
#define BLINK_FAST 						110-70
#define BLINK_SLOW 						400-50

#define TARGET_CURRENT 					40
#define LOW_BAT_TARGET_CURRENT 			15

#define PWM_DUTY_REGISTER_L 	PWM1L
#define PWM_OUTPUT_ENABLE 		set_PIO01
#define PWM_OUTPUT_DISABLE 		clr_PIO01

#define LED_DISABLE 					clr_P11
#define LED_ENABLE 						set_P11
#define	POWER_ENABLE 					set_P10
#define POWER_DISABLE 				clr_P10

/////////////////////////////////
//battery voltage
/////////////////////////////////

float get_battery_voltage()
{
	static UINT16 Bandgap_Voltage = 0;
	UINT16 bgvalue;
	float VDD_Voltage;
	
	if (Bandgap_Voltage == 0)
		Bandgap_Voltage = READ_BANDGAP(); 
	
	Enable_ADC_BandGap; 
	ADC_Bypass(); 
	clr_ADCF; 
	set_ADCS; 
	while(ADCF == 0);
	bgvalue = (ADCRH<<4) + ADCRL;
	Disable_ADC;
	VDD_Voltage = (4095.0/bgvalue)*Bandgap_Voltage;
	return VDD_Voltage;
}

UINT16 READ_BANDGAP() 
{ 
	UINT8 BandgapLow, BandgapMark, BandgapHigh;
	UINT16 Bandgap_Value, Bandgap_Voltage, Bandgap_Voltage_Temp;
	
	set_IAPEN;
	IAPCN = READ_UID;
	IAPAL = 0x0d;
	IAPAH = 0x00;
	set_IAPGO;
	BandgapLow = IAPFD;
	BandgapMark = BandgapLow&0xF0;
		
	if (BandgapMark==0x80)
	{
			BandgapLow = BandgapLow&0x0F;
			IAPAL = 0x0C;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage_Temp = Bandgap_Value*3/4;
			Bandgap_Voltage = Bandgap_Voltage_Temp - 33;			//the actually banggap voltage value is similar this value.
	}
	if (BandgapMark==0x00)
	{
			BandgapLow = BandgapLow&0x0F;
			IAPAL = 0x0C;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage= Bandgap_Value*3/4;
	}
	if (BandgapMark==0x90)
	{
			IAPAL = 0x0E;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapHigh = IAPFD;
			IAPAL = 0x0F;
			IAPAH = 0x00;
			set_IAPGO;
			BandgapLow = IAPFD;
			BandgapLow = BandgapLow&0x0F;
			Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
			Bandgap_Voltage= Bandgap_Value*3/4;
	}
	clr_IAPEN;
	//set_LIRC();
	return Bandgap_Voltage;
}

void ADC_Bypass (void) // The first three times convert should be bypass 
{ 
	UINT8 ozc; 
	for (ozc=0; ozc < 3; ozc++) 
	{ 
		clr_ADCF; 
		set_ADCS; 
		while(ADCF == 0); 
	}
}

bit cycles (UINT16 cycles)
{
	static UINT16 i = 0;
	if ( i < cycles)
		i++;
	else
	{
		i=0;
		return 1;
	}
	return 0;
}
	
	
UINT16 get_VDD (void)
{
	return get_battery_voltage();
}

void set_PWM (UINT16 VDD, bit low_PWM)
{
	UINT16 temp = 0;
	
	if (VDD < 3550 || low_PWM)
	{
		temp = VDD - 1900; 										//resistor voltage = VDD - LED voltage
		temp = temp/33;												//resistor current = resistor voltage / resistor value
		temp = (LOW_BAT_TARGET_CURRENT * 100)/ temp; 	//Duty, % = (target_current*100%)/resistor current
		PWM_DUTY_REGISTER_L = (PWMPL * temp) / 100;					//% = (D1.0 / 100)* Duty
		PWM1H = 0;
		set_LOAD;
		return;
	}
	
	temp = VDD - 2050; 										//resistor voltage = VDD - LED voltage
	temp = temp/33;												//resistor current = resistor voltage / resistor value
	temp = (TARGET_CURRENT * 100)/ temp; 	//Duty, % = (target_current*100%)/resistor current
	PWM_DUTY_REGISTER_L = (PWMPL * temp) / 100;					//% = (D1.0 / 100)* Duty
	PWM1H = 0;
	set_LOAD;
}


/////////////////////////////////
//Light sensor
/////////////////////////////////
/*
void get_light_resisistance (void)
{
	Enable_ADC_BandGap; 
	ADC_Bypass(); 
	clr_ADCF; 
	set_ADCS; 
	while(ADCF == 0);
	bgvalue = (ADCRH<<4) + ADCRL;
	680/ = adc/
	x			4095-adc
}
*/
/////////////////////////////////
//init charge lvl
/////////////////////////////////
void init_charge (UINT16 VDD)
{
	if (VDD > 3500)						//20%
	{
		charge_blink();
	}
	if (VDD > 3640)						//40%
	{
		charge_blink();
	}
	if (VDD > 3770)						//60%
	{
		charge_blink();
	}
	if (VDD > 3910)						//80%
	{
		charge_blink();
	}
	if (VDD > 4050)						//100%
	{
		charge_blink();
	}
}
			
void charge_blink (void)
{
	UINT8 k;
	PWM_OUTPUT_ENABLE;
	for (k = 0; k < 150; k++);
	PWM_OUTPUT_DISABLE;
	for (k = 0; k < 150; k++);
}


/////////////////////////////////
//clock
/////////////////////////////////
void set_LIRC(void)
{
	CKDIV = 0x00;
	set_OSC1;													
	clr_OSC0;  
	while((CKEN&SET_BIT0)==1);
	clr_HIRCEN;
}

void set_HIRC(void)
{
	set_HIRCEN;
	while((CKEN&SET_BIT0)==1);
	clr_OSC1;													
	clr_OSC0; 
	CKDIV = 0;
}
/////////////////////////////////
//blink
/////////////////////////////////
bit timer_blink_check (void) //time ms
{
	
	if (!TF1)
		return 0;
	
	if (TF1)
	{
		clr_TR1;
		clr_TF1;
		return 1;
	}
	return 0;
}

void timer_blink_start (UINT16 time) //6553 ms max @ 10kHz
{
	UINT16 tics;

	if (TR1)
		return;
	tics = (CLOCK/1000)*time;
	tics = ~tics;
	TL1 = tics & 0xFF;
	TH1 = tics >> 8;
	set_TR1;
}

void timer_blink_reset (void)
{
	clr_TR1;
	clr_TF1;
	TL1 = 0;
	TH1 = 0;
}

void low_bat_blink (void)
{
	static UINT8 stage = 0;
	
	switch (stage)
	{
		case 0:
		{
			PWM_OUTPUT_ENABLE;
			timer_blink_start(LOW_BAT_SHORT);
			if (timer_blink_check())
				stage++;
			break;
		}
		case 1:
		{
			PWM_OUTPUT_DISABLE;
			timer_blink_start(LOW_BAT_SHORT);
			if (timer_blink_check())
				stage++;
			break;
		}
		case 2:
		{
			PWM_OUTPUT_ENABLE;
			timer_blink_start(LOW_BAT_SHORT);
			if (timer_blink_check())
				stage++;
			break;
		}
				case 3:
		{
			PWM_OUTPUT_DISABLE;
			timer_blink_start(LOW_BAT_SHORT);
			if (timer_blink_check())
				stage++;
			break;
		}
				case 4:
		{
			PWM_OUTPUT_ENABLE;
			timer_blink_start(LOW_BAT_SHORT);
			if (timer_blink_check())
				stage++;
			break;
		}
				case 5:
		{
			PWM_OUTPUT_DISABLE;
			timer_blink_start(LOW_BAT_LONG);
			if (timer_blink_check())
				stage++;
			break;
		}
	}
		
		if (stage > 5)
			stage = 0;

}

void blink_fast (void)
{
	static UINT8 stage = 0;
	
	switch (stage)
	{
		case 0:
		{
			PWM_OUTPUT_ENABLE;
			timer_blink_start(BLINK_FAST);
			if (timer_blink_check())
				stage=1;
			break;
		}
		case 1:
		{
			PWM_OUTPUT_DISABLE;
			timer_blink_start(BLINK_FAST);
			if (timer_blink_check())
				stage=0;
			break;
		}
	
	}
}

void blink_slow (void)
{
	static UINT8 stage = 0;
	
	switch (stage)
	{
		case 0:
		{
			PWM_OUTPUT_ENABLE;
			timer_blink_start(BLINK_SLOW);
			if (timer_blink_check())
				stage = 1;
			break;
		}
		case 1:
		{
			PWM_OUTPUT_DISABLE;
			timer_blink_start(BLINK_SLOW);
			if (timer_blink_check())
				stage = 0;
			break;
		}
	}
}
/////////////////////////////////
//Power Down
/////////////////////////////////
void power_down (void)
{
	PWM_OUTPUT_DISABLE;
	LED_DISABLE;
	set_HIRC();
	set_button_int();
	set_EA;
	set_EPI;
	set_PD;
}

void set_button_int(void) //P1.5
{
	set_PIT45;
	set_PIPS0;
	clr_PIPS1;
	set_PINEN5; //falling edge triger
}

void clr_button_int(void)
{
	clr_PIF5;
	clr_PIT45;
	clr_PIPS0;
	clr_PIPS1;
	clr_PINEN5;
}

void button_int (void) interrupt 7
{
	clr_button_int();
	set_SWRST;
	P11_Input_Mode;
}