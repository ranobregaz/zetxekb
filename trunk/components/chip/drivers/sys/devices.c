/***********************************************************************//** 
 * \file  devices.c
 * \brief  source file for the devices
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-5-13 <td>V0.0  <td>ZJY    <td>initial
 * </table>
 * *********************************************************************
*/
//#include <csi_config.h>
#include <stdio.h>
#include <soc.h>
#include <sys_clk.h>
#include <drv/uart.h>
#include <drv/timer.h>
#include <drv/dma.h>
#include <drv/iic.h>
#include <drv/rtc.h>
#include <drv/spi.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>

///!!! DO NOT DEFINE HERE, DEFINE in "Define" in Compiler tab to make it globally available !!! 
//#define IS_CHIP_102 1
//#define IS_CHIP_1021 1
//#define IS_CHIP_1022 1
//#define IS_CHIP_1023 1
//#define IS_CHIP_S003 1

const csi_perip_info_t g_soc_info[] = {
	
	{CK801_ADDR_BASE,			CORET_IRQn,			0,    DEV_CORET_TAG},
    {APB_SYS_BASE,				SYSCON_IRQn,        0,    DEV_SYSCON_TAG},
    {APB_IFC_BASE,              IFC_IRQn,           0,    DEV_IFC_TAG},
    {APB_ADC0_BASE,             ADC_IRQn,			0,    DEV_ADC_TAG},
    {APB_EPT0_BASE,           	EPT0_IRQn,          0,    DEV_EPT_TAG},
	/// for IWDT
	{APB_SYS_BASE,           	SYSCON_IRQn,        0,    DEV_WDT_TAG}, 
	/// for WWDT
    {APB_WWDT_BASE,           	WWDT_IRQn,          1,    DEV_WDT_TAG},
	{AHB_CRC_BASE,           	-1,           		0,    DEV_CRC_TAG},
    {APB_GPTA0_BASE,           	GPT0_IRQn,          0,    DEV_GPT_TAG},

#if defined(IS_CHIP_102) || defined(IS_CHIP_1021) || defined(IS_CHIP_1022) || defined(IS_CHIP_1023)
    {APB_RTC_BASE,           	RTC_IRQn,           0,    DEV_RTC_TAG},
#endif
    {APB_UART0_BASE,           	UART0_IRQn,         0,    DEV_UART_TAG},
#if defined(IS_CHIP_1023)
    {APB_UART1_BASE,            UART1_IRQn,         1,    DEV_UART_TAG},
#endif
	{APB_UART2_BASE,           	UART2_IRQn,         2,    DEV_UART_TAG},
    {APB_I2C0_BASE,             I2C_IRQn,           0,    DEV_IIC_TAG},
    {APB_SPI0_BASE,             SPI_IRQn,           0,    DEV_SPI_TAG},
#if defined(IS_CHIP_102) || defined(IS_CHIP_1023)
    {APB_SIO0_BASE,             SIO_IRQn,           0,    DEV_SIO_TAG},
#endif
	{APB_GPIOA0_BASE,           EXI0_IRQn,          0,    DEV_EXI_TAG},
    {APB_GPIOB0_BASE,           EXI1_IRQn,          1,    DEV_EXI_TAG},
    {APB_CNTA_BASE,            	CNTA_IRQn,          0,    DEV_CNTA_TAG},
#if defined(IS_CHIP_1021) || defined(IS_CHIP_1023)
    {APB_TKEY_BASE,             TKEY_IRQn,          0,    DEV_TKEY_TAG},
#endif
    {APB_LPT_BASE,            	LPT_IRQn,           0,    DEV_LPT_TAG},
    {APB_BT0_BASE,              BT0_IRQn,           0,    DEV_BT_TAG},
	{APB_BT1_BASE,              BT1_IRQn,           1,    DEV_BT_TAG},
    {APB_ETCB_BASE,      		-1,                 0,    DEV_ETCB_TAG},
    {0, 0, 0, 0}
};

const csi_irqmap_t irq_map[] = {
	
	{CK801_ADDR_BASE,			CORET_IRQn		},
    {APB_SYS_BASE,				SYSCON_IRQn		},
    {APB_IFC_BASE,              IFC_IRQn		},
    {APB_ADC0_BASE,             ADC_IRQn		},
    {APB_EPT0_BASE,           	EPT0_IRQn		},
    {APB_WWDT_BASE,           	WWDT_IRQn		},
    {APB_GPTA0_BASE,           	GPT0_IRQn		},
#if defined(IS_CHIP_102) || defined(IS_CHIP_1021) || defined(IS_CHIP_1022) || defined(IS_CHIP_1023)
    {APB_RTC_BASE,           	RTC_IRQn		},
#endif
    {APB_UART0_BASE,           	UART0_IRQn		},
#if defined(IS_CHIP_1023)
    {APB_UART1_BASE,            UART1_IRQn		},
#endif
	{APB_UART2_BASE,           	UART2_IRQn		},
    {APB_I2C0_BASE,             I2C_IRQn		},
    {APB_SPI0_BASE,             SPI_IRQn		},
#if defined(IS_CHIP_102) || defined(IS_CHIP_1023)
    {APB_SIO0_BASE,             SIO_IRQn		},
#endif
    {APB_CNTA_BASE,            	CNTA_IRQn		},
#if defined(IS_CHIP_1021) || defined(IS_CHIP_1023)
    {APB_TKEY_BASE,             TKEY_IRQn		},
#endif
    {APB_LPT_BASE,            	LPT_IRQn		},
    {APB_BT0_BASE,              BT0_IRQn		},
	{APB_BT1_BASE,              BT1_IRQn		},
    {0,  						0				}
};

const csi_clkmap_t clk_map[] = {
    {APB_IFC_BASE,				IFC_SYS_CLK		},
    {APB_ADC0_BASE,				ADC_SYS_CLK		},
    {APB_TKEY_BASE,				TKEY_SYS_CLK	},
    {APB_ETCB_BASE,				ETCB_SYS_CLK	},
    {APB_UART0_BASE,			UART0_SYS_CLK	},
    {APB_UART1_BASE,			UART1_SYS_CLK	},
    {APB_UART2_BASE,			UART2_SYS_CLK	},
    {APB_SPI0_BASE,				SPI_SYS_CLK		},
    {APB_SIO0_BASE,				SIO_SYS_CLK		},
    {APB_I2C0_BASE,				I2C_SYS_CLK		},
    {APB_WWDT_BASE,				WWDT_SYS_CLK	},
    {APB_RTC_BASE,				RTC_SYS_CLK		},
    {APB_LPT_BASE,				LPT_SYS_CLK		},
    {APB_CNTA_BASE,				CNTA_SYS_CLK	},
    {APB_BT0_BASE,				BT0_SYS_CLK		},
    {APB_BT1_BASE,				BT1_SYS_CLK		},
    {APB_GPTA0_BASE,			GPT_SYS_CLK		},
    {APB_EPT0_BASE,				EPT_SYS_CLK		},
    {0x00U, 					0xFFFFFFFFU		}
};


const csi_pinmap_t gpio_pinmap[] = {
    {PA00,           0,     0,           GPIO_PIN_INPUT },
    {PA01,           0,     1,           GPIO_PIN_INPUT },
    {PA02,           0,     2,           GPIO_PIN_INPUT },
    {PA03,           0,     3,           GPIO_PIN_INPUT },
    {PA04,           0,     4,           GPIO_PIN_INPUT },
    {PA05,           0,     5,           GPIO_PIN_INPUT },
    {PA06,           0,     6,           GPIO_PIN_INPUT },
    {PA07,           0,     7,           GPIO_PIN_INPUT },
    {PA08,           0,     8,           GPIO_PIN_INPUT },
    {PA09,           0,     9,           GPIO_PIN_INPUT },
    {PA010,          0,     10,          GPIO_PIN_INPUT },
    {PA011,          0,     11,          GPIO_PIN_INPUT },
    {PA012,          0,     12,          GPIO_PIN_INPUT },
    {PA013,          0,     13,          GPIO_PIN_INPUT },
    {PA014,          0,     14,          GPIO_PIN_INPUT },
    {PB00,           1,     0,           GPIO_PIN_INPUT },
    {PB01,           1,     1,           GPIO_PIN_INPUT },
    {PB02,           1,     2,           GPIO_PIN_INPUT },
    {PB03,           1,     3,           GPIO_PIN_INPUT },
    {PB04,           1,     4,           GPIO_PIN_INPUT },
    {PB05,           1,     5,           GPIO_PIN_INPUT },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,        0xFFFFFFFFU   },
};

const csi_pinmap_t adc_pinmap[] = {
    {PB01,           0,     1,           PB01_ADC_AIN0 },
    {PA00,           0,     2,           PA00_ADC_AIN1 },
    {PA01,           0,     3,           PA01_ADC_AIN2 },
    {PA03,           0,     4,           PA03_ADC_AIN3 },
	{PA05,           0,     5,           PA05_ADC_AIN4 },
	{PA06,           0,     6,           PA06_ADC_AIN5 },
	{PA07,           0,     7,           PA07_ADC_AIN6 },
	{PB02,           0,     8,           PB02_ADC_AIN7 },
	{PB03,           0,     9,           PB03_ADC_AIN8 },
	{PA08,           0,     10,          PA08_ADC_AIN9 },
	{PA09,           0,     11,          PA09_ADC_AIN10 },
	{PA010,          0,     12,          PA010_ADC_AIN11 },
	{PA011,          0,     13,          PA011_ADC_AIN12 },
	{PA012,          0,     14,          PA012_ADC_AIN13 },
	{PA013,          0,     15,          PA013_ADC_AIN14 },
	{PB00,           0,     16,          PB00_ADC_AIN15 },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t uart_pinmap[] = {
    {PA01,            0,     PIN_UART_RX,    PA01_UART0_RX  },
    {PA00,            0,     PIN_UART_TX,    PA00_UART0_TX  },
	{PA05,            0,     PIN_UART_RX,    PA05_UART0_RX  },
    {PA012,           0,     PIN_UART_TX,    PA012_UART0_TX  },
	{PB02,            0,     PIN_UART_RX,    PB02_GROUP1_UART0_RX  },
    {PB03,            0,     PIN_UART_TX,    PB03_GROUP1_UART0_TX  },
    {PA04,            1,     PIN_UART_TX,    PA04_UART1_RX  },
    {PA03,            1,     PIN_UART_RX,    PA03_UART1_TX  },
    {PA013,           1,     PIN_UART_TX,    PA013_UART1_RX  },
    {PB00,            1,     PIN_UART_TX,    PB00_UART1_TX  },
    {PA015,           1,     PIN_UART_RX,    PA015_UART1_RX  },
    {PA014,           1,     PIN_UART_TX,    PA014_UART1_TX  },
    {PA00,            2,     PIN_UART_RX,    PA00_UART2_RX  },
    {PA01,            2,     PIN_UART_TX,    PA01_UART2_TX  },
    {PA07,            2,     PIN_UART_RX,    PA07_UART2_RX  },
    {PA06,            2,     PIN_UART_TX,    PA06_UART2_TX  },
    {PB05,            2,     PIN_UART_RX,    PB05_UART2_RX },
    {PB04,            2,     PIN_UART_TX,    PB04_UART2_TX },
    {0xFFFFFFFFU, 0xFFU, 0xFFU,      0xFFFFFFFFU  },
};

const csi_pinmap_t iic_pinmap[] = {
	{PA00,            0, 	PIN_IIC_SDA, 	PA00_I2C_SDA },
    {PA01,            0, 	PIN_IIC_SCL, 	PA01_I2C_SCL },
	{PA03,            0, 	PIN_IIC_SDA, 	PA03_I2C_SDA },
	{PA04,            0, 	PIN_IIC_SCL, 	PA04_I2C_SCL },
	{PA07,            0, 	PIN_IIC_SDA, 	PA03_I2C_SDA },
    {PB02,            0, 	PIN_IIC_SCL, 	PA04_I2C_SCL },
	{PA013,           0, 	PIN_IIC_SDA, 	PA013_I2C_SDA },
    {PB00,            0, 	PIN_IIC_SCL, 	PB00_I2C_SCL },
	{PA014,           0, 	PIN_IIC_SDA, 	PA014_I2C_SDA },
    {PA015,           0, 	PIN_IIC_SCL, 	PA015_I2C_SCL },
	{PA06,            0, 	PIN_IIC_SCL, 	PA06_I2C_SCL },
    {0xFFFFFFFFU, 0xFFU,       0xFFU,   0xFFFFFFFFU},
};

const csi_pinmap_t spi_pinmap[] = {
    {PA06,            0,    PIN_SPI_CS,    	PA06_SPI_NSS  },
    {PA09,            0,    PIN_SPI_SCK,   	PA09_SPI_SCK  },
    {PA010,           0,    PIN_SPI_MOSI,   PA010_SPI_MOSI },
    {PA011,           0,    PIN_SPI_MISO,   PA011_SPI_MISO },
	{PB05,            0,    PIN_SPI_CS,    	PB05_SPI_NSS  },
    {PB04,            0,    PIN_SPI_SCK,   	PB04_SPI_SCK  },
    {PA014,           0,    PIN_SPI_MOSI,   PA014_SPI_MOSI },
    {PA015,           0,    PIN_SPI_MISO,   PA015_SPI_MISO },
	{PA07,            0,    PIN_SPI_CS,    	PA07_GROUP0_SPI_NSS  },
    {PA06,            0,    PIN_SPI_SCK,   	PA06_GROUP0_SPI_SCK  },
    {PA04,            0,    PIN_SPI_MOSI,   PA04_GROUP0_SPI_MOSI },
    {PA05,            0,    PIN_SPI_MISO,   PA05_GROUP0_SPI_MISO },
	{PB02,            0,    PIN_SPI_SCK,   	PB02_SPI_SCK  },
    {PB03,            0,    PIN_SPI_MOSI,   PB03_SPI_MOSI },
    {PA08,            0,    PIN_SPI_MISO,   PA08_SPI_MISO },
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t cnta_pinmap[] = {
    {PB01,            0,   0,    	PB01_CNTA_BUZ },
    {PA05,            0,   0,   	PA05_CNTA_BUZ },
	{PA011,           0,   0,   	PA011_CNTA_BUZ },
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t bt_pinmap[] = {
	{PA00,            0,   0,    	PA00_BT0_OUT },
	{PA02,            0,   0,    	PA02_BT0_OUT },
	{PA05,            0,   0,    	PA05_BT0_OUT },
	{PA011,           0,   0,    	PA011_BT0_OUT },
	{PA013,           0,   0,    	PA013_BT0_OUT },
	{PA015,           0,   0,    	PA015_BT0_OUT },
	{PB02,            0,   0,    	PB02_BT0_OUT },
    {PB05,            0,   0,    	PB05_BT0_OUT },
	{PA01,            1,   0,   	PA01_BT1_OUT },
	{PA06,            1,   0,   	PA06_BT1_OUT },
	{PA08,            0,   0,    	PA08_BT1_OUT },
	{PA012,           0,   0,    	PA012_BT1_OUT },
	{PA014,           1,   0,   	PA014_BT1_OUT },
	{PB00,            1,   0,   	PB00_BT1_OUT },
    {PB04,            1,   0,   	PB04_BT1_OUT },
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t tkey_pinmap[] = {
    {PB01,            0,   0,    	PB01_TKEY_CH0 },
    {PA00,            0,   1,   	PA00_TKEY_CH1 },
	{PA01,            0,   2,    	PA01_TKEY_CH2 },
    {PA02,            0,   3,   	PA02_TKEY_CH3 },
	{PA03,            0,   4,    	PA03_TKEY_CH4 },
    {PA04,            0,   5,   	PA04_TKEY_CH5 },
	{PB02,            0,   6,    	PB02_TKEY_CH6 },
	{PB03,            0,   7,    	PB03_TKEY_CH7 },
	{PA08,            0,   8,   	PA08_TKEY_CH8 },
	{PA09,            0,   9,    	PA09_TKEY_CH9 },
	{PA010,           0,   10,   	PA010_TKEY_CH10 },
	{PA011,           0,   11,    	PA011_TKEY_CH11 },
	{PA012,           0,   12,   	PA012_TKEY_CH12 },
	{PA013,           0,   13,    	PA013_TKEY_CH13 },
	{PB00,            0,   14,    	PB00_TKEY_CH14 },
	{PB04,            0,   15,    	PB04_TKEY_CH15 },
	{PB05,            0,   16,    	PB05_TKEY_CH16 },
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};


/*
const csi_pinmap_t pwm_pinmap[] = {
    {PA0,    0,    0,    PA0_PWM_CH0  },
    {PA1,    0,    1,    PA1_PWM_CH1  },
    {PA2,    0,    2,    PA2_PWM_CH2  },
    {PA3,    0,    3,    PA3_PWM_CH3  },
    {PA4,    0,    4,    PA4_PWM_CH4  },
    {PA5,    0,    5,    PA5_PWM_CH5  },
    {PA6,    0,    6,    PA6_PWM_CH6  },
    {PA7,    0,    7,    PA7_PWM_CH7  },
    {PA8,    0,    8,    PA8_PWM_CH8  },
    {PA9,    0,    9,    PA9_PWM_CH9  },
    {PA15,   0,    4,    PA15_PWM_CH4 },
    {PA16,   0,    5,    PA16_PWM_CH5 },
    {PA17,   0,   10,    PA17_PWM_CH10},
    {PA18,   0,   11,    PA18_PWM_CH11},
    {PA23,   0,    0,    PA23_PWM_CH0 },
    {PA24,   0,    1,    PA24_PWM_CH1 },
    {PA25,   0,    2,    PA25_PWM_CH2 },
    {PA26,   0,    3,    PA26_PWM_CH3 },
    {PA27,   0,    4,    PA27_PWM_CH4 },
    {PA28,   0,    5,    PA28_PWM_CH5 },
    {PA29,   0,    6,    PA29_PWM_CH6 },
    {PA30,   0,    7,    PA30_PWM_CH7 },
    {PA31,   0,    8,    PA31_PWM_CH8 },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t uart_pinmap[] = {
    {PA0,             0,     PIN_UART_RX,    PA0_UART0_RX   },
    {PA1,             0,     PIN_UART_TX,    PA1_UART0_TX   },
    {PA4,             1,    PIN_UART_CTS,    PA4_UART1_CTS  },
    {PA5,             1,    PIN_UART_RTS,    PA5_UART1_RTS  },
    {PA6,             0,     PIN_UART_TX,    PA6_UART0_TX   },
    {PA8,             0,     PIN_UART_RX,    PA8_UART0_RX   },
    {PA10,            0,     PIN_UART_TX,    PA10_UART0_TX  },
    {PA16,            1,     PIN_UART_TX,    PA16_UART1_TX  },
    {PA17,            0,     PIN_UART_RX,    PA17_UART0_RX  },
    {PA18,            0,     PIN_UART_TX,    PA18_UART0_TX  },
    {PA19,            1,     PIN_UART_RX,    PA19_UART1_RX  },
    {PA20,            1,     PIN_UART_TX,    PA20_UART1_TX  },
    {PA21,            0,    PIN_UART_CTS,    PA21_UART0_CTS },
    {PA21,            1,    PIN_UART_CTS,    PA21_UART1_CTS },
    {PA22,            0,    PIN_UART_RTS,    PA22_UART0_RTS },
    {PA22,            1,    PIN_UART_RTS,    PA22_UART1_RTS },
    {PA23,            0,     PIN_UART_TX,    PA23_UART0_TX  },
    {PA24,            0,     PIN_UART_RX,    PA24_UART0_RX  },
    {PA25,            0,    PIN_UART_CTS,    PA25_UART0_CTS },
    {PA26,            0,    PIN_UART_RTS,    PA26_UART0_RTS },
    {PA27,            1,     PIN_UART_RX,    PA27_UART1_RX  },
    {PA28,            1,     PIN_UART_TX,    PA28_UART1_TX  },
    {PA29,            1,    PIN_UART_CTS,    PA29_UART1_CTS },
    {PA30,            1,    PIN_UART_RTS,    PA30_UART1_RTS },
    {PA31,            0,     PIN_UART_TX,    PA31_UART0_TX  },
    {0xFFFFFFFFU, 0xFFU, 0xFFU,      0xFFFFFFFFU  },
};

const csi_pinmap_t iic_pinmap[] = {
    {PA15,            0, PIN_IIC_SCL, PA15_IIC0_SCL},
    {PA16,            0, PIN_IIC_SDA, PA16_IIC0_SDA},
    {0xFFFFFFFFU, 0xFFU,       0xFFU,   0xFFFFFFFFU},
};


const csi_pinmap_t spi_pinmap[] = {
    {PA0,             0,   PIN_SPI_CS,    PA0_SPI0_CS},
    {PA1,             0,  PIN_SPI_SCK,   PA1_SPI0_SCK},
    {PA23,            0,   PIN_SPI_CS,   PA23_SPI0_CS},
    {PA24,            0,  PIN_SPI_SCK,  PA24_SPI0_SCK},
    {PA25,            0, PIN_SPI_MOSI, PA25_SPI0_MOSI},
    {PA26,            0, PIN_SPI_MISO, PA26_SPI0_MISO},
    {PA27,            1,   PIN_SPI_CS,   PA27_SPI1_CS},
    {PA28,            1,  PIN_SPI_SCK,  PA28_SPI1_SCK},
    {PA29,            1, PIN_SPI_MOSI, PA29_SPI1_MOSI},
    {PA30,            1, PIN_SPI_MISO, PA30_SPI1_MISO},
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t i2s_pinmap[] = {
    {PA2,             1,      PIN_I2S_SCLK, PA2_I2S1_SCLK },
    {PA3,             1,     PIN_I2S_WSCLK, PA3_I2S1_WSCLK},
    {PA5,             1,       PIN_I2S_SDA, PA5_I2S1_SDA  },
    {PA6,             0,      PIN_I2S_SCLK, PA6_I2S0_SCLK },
    {PA7,             0,     PIN_I2S_WSCLK, PA7_I2S0_WSCLK},
    {PA8,             0,       PIN_I2S_SDA, PA8_I2S0_SDA  },
    {PA10,            0,      PIN_I2S_MCLK, PA10_I2S0_MCLK},
    {PA26,            1,      PIN_I2S_MCLK, PA26_I2S1_MCLK},
    {0xFFFFFFFFU, 0xFFU, 0xFFU, 0xFFFFFFFFU   },
};

const csi_clkmap_t clk_map[] = {
    {TIM0_CLK,   DEV_DW_TIMER_TAG,      0},
    {TIM1_CLK,   DEV_DW_TIMER_TAG,      1},
    {RTC0_CLK,   DEV_WJ_RTC_TAG,        0},
    {WDT_CLK,    DEV_DW_WDT_TAG,        0},
    {SPI0_CLK,   DEV_DW_SPI_TAG,        0},
    {UART0_CLK,  DEV_DW_UART_TAG,       0},
    {IIC0_CLK,   DEV_DW_IIC_TAG,        0},
    {PWM_CLK,    DEV_WJ_PWM_TAG,        0},
    {QSPI0_CLK,  DEV_CD_QSPI_TAG,       0},
    {PWMR_CLK,   DEV_BLANK_TAG,         0},
    {EFUSE_CLK,  DEV_WJ_EFUSE_TAG,      0},
    {I2S0_CLK,   DEV_WJ_I2S_TAG,        0},
    {I2S1_CLK,   DEV_WJ_I2S_TAG,        1},
    {GPIO0_CLK,  DEV_DW_GPIO_TAG,       0},
    {TIM2_CLK,   DEV_DW_TIMER_TAG,      2},
    {TIM3_CLK,   DEV_DW_TIMER_TAG,      3},
    {SPI1_CLK,   DEV_DW_SPI_TAG,        1},
    {UART1_CLK,  DEV_DW_UART_TAG,       1},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        5},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        6},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        7},
    {ADC_CLK,    DEV_WJ_ADC_TAG,        0},
    {ETB_CLK,    DEV_WJ_ETB_TAG,        0},
    {I2S2_CLK,   DEV_WJ_I2S_TAG,        2},
    {IOC_CLK,    DEV_BLANK_TAG,         0},
    {CODEC_CLK,  DEV_RCHBAND_CODEC_TAG, 0},
    {0xFFFFFFFFU, 0xFFFFU,          0xFFU}
};

*/