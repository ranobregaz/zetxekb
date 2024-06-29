
/***********************************************************************//** 
 * \file  power_manage.c
 * \brief  power management 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/
//#ifdef CONFIG_PM

#include <drv/pm.h>

#ifdef CONFIG_USER_PM
/// to make user defined prepaare_to_stop() and wkup_frm_stop() possible
pm_core_t g_tPmCore;

/**
  \brief       to attach user defined functions, 
   * to use in case user wants to preserve the scene in lp mode 
  \param	eMd: low power mode
  \param   pBeforeSlp: funtion to be called before lp
  \param   pBeforeSlp: funtion to be called after wakeup 
  \return      None.
*/
void csi_pm_attach_callback(csi_pm_mode_t eMd, void *pBeforeSlp, void *pWkup)
{
	switch(eMd)
	{
		case (PM_MODE_SLEEP):g_tPmCore.prepare_to_sleep = pBeforeSlp;
				g_tPmCore.wkup_frm_sleep = pWkup;
			break;
		case (PM_MODE_DEEPSLEEP):g_tPmCore.prepare_to_deepsleep = pBeforeSlp;
				g_tPmCore.wkup_frm_deepsleep = pWkup;
			break;
		default:
			break;
	}
	
}
#endif

/**
  \brief       to config wakeup source 
   * should be used if user wants to preserve the scene in lp mode 
  \param[in]   eWkupSrc: wakeup source
  \param[in]   bEnable: ENABLE/DISABLE 
  \return      csi_errpt_t
*/
csi_error_t csi_pm_config_wakeup_source(wakeupn_type_e eWkupSrc, bool bEnable) ///TODO : beter to use dev_tag?
{
    csi_error_t ret = CSI_OK;
    ret = soc_pm_config_wakeup_source(eWkupSrc, bEnable);
    return ret;
}

/**
  \brief       to enter LP mode
  \param[in]   mode: LP mode
  \return      csi_errpt_t
*/
csi_error_t csi_pm_enter_sleep(csi_pm_mode_t mode)
{
    csi_error_t ret = CSI_OK;
	ret = soc_pm_enter_sleep(mode);
	return ret;
}


//#endif