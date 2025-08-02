/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////                          ����ARM��FFT�ں�ʵ��FFT�����ļ�                                          //////////
////////1�����ļ������������ֵ��Ƶ�ʣ����ʺ� ����������������Ĺ��ĸ��ڲ����� ��һ��ʵ��FFT������ⲿ���� //////////
////////2������FFT�ں˹���Ҫ��������                                                                       //////////
////////	�ٶ���һ���ṹ��scfft����ʼ����    															   //////////
////////					arm_cfft_radix4_instance_f32   scfft;										   //////////
////////					arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);							   //////////
////////	�ڵ���FFT�ˣ�																				   //////////
////////					arm_cfft_radix4_f32(&scfft,FFT_databuf);									   //////////
////////	�ۼ���ģֵ��																				   //////////
////////					arm_cmplx_mag_f32(FFT_databuf,FFT_databuf,FFT_LENGTH);						   //////////
////////3����FFT���ǻ�4��FFT�任��֧�� 16, 64, 256, 1024��	                                               //////////
////////4������ARM DSP����Ҫ�򵥵Ĺ������ã����岽��ο�����ԭ��̽����A�̿���ָ�Ͽ⺯�����51��            //////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FFT.h"


//#include "timer_FFT.h"

//��¼��ǰ���������������ж��Ƿ����FFT_length����
u32 SampleNum=0;

//FFT���ݻ������飬�����ÿ���㣨�����������ÿ�����ģ��ʵ�����Ĵ�����������У���ͬһ�������ʡ�ռ�
//��������Ԫ�طֱ���һ���������鲿��ʵ�����������鳤����FFT���ݳ��ȵ�����������������ʱ���жϺ���
float FFT_Vdatabuf[FFT_LENGTH*2] = {0};

float FFT_Draw[FFT_LENGTH];
float FFT_VoltageBuf[FFT_LENGTH/2];	//Ƶ���и�г����ѹ��ֵ����
float FFT_Powerbuf[FFT_LENGTH/2];   //Ƶ���и�г����������
u32 FFT_Freqbuf[FFT_LENGTH/2];      //Ƶ���и�г��Ƶ������
arm_cfft_radix4_instance_f32 scfft;//FFT�˳�ʼ����ؽṹ��
u32 Iol_Val[FFT_LENGTH];
u32 Vol_Val[FFT_LENGTH];
float Vol_Data[FFT_LENGTH];
float I_Data[FFT_LENGTH];



/******************************************************************************
**����ԭ�ͣ�FFT_Freq(u32 Freq_samp,u32 FFT_length)
**�������ܣ���fft�õ��ĸ����������ǵ�Ƶ��
**���������Freq_samp��������      FFT_length��FFT�任���ݳ���
**�����������
**��    ע���ڲ�����
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
**����ԭ�ͣ�FFT_Voltage(u32 FFT_length)
**�������ܣ������г����ֵ������������FFT_VoltageBuf[]��
**���������FFT_length��FFT�任���ݳ���
**�����������
**˵    ������
**��    ע���ڲ�����
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
**����ԭ�ͣ�FFT_Power(u32 FFT_length)
**�������ܣ������г�����ʡ�����������FFT_Power[]��
**���������FFT_length��FFT�任���ݳ���
**�����������
**˵    �����ù����Ǽٶ������ѹ����1������ʱ�Ĺ��ʡ�����Чֵ��ƽ����
**��    ע���ڲ�����
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
**����ԭ�ͣ�FFT_Sort_F_V_P(u32 N)
**�������ܣ��ֱ�Դ�FFT�������save_freq[]��FFT_value[]��FFT_Power[]����
**�������������NUMΪ��������
**�����������
**˵    ��������ֵ�ɴ�С���򣬽�10����ֵ�ϴ�ķ�����������ǰ��
			������֮�󣬴Ӹ������2��Ԫ�أ��±�Ϊ1����ʼ����һ������ֱ��������
**��    ע���ڲ�����
******************************************************************************/
void FFT_Sort_F_V_P(u32 FFT_length)
{
	u16 i;
	u8 j;
	float temp_value;
	u32 temp_freq;
	FFT_VoltageBuf[0]=0; 				//ȥ��ֱ������
	for(j=1;j<11;j++)	           	//ֻ�ų�����10�� ��ʹ��ð������
	{								//ʱ���ܶ�		
		for(i=1;i<FFT_length/2-j;i++)
		{
			if(FFT_VoltageBuf[j]<FFT_VoltageBuf[j+i]) //�Ӵ�С����
			{
				//��ֵ
				temp_value=FFT_VoltageBuf[j];
				FFT_VoltageBuf[j]=FFT_VoltageBuf[i+j];
				FFT_VoltageBuf[i+j]=temp_value;
				//Ƶ��
				temp_freq=FFT_Freqbuf[j];
				FFT_Freqbuf[j]=FFT_Freqbuf[j+i];
				FFT_Freqbuf[i+j]=temp_freq;
//				//����
//				temp_power=FFT_Powerbuf[j];
//				FFT_Powerbuf[j]=FFT_Powerbuf[i+j];
//				FFT_Powerbuf[i+j]=temp_power;
			}
		}			
	}
} 


/********************************************************************************************
**����ԭ�ͣ�FFT_Transform(u16 Freq_samp,u16 FFT_length)
**�������ܣ�ʵ��FFT�任
**���������Freq_samp��������      FFT_length��FFT�任���ݳ���
**�����������
**˵    ������
**��    ע���ⲿ����
*********************************************************************************************/

void FFT_Transform(void)
{	
	//--------------------------------------------------------------
	//��ʼ��scfft�ṹ�壬�趨FFT��ز��������任������������0��1���ù�
	//--------------------------------------------------------------
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);
	//--------------------------------------------------------------
	//ͨ����ʱ����ȡ������
	//--------------------------------------------------------------
	SampleNum=0;                      //������������
	memset(Vol_Val,0,FFT_LENGTH);
	MAP_Timer32_setCount(TIMER32_0_BASE, 	(uint32_t)(48e6/FFT_Fs_temp));
	MAP_Interrupt_enableInterrupt(INT_T32_INT1);


	while(SampleNum<FFT_LENGTH)
	{
		delay_us(1);//��ʱ��ֹ�ڶ�ʱ���ж��п���
	}     
	//�ȵ����ݲ����رն�ʱ��
	
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
	//�����ں˽���FFT����
	//--------------------------------------------------------------
	arm_cfft_radix4_f32(&scfft,FFT_Vdatabuf);	//FFT���㣨��4��
	//--------------------------------------------------------------
	//��������ģ���ڶ������������ģֵ��ŵ��׵�ַ
	//--------------------------------------------------------------
	arm_cmplx_mag_f32(FFT_Vdatabuf,FFT_Vdatabuf,FFT_LENGTH);	//��������������ģ�÷�ֵ 	
	//--------------------------------------------------------------
	//Ƶ�ײ�������
	//--------------------------------------------------------------

	FFT_Freq(FFT_Fs_temp,FFT_LENGTH);//ÿ��Ƶ�ʷ�����Ƶ��
	FFT_Voltage(FFT_LENGTH);       //ÿ��Ƶ�ʷ����ķ�ֵ
	FFT_Power(FFT_LENGTH);         //ÿ��Ƶ�ʷ����ĵ�ѹ
	
	for(u8 q = 1;q<Har_Len;q++)
	{
		P.Harmonic[q-1] = FFT_VoltageBuf[(u16)(50*q/(FFT_Fs_temp/FFT_LENGTH))];
	}
	for(u16 q = 1;q<FFT_LENGTH;q++)
	{
		FFT_Draw[q-1] = FFT_VoltageBuf[q];
	}
	
	FFT_Sort_F_V_P(FFT_LENGTH);    //����ֵ�ɴ�С���򣬽�10����ֵ�ϴ�ķ�����������ǰ��
}



