
/***********************************************************************//** 
 * \file  sys_clk.h
 * \brief  head file for sys_clk.c  
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0 <td>WNN     <td>initial for csi
 * <tr><td> 2020-5-13 <td>V0.0 <td>ZJY     <td>initial for csi
 * </table>
 * *********************************************************************
*/
 
#ifndef _SYS_CLK_H_
#define _SYS_CLK_H_

#include <stdint.h>
#include <soc.h>
#include "csp_syscon.h"
#include "csp_hwdiv.h"

//#define SYSCON_REG_BASE	(csp_syscon_t *)APB_SYS_BASE
//#define IFC_REG_BASE	(csp_ifc_t *)APB_IFC_BASE



typedef enum {
    SRC_IMOSC = 0,
    SRC_EMOSC,
    SRC_HFOSC,
	SRC_ISOSC = 4
} cclk_src_e;

typedef enum{
	PCLK_PM = 8,
	HCLK_PM,
	ISOSC_PM = 12, 
	IMOSC_PM,
	EMOSC_PM = 15
}cclk_pm_e;

typedef enum {
	IM_5M = IMOSC_5M_VALUE,
	IM_4M = IMOSC_4M_VALUE,
	IM_2M = IMOSC_2M_VALUE, 
	IM_131K = IMOSC_131K_VALUE
}imo_freq_e;

typedef enum{
	HF_48M 	= HFOSC_48M_VALUE,
	HF_24M  = HFOSC_24M_VALUE,
	HF_12M  = HFOSC_12M_VALUE,
	HF_6M   = HFOSC_6M_VALUE
}hfo_freq_e;

typedef enum{
	PCLK_DIV1	= 0,
	PCLK_DIV2,
	PCLK_DIV4,
	PCLK_DIV8 	= 4,
	PCLK_DIV16 	= 8
}pclk_div_e;

typedef enum{
	SCLK_DIV1	= 1,
	SCLK_DIV2,
	SCLK_DIV3,
	SCLK_DIV4,
	SCLK_DIV5,
	SCLK_DIV6,
	SCLK_DIV8	=7,
	SCLK_DIV12	=8,
	SCLK_DIV16	=9,
	SCLK_DIV24	=10,
	SCLK_DIV32	=11,
	SCLK_DIV36	=12,
	SCLK_DIV64	=13,
	SCLK_DIV128	=14,
	SCLK_DIV256	=15
}hclk_div_e;

/// \struct system_clk_config_t
/// members to be used in system clock management, including osc src, osc freq(if seletable), HCLK and PCLK divider
typedef struct {
	cclk_src_e		eClkSrc;	//clock source
	uint32_t		wFreq;		//clock frequency
	hclk_div_e		eSdiv;		//SDIV
	pclk_div_e		ePdiv;		//PDIV
	uint32_t		wSclk;		//SCLK
	uint32_t		wPclk;
}csi_clk_config_t;

extern csi_clk_config_t tClkConfig;

//typedef struct {
//    cclk_src_e	eSysClkSrc;      /* select sysclk source clock */
//	uint32_t 	wOscFreq;        /* select frequence */
//    hclk_div_e 	eHclkDivider;    /* ratio between fs_mclk clock and mclk clock */
//    pclk_div_e 	ePclkDivider;    /* ratio between mclk clock and apb0 clock */
//} system_clk_config_t;

/// \enum clk_module_t
/// \brief all the peri clock enable bits in SYSCON level
/// \todo  clk_module_t or clk_module_e
typedef enum {
    IFC_SYS_CLK		= 0U,
    ADC_SYS_CLK		= 4U,
    TKEY_SYS_CLK	= 6U,
    ETCB_SYS_CLK	= 7U,
    UART0_SYS_CLK	= 8U,
	UART1_SYS_CLK	= 9U,
	UART2_SYS_CLK	= 10U,
    SPI_SYS_CLK		= 16U,
    SIO_SYS_CLK		= 20U,
    I2C_SYS_CLK		= 22U,
	
    WWDT_SYS_CLK	= 39U,
    RTC_SYS_CLK		= 40U,
    LPT_SYS_CLK		= 41U,
	CNTA_SYS_CLK	= 42U,
	BT0_SYS_CLK		= 43U,
	BT1_SYS_CLK		= 44U,
	GPT_SYS_CLK		= 45U,
	EPT_SYS_CLK		= 49U
} clk_module_t;

//
extern uint32_t g_wSystemClk;

//
csi_error_t soc_sysclk_config(void);
csi_error_t soc_clo_config(clo_src_e, clo_div_e, pin_name_e);
//
void soc_clk_pm_enable(clk_pm_e eClk, bool bEnable);
void soc_clk_pm_enable(clk_pm_e eClk, bool vEnable);



/**
  \brief       Soc get sclk frequence.
  \param[in]   none
  \return      system clk frequence
*/
uint32_t soc_get_sclk_freq(void);

/**
  \brief       Soc get pclk frequence.
  \param[in]   none
  \return      Peripherals clk frequence
*/
uint32_t soc_get_pclk_freq(void);

/**
  \brief       Soc get coret frequence.
  \param[in]   none
  \return      coret frequence
*/
uint32_t soc_get_coret_freq(void);

/**
  \brief       Soc get bt frequence.
  \param[in]   none
  \return      coret frequence
*/
uint32_t soc_get_bt_freq(uint32_t idx);

/*
  \brief       Soc enable device clock
  \param[in]   module   clock module, defined in sys_clk.h, \ref clk_module_t
  \return      none
*/
void soc_clk_enable(int32_t module);

/*
  \brief       Soc disable device clock
  \param[in]   module   clock module, defined in sys_clk.h, \ref clk_module_t
  \return      none
*/
void soc_clk_disable(int32_t module);


/**
  \brief       Soc get device frequence.
  \param[in]   freq     cpu frequence
  \return      none
*/
void soc_set_sys_freq(uint32_t freq);

/**
  \brief       SOC Dcache clean & invalid by range.
  \return      none
*/
void soc_dcache_clean_invalid_range(uint32_t addr, uint32_t size);

/**
  \brief       SOC Dcache clean & invalid all.
  \return      none
*/
void soc_dcache_clean_invalid_all(void);

/**
  \brief       SOC Dcache invalid by range.
  \return      none
*/
void soc_dcache_invalid_range(uint32_t addr, uint32_t size);

/**
  \brief       SOC dma address remap.
  \return      remaped address
*/
extern uint32_t soc_dma_address_remap(uint32_t addr);


//#ifdef CONFIG_PM
/**
  \brief       SoC enter low-power mode, each chip's implementation is different
               called by csi_pm_enter_sleep
  \param[in]   mode        low-power mode
  \return      error code
*/
csi_error_t soc_pm_enter_sleep(csi_pm_mode_t mode);

/**
  \brief       SoC the wakeup source.
  \param[in]   wakeup_num  wakeup source num
  \param[in]   enable      flag control the wakeup source is enable or not
  \return      error code
*/
csi_error_t soc_pm_config_wakeup_source(uint32_t wakeup_num, bool enable);


#endif /* _SYS_CLK_H_ */