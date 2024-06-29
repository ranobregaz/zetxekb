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
  \brief       Initialize pm module
  \return      error code
*/
csi_error_t csi_pm_init(void);

/**
  \brief       De-initialize pm module
  \return      None
*/
void csi_pm_uninit(void);

/**
  \brief       Set the retention memory used to save registers
  \param[in]   mem        retention memory(word align)
  \param[in]   num        number of memory(1: 1 word)
  \return      error code
*/
csi_error_t csi_pm_set_reten_mem(uint32_t *mem, uint32_t num);

/**
  \brief       Config the wakeup source.
  \param[in]   wakeup_num  wakeup source num
  \param[in]   enable      flag control the wakeup source is enable or not
  \return      error code
*/
csi_error_t csi_pm_config_wakeup_source(uint32_t wakeup_num, bool enable);

/**
  \brief       System enter low-power mode
  \param[in]   mode        low-power mode, \ref csi_pm_mode_t
  \return      error code
*/
csi_error_t csi_pm_enter_sleep(csi_pm_mode_t mode);

/**
  \brief       Register device to the pm list
  \param[in]   dev        csi dev
  \param[in]   pm_action  pm action function
  \param[in]   mem_size   size of memory for saving registers
  \param[in]   priority   pm dev priority(0-3), The smaller the value,
  the last execution before entering low power consumption,
  the first execution after exiting low power consumption.
  \return      error code
*/
csi_error_t csi_pm_dev_register(csi_dev_t *dev, void *pm_action, uint32_t mem_size, uint8_t priority);

/**
  \brief       Deregister device to the pm list
  \param[in]   dev        csi dev
  \return      None
*/
void csi_pm_dev_unregister(csi_dev_t *dev);

/**
  \brief       Save registers to memory
  \param[in]   mem        mem to store registers
  \param[in]   addr       registers address
  \param[in]   num        number of memory(1: 1 word)
  \return      None
*/
void csi_pm_dev_save_regs(uint32_t *mem, uint32_t *addr, uint32_t num);

/**
  \brief       Save registers to memory
  \param[in]   mem        mem to store registers
  \param[in]   addr       registers address
  \param[in]   num        number of memory(1: 1 word)
  \return      None
*/
void csi_pm_dev_restore_regs(uint32_t *mem, uint32_t *addr, uint32_t num);

/**
  \brief       Notify devices enter low-power states
  \param[in]   action       device low-power action
  \return      error code
*/
csi_error_t csi_pm_dev_notify(csi_pm_dev_action_t action);

/**
  \brief       to attach user defined functions, 
   * to use in case user wants to preserve the scene in lp mode 
  \param	eMd: low power mode
  \param   pBeforeSlp: funtion to be called before lp
  \param   pBeforeSlp: funtion to be called after wakeup 
  \return      None.
*/
void csi_pm_attach_callback(csi_pm_mode_t eMd, void *pBeforeSlp, void *pWkup);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_PM_H_ */
