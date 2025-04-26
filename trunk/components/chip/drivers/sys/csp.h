/***********************************************************************//** 
 * \file  csp.h
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 20201-5-13 <td>V0.0  <td>ZJY   <td>add *IWDT
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef _CSP_H
#define _CSP_H

/* Includes ------------------------------------------------------------------*/
#include "csp_adc.h"
#include "csp_bt.h"
#include "csp_cnta.h"
#include "csp_crc.h"
#include "csp_ept.h"
#include "csp_etb.h"
#include "csp_gpio.h"
#include "csp_gpta.h"
#include "csp_i2c.h"
#include "csp_ifc.h"
#include "csp_lpt.h"
#include "csp_rtc.h"
#include "csp_sio.h"
#include "csp_spi.h"
#include "csp_syscon.h"
#include "csp_uart.h"
#include "csp_wwdt.h"
#include "core_801.h"

typedef csp_syscon_t 	csp_iwdt_t;

extern VIC_Type  		*CK801INT;
extern CORET_Type 		*CK801CORET;
extern csp_ifc_t 		*IFC;
extern csp_syscon_t 	*SYSCON;
extern csp_etb_t   		*ETCB;
extern csp_adc_t 		*ADC0;
extern csp_gpio_t 		*GPIOA0, *GPIOB0;
extern csp_igrp_t		*GPIOGRP;
extern csp_uart_t 		*UART0, *UART1, *UART2;
extern csp_spi_t  		*SPI0;
extern csp_sio_t  		*SIO0;
extern csp_i2c_t  		*I2C0;
extern csp_cnta_t  		*CA0;
extern csp_gpta_t 		*GPT0;
extern csp_ept_t 		*EPT0;
extern csp_lpt_t 		*LPT;
extern csp_wwdt_t 		*WWDT;
extern csp_bt_t  		*BT0;
extern csp_bt_t  		*BT1;
extern csp_crc_t 		*CRC;
extern csp_rtc_t 		*RTC;
extern csp_iwdt_t		*IWDT;

#endif

/******************* (C) COPYRIGHT 2020 APT Chip *****END OF FILE****/