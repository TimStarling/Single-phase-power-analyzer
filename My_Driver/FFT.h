#ifndef _FFT_H
#define _FFT_H


#include "main.h"


//FFT的实际采样率和采样点数，采样点数可取16, 64, 256, 1024 ,4096
//频率分辨率为    FFT_Fs_temp/FFT_Data_temp
//默认频率分辨率  20hz
//FFT长度，默认是1024点FFT，该宏定义的作用是定义一些存数据的数组长度
//在实际调用FFT变换函数时，数据长度可以取其他值。实际值见主函数
//由于采样率要大于20k,会根据档位重新宏定义这个FFT长度
#define FFT_Fs_temp   25600
#define FFT_LENGTH		1024 
#define FFT_Input_Res   50  //输入电阻大小

extern u32 SampleNum;
extern float FFT_VoltageBuf[FFT_LENGTH/2];	//频谱中各谐波电压幅值数组
extern float FFT_Powerbuf[FFT_LENGTH/2];   //频谱中各谐波功率数组
extern u32   FFT_Freqbuf[FFT_LENGTH/2];      //频谱中各谐波频率数组
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


