#ifndef __ADS8691_1_H
#define __ADS8691_1_H

#include "msp432.h"
#include "stdint.h"
#include "stdio.h"
//#define SDO PBin(2)
//#define CLK PBout(3)
//#define CS  PBout(4)
//#define  SDI PBout(5)

#define  SDO1 BITBAND_PERI(P8IN,6)
#define  CLK1 BITBAND_PERI(P8OUT,7)
#define  CS_AD86911  BITBAND_PERI(P9OUT,1)
#define  SDI1 BITBAND_PERI(P8OUT,3)


#define VREF1 4.096
//#define VREF 4.141	//基准电压变化后

uint32_t ADS8691_read1(void);
void ADS8691_Init1(void);
float ADS8691_getvlaue1(void);
void ADS8691_getvlaues1(float *buf, int len);
void ADS8691_countvlaue1(float *buf, int len );
float ADS8691_sortvlaue1(float *buf, int len );
float ADS8691_getmax1(float *buf, int len );
void delay1(void);
//void ADS8691_RST(void);
#endif
