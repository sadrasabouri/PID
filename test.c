#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define MAX 2.56
#define MIN 0

#define SCALE 2

float integral = 0;
float derivative = 0;
float p_error = 0;
float k_p, k_i, k_d;


void set_float_values(int k_p_in, int k_i_in, int k_d_in)
{
   k_p = (float) k_p_in;
   k_p /= 16.0;
   k_i = (float) k_i_in;
   k_i /= 16.0;
   k_d = (float) k_d_in;
   k_d /= 16.0;
}


void report_output(float value)
{
   float val = value * 100;               //   0 < val < 256
   //val += 256;
   //val /= 2; //   0 < val < 256
   int out = (int) val;
   //PORTC = out;
   printf("OUT(int) = %d ***** OUT(float) = %f\n", out, value);
}


float calculate(float set_point, float pv)
{
   float error = set_point;
   error -= pv;
   float p_out, i_out, d_out;
   p_out = k_p * error;
   integral += error;
   i_out = k_i * integral / SCALE;
   derivative = error - p_error;
   d_out = k_d * derivative * SCALE;      // divided by 0.02
   float output = p_out + d_out + i_out;
   if(output > MAX)
	 output = MAX;
   if(output < MIN)
	 output = MIN;
   p_error = error;
   return output;
}


int main()
{
   int k_p = 1;
   int k_i = 6;
   int k_d = 1;
   float set_point = 1.22;
   float pv = 0.5;
   
   set_float_values(k_p, k_i, k_d);
   
   //DDRC = 0xFF;
   while (1)
   {
      pv = calculate(set_point, pv);
      report_output(pv);
      //_delay_ms(20);
      Sleep(250);
   }
   
   return 0;
}
