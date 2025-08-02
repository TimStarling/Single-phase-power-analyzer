#include "Power.h"

u8 Relay_Open = 0;
char hdmi_Lcd[50] = {0};
struct Power_Data P;
bool Relay_Flag = true;
bool Relay_Auto = false;
u8 flag = 1;
u8 Page0_Change = 0;


void Power_Cal(void)
{
//	double jiaodu = 0;
	float I_max_v = 0;
	double Sum = 0;
	FFT_Transform();
//	I_max_v = Find_Max(I_Data,FFT_LENGTH);
//				sprintf(hdmi_Lcd,"t0.txt=\"%-.4f\"\xff\xff\xff",I_max_v);USART_HMI_SendString(hdmi_Lcd);
	P.Vol = (Find_Max(Vol_Data,FFT_LENGTH)*1.0f-Find_Min(Vol_Data,FFT_LENGTH)*1.0f)/(2.0f*sqrt(2));
	P.Fre = FFT_Freqbuf[1];
	P.Pow_Pos = Pow_Pos_Cul(Vol_Data,I_Data);
	P.Pow_Neg = sqrt(P.Pow_App*P.Pow_App-P.Pow_Pos*P.Pow_Pos);
	P.THD = 0;
	for(u8 n = 1;n<THD_Len;n++)
	{
		P.THD += P.Harmonic[n]*P.Harmonic[n];
	}
	for(u16 i=0 ;i<FFT_LENGTH;i++)
	{
		Sum+=((I_Data[i])*(I_Data[i]));
//		if(I_Data[i]>=0)
//			Sum+=I_Data[i];
//		else
//			Sum+=(-I_Data[i]);
	}
//	P.I = Sum*1.0f/FFT_LENGTH;
//	P.I = I_Coe_150(sqrt(Sum*1.0f/FFT_LENGTH));;
	
	P.I = (sqrt(Sum*1.0f/FFT_LENGTH));
	P.THD = sqrt(P.THD)/FFT_VoltageBuf[1];
	P.Pow_App = P.I * P.Vol;
	P.Factor = P.Pow_Pos/P.Pow_App-0.01;
	if(P.Factor > 1)
		P.Factor=0.9994;
//	jiaodu = acos((double)P.Factor);
//	jiaodu -= 0.1/20.0*2.0*paiai;
//	P.Factor = cos(P.Factor);
}

float Find_Max(float *Buffer,uint16_t Size)
{
	float temp = 0;
	for(uint16_t o = 0;o < Size;o++)
	{
		if(Buffer[o]>0)
		{
			if(Buffer[o] > temp)
				temp = Buffer[o];
		}
//		else
//		{
//			if((-1.0f*Buffer[o]) > temp)
//				temp = -Buffer[o];
//		}
	}
	return temp;
}

float Find_Min(float *Buffer,uint16_t Size)
{
	float temp = 0;
	for(uint16_t o = 0;o < Size;o++)
	{
		if(Buffer[o]<0)
//		{
//			if(Buffer[o] < temp)
//				temp = Buffer[o];
//		}
//		else
		{
			if((Buffer[o]) < temp)
				temp = Buffer[o];
		}
	}
	return temp;
}


double Pow_Pos_Cul(float* V,float* I)
{
	double WATT = 0;
	for(uint16_t p = 0;p<FFT_LENGTH-1;p++)
	{
		WATT += 1.0f*V[p+1]*I[p]/FFT_Fs_temp;//积分
	}
	return WATT*1.0f/(FFT_LENGTH*1.0f/FFT_Fs_temp);
}

void I_Signal(void)
{
	uint16_t i;
	USART_HMI_Printf("addt s0.id,0,256\xff\xff\xff");
			delay_ms(1);
	{
		for(i=0;i<256;i++)
		{
			USART_HMI_Printf("%c",(u8)(I_Data[4*i]*8.0+128));
		}
			delay_ms(1);
	}
 USART_HMI_Printf("\x01\xff\xff\xff");
}

void V_Signal(void)
{
	uint16_t i;
	USART_HMI_Printf("addt s0.id,1,256\xff\xff\xff");
		delay_ms(2);
	
	{
		for(i=0;i<256;i++)
		{
			USART_HMI_Printf("%c",(u8)(Vol_Data[4*i]/2.8+128));
		}
			delay_ms(1);
	}
 USART_HMI_Printf("\x01\xff\xff\xff");
}

void H_Signal(void)
{
	uint16_t i;
	USART_HMI_Printf("addt s0.id,0,256\xff\xff\xff");
			delay_ms(2);
	{
		for(i=1;i<257;i++)
		{
			USART_HMI_Printf("%c",(u8)((FFT_Draw[i])*30));
		}
			delay_ms(1);
	}
	USART_HMI_Printf("\x01\xff\xff\xff");
	
}

void USART_HMI_DATA(void)
{


	USART_HMI_ITRecieveWord();

	switch(page_number)
	{
		case 0:
			if((Relay_Flag == true&&Relay_Auto == false)||Page0_Change)
			{
				Relay_Select();Relay_Flag = false;
				switch(Relay_Open)
				{
					case 0:sprintf(hdmi_Lcd,"t4.txt=\"6\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
						   sprintf(hdmi_Lcd,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
					break;
					case 1:sprintf(hdmi_Lcd,"t4.txt=\"2\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
						   sprintf(hdmi_Lcd,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
					break;
					case 2:sprintf(hdmi_Lcd,"t4.txt=\"500\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
						   sprintf(hdmi_Lcd,"t5.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
					break;
					case 3:sprintf(hdmi_Lcd,"t4.txt=\"200\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
						   sprintf(hdmi_Lcd,"t5.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
					break;
				}
			}
			Page0_Change = 0;
			if(P.I > 0.8)
			{
	  	  sprintf(hdmi_Lcd,"t3.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
				sprintf(hdmi_Lcd,"t2.txt=\"%-.2f\"\xff\xff\xff",P.I);USART_HMI_SendString(hdmi_Lcd); 
			}
			else
			{
	  	  sprintf(hdmi_Lcd,"t3.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
				sprintf(hdmi_Lcd,"t2.txt=\"%-.2f\"\xff\xff\xff",1000.0*P.I);USART_HMI_SendString(hdmi_Lcd); 
			}
			break;
		case 1:				
			UART_disableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.关闭串口相关中断
			V_Signal();I_Signal();
		
			sprintf(hdmi_Lcd,"t1.txt=\"%-.3f\"\xff\xff\xff",P.Vol);USART_HMI_SendString(hdmi_Lcd); 
			if(P.I > 0.8)
			{
	  	  sprintf(hdmi_Lcd,"t5.txt=\"A\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
				sprintf(hdmi_Lcd,"t4.txt=\"%-.3f\"\xff\xff\xff",P.I);USART_HMI_SendString(hdmi_Lcd); 
			}
			else
			{
	  	  sprintf(hdmi_Lcd,"t5.txt=\"mA\"\xff\xff\xff");USART_HMI_SendString(hdmi_Lcd);
				sprintf(hdmi_Lcd,"t4.txt=\"%-.2f\"\xff\xff\xff",1000.0*P.I);USART_HMI_SendString(hdmi_Lcd); 
			}
			USART_HMI_ITRecieveWord();
		break;
		case 2:
			sprintf(hdmi_Lcd,"t4.txt=\"%-.3f\"\xff\xff\xff",P.Factor);USART_HMI_SendString(hdmi_Lcd); 
			sprintf(hdmi_Lcd,"t5.txt=\"%-.3f\"\xff\xff\xff",P.Pow_Neg);USART_HMI_SendString(hdmi_Lcd); 
			sprintf(hdmi_Lcd,"t6.txt=\"%-.3f\"\xff\xff\xff",P.Pow_Pos);USART_HMI_SendString(hdmi_Lcd); 
			sprintf(hdmi_Lcd,"t7.txt=\"%-.3f\"\xff\xff\xff",P.Pow_App);USART_HMI_SendString(hdmi_Lcd); 
		break;
		case 3:
			for(u8 i = 0;i<10;i++){
			sprintf(hdmi_Lcd,"t%d.txt=\"%-.4f\"\xff\xff\xff",20+i,P.Harmonic[i]);USART_HMI_SendString(hdmi_Lcd); 
			sprintf(hdmi_Lcd,"t%d.txt=\"%-.4f\"\xff\xff\xff",50+i,P.Harmonic[10+i]);USART_HMI_SendString(hdmi_Lcd); 
			}
		break;
		case 4:
			for(u8 i = 0;i<10;i++){
			sprintf(hdmi_Lcd,"t%d.txt=\"%-.4f\"\xff\xff\xff",20+i,P.Harmonic[20+i]);USART_HMI_SendString(hdmi_Lcd); 
			sprintf(hdmi_Lcd,"t%d.txt=\"%-.4f\"\xff\xff\xff",50+i,P.Harmonic[30+i]);USART_HMI_SendString(hdmi_Lcd); 
			}
		break;
		case 5:				
			UART_disableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.关闭串口相关中断
			H_Signal();
			sprintf(hdmi_Lcd,"t1.txt=\"%-.2f\"\xff\xff\xff",P.THD*100);USART_HMI_SendString(hdmi_Lcd); 
			USART_HMI_ITRecieveWord();
		break;
		default:break;
					USART_HMI_ITRecieveWord();

	}
}
void Relay_Init(void)
{
	GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN5);
	GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN6);
	GPIO_setAsOutputPin(GPIO_PORT_P6,GPIO_PIN5);
	GPIO_setAsOutputPin(GPIO_PORT_P6,GPIO_PIN4);
	
	GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5);
	GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
}
void Relay_Select(void)
{
	GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5);
	GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN5);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6,GPIO_PIN4);
	
	switch(Relay_Open)
	{
		case 1:GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN5);break;
		case 2:GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN6);break;
		case 3:GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN5);break;
		case 0:GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN4);break;
		default :break;
	}
}



