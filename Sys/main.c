#include "main.h"

	u8 i =0;
int main(void)
{
	SysInit();
	USART_HMI_Init(921600);
	uart1_init(115200);
	Tim32_0_Int_Init(48e6,0);//默认溢出中断间隔为1秒
//	FPGA_SPI_Init();
	delay_init();
	LED1_Init();
	LED2_Init();
	ADC_Config();
	Interrupt_enableMaster();
	/*************/

	ADS8691_Init();
	ADS8691_Init1();
	Relay_Init();
	char Lcd_Buffer[50] = {0};

	while(1)
	{
		Power_Cal();
		USART_HMI_DATA();
	if(Relay_Auto == true)
	{
		if(P.I >= 2&&Relay_Open != 0)
		{
			Relay_Open = 0;
			Relay_Select();
			if(page_number == 0)
			{
				sprintf(Lcd_Buffer,"t4.txt=\"6\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
				sprintf(Lcd_Buffer,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
			}
		}
		else if(P.I >= 0.5&&P.I <= 2&&Relay_Open != 1)
		{
			Relay_Open = 1;
			Relay_Select();
			if(page_number == 0)
			{
				sprintf(Lcd_Buffer,"t4.txt=\"2\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
				sprintf(Lcd_Buffer,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
			}
		}
		else if(P.I >= 0.2&&P.I <= 0.5&&Relay_Open != 2)
		{
			Relay_Open = 2;
			Relay_Select();
			if(page_number == 0)
			{
				sprintf(Lcd_Buffer,"t4.txt=\"500\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
				sprintf(Lcd_Buffer,"t5.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
			}
		}
		else if(P.I < 0.2&&Relay_Open != 3)
		{
			Relay_Open = 3;
			Relay_Select();
			if(page_number == 0)
			{
				sprintf(Lcd_Buffer,"t4.txt=\"200\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
				sprintf(Lcd_Buffer,"t5.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
			}
		}
//		else if(Relay_Open != 0)
//		{
//			Relay_Open = 0;
//			Relay_Select();
//			sprintf(Lcd_Buffer,"t4.txt=\"6\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
//			sprintf(Lcd_Buffer,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(Lcd_Buffer);
//		}
	}
		delay_ms(500);
	}
}

				