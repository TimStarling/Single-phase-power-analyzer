#include "spi_fpga.h"


/******************************************************************************
 * MSP432 SPI - 3-wire Master Incremented Data
 *
 * This example shows how SPI master talks to SPI slave using 3-wire mode.
 * Incrementing data is sent by the master starting at 0x01. Received data is
 * expected to be same as the previous transmission.  eUSCI RX ISR is used to
 * handle communication with the CPU, normally in LPM0. Because all execution 
 * after LPM0 is in ISRs, initialization waits for DCO to stabilize against 
 * ACLK.
 *
 * Note that in this example, EUSCIB0 is used for the SPI port. If the user
 * wants to use EUSCIA for SPI operation, they are able to with the same APIs
 * with the EUSCI_AX parameters.
 *
 * ACLK = ~32.768kHz, MCLK = SMCLK = DCO 3MHz
 *
 * Use with SPI Slave Data Echo code example.
 *
 *                MSP432P401
 *              -----------------
 *             |                 |
 *             |                 |
 *             |                 |
 *             |             P1.6|-> Data Out (UCB0SIMO)
 *             |                 |
 *             |             P1.7|<- Data In (UCB0SOMI)
 *             |                 |
 *             |             P1.5|-> Serial Clock Out (UCB0CLK)
 * Author: Timothy Logan
*******************************************************************************/


//与FPGA相连接的SPI的初始化
void FPGA_SPI_Init(void)
{
	const eUSCI_SPI_MasterConfig FPGA_SPI =
	{
		EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
		3000000,                                   // SMCLK = DCO = 3MHZ
		500000,                                    // SPICLK = 500khz
		EUSCI_B_SPI_MSB_FIRST,                     // MSB First
		EUSCI_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
		EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // High polarity
		EUSCI_B_SPI_3PIN                          // 3Wire SPI Mode
	};
	    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
																						GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
	GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN0);
	SPI_initMaster(EUSCI_B0_BASE,&FPGA_SPI);
	SPI_enableModule(EUSCI_B0_BASE);
	
//	   /* Enabling interrupts */
//   SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
//    Interrupt_enableInterrupt(INT_EUSCIB0);
//    Interrupt_enableSleepOnIsrExit();
//    TXData = 0x01;
}


/*************************************************************************************************************************/
/*-------------------------------------------------SPI基础操作-----------------------------------------------------------*/
/*************************************************************************************************************************/
//向FPGA发送一??字节
void FPGA_Send_Byte(uint8_t DataByte)
{
		while(SPI_isBusy(EUSCI_B0_BASE))
    {
			//等待SPI空闲
    };
    SPI_transmitData(EUSCI_B0_BASE,DataByte);
    while(SPI_isBusy(EUSCI_B0_BASE))
    {
			//等待SPI空闲
    };
		SPI_receiveData(EUSCI_B0_BASE);
}

//从FPGA接受一??字节
uint8_t FPGA_Recive_Byte(void)
{
    FPGA_Send_Byte(0xFF);
    return SPI_receiveData(EUSCI_B0_BASE);
}
/*************************************************************************************************************************/
/*----------------------------------------------读写FPGA寄存器操作-------------------------------------------------------*/
/*************************************************************************************************************************/

//写一帧数据
void FPGA_Write_Reg(uint16_t address,uint16_t data)
{
	FPGA_NSS_Clr();
	FPGA_Send_Byte (0x02);//命令
	
	FPGA_Send_Byte (address>>8);
	FPGA_Send_Byte (address);//地址
	
	FPGA_Send_Byte (data>>8);
	FPGA_Send_Byte (data);//数据
	FPGA_NSS_Set();
}


//读一帧数据
uint16_t FPGA_Read_Reg(uint16_t address)
{
	uint16_t val = 0;
	FPGA_NSS_Clr();
	FPGA_Send_Byte (0x03);//命令
	FPGA_Send_Byte (address>>8);
	FPGA_Send_Byte (address);//地址
	val |= FPGA_Recive_Byte()<<8;	
	val |= FPGA_Recive_Byte();	
	FPGA_NSS_Set();
		return val;
}
