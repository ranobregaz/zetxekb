/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/pm.h
 * @brief    header File for pm manage
 * @version  V1.0
 * @date     16. Mar 2020
 ******************************************************************************/

#ifndef _PM_H_
#define _PM_H_

#include <stdint.h>
#include <drv/common.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_USER_PM
typedef struct{
	void (*prepare_to_sleep)(void );
	void (*wkup_frm_sleep)(void );
	void (*prepare_to_deepsleep)(void );
	void (*wkup_frm_deepsleep)(void );
}pm_core_t;
#endif

typedef enum {

    WKUP_EXI0 = 0,
	WKUP_EXI1,
	WKUP_EXI2,
	WKUP_EXI3,
	WKUP_EXI4,
	WKUP_IWDT = 8,
	WKUP_RTC,
	WKUP_LPT,
	WKUP_LVD,
	WKUP_TCH,
	WKUP_I2C
} wakeupn_type_e;


/**
  \brief       SoC enter low-power mode, each chip's implementation is different
               called by csi_pm_enter_sleep
  \param[in]   mode        low-power mode
  \return      error code
*/
csi_error_t csi_pm_enter_sleep(csi_pm_mode_e eMode);

/**
  \brief       SoC the wakeup source.
  \param[in]   wakeup_num  wakeup source num
  \param[in]   enable      flag control the wakeup source is enable or not
  \return      error code
*/
csi_error_t csi_pm_config_wakeup_source(uint32_t wakeup_num, bool enable);


/**
  \brief       to attach user defined functions, 
   * to use in case user wants to preserve the scene in lp mode 
  \param	eMd: low power mode
  \param   pBeforeSlp: funtion to be called before lp
  \param   pBeforeSlp: funtion to be called after wakeup 
  \return      None.
*/
void csi_pm_attach_callback(csi_pm_mode_e eMd, void *pBeforeSlp, void *pWkup);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_PM_H_ */
