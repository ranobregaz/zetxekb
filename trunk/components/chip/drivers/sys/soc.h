
 
/***********************************************************************//** 
 * \file  soc.h
 * \brief  CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/  

#ifndef _SOC_H_
#define _SOC_H_

#include <stdint.h>
#include <csi_core.h>
#include <drv/clk.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef ISOSC_VALU
#define ISOSC_VALUE			27000U
#endif

#ifndef EMOSC_32K_VALUE
#define EMOSC_32K_VALUE		32768U
#endif

#ifndef IMOSC_5M_VALUE
#define IMOSC_5M_VALUE		5556000U
#endif
#ifndef IMOSC_4M_VALUE
#define IMOSC_4M_VALUE      4194000U
#endif
#ifndef IMOSC_2M_VALUE
#define IMOSC_2M_VALUE      2094000U
#endif
#ifndef IMOSC_131K_VALUE
#define IMOSC_131K_VALUE	131000U
#endif

#ifndef HFOSC_48M_VALUE
#define HFOSC_48M_VALUE     48000000U
#endif
#ifndef HFOSC_24M_VALUE
#define HFOSC_24M_VALUE     24000000U
#endif
#ifndef HFOSC_12M_VALUE
#define HFOSC_12M_VALUE     12000000U
#endif
#ifndef HFOSC_6M_VALUE
#define HFOSC_6M_VALUE      6000000U
#endif

//UARTx id number
#define UART_IDX_NUM   		3
#define UART_RECV_MAX_LEN	256

//FLASH
#define PFLASH_PAGES		256
#define DFLASH_PAGES		32

typedef enum {
	
	CORET_IRQn						= 0U,						 					
	SYSCON_IRQn						= 1U,
	IFC_IRQn						= 2U,						 					
	ADC_IRQn						= 3U,
	EPT0_IRQn						= 4U,
	//DUMMY0_IRQn      				= 5U,
	WWDT_IRQn						= 6U,	
	EXI0_IRQn						= 7U,
	EXI1_IRQn						= 8U,
	GPT0_IRQn						= 9U,
	//DUMMY1_IRQn      				= 10U,
	//DUMMY2_IRQn      				= 11U,
	RTC_IRQn     					= 12U,
	UART0_IRQn    					= 13U,
	UART1_IRQn    					= 14U,
	UART2_IRQn      				= 15U,
	//DUMMY3_IRQn      				= 16U,
	I2C_IRQn     					= 17U,
	//DUMMY4_IRQn					= 18U,
	SPI_IRQn    					= 19U,
	SIO_IRQn						= 20U,
	EXI2_IRQn    					= 21U,
	EXI3_IRQn    					= 22U,
	EXI4_IRQn    					= 23U,
	CNTA_IRQn						= 24U,
	TKEY_IRQn    					= 25U,
	LPT_IRQn      					= 26U,
	//DUMMY5_IRQn      				= 27U,
	BT0_IRQn     					= 28U,
	BT1_IRQn     					= 29U,
	//DUMMY6_IRQn      				= 30U,
	//DUMMY7_IRQn      				= 31U,
} irqn_type_e;

typedef enum {
	PA00			= 0U,
	PA01			= 1U,		
	PA02			= 2U,
    PA03			= 3U,
    PA04			= 4U,
    PA05			= 5U,
    PA06			= 6U,
    PA07			= 7U,
    PA08			= 8U,
    PA09			= 9U,
    PA010			= 10U,
    PA011			= 11U,
    PA012			= 12U,
    PA013			= 13U,
    PA014			= 14U,
    PA015			= 15U,
    PB00			= 16U,
    PB01			= 17U,
    PB02			= 18U,
    PB03			= 19U,
    PB04			= 20U,
    PB05			= 21U,
} pin_name_e;


typedef enum {
	
	PA00_GPD				=  0U,
	PA00_INPUT				=  1U,		//input
	PA00_OUTPUT				=  2U,		//output
	PA00_OUTPUT_MONI		=  3U,		//output with monitor
    PA00_UART0_TX			=  4U,
    PA00_I2C_SDA            =  5U,
    PA00_BT0_OUT            =  6U,
    PA00_UART2_RX           =  7U,
	PA00_DBUG_SWDIO			=  8U,
    PA00_TKEY_CH1           =  9U,
	PA00_GROUP0_I2C_SCL     =  10U,
    PA00_ADC_AIN1           =  11U,
	
	PA01_GPD				=  0U,
	PA01_INPUT				=  1U,		//input
	PA01_OUTPUT				=  2U,		//output
	PA01_OUTPUT_MONI		=  3U,		//output with monitor
	PA01_UART0_RX			=  4U,
    PA01_I2C_SCL            =  5U,
    PA01_BT1_OUT            =  6U,
    PA01_UART2_TX           =  7U,
	PA01_DBUG_SWCLK			=  8U,
    PA01_TKEY_CH2           =  9U,
	PA01_GROUP0_I2C_SDA     =  10U,
    PA01_ADC_AIN2           =  11U,
	
	PA02_GPD				=  0U,
	PA02_INPUT				=  1U,		//input
	PA02_OUTPUT				=  2U,		//output
	PA02_OUTPUT_MONI		=  3U,		//output with monitor
    PA02_SIO				=  4U,
    PA02_RTC_ALM            =  5U,
    PA02_BT0_OUT            =  6U,
    PA02_CLO           		=  7U,
	PA02_VREF				=  8U,
    PA02_TKEY_CH3           =  9U,
	PA02_GROUP0_GPT_CHA     =  10U,

	PA03_GPD				=  0U,
	PA03_INPUT				=  1U,		//input
	PA03_OUTPUT				=  2U,		//output
	PA03_OUTPUT_MONI		=  3U,		//output with monitor
	PA03_OSC_XI				=  4U,
    PA03_EPT_CHCX           =  5U,
    PA03_I2C_SDA            =  6U,
    PA03_UART1_TX           =  7U,
	PA03_SIO				=  8U,
    PA03_TKEY_CH4           =  9U,
	PA03_GROUP0_GPT_CHB     =  10U,
	PA03_ADC_AIN3           =  11U,
	
	PA04_GPD				=  0U,
	PA04_INPUT				=  1U,		//input
	PA04_OUTPUT				=  2U,		//output
	PA04_OUTPUT_MONI		=  3U,		//output with monitor
	PA04_OSC_XO				=  4U,
    PA04_EPT_CHCY           =  5U,
    PA04_I2C_SCL            =  6U,
    PA04_UART1_RX           =  7U,
	PA04_LVDIN				=  8U,
    PA04_TKEY_CH5           =  9U,
	PA04_GROUP0_SPI_MOSI    =  10U,
	
	PA05_GPD				=  0U,
	PA05_INPUT				=  1U,		//input
	PA05_OUTPUT				=  2U,		//output
	PA05_OUTPUT_MONI		=  3U,		//output with monitor
	PA05_DBUG_SWCLK			=  4U,
    PA05_BT0_OUT            =  5U,
    PA05_CNTA_BUZ           =  6U,
    PA05_UART0_RX           =  7U,
	PA05_EPT_CHBY			=  8U,
    PA05_TKEY_C0            =  9U,
	PA05_GROUP0_SPI_MISO    =  10U,
	PA05_ADC_AIN4           =  11U,
	
	PA06_GPD				=  0U,
	PA06_INPUT				=  1U,		//input
	PA06_OUTPUT				=  2U,		//output
	PA06_OUTPUT_MONI		=  3U,		//output with monitor
	PA06_BT1_OUT			=  4U,
    PA06_SPI_NSS            =  5U,
    PA06_I2C_SCL            =  6U,
    PA06_UART2_TX           =  7U,
	PA06_DBUG_SWCLK			=  8U,
	PA06_GROUP0_SPI_SCK     =  10U,
	PA06_ADC_AIN5           =  11U,
	
	PA07_GPD				=  0U,
	PA07_INPUT				=  1U,		//input
	PA07_OUTPUT				=  2U,		//output
	PA07_OUTPUT_MONI		=  3U,		//output with monitor
	PA07_I2C_SDA			=  4U,
    PA07_EPI0            	=  5U,
    PA07_EPT_CHAX           =  6U,
    PA07_UART2_RX  	        =  7U,
	PA07_DBUG_SWDIO			=  8U,
	PA07_GROUP0_SPI_NSS     =  10U,
	PA07_ADC_AIN6           =  11U,
	
	PB02_GPD				=  0U,
	PB02_INPUT				=  1U,		//input
	PB02_OUTPUT				=  2U,		//output
	PB02_OUTPUT_MONI		=  3U,		//output with monitor
	PB02_I2C_SCL			=  4U,
    PB02_BT0_OUT           	=  5U,
    PB02_EPT_CHBX           =  6U,
    PB02_EPI3  	        	=  7U,
	PB02_SPI_SCK			=  8U,
	PB02_TKEY_CH6           =  9U,
	PB02_GROUP1_UART0_RX    =  10U,
	PB02_ADC_AIN7           =  11U,
	
	PB03_GPD				=  0U,
	PB03_INPUT				=  1U,		//input
	PB03_OUTPUT				=  2U,		//output
	PB03_OUTPUT_MONI		=  3U,		//output with monitor
	PB03_EPT_CHCX			=  4U,
    PB03_EPT_CHAY           =  5U,
    PB03_EPT_CHD            =  6U,
    PB03_EPI2  	        	=  7U,
	PB03_SPI_MOSI			=  8U,
	PB03_TKEY_CH7           =  9U,
	PB03_GROUP1_UART0_TX    =  10U,
	PB03_ADC_AIN8           =  11U,
	
	PA08_GPD				=  0U,
	PA08_INPUT				=  1U,		//input
	PA08_OUTPUT				=  2U,		//output
	PA08_OUTPUT_MONI		=  3U,		//output with monitor
	PA08_EPT_CHD			=  4U,
    PA08_EPT_CHBY           =  5U,
    PA08_BT1_OUT            =  6U,
    PA08_CLO	  	        =  7U,
	PA08_SPI_MISO			=  8U,
	PA08_TKEY_CH8           =  9U,
	PA08_GROUP1_EPT_CHAX    =  10U,
	PA08_ADC_AIN9           =  11U,
	
	PA09_GPD				=  0U,
	PA09_INPUT				=  1U,		//input
	PA09_OUTPUT				=  2U,		//output
	PA09_OUTPUT_MONI		=  3U,		//output with monitor
	PA09_SPI_SCK			=  4U,
    PA09_GPT_CHA            =  5U,
    PA09_CLO                =  6U,
    PA09_EPT_CHCY	  	    =  7U,
	PA09_LPT_OUT			=  8U,
	PA09_TKEY_CH9           =  9U,
	PA09_GROUP1_EPT_CHBX    =  10U,
	PA09_ADC_AIN10          =  11U,
	
	PA010_GPD				=  0U,
	PA010_INPUT				=  1U,		//input
	PA010_OUTPUT			=  2U,		//output
	PA010_OUTPUT_MONI		=  3U,		//output with monitor
	PA010_SPI_MOSI			=  4U,
    PA010_EPT_CHAX          =  5U,
    PA010_GPT_CHA           =  6U,
    PA010_GPT_CHB	  	    =  7U,
	PA010_LPT_IN			=  8U,
	PA010_TKEY_CH10         =  9U,
	PA010_GROUP1_EPT_CHCX   =  10U,
	PA010_ADC_AIN11         =  11U,

	PA011_GPD				=  0U,
	PA011_INPUT				=  1U,		//input
	PA011_OUTPUT			=  2U,		//output
	PA011_OUTPUT_MONI		=  3U,		//output with monitor
	PA011_SPI_MISO			=  4U,
    PA011_EPT_CHBX          =  5U,
    PA011_GPT_CHB           =  6U,
    PA011_CNTA_BUZ	  	    =  7U,
	PA011_BT0_OUT			=  8U,
	PA011_TKEY_CH11         =  9U,
	PA011_GROUP1_EPT_CHAY   =  10U,
	PA011_ADC_AIN12         =  11U,

	PA012_GPD				=  0U,
	PA012_INPUT				=  1U,		//input
	PA012_OUTPUT			=  2U,		//output
	PA012_OUTPUT_MONI		=  3U,		//output with monitor
	PA012_DBUG_SWDIO		=  4U,
    PA012_EPT_CHAY          =  5U,
    PA012_BT1_OUT           =  6U,
    PA012_UART0_TX	  	    =  7U,
	PA012_SIO				=  8U,
	PA012_TKEY_CH12         =  9U,
	PA012_GROUP1_EPT_CHBY   =  10U,
	PA012_ADC_AIN13         =  11U,
	
	PA013_GPD				=  0U,
	PA013_INPUT				=  1U,		//input
	PA013_OUTPUT			=  2U,		//output
	PA013_OUTPUT_MONI		=  3U,		//output with monitor
	PA013_EPI1				=  4U,
    PA013_EPT_CHCY          =  5U,
    PA013_I2C_SDA           =  6U,
    PA013_UART1_RX	  	    =  7U,
	PA013_BT0_OUT			=  8U,
	PA013_TKEY_CH13         =  9U,
	PA013_GROUP1_EPT_CHCY   =  10U,
	PA013_ADC_AIN14         =  11U,
	
	PB00_GPD				=  0U,
	PB00_INPUT				=  1U,		//input
	PB00_OUTPUT				=  2U,		//output
	PB00_OUTPUT_MONI		=  3U,		//output with monitor
	PB00_GPT_CHB			=  4U,
    PB00_EPT_CHCX          	=  5U,
    PB00_I2C_SCL            =  6U,
    PB00_UART1_TX	  	    =  7U,
	PB00_BT1_OUT			=  8U,
	PB00_TKEY_CH14          =  9U,
	PB00_ADC_AIN15          =  11U,
	
	PB01_GPD				=  0U,
	PB01_INPUT				=  1U,		//input
	PB01_OUTPUT				=  2U,		//output
	PB01_OUTPUT_MONI		=  3U,		//output with monitor
	PB01_CNTA_BUZ			=  4U,
    PB01_GPT_CHA          	=  5U,
    PB01_GPT_CHB            =  6U,
    PB01_SIO		  	    =  7U,
	PB01_LPT_OUT			=  8U,
	PB01_TKEY_CH0           =  9U,
	PB01_ADC_AIN0           =  11U,
	
	PB04_GPD				=  0U,
	PB04_INPUT				=  1U,		//input
	PB04_OUTPUT				=  2U,		//output
	PB04_OUTPUT_MONI		=  3U,		//output with monitor
	PB04_EPT_CHCY			=  4U,
    PB04_EPT_CHBY          	=  5U,
    PB04_UART2_TX           =  6U,
    PB04_BT1_OUT		  	=  7U,
	PB04_SPI_SCK			=  8U,
	PB04_TKEY_CH15          =  9U,
	
	PB05_GPD				=  0U,
	PB05_INPUT				=  1U,		//input
	PB05_OUTPUT				=  2U,		//output
	PB05_OUTPUT_MONI		=  3U,		//output with monitor
	PB05_EPT_CHCX			=  4U,
    PB05_EPT_CHAY          	=  5U,
    PB05_UART2_RX           =  6U,
    PB05_BT0_OUT		  	=  7U,
	PB05_SPI_NSS			=  8U,
	PB05_TKEY_CH16          =  9U,
	
	PA014_GPD				=  0U,
	PA014_INPUT				=  1U,		
	PA014_OUTPUT			=  2U,		
	PA014_OUTPUT_MONI		=  3U,		
	PA014_EPT_CHBX			=  4U,
    PA014_BT1_OUT          	=  5U,
    PA014_I2C_SDA           =  6U,
    PA014_UART1_TX		  	=  7U,
	PA014_SPI_MOSI			=  8U,
	
	PA015_GPD				=  0U,
	PA015_INPUT				=  1U,		
	PA015_OUTPUT			=  2U,		
	PA015_OUTPUT_MONI		=  3U,		
	PA015_EPT_CHAX			=  4U,
    PA015_BT0_OUT          	=  5U,
    PA015_I2C_SCL           =  6U,
    PA015_UART1_RX		  	=  7U,
	PA015_SPI_MISO			=  8U,
	
	IOMAP					=  10U
} pin_func_e;

typedef enum{
	SWD_GRP0 = 0,
	SWD_GRP1,
	SWD_GRP2
}swd_grp_e;

//APT32F102
#define FLASHBASE			0x00000000
#define FLASHSIZE			0x00010000
#define FLASHPAGE			256
#define FLASHLIMIT			(FLASHBASE + FLASHSIZE) 
#define USEROPTION_ADDR		0x000800C0
#define DFLASHBASE			0x10000000
#define DFLASHSIZE			0x00000800
#define DFLASHPAGE			32
#define DFLASHLIMIT 		(DFLASHBASE + DFLASHSIZE) 

#ifdef REMAP
	#define SRAMBASE		0x00000000
	#define SRAMSIZE 		0x00000800
	#define SRAMLIMIT		(SRAMBASE + SRAMSIZE) 
	#define MEMVECTORBASE	0x00000700
	#define MEMVECTORSIZE	(0x50 << 2)
#else
	#define SRAMBASE 		0x20000000
	#define SRAMSIZE 		0x00001000
	#define SRAMLIMIT 		(SRAMBASE + SRAMSIZE) 
	#define MEMVECTORBASE 	0x20000F00
	#define MEMVECTORSIZE 	(0x50 << 2)
#endif

//APB for APT32F102
//--Peripheral Address Setting
#define APB_PERI_BASE		0x40000000UL 
#define APB_IFC_BASE    	(APB_PERI_BASE + 0x10000)
#define APB_SYS_BASE  	    (APB_PERI_BASE + 0x11000)
#define APB_ETCB_BASE    	(APB_PERI_BASE + 0x12000)
#define APB_TKEY_BASE    	(APB_PERI_BASE + 0x20000)
#define APB_ADC0_BASE    	(APB_PERI_BASE + 0x30000)
#define APB_CNTA_BASE     	(APB_PERI_BASE + 0x50000)
#define APB_BT0_BASE       	(APB_PERI_BASE + 0x51000)
#define APB_BT1_BASE      	(APB_PERI_BASE + 0x52000)
#define APB_GPTA0_BASE    	(APB_PERI_BASE + 0x55000)
#define APB_EPT0_BASE   	(APB_PERI_BASE + 0x59000)
#define APB_RTC_BASE		(APB_PERI_BASE + 0x60000)
#define APB_LPT_BASE     	(APB_PERI_BASE + 0x61000)
#define APB_WWDT_BASE		(APB_PERI_BASE + 0x62000)
#define APB_UART0_BASE   	(APB_PERI_BASE + 0x80000)
#define APB_UART1_BASE   	(APB_PERI_BASE + 0x81000)
#define APB_UART2_BASE  	(APB_PERI_BASE + 0x82000)
#define APB_SPI0_BASE  		(APB_PERI_BASE + 0x90000)
#define APB_I2C0_BASE   	(APB_PERI_BASE + 0xA0000)
#define APB_SIO0_BASE  		(APB_PERI_BASE + 0xB0000)
//
#define AHB_GPIO_BASE 		0x60000000
#define APB_GPIOA0_BASE  	(AHB_GPIO_BASE + 0x0000) 	//A0  
#define APB_GPIOB0_BASE  	(AHB_GPIO_BASE + 0x2000) 	//B0 
#define APB_IGRP_BASE		(AHB_GPIO_BASE + 0xF000)
//
#define AHB_CRC_BASE    	0x50000000
#define APB_HWD_BASE		0x70000000	
#define CK801_ADDR_BASE  	0xE000E000	


//--Interrupt Bit Position
#define CORET_INT   		(0x01ul<<0)       //IRQ0
#define SYSCON_INT  		(0x01ul<<1)       //IRQ1
#define IFC_INT     		(0x01ul<<2)       //IRQ2
#define ADC_INT     		(0x01ul<<3)       //IRQ3
#define EPT0_INT    		(0x01ul<<4)       //IRQ4
//DUMMY       //IRQ5
#define WWDT_INT    		(0x01ul<<6)       //IRQ6
#define EXI0_INT    		(0x01ul<<7)       //IRQ7
#define EXI1_INT    		(0x01ul<<8)       //IRQ8
#define GPT0_INT    		(0x01ul<<9)       //IRQ9 
//DUMMY      //IRQ10
//DUMMY      //IRQ11
#define RTC_INT     		(0x01ul<<12)      //IRQ12
#define UART0_INT   		(0x01ul<<13)      //IRQ13
#define UART1_INT   		(0x01ul<<14)      //IRQ14
#define UART2_INT   		(0x01ul<<15)      //IRQ15
//DUMMY      //IRQ16
#define I2C_INT     		(0x01ul<<17)      //IRQ17
//DUMMY      //IRQ18
#define SPI_INT     		(0x01ul<<19)      //IRQ19
#define SIO_INT     		(0x01ul<<20)      //IRQ20
#define EXI2_INT    		(0x01ul<<21)      //IRQ21
#define EXI3_INT    		(0x01ul<<22)      //IRQ22
#define EXI4_INT    		(0x01ul<<23)      //IRQ23
#define CA_INT      		(0x01ul<<24)      //IRQ24
#define TKEY_INT    		(0x01ul<<25)      //IRQ25
#define LPT_INT     		(0x01ul<<26)      //IRQ26
//DUMMY      //IRQ27
#define BT0_INT     		(0x01ul<<28)      //IRQ28
#define BT1_INT     		(0x01ul<<29)      //IRQ29
//DUMMY      //IRQ30
//DUMMY      //IRQ31

#define CONFIG_IRQ_NUM				32U
#define CK_CPU_ENALLNORMALIRQ		asm ("psrset ee,ie")
#define CK_CPU_DISALLNORMALIRQ		asm ("psrclr ie")



//ISR Define for generating special interrupt related ASM (CK802), with compile option -mistack
void MisalignedHandler(void)	__attribute__((isr));
void IllegalInstrHandler(void)	__attribute__((isr));
void AccessErrHandler(void) 	__attribute__((isr));
void BreakPointHandler(void)	__attribute__((isr));
void UnrecExecpHandler(void) 	__attribute__((isr));
void Trap0Handler(void) 		__attribute__((isr));
void Trap1Handler(void) 		__attribute__((isr));
void Trap2Handler(void) 		__attribute__((isr));
void Trap3Handler(void) 		__attribute__((isr));
void PendTrapHandler(void) 		__attribute__((isr));


void CORETHandler(void) 		__attribute__((isr));
void SYSCONIntHandler(void) 	__attribute__((isr));
void csp_ifc_irq_handler(void) 	__attribute__((isr));
void ADCIntHandler(void) 		__attribute__((isr));
void EPT0IntHandler(void) 		__attribute__((isr));
void WWDTHandler(void) 			__attribute__((isr));
void EXI0IntHandler(void) 		__attribute__((isr));
void EXI1IntHandler(void) 		__attribute__((isr));
void EXI2to3IntHandler(void) 	__attribute__((isr));
void EXI4to9IntHandler(void) 	__attribute__((isr));
void EXI10to15IntHandler(void) 	__attribute__((isr));
void UART0IntHandler(void) 		__attribute__((isr));
void UART1IntHandler(void) 		__attribute__((isr));
void UART2IntHandler(void) 		__attribute__((isr));
void I2CIntHandler(void) 		__attribute__((isr));
void GPT0IntHandler(void) 		__attribute__((isr));
void LEDIntHandler(void) 		__attribute__((isr));
void TKEYIntHandler(void) 		__attribute__((isr));
void SPI0IntHandler(void) 		__attribute__((isr));
void SIO0IntHandler(void) 		__attribute__((isr));
void CNTAIntHandler(void) 		__attribute__((isr));
void RTCIntHandler(void) 		__attribute__((isr));
void LPTIntHandler(void) 		__attribute__((isr));
void BT0IntHandler(void) 		__attribute__((isr));
void BT1IntHandler(void) 		__attribute__((isr));


#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
