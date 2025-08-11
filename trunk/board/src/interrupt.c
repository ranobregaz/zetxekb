/***********************************************************************//** 
 * \file  interrupt.c
 * \brief  source file for interrypt handlers
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <soc.h>
#include <drv/gpio.h>
#include <drv/uart.h>
#include <drv/adc.h>
#include <drv/pin.h>

#include "board_config.h"
#include "csp.h"
/* externs function--------------------------------------------------------*/
extern void tick_irq_handler(void *arg);		//system coret 
/* private function--------------------------------------------------------*/

/* extern variablesr------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/

uint32_t g_wAdcData[16];
uint8_t g_byAdcDone;
/*************************************************************/
//CORET Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/

void CORETHandler(void) 
{
    // ISR content ...
	//CK801->CORET_CVR = 0x0;		// Clear counter and flag
	tick_irq_handler(NULL);
}

void SYSCONIntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wSysIntSta; 
	wSysIntSta = csp_syscon_get_int_st(SYSCON);
	
	if(wSysIntSta & (IWDT_INT))
	{
		nop;
		csp_syscon_int_clr(SYSCON, IWDT_INT);
		
	}
}

void IFCIntHandler(void) 
{	
    // ISR content ...
}

void ADCIntHandler(void) 
{
//	uint8_t i;
//	uint32_t tmp;
//    	
//	uint16_t wSeqMax = csp_adc_get_seq_num(ADC0);
//	tmp = csp_gpio_read_output_port(GPIOB0);
//	csp_gpio_write_output_port(GPIOB0, tmp ^ GPIO_PIN_MSK); //toggle PB02
//	
//	for(i=0;i<wSeqMax; i++)
//	{
//		g_wAdcData[i] = csp_adc_get_data(ADC0, i);
//	}
//	g_byAdcDone = 1;
	
	//csp_adc_clr_status(ADC0, wSeqMax);
		
}



void EPT0IntHandler(void) 
{	
    csp_ept_clr_int(EPT0, 0xffffffff);
}
void EPT0EMIntHandler(void)
{
	 // ISR content ...
}
void WWDTHandler(void)
{
	// ISR content ...
}
void GPT0IntHandler(void) 
{
    // ISR content ...
}
void RTCIntHandler(void) 
{
    // ISR content ...
}
void UART0IntHandler(void) 
{
    // ISR content ...
	//csi_uart_irqhandler(0);
}
void UART1IntHandler(void) 
{
    // ISR content ...
	//csi_uart_irqhandler(1);
}
void UART2IntHandler(void) 
{
    // ISR content ...
	//csi_uart_irqhandler(2);
}
void I2CIntHandler(void) 
{
    // ISR content ...
}
void SPI0IntHandler(void) 
{
    // ISR content ...

}
void SIO0IntHandler(void) 
{
   // ISR content ...
}
void EXI0IntHandler(void) 
{
	// ISR content ...
	volatile uint32_t wExiSta; 
	wExiSta = csp_exi_get_port_irq(SYSCON);
	
	switch(wExiSta)
	{
		case STATUS_EXI0:						//int group0
			break;
		case STATUS_EXI16:						//int group16
			break;
	}
	csp_exi_clr_port_irq(SYSCON,wExiSta);		//clear interrput 
   
}
void EXI1IntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wExiSta; 
	wExiSta = csp_exi_get_port_irq(SYSCON);
	
	switch(wExiSta)
	{
		case STATUS_EXI1:						//int group1
			break;
		case STATUS_EXI17:						//int group17
			break;
	}
	csp_exi_clr_port_irq(SYSCON,wExiSta);		//clear interrput 
}
void EXI2to3IntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wExiSta; 
	wExiSta = csp_exi_get_port_irq(SYSCON);
	
	switch(wExiSta)
	{
		case STATUS_EXI2:						//int group2
			break;
		case STATUS_EXI3:						//int group3
			break;
		case STATUS_EXI18:						//int group18
			break;
		case STATUS_EXI19:						//int group19
			break;
	}
	csp_exi_clr_port_irq(SYSCON,wExiSta);		//clear interrput 
}
void EXI4to9IntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wExiSta; 
	wExiSta = csp_exi_get_port_irq(SYSCON);
	
	switch(wExiSta)
	{
		case STATUS_EXI4:						//int group4
			break;
		case STATUS_EXI5:						//int group5
			break;
		case STATUS_EXI6:						//int group6
			break;
		case STATUS_EXI7:						//int group7
			break;
		case STATUS_EXI8:						//int group8
			break;
		case STATUS_EXI9:						//int group9
			break;
	}
	csp_exi_clr_port_irq(SYSCON,wExiSta);		//clear interrput 
}
void EXI10to15IntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wExiSta; 
	wExiSta = csp_exi_get_port_irq(SYSCON);
	
	switch(wExiSta)
	{
		case STATUS_EXI10:						//int grou10
			break;
		case STATUS_EXI11:						//int group11
			break;
		case STATUS_EXI12:						//int group12
			break;
		case STATUS_EXI13:						//int group13
			break;
		case STATUS_EXI14:						//int group14
			break;
		case STATUS_EXI15:						//int group15
			break;
	}
	csp_exi_clr_port_irq(SYSCON,wExiSta);		//clear interrput 
}

void CNTAIntHandler(void) 
{
    // ISR content ...

}
void TKEYIntHandler(void) 
{
    // ISR content ...
}

void LPTIntHandler(void) 
{
    // ISR content ...
}

void BT0IntHandler(void) 
{
    // ISR content ...
	volatile uint32_t wMisr = csp_bt_get_isr(BT0);
	
	if(wMisr & BT_PEND_INT)					//PEND interrupt
	{
		csp_bt_clr_isr(BT0, BT_PEND_INT);
		csi_pin_toggle(PA01);
		//csp_gpio_set_high(GPIOA0, 1);
	}
	
	if(wMisr & BT_CMP_INT)					//CMP interrupt
	{
		csp_bt_clr_isr(BT0, BT_CMP_INT);
		csi_pin_toggle(PA01);
		//csp_gpio_set_low(GPIOA0, 1);
	}
	
}

void BT1IntHandler(void) 
{
    // ISR content ...
	// ISR content ...
	volatile uint32_t wMisr = csp_bt_get_isr(BT1);
	
	if(wMisr & BT_PEND_INT)				//PEND interrupt
	{
		csp_bt_clr_isr(BT1, BT_PEND_INT);
		csi_pin_toggle(PA01);
		//csp_gpio_set_high(GPIOA0, 1);
	}
	
	if(wMisr & BT_CMP_INT)				//CMP interrupt
	{
		csp_bt_clr_isr(BT1, BT_CMP_INT);
		csi_pin_toggle(PA01);
		//csp_gpio_set_low(GPIOA0, 1);
	}
}
/*************************************************************/
/*************************************************************/
/*************************************************************/
void PriviledgeVioHandler(void) 
{
    // ISR content ...
}

void SystemDesPtr(void) 
{
    // ISR content ...
}

void MisalignedHandler(void) 
{
    // ISR content ...
}

void IllegalInstrHandler(void) 
{
    // ISR content ...
}

void AccessErrHandler(void) 
{
    // ISR content ...
}

void BreakPointHandler(void) 
{
    // ISR content ...
}

void UnrecExecpHandler(void) 
{
    // ISR content ...
}

void Trap0Handler(void) 
{
    // ISR content ...
}

void Trap1Handler(void) 
{
    // ISR content ...
}

void Trap2Handler(void) 
{
    // ISR content ...
}

void Trap3Handler(void) 
{
    // ISR content ...

}

void PendTrapHandler(void) 
{
    // ISR content ...
}

/******************* (C) COPYRIGHT 2020 APT Chip *****END OF FILE****/

