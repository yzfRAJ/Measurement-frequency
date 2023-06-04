#include "stm32f10x.h"
#include "math.h"

#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "adc.h"

#define Time 8  //滤波次数

extern u32 T;

//中位值均值滤波
double Mid_Value_Filter(double *databuffer)
{
	double temp;
	u16 i,j;
	u8  times = Time;
	
	for(i=0;i<times-1;i++)
	{
		for(j=0;j<times-1-i;j++)
		{
			if(databuffer[j]>databuffer[j+1])           	//升序排列
			{
				temp=databuffer[j+1];
				databuffer[j+1]=databuffer[j];
				databuffer[j]=temp;
			}
		}
	}
	
	temp=0;
	for(i=times/4;i<(times-times/4);i++)							//抽取中间进行均值运算
	{
		temp+=databuffer[i];
	}
	temp=2*temp/times;
	
	return temp;
}


int main( void )
{
	int n = 0;
	float v = 0;   //电压  mV
	double f = 0;  //频率  HZ
	double Freq[Time];
	
  delay_init();
	uart_init(9600);
	
	led_init();       //PB12  -- >  LED = 0(亮)  |  LED = 1(灭)
	Adc_Init();       //PA1
	TIM3_Init();
	TIM2_Cap_Init();  //PA0
	
	while(1)
	{
		v = (float)Get_Adc(ADC_Channel_1) * 3300.0 / 4096;
		
		f = (double)23040000000 / (T*1.025632898742166) - (-0.000098);  
		Freq[n] = f;
		n++;
		if(n >= Time)
		{
			f = Mid_Value_Filter(Freq);
			printf("%.3lf\r\n",f);
			//printf("%ld\r\n",T);
			n = 0;
		}
		//printf("%.3lf\r\n",f);
		//printf("%ld\r\n",T);
		
		delay_ms(10);
	}
	
}

