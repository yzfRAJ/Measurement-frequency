#include "timer.h"
#include "usart.h"
#include "sys.h"

//��ʱ��3����
void TIM3_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
	
	//��ʼ����ʱ��3 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = 9999;                   //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 	                   //Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;        //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;               //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                                  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

  TIM_Cmd(TIM3,ENABLE ); 	             //ʹ�ܶ�ʱ��2
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//�ж�		

}


//��ʱ��2ͨ��1���벶������
TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_Cap_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;    //PA0���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0����  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);						    //PA0����
	
	//��ʼ����ʱ��2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = 0xffff;                   //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 	                   //Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM2���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	   //�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;	         //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;               //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                                  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

  TIM_Cmd(TIM2,ENABLE ); 	             //ʹ�ܶ�ʱ��2
	
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//�����ж�		

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
		
		TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //����жϱ�־λ
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
			TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //����жϱ�־λ
			flag = 0;
		}else
		{	
			n++;
			if(n == 40)  //�ɼ�40������
			{
				time += TIM3->CNT;
				if((TIM3->SR&((uint16_t)0x0001))&&(TIM3->CNT<5000))
				{
					time += 10000;
					TIM3->SR = (uint16_t)~((uint16_t)0x0001) ; //����жϱ�־λ
				}
				T = time;
				time = 0;
				n = 0;
				flag = 1;
			}
					
			TIM2->SR = (uint16_t)~((uint16_t)0x0002) ; //����жϱ�־λ
		}
	}
		
}




