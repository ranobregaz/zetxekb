
/***********************************************************************//** 
 * \file  board_config.h
 * \brief  system clock management for cpu clock(HCLK) and peri clock(PCLK)
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <stdint.h>
#include "soc.h"
#include "sys_clk.h"

#ifdef __cplusplus
extern "C" {
#endif

//如果定义了DBG_PRINT2PC，串口对应脚和PC连上后，可以通过serial pane里的UART #x 看到串口数据
//#define DBG_PRINT2PC	

//extern system_clk_config_t g_tSystemClkConfig[];
#ifndef EMOSC_VALUE
#define EMOSC_VALUE         32768U
#endif



/* example pin manager */

#define CONSOLE_IDX                     1
#define CONSOLE_TXD                     PB00
#define CONSOLE_RXD                     PA013
#define CONSOLE_TXD_FUNC                PB00_UART1_TX
#define CONSOLE_RXD_FUNC                PA013_UART1_RX

#define XIN_PIN							PA03
#define XOUT_PIN						PA04
#define XIN_PIN_FUNC					PA03_OSC_XI
#define XOUT_PIN_FUNC					PA04_OSC_XO


#define EXI_PIN                      	PA09
#define EXI_PIN_FUNC                 	PIN_FUNC_INPUT

#define SPI_IDX                 		0
#define SPI_MOSI_PIN            		PA014            
#define SPI_MISO_PIN            		PA015            
#define SPI_NSS_PIN             		PB05             
#define SPI_SCK_PIN             		PB04             
#define SPI_MOSI_PIN_FUNC       		PA014_SPI_MOSI
#define SPI_MISO_PIN_FUNC       		PA015_SPI_MISO
#define SPI_NSS_PIN_FUNC        		PB05_SPI_NSS
#define SPI_SCK_PIN_FUNC				PB04_SPI_SCK

#define IIC_IDX		            		0
#define IIC_SDA_PIN             		PA013        
#define IIC_SCL_PIN             		PB00        
#define IIC_SDA_PIN_FUNC        		PA013_I2C_SDA
#define IIC_SCL_PIN_FUNC        		PB00_I2C_SCL

#define GPIO_IDX						1
#define GPIO_PIN                		PB02
#define GPIO_PIN_MSK	     			0x4
#define GPIO_PIN_FUNC           		PIN_FUNC_OUTPUT


#define EPT_IDX                 		0
#define EPT_PWMCHAY_PIN              	PA012
#define EPT_PWMCHAX_PIN					PA010
#define EPT_PWMCHAX_PIN_FUNC            PA010_EPT_CHAX
#define EPT_PWMCHAY_PIN_FUNC			PA012_EPT_CHAY
#define EPT_EBI_PIN						PB02
#define EPT_EBI							PB02_EPI3

#define ADC_IDX							0
#define ADC_PIN							PA011
#define ADC_PIN_FUNC					PA011_ADC_AIN12
#define ADC_CH							ADCIN12
#define ADC_PIN1						PA013
#define ADC_PIN1_FUNC					PA013_ADC_AIN14
#define ADC_CH1							ADCIN14


#define TCH_IDX							0
#define TCH_PIN1						PB03
#define TCH_PIN2						PA08
#define	TCH_PIN1_FUNC					PB03_TKEY_CH7
#define TCH_PIN2_FUNC					PA08_TKEY_CH8

#define SIO_IDX					        0
#define SIO_PIN					        PA02
#define SIO_PIN_FUNC					PA02_SIO


__attribute__((weak)) void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_CONFIG_H_ */

