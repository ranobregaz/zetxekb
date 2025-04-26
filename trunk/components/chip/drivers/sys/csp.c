/***********************************************************************//** 
 * \file  csp.c
 * \brief for debugging and using lower-level driver apis out of CSI
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-5-13  <td>V0.0  <td>ZJY   <td>add *IWDT
 * </table>
 * *********************************************************************
*/

#include "csp.h"
#include "soc.h"

VIC_Type  		*CK801INT 	= (VIC_Type*) (CK801_ADDR_BASE +0x100)  ;
CORET_Type 		*CK801CORET = (CORET_Type *)(CK801_ADDR_BASE + 0x10);
csp_ifc_t 		*IFC   		= (csp_ifc_t *)(APB_IFC_BASE) ;
csp_syscon_t 	*SYSCON  	= (csp_syscon_t *)(APB_SYS_BASE);
csp_etb_t		*ETCB 		= (csp_etb_t *)(APB_ETCB_BASE); 
csp_adc_t 		*ADC0    	= (csp_adc_t *)(APB_ADC0_BASE) ;
csp_gpio_t 		*GPIOA0		= (csp_gpio_t *)(APB_GPIOA0_BASE);
csp_gpio_t 		*GPIOB0		= (csp_gpio_t *)(APB_GPIOB0_BASE);
csp_igrp_t		*GPIOGRP  	= (csp_igrp_t *)(APB_IGRP_BASE);
csp_uart_t 		*UART0   	= (csp_uart_t *)(APB_UART0_BASE);
csp_uart_t 		*UART1   	= (csp_uart_t *)(APB_UART1_BASE);
csp_uart_t 		*UART2   	= (csp_uart_t *)(APB_UART2_BASE);
csp_spi_t  		*SPI0     	= (csp_spi_t *)(APB_SPI0_BASE);
csp_sio_t  		*SIO0     	= (csp_sio_t *)(APB_SIO0_BASE);
csp_i2c_t  		*I2C0     	= (csp_i2c_t *)(APB_I2C0_BASE);
csp_cnta_t  	*CA0      	= (csp_cnta_t *)(APB_CNTA_BASE);
csp_gpta_t 		*GPT0     	= (csp_gpta_t *)(APB_GPTA0_BASE);
csp_ept_t 		*EPT0     	= (csp_ept_t *)(APB_EPT0_BASE);
csp_lpt_t 		*LPT     	= (csp_lpt_t *)(APB_LPT_BASE) ;
csp_wwdt_t 		*WWDT      	= (csp_wwdt_t *)(APB_WWDT_BASE);
csp_bt_t  		*BT0 	   	= (csp_bt_t *)(APB_BT0_BASE) ;
csp_bt_t  		*BT1 	   	= (csp_bt_t *)(APB_BT1_BASE) ;
csp_crc_t 		*CRC      	= (csp_crc_t *)(AHB_CRC_BASE);
csp_rtc_t 		*RTC      	= (csp_rtc_t *)(APB_RTC_BASE);
csp_iwdt_t 		*IWDT		= (csp_iwdt_t *)(APB_SYS_BASE);