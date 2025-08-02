#ifndef _AD9959_H_
#define _AD9959_H_
#include <driverlib.h>
#include "stdint.h"

// AD9959�ܽź궨��

#define AD9959_PWR BITBAND_PERI(P10OUT, 5)
#define Reset BITBAND_PERI(P10OUT, 3)

#define UPDATE BITBAND_PERI(P10OUT, 1)
#define AD9959_CS BITBAND_PERI(P7OUT, 7)
#define AD9959_SCLK BITBAND_PERI(P7OUT, 5)

#define PS0 BITBAND_PERI(P10OUT, 4)
#define PS1 BITBAND_PERI(P10OUT, 2)
#define PS2 BITBAND_PERI(P10OUT, 0)
#define PS3 BITBAND_PERI(P7OUT, 6)

#define SDIO0 BITBAND_PERI(P7OUT, 4)
#define SDIO1 BITBAND_PERI(P8OUT, 0)
#define SDIO2 BITBAND_PERI(P9OUT, 6)
#define SDIO3 BITBAND_PERI(P9OUT, 7)

// AD9959�Ĵ�����ַ����
#define CSR_ADD 0x00 // CSR ͨ��ѡ��Ĵ���
#define FR1_ADD 0x01 // FR1 ���ܼĴ���1
#define FR2_ADD 0x02 // FR2 ���ܼĴ���2
#define CFR_ADD 0x03 // CFR ͨ�����ܼĴ���

#define CFTW0_ADD 0x04 // CTW0 ͨ��Ƶ��ת���ּĴ���
#define CPOW0_ADD 0x05 // CPW0 ͨ����λת���ּĴ���
#define ACR_ADD 0x06   // ACR ���ȿ��ƼĴ���

#define LSRR_ADD 0x07 // LSR ����ɨ��б�ʼĴ���
#define RDW_ADD 0x08  // RDW ����ɨ�������Ĵ���
#define FDW_ADD 0x09  // FDW �½�ɨ�������Ĵ���

#define PROFILE_ADDR_BASE 0x0A // Profile�Ĵ���,�����ļ��Ĵ�����ʼ��ַ

// CSR[7:4]ͨ��ѡ������λ
#define CH0 0x10
#define CH1 0x20
#define CH2 0x40
#define CH3 0x80

// FR1[9:8] ���Ƶ�ƽѡ��λ
#define LEVEL_MOD_2 0x00  // 2��ƽ���� 2�׵���
#define LEVEL_MOD_4 0x01  // 4��ƽ����	4�׵���
#define LEVEL_MOD_8 0x02  // 8��ƽ����	8�׵���
#define LEVEL_MOD_16 0x03 // 16��ƽ����	16�׵���

// CFR[23:22]  ��Ƶ��λ��AFP��ѡ��λ
#define DISABLE_Mod 0x00 // 00	�����ѽ���
#define ASK 0x40         // 01	������ƣ����Ƽ���
#define FSK 0x80         // 10	Ƶ�ʵ��ƣ�Ƶ�Ƽ���
#define PSK 0xc0         // 11	��λ���ƣ����Ƽ���

// ��CFR[14]������ɨ������ sweep enable
#define SWEEP_ENABLE 0x40  // 1	����
#define SWEEP_DISABLE 0x00 // 0	������

void delay1(uint32_t length); // ��ʱ
void IntReset(void);          // AD9959��λ
void IO_Update(void);         // AD9959��������
void Intserve(void);          // IO�ڵ�ƽ״̬��ʼ��
void AD9959_Init(void);       // IO�ڳ�ʼ��

/***********************AD9959�����Ĵ�����������*****************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData); // ��AD9959д����
void Write_CFTW0(uint32_t fre);                                                                   // дCFTW0ͨ��Ƶ��ת���ּĴ���
void Write_ACR(uint16_t Ampli);                                                                   // дACRͨ������ת���ּĴ���
void Write_CPOW0(uint16_t Phase);                                                                 // дCPOW0ͨ����λת���ּĴ���

void Write_LSRR(uint8_t rsrr, uint8_t fsrr); // дLSRR����ɨ��б�ʼĴ���
void Write_RDW(uint32_t r_delta);            // дRDW���������Ĵ���
void Write_FDW(uint32_t f_delta);            // дFDW�½������Ĵ���

void Write_Profile_Fre(uint8_t profile, uint32_t data);   // дProfile�Ĵ���,Ƶ��
void Write_Profile_Ampli(uint8_t profile, uint16_t data); // дProfile�Ĵ���,����
void Write_Profile_Phase(uint8_t profile, uint16_t data); // дProfile�Ĵ���,��λ
/********************************************************************************************/

/*****************************��Ƶ��������***********************************/
void AD9959_Set_Fre(uint8_t Channel, uint32_t Freq);    // дƵ��
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli);   // д����
void AD9959_Set_Phase(uint8_t Channel, uint16_t Phase); // д��λ
/****************************************************************************/

/*****************************���Ʋ�������  ***********************************/
void AD9959_Modulation_Init(uint8_t Channel, uint8_t Modulation, uint8_t Sweep_en, uint8_t Nlevel); // ���ø���ͨ���ĵ���ģʽ��
void AD9959_SetFSK(uint8_t Channel, uint32_t *data, uint16_t Phase);                                // ����FSK���ƵĲ���
void AD9959_SetASK(uint8_t Channel, uint16_t *data, uint32_t fre, uint16_t Phase);                  // ����ASK���ƵĲ���
void AD9959_SetPSK(uint8_t Channel, uint16_t *data, uint32_t Freq);                                 // ����PSK���ƵĲ���

void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data, uint32_t e_data, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint16_t Ampli, uint16_t Phase); // ��������ɨƵ�Ĳ���
void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli, uint16_t e_Ampli, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t fre, uint16_t Phase); // ��������ɨ���Ĳ���
void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data, uint16_t e_data, uint16_t r_delta, uint16_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t fre, uint16_t Ampli); // ��������ɨ��Ĳ���
/********************************************************************************************/

#endif
