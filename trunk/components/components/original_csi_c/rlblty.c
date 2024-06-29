/***********************************************************************//** 
 * \file  rlblty.c
 * \brief  reliability related: LVD, IWDT, SRAM check, Flash check, RST
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-13 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include "csp_lvd.h"
#include "drv/common.h"
#include "drv/rlblty.h"
#include <drv/irq.h>
#include "csp_syscon.h"

csi_lvd_t tLvd;
csi_sram_check_t tSramChk;
csi_reliability_t tReliability;
csi_emcm_check_t tEmcm;


/**
  \brief       Rlblty interrupt handling function
  \pasram[in]   arg    Callback function member variables
  \return      None
*/
void rlblty_irq_handler(void)
{
   
    csp_syscon_t *ptLvdBase = (csp_syscon_t *)(tReliability.dev.reg_base);
	uint32_t wInt = csp_syscon_get_int_st(ptLvdBase);
	if (wInt & (0x1 << LVD_INT) && tLvd.callback)
	{
		csp_syscon_int_clr(ptLvdBase, LVD_INT);
		tLvd.callback();
	}
	
}

/**
  \brief       Initialize Virtual Reliablity Interface. Initializes the resources needed for the EPT interface
  \param       none
  \return      error code \ref csi_error_t
*/
csi_error_t csi_reliability_init(void)
{
	csi_error_t ret = CSI_OK;
	
	if (CSI_OK == target_get(DEV_SYSCON_TAG, 0, &(tReliability.dev))){
		csi_irq_attach(SYSCON_IRQn, &rlblty_irq_handler, & (tReliability.dev));
	}
	else {
		ret = CSI_ERROR;
	}
	return ret;
}

/**
  \brief       Attach the callback handler to LVD module
  
  \pasram[in]   callback    user callback function
  \return      error  code \ref csi_error_t
*/
csi_error_t csi_lvd_attach_callback(void *callback)
{	
	tLvd.callback = callback;   
    csi_irq_enable(SYSCON_IRQn);

    return CSI_OK;
}


/// ************************************************************************
///						for LVD module
///*************************************************************************
/**
  \brief       lvd falling int enable   
  \pasram      eLvl      lvd level
  \param       callback: user callback function
  \return     none
*/
void csi_lvd_f_enable(clvd_level_e eLvl, void *callback)
{
	csp_syscon_t *ptLvdBase;	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	
	csp_set_lvd_level(ptLvdBase, eLvl);	
	csp_set_lvd_int_pol(ptLvdBase, LVDINT_F);
	
	csp_syscon_int_enable(ptLvdBase, LVD_INT, ENABLE);
	csp_lvd_lvr_enable(ptLvdBase, 1);
	
	tLvd.callback = callback;   
    csi_irq_enable(SYSCON_IRQn);
}

/**
  \brief       lvd V risng int enable 
  \pasram     eLvl      lvd level
  \param       callback: user callback function
  \return     none
*/
void csi_lvd_r_enable(clvd_level_e eLvl, void *callback)
{
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	
	csp_set_lvd_level(ptLvdBase, eLvl);	
	csp_set_lvd_int_pol(ptLvdBase, LVDINT_R);
	
	csp_syscon_int_enable(ptLvdBase, LVD_INT, ENABLE);
	csp_lvd_lvr_enable(ptLvdBase, 1);
	
	tLvd.callback = callback;   
    csi_irq_enable(SYSCON_IRQn);
}

/**
  \brief       lvd V risng and falling int enable 
  \pasram[in]   eLvl      lvd level
  \pasram[in]   callback  user callback funtion
  \return     none
*/
void csi_lvd_f_r_enable(clvd_level_e eLvl, void *callback)
{
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	
	csp_set_lvd_level(ptLvdBase, eLvl);	
	csp_set_lvd_int_pol(ptLvdBase, LVDINT_BOTH);
	
	csp_syscon_int_enable(ptLvdBase, LVD_INT, ENABLE);
	csp_lvd_lvr_enable(ptLvdBase, 1);
	
	tLvd.callback = callback;   
    csi_irq_enable(SYSCON_IRQn);
}

/**
  \brief       Enable LVR
  \param       eLvl LVR level
  \return      error code \ref csi_error_t
*/
void csi_lvr_enable(clvr_level_e eLvl)
{
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	csp_set_lvr_level(ptLvdBase, eLvl);	
	csp_lvd_lvr_enable(ptLvdBase, 1);
}

/**
  \brief       Disable LVR
  \return      error code \ref csi_error_t
*/
void csi_lvr_disable(void)
{
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	csp_lvd_lvr_enable(ptLvdBase, 0);
}



/**
  \brief       Get lvd level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_lvd_get_lvdlevel(void)
{	
	
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	return (csp_lvd_get_lvdlevel(ptLvdBase));
	
}

/**
  \brief       Get lvr level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_lvd_get_lvrlevel(void)
{
	csp_syscon_t *ptLvdBase;
	
	ptLvdBase = (csp_syscon_t *) APB_SYS_BASE;
	return (csp_lvd_get_lvrlevel(ptLvdBase));
}


/// ************************************************************************
///						for Reset Source Check
///*************************************************************************

/**
  \brief       get chip reset reason
  \param       none
  \return      csi_rst_rsr_e 
*/
csi_rst_rsr_e csi_get_rst_reason(void)
{
	uint32_t wRsr;
	csi_rst_rsr_e eRsr = NO_RST;
	uint8_t i;

	csp_syscon_t *rsr_base;
	
	rsr_base = (csp_syscon_t *) APB_SYS_BASE;
	wRsr = csp_rst_rd_st(rsr_base);
	i = 0;

	while(wRsr)
	{
		if (wRsr & (0x1 << i))
		{
			eRsr = i;
			break;
		}
		i++;
	}
	return (eRsr);
}

/// ************************************************************************
///						for sram check error
///*************************************************************************
/**
  \brief       set sramcheck fail time.
  \param       wVal times
  \return      error code \ref csi_error_t
*/
void csi_sramcheck_set_times(uint16_t wVal)
{

	csp_syscon_t *ptSramCheckBase;
	
	ptSramCheckBase = (csp_syscon_t *) APB_SYS_BASE;
	csp_sramcheck_set_times(ptSramCheckBase, wVal);
	
}

/**
  \brief       set chip to reset when sramcheck times > preset value
  \param       none
  \return      none
*/
void csi_sramcheck_rst(void)
{

	csp_syscon_t *ptSramCheckBase;
	
	ptSramCheckBase = (csp_syscon_t *)APB_SYS_BASE;
	csp_sramcheck_rst(ptSramCheckBase);
	csp_sramcheck_enable(ptSramCheckBase, ENABLE);
	
}

/**
  \brief       claim INT when sramcheck times > preset value
  \param       callback: user callback function
  \return      none
*/
void csi_sramcheck_int(void *callback)
{
	csp_syscon_t *ptSramCheckBase;
	
	ptSramCheckBase = (csp_syscon_t *) APB_SYS_BASE;
	csp_sramcheck_int(ptSramCheckBase);
	tSramChk.callback = callback;
	csi_irq_enable(SYSCON_IRQn);
	csp_sramcheck_enable(ptSramCheckBase, ENABLE);
}

/**
  \brief       disable sramcheck funtion
  \param       none
  \return      none
*/
void csi_sramcheck_disable(void)
{
	csp_syscon_t *ptSramCheckBase;
	
	ptSramCheckBase = (csp_syscon_t *) APB_SYS_BASE;
	csp_sramcheck_enable(ptSramCheckBase, DISABLE);
}



///for  flash check error

/**
  \brief       set flashcheck fail time.
  \param       wVal times
  \return      error code \ref csi_error_t
*/
void csi_flashcheck_set_times(uint32_t wVal)
{	
	
	csp_syscon_t *flashheck_base;
	
	flashheck_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_flashcheck_set_times(flashheck_base, wVal);
	
}

/**
  \brief       set chip to reset when flashcheck times > preset value
  \param       none
  \return      none
*/
void csi_flashcheck_rst(void)
{
	
	csp_syscon_t *flashheck_base;
	
	flashheck_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_flashcheck_enable(flashheck_base, ENABLE);
	
}

/**
  \brief       disable flashcheck funtion
  \param       none
  \return      none
*/
void csi_flashcheck_disable(void)
{

	csp_syscon_t *flashheck_base;
	
	flashheck_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_flashcheck_enable(flashheck_base, DISABLE);
}

/// ************************************************************************
///						for EM clock monitor
///*************************************************************************

/**
  \brief       claim INT and switch sysclk to IMOSC when EMOSC failure detected
  \param       callback: user callback function
  \return      none
*/
void csi_EMCM_2_IMOSC_int(void * callback)
{
	csp_syscon_t *emcm_base;	
	emcm_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_emcm_enable(emcm_base, ENABLE);
	csp_emcm_rst_enable(emcm_base, DISABLE);
	csp_syscon_int_enable(emcm_base, EMFAIL_INT, ENABLE);
	tEmcm.callback = callback;
	csi_irq_enable(SYSCON_IRQn);
}

/**
  \brief      rest chip when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_EMCM_rst(void)
{	
	csp_syscon_t *emcm_base;
	
	emcm_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_emcm_enable(emcm_base, ENABLE);
	csp_emcm_rst_enable(emcm_base, ENABLE);
	
}

/**
  \brief       switch sysclk to IMOSC when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_EMCM_2_IMOSC(void)
{
	csp_syscon_t *emcm_base;
	
	emcm_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_emcm_enable(emcm_base, ENABLE);
	csp_emcm_rst_enable(emcm_base, DISABLE);
}

/**
  \brief       disable EMOSC monitor funtion
  \param       none
  \return      none
*/
void csi_EMCM_disable(void)
{
	csp_syscon_t *emcm_base;
	
	emcm_base = (csp_syscon_t *) APB_SYS_BASE;
	csp_emcm_enable(emcm_base, DISABLE);
	
}