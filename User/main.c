#include "user_clock.h"
#include "user_nvic.h"
#include "user_i2c.h"
#include "hw_signal.h"
#include "hw_key.h"
#include "hw_timestamp.h"
#include "hw_oled.h"
#include "hw_qei.h"
#include "hw_motor.h"
#include "hw_adc.h"
timestamp_t timestamp = {0}; // 时间戳组
qei_info_t qei_group = {0};	 // 编码器组
uint16_t adc_value[2] = {0}; // ADC值组
void User_HW_Init(void)
{
	nvic_init_t nvic_group[] = {
		{TIM4_IRQn, 0, 0},
	};
	RCC_AHBPeriphClockCmd(AHB_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(APB1_USART2 | APB1_USART3 | APB1_TIM2 | APB1_TIM3 | APB1_TIM4 | APB1_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(APB2_AFIO | APB2_GPIOA | APB2_GPIOB | APB2_GPIOC | APB2_TIM1 | APB2_USART1 | APB2_ADC1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	User_NVIC_All_Init(NVIC_PriorityGroup_2, nvic_group, 1);
	HW_I2C_Init(I2C1);
}

void HW_OLED_Show(void)
{
	OLED_ShowSignedNum(0, 0, qei_group.l_speed, 5, OLED_6X8);
	OLED_ShowSignedNum(0, 8, qei_group.r_speed, 5, OLED_6X8);
	OLED_ShowSignedNum(0, 16, adc_value[0], 4, OLED_6X8);
	OLED_ShowSignedNum(0, 24, adc_value[1], 4, OLED_6X8);
	OLED_Update();
}
int main(void)
{
	User_Delay_Ms(1000);
	User_HW_Init();
	HW_Signal_Init();
	HW_Key_Init();
	HW_Timestamp_Init();
	HW_OLED_Init();
	HW_QEI_Init();
	HW_Motor_Init();
	HW_ADC_Init();
	while (1)
	{
		HW_OLED_Show();
		HW_QEI_Update();
		HW_Motor_Run(700, 700);
	}
}
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		HW_Key_Scan();
		timestamp.count++;
	}
}
