/****************************************************/
// MSP432P401R
// 定时器32
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/9/8
/****************************************************/

//Timer_32时钟为48MHz

#include "tim32.h"

void Tim32_0_Int_Init(uint32_t aar, uint32_t psc)//溢出中断时间48e6/(arr*psc)，理论上是这样，实际好像不是
{
    MAP_Timer32_initModule(TIMER32_0_BASE, psc, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_setCount(TIMER32_0_BASE, aar);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false); //连续计数模式 false
    MAP_Interrupt_disableInterrupt(INT_T32_INT1);
}

/* Timer32 ISR */

/*FFT采样*/
void T32_INT1_IRQHandler(void)
{

  MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);
	
    /*开始填充用户代码*/
//	ADC14_toggleConversionTrigger();

//	FFT_Vdatabuf[2*SampleNum] = ADS8691_read1();
//	FFT_Vdatabuf[2*SampleNum+1] = 0;
	
	Vol_Val[SampleNum] = ADS8691_read();
	Iol_Val[SampleNum] = ADS8691_read1();
	SampleNum++;
	LED1_OC(2);

    /*结束填充用户代码*/
}

void Tim32_1_Int_Init(uint32_t aar, uint32_t psc)
{
    MAP_Timer32_initModule(TIMER32_1_BASE, psc, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_setCount(TIMER32_1_BASE, aar);
    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);
    MAP_Timer32_startTimer(TIMER32_1_BASE, false); //连续计数模式 false
    MAP_Interrupt_enableInterrupt(INT_T32_INT2);
}

/* Timer32 ISR */
void T32_INT2_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE);

    /*开始填充用户代码*/

    /*结束填充用户代码*/
}
