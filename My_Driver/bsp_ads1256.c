#include "bsp_ads1256.h"



enum
{
	/* �Ĵ�����ַ�� �����Ǹ�λ��ȱʡֵ */
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

/* ����壺 TTable 24. Command Definitions --- ADS1256�����ֲ��34ҳ */
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
	
//    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
//    GPIO_InitTypeDef GPIO_InitStructure;

//    /*��������led��GPIO�Ķ˿�ʱ��*/
//    RCC_AHB1PeriphClockCmd(ADC_DRDY_GPIO_CLK|ADC_SPI_CS_GPIO_CLK|ADC_SPI_MOSI_GPIO_CLK|ADC_SPI_MISO_GPIO_CLK|ADC_SPI_SCK_GPIO_CLK, ENABLE);

///*SCK����*/
//    GPIO_InitStructure.GPIO_Pin = ADC_SPI_SCK_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(ADC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

///*MOSI����*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_MOSI_PIN;
//		GPIO_Init(ADC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
//	
///*CS����*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_CS_PIN;
//		GPIO_Init(ADC_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
//		
///*MISO����*/
//		GPIO_InitStructure.GPIO_Pin = ADC_SPI_MISO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//    GPIO_Init(ADC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
//		
///*DRDY����*/
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
    for (i = 0; i < 8; i++) // д8λ����
    {
        if (TXDATA & 0x80) // ׼������
            SPI_ADC_MOSI_HIGH();
        else
            SPI_ADC_MOSI_LOW();
				
        SPI_ADC_SCK_HIGH(); // ����ʱ��
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
    for (i = 7; i >= 0 ; i--) // д8λ����
    {
        SPI_ADC_SCK_HIGH(); // ����ʱ��
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

	SPI_ADC_CS_LOW();	/* SPIƬѡ = 0 */
	ADS1256_SPI_TXDATA(CMD_RREG | _RegID);	/* д�Ĵ���������, �����ͼĴ�����ַ */
	ADS1256_SPI_TXDATA(0x00);	/* �Ĵ������� - 1, �˴���1���Ĵ��� */

	ADS1256_Delay(5);	/* �����ӳٲ��ܶ�ȡоƬ�������� */

	read = ADS1256_SPI_RXDATA();	/* ���Ĵ���ֵ */
	SPI_ADC_CS_HIGH();	/* SPIƬѡ = 1 */

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
*	�� �� ��: ADS1256_CfgADC
*	����˵��: ����ADC����������������������
*	��    ��: 
*ainp : ͨ��
*gain : ����    
Bits7  0

Bits 6-5
00 = Clock Out OFF
01 = Clock Out Frequency = fCLKIN (default)
10 = Clock Out Frequency = fCLKIN/2
11 = Clock Out Frequency = fCLKIN/4

Bits 4-2 
00 = Sensor Detect OFF (default)
01 = Sensor Detect Current = 0.5��A
10 = Sensor Detect Current = 2��A
11 = Sensor Detect Current = 10��A

Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
000 = 1 (default)
001 = 2
010 = 4
011 = 8
100 = 16
101 = 32
110 = 64
111 = 64

*drate : �����������    
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
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADS1256_cfg(u8 ainp,u8 gain, u8 drate)	
{
	ADS1256_WaitDRDY();
	{
		uint8_t buf[4];		/* �ݴ�ADS1256 �Ĵ������ò�����֮������д4���Ĵ��� */
		buf[0] = (0 << 3) | (1 << 2) | (1 << 1);
		buf[1] = 0x08|(ainp << 4);	/* ����λ0��ʾAINP�� AIN0,  ����λ8��ʾ AINN �̶��� AINCOM */
		buf[2] = gain;
		//ADS1256_WriteReg(REG_ADCON, (0 << 5) | (0 << 3) | (GAIN_1 << 0));	/* ѡ��1;1����, ��������5V */

		/* ��Ϊ�л�ͨ���Ͷ����ݺ�ʱ 123uS, ���ɨ���ж�ģʽ����ʱ��������� = DRATE_1000SPS */
		buf[3] = drate;	// DRATE_10SPS;	/* ѡ������������� */
		
		SPI_ADC_CS_LOW();	/* SPIƬѡ = 0 */
		ADS1256_SPI_TXDATA(CMD_WREG | 0);	/* д�Ĵ���������, �����ͼĴ�����ַ */
		ADS1256_SPI_TXDATA(0x03);			/* �Ĵ������� - 1, �˴�3��ʾд4���Ĵ��� */

		ADS1256_SPI_TXDATA(buf[0]);	/* ����״̬�Ĵ��� */
		ADS1256_SPI_TXDATA(buf[1]);	/* ��������ͨ������ */
		ADS1256_SPI_TXDATA(buf[2]);	/* ����ADCON���ƼĴ��������� */
		ADS1256_SPI_TXDATA(buf[3]);	/* ��������������� */

		SPI_ADC_CS_HIGH();	/* SPIƬѡ = 1 */
	}

	ADS1256_Delay(5);
	ADS1256_WaitDRDY();
}

/*
*********************************************************************************************************
*	�� �� ��: ADS1256_ReadData
*	����˵��: ��ADC����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t ADS1256_ReadData(void)
{
	uint32_t read = 0;

	SYSTICK_STOP();
	
	SPI_ADC_CS_LOW();
	ADS1256_SPI_TXDATA(CMD_RDATA);
	ADS1256_Delay(5);
	/* �����������3���ֽڣ����ֽ���ǰ */
	read = ADS1256_SPI_RXDATA() << 16;
	read += (ADS1256_SPI_RXDATA() << 8);
	read += ADS1256_SPI_RXDATA();
	SPI_ADC_CS_HIGH();
	
	SYSTICK_START();
	/* ����������չ��24λ�з�������չΪ32λ�з����� */
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
//		printf("ADS1256_WaitDRDY() Time Out ...\r\n");		/* �������. �����Ŵ� */
		;
	}
}

void ADS1256_Delay(uint32_t Num)
{
	volatile u16 i,j;
	for(j = 0 ; j <Num ; j++)
	for(i = 0; i< 15;i++);

}





