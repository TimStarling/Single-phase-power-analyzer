#ifndef _SPI_FPGA_H_
#define _SPI_FPGA_H_

#include "main.h"




/***
读写模式ModeSel 选项
***/
#define MODESEL_BURST_RX 		1 //突发读
#define MODESEL_WRITE 			2 //写
#define MODESEL_READ 				3 //读


/*引脚定义相关宏*******************************************************************/
/*
CS 		P1.4
SCK		P1.5
MISO	P1.7
MOSI	P1.6
*/

/*软件控制引脚宏******************************************************************/
#define FPGA_NSS_Clr()                  	GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN0)//NSS
#define FPGA_NSS_Set()                  	GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN0)    
/***********************************************************************************/



void FPGA_SPI_Init(void);

void FPGA_Send_Byte(uint8_t DataByte);
uint8_t FPGA_Recive_Byte(void);

void FPGA_Write_Reg(uint16_t address,uint16_t data);
uint16_t FPGA_Read_Reg(uint16_t address);

//void FPGA_Set_AD_CLKin_and_CLKDiv(double SmpFreq,uint16_t AD_Smp_Div_Val);
//void FPGA_AD_Start();
//uint16_t FPGA_Get_AD_Status();
//void FPGA_Read_AD_Samp_DigitalVal(uint16_t *DigitalVal,uint16_t DataSize);
//void FPGA_AD_DigitalVal_to_FloatVal(uint16_t *DigitalVal,float *FloatVal,uint16_t DataSize);
//void FPGA_AD_FloatVAl_Correction(float *FloatVal,uint16_t DataSize);

//void Delay(uint32_t Num);
//void Freq_Init();
//double val_frq();

//void FPGA_Lock_In_Amp_SetFreq(double freq);
//double FPGA_Lock_In_Amp_GetVal();



#endif /* _SPI_FPGA_H_ */
