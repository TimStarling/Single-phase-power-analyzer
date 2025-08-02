#include "ADS8691.h"
#include "stdio.h"
#include "math.h"
#include <driverlib.h>
#include "stdint.h"
/*
0000b = ±3 × VREF
0001b = ±2.5 × VREF
0010b = ±1.5 × VREF
0011b = ±1.25 × VREF
0100b = ±0.625 × VREF
1000b = 3 × VREF
1001b = 2.5 × VREF
1010b = 1.5 × VREF
1011b = 1.25 × VREF
*/
#define COMPENSATE 1

//#define VREF_0625          0x0004//输入的电压范围是+-0.625倍的参考电压2.53952
#define VREF_3          0x0000//输入的电压范围是正负3倍的参考电压12.2888
#define COM_WIT_RANGE_LSB 0xD414//写命令的输入地址是14H的低16位，用来控制输入电压的范围
void ADS8691_Init(void)//GPIO初始化，若cubemx配置过则不需
{
	/********************HAL库**************************/
	

	
	
	GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN5);
	GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN4|GPIO_PIN2);	
	GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN0);	
	
	
	
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	
//	/*Configure GPIO pins : PB2 */
//  GPIO_InitStruct.Pin = GPIO_PIN_2;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//	/*Configure GPIO pins : PB3 PB4 PB5 */
//  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
/********************标准库**************************/
//	GPIO_InitTypeDef  GPIO_InitStructure;

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4|GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

	CS_AD8691 = 0;
	CLK = 0;
	SDI = 0;
}
// static uint32_t g_fac_us = 0;       /* us延时倍乘数 */
// 
//void delay_us(uint32_t nus)
//{
//    uint32_t ticks;
//    uint32_t told, tnow, tcnt = 0;
//    uint32_t reload = SysTick->LOAD;        /* LOAD的值 */
//    ticks = nus * g_fac_us;                 /* 需要的节拍数 */
//    told = SysTick->VAL;                    /* 刚进入时的计数器值 */
//    while (1)
//    {
//        tnow = SysTick->VAL;
//        if (tnow != told)
//        {
//            if (tnow < told)
//            {
//                tcnt += told - tnow;        /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
//            }
//            else 
//            {
//                tcnt += reload - tnow + told;
//            }
//            told = tnow;
//            if (tcnt >= ticks)
//            {
//                break;                      /* 时间超过/等于要延迟的时间,则退出 */
//            }
//        }
//    }
//}

//void ADS8691_RST()
//{
//	u8 i;
//  	CS = 0;
//	CS = 1;
//	CS = 0;
//	//先输入复位前指令	
//	for(i = 0;i < 16; i++)
//	{
//		CLK  = 0;
//		delay();
//		SDI = (0X0569 << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
//		CLK  = 1; 
//	}
//	//先确定写复位命令的寄存器地址	
//	for(i = 0;i < 16; i++)
//	{
//		CLK  = 0;
//		delay();
//		SDI = (0XD404 << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
//		CLK  = 1; 
//	}
//	//输出复位命令	
//	for(i = 0;i < 16; i++)
//	{
//		CLK  = 0;
//		delay();
//		SDI = (0X0000 << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
//		CLK  = 1; 
//	}
//}

void ADS8691_WIT_CON()
{
	uint8_t i;
  	CS_AD8691 = 0;
	CS_AD8691 = 1;
	CS_AD8691 = 0;
	//先确定写命令的地址	
	for(i = 0;i < 16; i++)
	{
		CLK  = 0;
//		delay_us(1);
//		HAL_Delay(1);
//		if((COM_WIT_RANGE_LSB << i) >> 15)
//			SDI = 1;
//		else
//			SDI = 0;
		delay();
		SDI = (COM_WIT_RANGE_LSB << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
		CLK  = 1; 
	}
	//将电压控制范围设定成0.625*VREF	
	for(i = 0;i < 16; i++)
	{
	    CLK  = 0;
			delay();
//		delay_us(1);
//		HAL_Delay(1);
		#ifdef VREF_0625 
//		if(((VREF_0625 << i) >> 15))
//			SDI = 1;
//		else
//			SDI = 0;
	    SDI = (VREF_0625 << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
	  #endif
	
	  #ifdef VREF_3 
	    SDI = (VREF_3 << i) >> 15;//写入控制命名表示要输入的是控制电压信号输入范围的寄存器
	  #endif
		CLK  = 1; 
	}
	 CS_AD8691=1;
}

//获取电信号的大小
uint32_t ADS8691_read()
{  
	uint8_t i = 0;
	uint32_t data = 0;
    CS_AD8691 = 0;
	CS_AD8691 = 1;
	CS_AD8691 = 0;
	data = 0;
	for(i = 0;i < 18; i++)
	{
	  	CLK  = 0;
//		delay();
	  data = (data << 1) + SDO; 
//		 data = (data << 1) + HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
		CLK  = 1;
//		delay_us(1);
	}
	data&=0x0003FFFF;
    CS_AD8691=1;
	return data;
}

//转换计算
float ADS8691_getvlaue()
{
	uint8_t i=0;
//	char str[10];
	uint8_t times = 10;
	float sum = 0;
	uint32_t data = 0;
    float average_data=0;
	float value=0;
	float multiple=0;//表示范围是参考电压的多少倍
	
	#ifdef VREF_0625 
	 multiple = 0.625;
	#endif
	
	#ifdef VREF_3 
	 multiple = 3;
	#endif
	
  ADS8691_WIT_CON();//先输入控制信号
	
	for (i=0;i<times;i++)
	{
		data = ADS8691_read();
	  	//value = (ADS8691_read()-65536*2)*multiple*2*VREF/(65536.0*4);
		value = (data - pow(2,17))/pow(2,17);
//		sprintf(str,"%.3f",value);
		//LCD_ShowString(150,150,100,16,16,(u8*)str);
		value = value*VREF*multiple;
//		sprintf(str,"%.3f",value);
		//LCD_ShowString(50,50,100,16,16,(u8*)str);
		//LCD_Clear(WHITE);
	  	sum = sum + value;
	}
	average_data = sum /times ;
//	average_data = 19.23076923076923*average_data+0.044230769;
  return average_data;
}
//获得len个ad电压值
void ADS8691_getvlaues(float *buf, int len)
{
	uint8_t i=0;

		for(i = 0;i<len;i++)
	{
		ADS8691_WIT_CON();//先输入控制信号
	  buf[i] = ADS8691_read();
	}

}
//计算求得电压值
void ADS8691_countvlaue(float *buf, int len ){
		int i; 
		float multiple=0;//表示范围是参考电压的多少倍	
	
		#ifdef VREF_0625 
		 multiple = 0.625;
		#endif
		
		#ifdef VREF_3 
		 multiple = 3;
		#endif
		for (i = 0; i < len; i++)
		{
			buf[i] = buf[i]-131072;
			buf[i] = buf[i]*multiple*VREF;
			buf[i] = buf[i]/131071.0;
			#ifdef COMPENSATE 	//补偿
			if(buf[i] < 2)
				buf[i] += 0.0113;
			else if(buf[i] < 7)
				buf[i] += 0.008;
			else if(buf[i] < 8)
				buf[i] += 0.009;
			else if(buf[i] > 8)
				buf[i] += 0.007;
			#endif
//		buf[i] = buf[i]+0.012;	//数据拟合
//		buf[i] = 1.003 *buf[i]+0.03551;//数据拟合
		}
}

//冒泡排序均值滤波，去除前后10%后取平均值
float ADS8691_sortvlaue(float *buf, int len ){
		int i, j; 
		float temp;
		for (i = 0; i < len - 1; i++)
				for (j = 0; j < len - 1 - i; j++)
						if (buf[j] > buf[j + 1]) {
								temp = buf[j];
								buf[j] = buf[j + 1];
								buf[j + 1] = temp;
						}
		temp=0;
		for(i=len*0.1;i<len - (len*0.1);i++){
				temp+=buf[i];
		}
		temp = temp/(len*0.8);
//		temp = 1.002 *temp+ 0.004301;//数据拟合
		return temp;
}

//冒泡排序获得最大值
float ADS8691_getmax(float *buf, int len )	{
		int i, j; 
		float temp;
		for (i = 0; i < len - 1; i++)
				for (j = 0; j < len - 1 - i; j++)
						if (buf[j] > buf[j + 1]) {
								temp = buf[j];
								buf[j] = buf[j + 1];
								buf[j + 1] = temp;
						}
		temp = 	buf[len - 1];
		return temp;				
}

void delay(void)
{
	for(int i=0;i<2;i++);
}
