#ifndef _FFT_H
#define _FFT_H


#include "main.h"


//FFT��ʵ�ʲ����ʺͲ�������������������ȡ16, 64, 256, 1024 ,4096
//Ƶ�ʷֱ���Ϊ    FFT_Fs_temp/FFT_Data_temp
//Ĭ��Ƶ�ʷֱ���  20hz
//FFT���ȣ�Ĭ����1024��FFT���ú궨��������Ƕ���һЩ�����ݵ����鳤��
//��ʵ�ʵ���FFT�任����ʱ�����ݳ��ȿ���ȡ����ֵ��ʵ��ֵ��������
//���ڲ�����Ҫ����20k,����ݵ�λ���º궨�����FFT����
#define FFT_Fs_temp   25600
#define FFT_LENGTH		1024 
#define FFT_Input_Res   50  //��������С

extern u32 SampleNum;
extern float FFT_VoltageBuf[FFT_LENGTH/2];	//Ƶ���и�г����ѹ��ֵ����
extern float FFT_Powerbuf[FFT_LENGTH/2];   //Ƶ���и�г����������
extern u32   FFT_Freqbuf[FFT_LENGTH/2];      //Ƶ���и�г��Ƶ������
extern float FFT_Vdatabuf[FFT_LENGTH*2];
extern u32 Iol_Val[FFT_LENGTH];
extern u32 Vol_Val[FFT_LENGTH];
extern float FFT_Draw[FFT_LENGTH];
extern float Vol_Data[FFT_LENGTH];
extern float I_Data[FFT_LENGTH];

void FFT_Transform(void);
void FFT_Sort_F_V_P(u32 FFT_length);
void FFT_Power(u32 FFT_length);
void FFT_Voltage(u32 FFT_length);
void FFT_Freq(u32 Freq_samp,u32 FFT_length);


#endif


