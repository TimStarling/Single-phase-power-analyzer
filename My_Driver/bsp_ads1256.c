#include "bsp_ads1256.h"



enum
{
	/* 寄存器地址， 后面是复位后缺省值 */
	REG_STATUS = 0,	// x1H
	REG_MUX    = 1, // 01H
	REG_ADCON  = 2, // 20H
	REG_DRATE  = 3, // F0H
	REG_IO     = 4, // E0H
	REG_OFC0   = 5, // xxH
	REG_OFC1   = 6, // xxH
	REG_OFC2   = 7, // xxH
	REG_FSC0   = 8, // xxH
	REG_FSC1   = 9, // xxH
	REG_FSC2   = 10, // xxH
};

/* 命令定义： TTable 24. Command Definitions --- ADS1256数据手册第34页 */
enum
{
	CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
	CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
	CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
	CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
	CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
	CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
	CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
	CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
	CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
	CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
	CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
	CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
	CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
	CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
};


void ADS1256_SPI_INT(void)
{
	GPIO_setAsOutputPin(ADC_SPI_SCK_GPIO_PORT, ADC_SPI_SCK_PIN);
	GPIO_setAsOutputPin(ADC_SPI_MOSI_GPIO_PORT, ADC_SPI_MOSI_PIN);
	GPIO_setAsInputPin(ADC_SPI_MISO_GPIO_PORT, ADC_SPI_MISO_PIN);
	GPIO_setAsInputPin(ADC_DRDY_GPIO_PORT, ADC_DRDY_PIN);
	GPIO_setAsOutputPin(ADC_SPI_CS_GPIO_PORT, ADC_SPI_CS_PIN);
	
//    /*定义一个GPIO_InitTypeDef类型的结构体*/
//    GPIO_InitTypeDef GPIO_InitStructure;

//    /*开启控制led的GPIO的端口时钟*/
//    RCC_AHB1PeriphClockCmd(ADC_DRDY_GPIO_CLK|ADC_SPI_CS_GPIO_CLK|ADC_SPI_MOSI_GPIO_CLK|ADC_SPI_MISO_GPIO_CLK|ADC_SPI_SCK_GPIO_CLK, ENABLE);

///*SCK配置*/
//    GPIO_InitStructure.GPIO_Pin = ADC_SPI_SCK_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(ADC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

///*MOSI配置*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_MOSI_PIN;
//		GPIO_Init(ADC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
//	
///*CS配置*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_CS_PIN;
//		GPIO_Init(ADC_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
//		
///*MISO配置*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_MISO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//    GPIO_Init(ADC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
//		
///*DRDY配置*/
//    GPIO_InitStructure.GPIO_Pin = ADC_DRDY_PIN;
//    GPIO_Init(ADC_DRDY_GPIO_PORT, &GPIO_InitStructure);
		
    SPI_ADC_CS_HIGH();
    SPI_ADC_MOSI_LOW();
    SPI_ADC_SCK_LOW();
		
}




void ADS1256_SPI_TXDATA(u8 TXDATA)
{
	  u8 i;
	ADS1256_Delay(2);
    for (i = 0; i < 8; i++) // 写8位数据
    {
        if (TXDATA & 0x80) // 准备数据
            SPI_ADC_MOSI_HIGH();
        else
            SPI_ADC_MOSI_LOW();
				
        SPI_ADC_SCK_HIGH(); // 操作时钟
        ADS1256_Delay(1);
				TXDATA <<= 1;
        SPI_ADC_SCK_LOW();
        ADS1256_Delay(1);
    }
}

uint8_t ADS1256_SPI_RXDATA(void)
{
    int i;
    u16 RXDATA = 0;
    for (i = 7; i >= 0 ; i--) // 写8位数据
    {
        SPI_ADC_SCK_HIGH(); // 操作时钟
        ADS1256_Delay(1);
        SPI_ADC_SCK_LOW();
				RXDATA |= SPI_MISO_READ() << i;
        ADS1256_Delay(1);
    }
    return RXDATA;

}


uint8_t ADS1256_ReadReg(uint8_t _RegID)
{
	uint8_t read;

	SPI_ADC_CS_LOW();	/* SPI片选 = 0 */
	ADS1256_SPI_TXDATA(CMD_RREG | _RegID);	/* 写寄存器的命令, 并发送寄存器地址 */
	ADS1256_SPI_TXDATA(0x00);	/* 寄存器个数 - 1, 此处读1个寄存器 */

	ADS1256_Delay(5);	/* 必须延迟才能读取芯片返回数据 */

	read = ADS1256_SPI_RXDATA();	/* 读寄存器值 */
	SPI_ADC_CS_HIGH();	/* SPI片选 = 1 */

	return read;
}

uint8_t ADS1256_ReadChipID(void)
{
	uint8_t id;

	ADS1256_WaitDRDY();
	id = ADS1256_ReadReg(REG_STATUS);
	return (id >> 4);
}

/*
*********************************************************************************************************
*	函 数 名: ADS1256_CfgADC
*	功能说明: 配置ADC参数，增益和数据输出速率
*	形    参: 
*ainp : 通道
*gain : 增益    
Bits7  0

Bits 6-5
00 = Clock Out OFF
01 = Clock Out Frequency = fCLKIN (default)
10 = Clock Out Frequency = fCLKIN/2
11 = Clock Out Frequency = fCLKIN/4

Bits 4-2 
00 = Sensor Detect OFF (default)
01 = Sensor Detect Current = 0.5μA
10 = Sensor Detect Current = 2μA
11 = Sensor Detect Current = 10μA

Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
000 = 1 (default)
001 = 2
010 = 4
011 = 8
100 = 16
101 = 32
110 = 64
111 = 64

*drate : 数据输出速率    
11110000 = 30,000SPS (default)
11100000 = 15,000SPS
11010000 = 7,500SPS
11000000 = 3,750SPS
10110000 = 2,000SPS
10100001 = 1,000SPS
10010010 = 500SPS
10000010 = 100SPS
01110010 = 60SPS
01100011 = 50SPS
01010011 = 30SPS
01000011 = 25SPS
00110011 = 15SPS
00100011 = 10SPS
00010011 = 5SPS
00000011 = 2.5SPS
*	返 回 值: 无
*********************************************************************************************************
*/
void ADS1256_cfg(u8 ainp,u8 gain, u8 drate)	
{
	ADS1256_WaitDRDY();
	{
		uint8_t buf[4];		/* 暂存ADS1256 寄存器配置参数，之后连续写4个寄存器 */
		buf[0] = (0 << 3) | (1 << 2) | (1 << 1);
		buf[1] = 0x08|(ainp << 4);	/* 高四位0表示AINP接 AIN0,  低四位8表示 AINN 固定接 AINCOM */
		buf[2] = gain;
		//ADS1256_WriteReg(REG_ADCON, (0 << 5) | (0 << 3) | (GAIN_1 << 0));	/* 选择1;1增益, 输入正负5V */

		/* 因为切换通道和读数据耗时 123uS, 因此扫描中断模式工作时，最大速率 = DRATE_1000SPS */
		buf[3] = drate;	// DRATE_10SPS;	/* 选择数据输出速率 */
		
		SPI_ADC_CS_LOW();	/* SPI片选 = 0 */
		ADS1256_SPI_TXDATA(CMD_WREG | 0);	/* 写寄存器的命令, 并发送寄存器地址 */
		ADS1256_SPI_TXDATA(0x03);			/* 寄存器个数 - 1, 此处3表示写4个寄存器 */

		ADS1256_SPI_TXDATA(buf[0]);	/* 设置状态寄存器 */
		ADS1256_SPI_TXDATA(buf[1]);	/* 设置输入通道参数 */
		ADS1256_SPI_TXDATA(buf[2]);	/* 设置ADCON控制寄存器，增益 */
		ADS1256_SPI_TXDATA(buf[3]);	/* 设置输出数据速率 */

		SPI_ADC_CS_HIGH();	/* SPI片选 = 1 */
	}

	ADS1256_Delay(5);
	ADS1256_WaitDRDY();
}

/*
*********************************************************************************************************
*	函 数 名: ADS1256_ReadData
*	功能说明: 读ADC数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t ADS1256_ReadData(void)
{
	uint32_t read = 0;

	SYSTICK_STOP();
	
	SPI_ADC_CS_LOW();
	ADS1256_SPI_TXDATA(CMD_RDATA);
	ADS1256_Delay(5);
	/* 读采样结果，3个字节，高字节在前 */
	read = ADS1256_SPI_RXDATA() << 16;
	read += (ADS1256_SPI_RXDATA() << 8);
	read += ADS1256_SPI_RXDATA();
	SPI_ADC_CS_HIGH();
	
	SYSTICK_START();
	/* 负数进行扩展。24位有符号数扩展为32位有符号数 */
	if (read & 0x800000)
	{
		read += 0xFF000000;
	}

	return (int32_t)read;
}












void ADS1256_WaitDRDY(void)
{
	uint32_t i;

	for (i = 0; i < 40000000; i++)
	{
		if (DRDY_READ() == 0)
		{
			break;
		}
	}
	if (i >= 40000000)
	{
//		printf("ADS1256_WaitDRDY() Time Out ...\r\n");		/* 调试语句. 用语排错 */
		;
	}
}

void ADS1256_Delay(uint32_t Num)
{
	volatile u16 i,j;
	for(j = 0 ; j <Num ; j++)
	for(i = 0; i< 15;i++);

}





