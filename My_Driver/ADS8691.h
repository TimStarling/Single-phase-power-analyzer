#ifndef __ADS8691_H
#define __ADS8691_H

#include "msp432.h"
#include "stdint.h"
#include "stdio.h"
//#define SDO PBin(2)
//#define CLK PBout(3)
//#define CS  PBout(4)
//#define  SDI PBout(5)

#define  SDO BITBAND_PERI(P8IN,5)
#define  CLK BITBAND_PERI(P9OUT,0)
#define  CS_AD8691  BITBAND_PERI(P8OUT,4)
#define  SDI BITBAND_PERI(P8OUT,2)


#define VREF 4.096
//#define VREF 4.141	//基准电压变化后

uint32_t ADS8691_read(void);
void ADS8691_Init(void);
float ADS8691_getvlaue(void);
void ADS8691_getvlaues(float *buf, int len);
void ADS8691_countvlaue(float *buf, int len );
float ADS8691_sortvlaue(float *buf, int len );
float ADS8691_getmax(float *buf, int len );
void delay(void);
//void ADS8691_RST(void);
#endif
