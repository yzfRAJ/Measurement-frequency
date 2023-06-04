#include "timer.h"
#include "usart.h"
#include "sys.h"

//定时器3配置
void TIM3_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
	
	//初始化定时器3 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = 9999;                   //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 	                   //预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;        //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;               //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                                  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

  TIM_Cmd(TIM3,ENABLE ); 	             //使能定时器2
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//中断		

}


//定时器2通道1输入捕获配置
TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_Cap_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;    //PA0清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);						    //PA0上拉
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff;                   //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 	                   //预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1;                //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	   //上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;	         //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;               //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                                  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

  TIM_Cmd(TIM2,ENABLE ); 	             //使能定时器2
	
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//允许中断		

}

u32 time = 0;
int n = 0;
u32 T = 0;
int flag = 1;

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus( TIM3 , TIM_IT_Update ) != RESET )   
	{
		time += 10000;
		
		TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //清除中断标志位
	}
			
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus( TIM2 , TIM_IT_CC1 ) != RESET )   
	{
		if(flag)
		{
			TIM3->CNT = 0;
			time = 0;
			TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //清除中断标志位
			flag = 0;
		}else
		{	
			n++;
			if(n == 40)  //采集40次周期
			{
				time += TIM3->CNT;
				if((TIM3->SR&((uint16_t)0x0001))&&(TIM3->CNT<5000))
				{
					time += 10000;
					TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //清除中断标志位
				}
				T = time;
				time = 0;
				n = 0;
				flag = 1;
			}
					
			TIM2->SR = (uint16_t)~((uint16_t)0x0002) ; //清除中断标志位
		}
	}
		
}




