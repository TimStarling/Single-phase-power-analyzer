#ifndef __POWER_H_
#define __POWER_H_

#include "main.h"

#define Har_Len 41    //谐波存储次数
#define THD_Len 10		//失真度计算谐波次数
#define Vol_Coe 293.8097
//#define I_Coe_150(n) 15.951f*n+0.0041

#define I_Coe_1000(n) (9.5697*n-0.0024)//9.6473*n-0.0032
#define I_Coe_160(n) (15.176*n-0.0026)//15.336*n-0.0008
#define I_Coe_50(n) (4.8391*n+0.0051)//4.9664*n-0.0038
#define I_Coe_25(n) (2.5001*n+0.0026)//u2.4

struct Power_Data{
	float Vol;       		//电压有效值
	float I;         		//电流有效值
	float Fre;			 		//频率
	float Pow_Pos;	 		//有功功率
	float Pow_Neg;   		//无功功率
	float Pow_App;   		//视在功率
	float Factor;		 		//功率因数
	float THD;			 		//失真度
	float Harmonic[Har_Len]; //谐波分量
};

extern struct Power_Data P;
extern u8 Relay_Open;
extern bool Relay_Flag;
extern bool Relay_Auto;
extern u8 Page0_Change;

void Power_Cal(void);
float Find_Max(float *Buffer,uint16_t Size);
double Pow_Pos_Cul(float* V,float* I);
float Find_Min(float *Buffer,uint16_t Size);

void V_Signal(void);
void I_Signal(void);
void H_Signal(void);

void USART_HMI_DATA(void);

void Relay_Init(void);
void Relay_Select(void);

#endif


