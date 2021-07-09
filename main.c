#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/eeprom.h>

/*
      **	PID Controller		**

   *	Pin Out	*
      +	PORTC : System Output
      
   *	Author(s)	*
      +	Omid RostamAbadi
      +	Sadra Sabouri
*/


#define MAX 2.56				//	Maximum PID output
#define MIN 0					//	Minimum PID output


#define SCALE 2    				// 	Timing scale for integration/derivation


/*
   Assuming that coeffs and setpoint are saved in
   bellow addresses by user before the execution
*/
#define ADDRESS_P 20			//	P coefficient place in ROM
#define ADDRESS_I 21			//	I coefficient place in ROM
#define ADDRESS_D 22			//	D coefficient place in ROM
#define ADDRESS_S 23 			//	Setpoint coefficient place in ROM


float integral = 0;  				// 	Integral of error
float derivative = 0;  				// 	Differenece between each two consecutive errors
float p_error = 0;  				// 	Previous error, needed for calculating derivative
float k_p, k_i, k_d; 				// 	Coeffs of PID controller
float set_point;


void user_input()
{
   /* 	function user_input
	    This function is assumed to be the user working with out program
	    it writes coeffs and setpoint to EEPROM prior to execution
   */
   uint8_t k_p = 1;
   uint8_t k_i = 6;
   uint8_t k_d = 1;
   uint8_t set_point = 122; 			// 	Setpoint is in range [0, 256]
   
   uint8_t *address;
   
   address = (uint8_t*) ADDRESS_P;
   eeprom_write_byte(address, k_p);
   
   address = (uint8_t*) ADDRESS_I;
   eeprom_write_byte(address, k_i);
   
   address = (uint8_t*) ADDRESS_D;
   eeprom_write_byte(address, k_d);
   
   address = (uint8_t*) ADDRESS_S;
   eeprom_write_byte(address, set_point);
}


void set_float_values(uint8_t k_p_in, uint8_t  k_i_in,
				uint8_t  k_d_in, uint8_t set_point_in)
{
   /*	function set_float_values
	    Received coeffs from user lie within range [0, 16]
	    we convert them to a value in range [0, 1]
	    setpoint is in range [0, 256] - we convert it to [0, 2.56]
   */
   k_p = (float) k_p_in;
   k_p /= 16.0;
   k_i = (float) k_i_in;
   k_i /= 16.0;
   k_d = (float) k_d_in;
   k_d /= 16.0;
   
   set_point = (float) set_point_in;
   set_point /= 100;
   
}


void set_coeffs()
{
   /*	function set_coeffs
	    Reads in coefficients from specified addresses
	    and sets the related variables.
   */
   uint8_t kp_tmp, ki_tmp, kd_tmp, setpoint_tmp;
   uint8_t *address;
   
   address = (uint8_t*) ADDRESS_P;
   kp_tmp = eeprom_read_byte(address);
   
   address = (uint8_t*) ADDRESS_I;
   ki_tmp = eeprom_read_byte(address);
   
   address = (uint8_t*) ADDRESS_D;
   kd_tmp = eeprom_read_byte(address);
   
   address = (uint8_t*) ADDRESS_S;
   setpoint_tmp = eeprom_read_byte(address);
   
   set_float_values(kp_tmp, ki_tmp, kd_tmp, setpoint_tmp);
}


void report_output(float value)
{
   /*	function report_output
	    We use a float value in range [0, 2.56] internally
	    we should scale this value to be represented as an integer in port c
   */
   float val = value * 100; 				// 	0 < val < 256
   int out = (int) val;
   PORTC = out;
}


float calculate(float pv) {
   /*	function calculate
	 the actual PID loop
	 returns updated output
   */
   float error = set_point;
   error -= pv;
   
   float p_out, i_out, d_out;
   p_out = k_p * error;
   
   integral += error;
   
   i_out = k_i * integral / SCALE;
   
   derivative = error - p_error;
   d_out = k_d * derivative * SCALE;
   
   float output = p_out + d_out + i_out;
   
   if(output > MAX)
	 output = MAX;
   if(output < MIN)
	 output = MIN;
   
   p_error = error;
   
   return output;
}


char if_time_print_time(char sec, char min, char hour)
{
   if (sec)							//	Sec should be zero	
      return 0;
   
   unsigned int mins = min + 60 * hour;
   if (mins % 16 == 0)
      return 1;
   
   return 0;
}


ISR (TIMER1_OVF_vect)    			//	Timer1 ISR
{
	unsigned char *p = (unsigned char *) 0x200;
	unsigned char sec = *p;	p++;	
	unsigned char min = *p;	p++; 
	unsigned char hour = *p;	p++;
	unsigned char day = *p;	p++;
	unsigned char week = *p;	p++;
	unsigned char month = *p;
	
	 sec++;
	 if (sec == 60)
	 {
	    sec = 0;
	    min++;
	 }
	 
	 if (min == 60)
	 {
	    min = 0;
	    hour++;
	 }
	 
	 if (hour == 24)
	 {
	    hour = 0;
	    day++;
	 }
	 
	 if (day == 7)
	 {
	    day = 0;
	    week++;
	 }
	 
	 if (week == 4)
	 {
	    week = 0;
	    month++;
	 }

	 //	Saving data to RAM
        p = (unsigned char *) 0x200;
	*p = sec;		p++;	
	*p = min;		p++; 
	*p = hour;		p++;
	*p = day;		p++;
	*p = week;	p++;
	*p = month;
	
	 
	 if (if_time_print_time(sec, min, hour))
	 {
	    //	In big-endian format
	    PORTD = sec;
	    _delay_ms(100);
	    PORTD = min;
	    _delay_ms(100);
	    PORTD = hour;
	    _delay_ms(100);
	    PORTD = day;
	    _delay_ms(100);
	    PORTD = week;
	    _delay_ms(100);
	    PORTD = month;
	    _delay_ms(100);
	    
	    PORTD = 0x00;
	 }
	TCNT1 = 63974;   				// 	For 1 sec at 16 MHz
}


void timer_setup()
{
   	DDRD = 0xFF;
	
	TCNT1 = 63974;   				//	For 1 sec at 16 MHz	

	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12); 	// 	Timer mode with 1024 prescler
	TIMSK = (1 << TOIE1) ;   			// 	Enable timer1 overflow interrupt(TOIE1)
	sei();        						// 	Enable global interrupts by setting global interrupt enable bit in SREG
   
	 //	RAM setup
	 unsigned char *p = (unsigned char *) 0x200;
	 char i;
	 for (i = 0; i < 6; ++i)
	 {
	    *p = 0x00;
	    p++;
	 }
}


int main()
{
   float pv = 0.3;						//	Initial value
   user_input();
   set_coeffs();
   
   DDRC = 0xFF;
   timer_setup();
   while (1)
   {
      pv = calculate(pv);
      report_output(pv);
      _delay_ms(20);
   }
   return 0;
}

