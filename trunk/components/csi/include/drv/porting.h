/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/porting.h
 * @brief    header file for soc porting
 * @version  V1.0
 * @date     8. Apr 2020
 * @model    porting
 ******************************************************************************/

#ifndef _DRV_PORTING_H_
#define _DRV_PORTING_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>

/**
  \brief       Soc get device frequence.
  \param[in]   idx      device index
  \return      frequence
*/
uint32_t soc_get_coret_freq(void);


/**
  \brief       Soc get device frequence.
  \param[in]   freq     cpu frequence
  \return      none
*/
void soc_set_sys_freq(uint32_t freq);

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

/*
  \brief       Get CPU ID
  \return      CPU ID, the val is 0, 1, 2...
*/
uint32_t soc_get_cpu_id(void);


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
//#endif

#endif /* _DRV_PORTING_H_ */
