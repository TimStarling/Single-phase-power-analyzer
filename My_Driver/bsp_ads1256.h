#ifndef _BSP_ADS1256_H_
#define _BSP_ADS1256_H_

#include "msp432.h"
#include "stdint.h"
#include "stdio.h"
#include "delay.h"
/*引脚定义相关宏*******************************************************************/
//时钟初始化函数

//SCK引脚   
#define ADC_SPI_SCK_PIN                  GPIO_PIN5
#define ADC_SPI_SCK_GPIO_PORT            GPIO_PORT_P7
//#define ADC_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB


//MOSI引脚
#define ADC_SPI_MOSI_PIN                 GPIO_PIN7
#define ADC_SPI_MOSI_GPIO_PORT           GPIO_PORT_P7
//#define ADC_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOB


//MISO引脚
#define ADC_SPI_MISO_PIN                 GPIO_PIN1
#define ADC_SPI_MISO_GPIO_PORT           GPIO_PORT_P10
//#define ADC_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOC


//DRDY引脚
#define ADC_DRDY_PIN             	       GPIO_PIN3
#define	ADC_DRDY_GPIO_PORT				 			 GPIO_PORT_P10
//#define	ADC_DRDY_GPIO_CLK								 RCC_AHB1Periph_GPIOC


//CS引脚
#define ADC_SPI_CS_PIN                   GPIO_PIN5
#define ADC_SPI_CS_GPIO_PORT             GPIO_PORT_P10
//#define ADC_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOC


#define  ADC_SPI_SCK 		BITBAND_PERI(P7OUT,5)
#define  ADC_SPI_MOSI	 	BITBAND_PERI(P7OUT,7)
#define  ADC_SPI_MISO 	BITBAND_PERI(P10IN,1)
#define  ADC_DRDY  			BITBAND_PERI(P10IN,3)
#define  ADC_SPI_CS 		BITBAND_PERI(P10OUT,5)


/***********************************************************************************/
#define SYSTICK_START()			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk
#define SYSTICK_STOP()			SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk)

/*软件控制引脚宏******************************************************************/
//控制 CS(NSS) 引脚输出低电平
#define SPI_ADC_CS_LOW()  ADC_SPI_CS=0
//控制 CS(NSS) 引脚输出高电平
#define SPI_ADC_CS_HIGH() ADC_SPI_CS=1

//控制SCK引脚输出低电平
#define SPI_ADC_SCK_LOW() ADC_SPI_SCK=0
//控制SCK引脚输出高电平
#define SPI_ADC_SCK_HIGH() ADC_SPI_SCK=1

//控制MOSI引脚输出低电平
#define SPI_ADC_MOSI_LOW() ADC_SPI_MOSI=0
//控制MOSI引脚输出高电平
#define SPI_ADC_MOSI_HIGH() ADC_SPI_MOSI=1


#define DRDY_READ() 			ADC_DRDY
#define SPI_MISO_READ() 	ADC_SPI_MISO

void ADS1256_SPI_INT(void);
void ADS1256_SPI_TXDATA(u8 TXDATA);
uint8_t ADS1256_SPI_RXDATA(void);
uint8_t ADS1256_ReadReg(uint8_t _RegID);
void ADS1256_Delay(uint32_t Num);
uint8_t ADS1256_ReadChipID(void);
void ADS1256_WaitDRDY(void);
void ADS1256_cfg(u8 ainp,u8 gain, u8 drate);
int32_t ADS1256_ReadData(void);
#endif 

