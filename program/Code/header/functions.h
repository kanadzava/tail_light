/////////////////////////////////
//battery voltage
/////////////////////////////////
float get_battery_voltage();
UINT16 READ_BANDGAP();
void 	ADC_Bypass (void);
bit cycles (UINT16 cycles);
UINT16 get_VDD (void);
void set_PWM (UINT16 VDD, bit low_PWM);
/////////////////////////////////
//Light sensor
/////////////////////////////////
void get_light_resisistance (void);
/////////////////////////////////
//start charge lvl
/////////////////////////////////
void init_charge (UINT16 VDD);
void charge_blink (void);
/////////////////////////////////
//clock
/////////////////////////////////
void set_LIRC(void);
void set_HIRC(void);
/////////////////////////////////
//blink
/////////////////////////////////
bit timer_blink_check (void);
void timer_blink_start (UINT16 time);
void timer_blink_reset (void);
void low_bat_blink (void);
void blink_fast (void);
void blink_slow (void);
void power_down (void);

void set_button_int(void);
void clr_button_int(void);
void button_int (void); //interrupt