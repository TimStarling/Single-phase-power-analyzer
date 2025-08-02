/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////                          利用ARM的FFT内核实现FFT运算文件                                          //////////
////////1、该文件共包含计算幅值，频率，功率和 对这三个参数排序的共四个内部函数 和一个实现FFT运算的外部函数 //////////
////////2、调用FFT内核共需要三个步骤                                                                       //////////
////////	①定义一个结构体scfft并初始化：    															   //////////
////////					arm_cfft_radix4_instance_f32   scfft;										   //////////
////////					arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);							   //////////
////////	②调用FFT核：																				   //////////
////////					arm_cfft_radix4_f32(&scfft,FFT_databuf);									   //////////
////////	③计算模值：																				   //////////
////////					arm_cmplx_mag_f32(FFT_databuf,FFT_databuf,FFT_LENGTH);						   //////////
////////3、该FFT核是基4的FFT变换，支持 16, 64, 256, 1024点	                                               //////////
////////4、调用ARM DSP核需要简单的工程配置，具体步骤参考正点原子探索者A盘开发指南库函数版第51章            //////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FFT.h"


//#include "timer_FFT.h"

//记录当前采样点数，用来判断是否采满FFT_length个点
u32 SampleNum=0;

//FFT数据缓存数组，输入的每个点（复数）和输出每个点的模（实数）的存在这个数组中，用同一个数组节省空间
//相邻两个元素分别是一个复数的虚部和实部，所以数组长度是FFT数据长度的两倍，数据填充见定时器中断函数
float FFT_Vdatabuf[FFT_LENGTH*2] = {0};

float FFT_Draw[FFT_LENGTH];
float FFT_VoltageBuf[FFT_LENGTH/2];	//频谱中各谐波电压幅值数组
float FFT_Powerbuf[FFT_LENGTH/2];   //频谱中各谐波功率数组
u32 FFT_Freqbuf[FFT_LENGTH/2];      //频谱中各谐波频率数组
arm_cfft_radix4_instance_f32 scfft;//FFT核初始化相关结构体
u32 Iol_Val[FFT_LENGTH];
u32 Vol_Val[FFT_LENGTH];
float Vol_Data[FFT_LENGTH];
float I_Data[FFT_LENGTH];



/******************************************************************************
**函数原型：FFT_Freq(u32 Freq_samp,u32 FFT_length)
**函数功能：将fft得到的复数计算他们的频率
**输入参数：Freq_samp：采样率      FFT_length：FFT变换数据长度
**输出参数：无
**备    注：内部函数
******************************************************************************/
void FFT_Freq(u32 Freq_samp,u32 FFT_length)
{
	u32 i;
	for(i=0;i<FFT_length/2;i++)
	{
		FFT_Freqbuf[i]=i*(Freq_samp/FFT_length);
	}	
}
/******************************************************************************
**函数原型：FFT_Voltage(u32 FFT_length)
**函数功能：计算各谐波幅值。并存在数组FFT_VoltageBuf[]中
**输入参数：FFT_length：FFT变换数据长度
**输出参数：无
**说    明：无
**备    注：内部函数
******************************************************************************/
void FFT_Voltage(u32 FFT_length)
{
	
	u32 i; 	
	for(i=0;i<FFT_length/2;i++)
	{	
		FFT_VoltageBuf[i] = 2.0f*sqrt(2.0)*FFT_Vdatabuf[i] / (2.0f*FFT_length);
	}
	
}
/******************************************************************************
**函数原型：FFT_Power(u32 FFT_length)
**函数功能：计算各谐波功率。并存在数组FFT_Power[]中
**输入参数：FFT_length：FFT变换数据长度
**输出参数：无
**说    明：该功率是假定被测电压经过1Ω电阻时的功率。（有效值的平方）
**备    注：内部函数
******************************************************************************/
void FFT_Power(u32 FFT_length)
{
//	u16 i;
//	for(i=0 ; i<FFT_length/2 ; i++)
//	{
//		FFT_Powerbuf[i] = (float)FFT_VoltageBuf[i] * FFT_VoltageBuf[i]/(2.0*FFT_Input_Res) ;
//	}	
}
/******************************************************************************
**函数原型：FFT_Sort_F_V_P(u32 N)
**函数功能：分别对存FFT结果数组save_freq[]、FFT_value[]、FFT_Power[]排序。
**输入参数：输入NUM为采样点数
**输出参数：无
**说    明：按幅值由大到小排序，将10个幅值较大的分量放在数组前面
			排序完之后，从各数组第2个元素（下标为1）开始，第一个点是直流分量。
**备    注：内部函数
******************************************************************************/
void FFT_Sort_F_V_P(u32 FFT_length)
{
	u16 i;
	u8 j;
	float temp_value;
	u32 temp_freq;
	FFT_VoltageBuf[0]=0; 				//去除直流分量
	for(j=1;j<11;j++)	           	//只排出最大的10个 不使用冒泡排序
	{								//时间快很多		
		for(i=1;i<FFT_length/2-j;i++)
		{
			if(FFT_VoltageBuf[j]<FFT_VoltageBuf[j+i]) //从大到小排序
			{
				//幅值
				temp_value=FFT_VoltageBuf[j];
				FFT_VoltageBuf[j]=FFT_VoltageBuf[i+j];
				FFT_VoltageBuf[i+j]=temp_value;
				//频率
				temp_freq=FFT_Freqbuf[j];
				FFT_Freqbuf[j]=FFT_Freqbuf[j+i];
				FFT_Freqbuf[i+j]=temp_freq;
//				//功率
//				temp_power=FFT_Powerbuf[j];
//				FFT_Powerbuf[j]=FFT_Powerbuf[i+j];
//				FFT_Powerbuf[i+j]=temp_power;
			}
		}			
	}
} 


/********************************************************************************************
**函数原型：FFT_Transform(u16 Freq_samp,u16 FFT_length)
**函数功能：实现FFT变换
**输入参数：Freq_samp：采样率      FFT_length：FFT变换数据长度
**输出参数：无
**说    明：无
**备    注：外部函数
*********************************************************************************************/

void FFT_Transform(void)
{	
	//--------------------------------------------------------------
	//初始化scfft结构体，设定FFT相关参数，正变换后面两个参数0和1不用管
	//--------------------------------------------------------------
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);
	//--------------------------------------------------------------
	//通过定时器获取采样点
	//--------------------------------------------------------------
	SampleNum=0;                      //采样点数归零
	memset(Vol_Val,0,FFT_LENGTH);
	MAP_Timer32_setCount(TIMER32_0_BASE, 	(uint32_t)(48e6/FFT_Fs_temp));
	MAP_Interrupt_enableInterrupt(INT_T32_INT1);


	while(SampleNum<FFT_LENGTH)
	{
		delay_us(1);//延时防止在定时器中断中卡死
	}     
	//等到数据采满关闭定时器
	
	MAP_Interrupt_disableInterrupt(INT_T32_INT1);
	for(uint16_t i = 0;i<FFT_LENGTH;i++)
	{
			int32_t temp = 0;
		 	temp = Iol_Val[i]-131072;
			FFT_Vdatabuf[2*i] = temp*3.0f*VREF1;
			if(Relay_Open == 0)
				FFT_Vdatabuf[2*i] = I_Coe_1000(FFT_Vdatabuf[2*i]/131071.0);
			else if(Relay_Open == 1)
				FFT_Vdatabuf[2*i] = I_Coe_160(FFT_Vdatabuf[2*i]/131071.0);
			else if(Relay_Open == 2)
				FFT_Vdatabuf[2*i] = I_Coe_50(FFT_Vdatabuf[2*i]/131071.0);
			else if(Relay_Open == 3)
				FFT_Vdatabuf[2*i] = I_Coe_25(FFT_Vdatabuf[2*i]/131071.0);
			
			
			FFT_Vdatabuf[2*i+1] = 0;
//			I_Data[i] = FFT_Vdatabuf[2*i]/5.988;
			I_Data[i] = FFT_Vdatabuf[2*i];
	}
	char LL[50] = {0};
	double Vol_Ame = 0;
	ADC14_toggleConversionTrigger();
	Vol_Ame = 0.95+ADC14_getResult(ADC_MEM3)*1.0f/163840.0f;
//	sprintf(LL,"t0.txt=\"q%-.3f\"\xff\xff\xff",Vol_Ame);
//	USART_HMI_SendString(LL); 
	for(uint16_t i = 0;i<FFT_LENGTH;i++)
	{
		
			int32_t temp = 0;
		 	temp = Vol_Val[i]-131072;
//			Vol_Data[i] = temp*3.0f*VREF1;
			Vol_Data[i] = Vol_Ame*Vol_Coe*temp*3.0f*VREF1*1.0f;
			Vol_Data[i] = Vol_Data[i]/131071.0;
	}
	
	//--------------------------------------------------------------
	//调用内核进行FFT运算
	//--------------------------------------------------------------
	arm_cfft_radix4_f32(&scfft,FFT_Vdatabuf);	//FFT计算（基4）
	//--------------------------------------------------------------
	//计算结果的模，第二个参数是输出模值存放的首地址
	//--------------------------------------------------------------
	arm_cmplx_mag_f32(FFT_Vdatabuf,FFT_Vdatabuf,FFT_LENGTH);	//把运算结果复数求模得幅值 	
	//--------------------------------------------------------------
	//频谱参数分析
	//--------------------------------------------------------------

	FFT_Freq(FFT_Fs_temp,FFT_LENGTH);//每个频率分量的频率
	FFT_Voltage(FFT_LENGTH);       //每个频率分量的幅值
	FFT_Power(FFT_LENGTH);         //每个频率分量的电压
	
	for(u8 q = 1;q<Har_Len;q++)
	{
		P.Harmonic[q-1] = FFT_VoltageBuf[(u16)(50*q/(FFT_Fs_temp/FFT_LENGTH))];
	}
	for(u16 q = 1;q<FFT_LENGTH;q++)
	{
		FFT_Draw[q-1] = FFT_VoltageBuf[q];
	}
	
	FFT_Sort_F_V_P(FFT_LENGTH);    //按幅值由大到小排序，将10个幅值较大的分量放在数组前面
}



