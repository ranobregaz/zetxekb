/***********************************************************************//** 
 * \file  reliability.c
 * \brief  reliability driver, including LVR/LVD, RSR, emcm and memory check
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-27 <td>V2.0 <td>WNN    <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "csp.h"
#include "reliability.h"
#include "irq.h"
#include "soc.h"
#include "csp_syscon.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/// ************************************************************************
///						for LVD module
///*************************************************************************
/**
  \brief       lvd  int enable  
  \param       ePol      lvd falling/rising/both 
  \param       eLvl      lvd level
  \return      none
*/
void csi_lvd_int_enable(clvd_pol_e ePol, clvd_level_e eLvl)
{
	
	csp_set_lvd_level(SYSCON, eLvl);	
	csp_set_lvd_int_pol(SYSCON, ePol);
	
	csp_syscon_int_enable(SYSCON, LVD_INT, ENABLE);
	csp_lvd_lvr_enable(SYSCON, ENABLE);
	 
	csi_vic_enable_irq(SYSCON_IRQn);
}

/**
  \brief       lvd  int disable  
  \return      none
*/
void csi_lvd_int_disable(void)
{
	csp_lvd_lvr_enable(SYSCON, DISABLE);
}

/**
  \brief       Enable LVR
  \param       eLvl LVR level
  \return      error code \ref csi_error_t
*/
void csi_lvr_enable(clvr_level_e eLvl)
{
	
	csp_set_lvr_level(SYSCON, eLvl);	
	csp_lvd_lvr_enable(SYSCON, ENABLE);
}

/**
  \brief       Disable LVR
  \return      none
*/
void csi_lvr_disable(void)
{
	csp_lvd_lvr_enable(SYSCON, 0);
}

/**
  \brief       Get lvd level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_get_lvdlevel(void)
{		
	uint32_t ret = 24;
	switch ((SYSCON->LVDCR & LVD_LV_MSK) >> 8)
	{
		case (0):
			ret = 24;
			break;
		case (1):
			ret = 21;
			break;
		case (2):
			ret = 27;
			break;
		case (3):
			ret = 30;
			break;
		case (4):
			ret = 33;
			break;
		case (5):
			ret = 36;
			break;
		case (6):
			ret = 39;
			break;
		case (7):
			ret = 10;
			break;
		default :
			break;	
	}
	return ret;
	
}

/**
  \brief       Get lvr level 
  \param       none
  \return      lvd    lvd level
*/
uint32_t csi_get_lvrlevel(void)
{
	uint32_t ret = 19;
	switch ((SYSCON->LVDCR & LVR_LV_MSK) >> LVR_LV_POS)
	{
		case (0):
			ret = 19;
			break;
		case (1):
			ret = 22;
			break;
		case (2):
			ret = 25;
			break;
		case (3):
			ret = 28;
			break;
		case (4):
			ret = 31;
			break;
		case (5):
			ret = 34;
			break;
		case (6):
			ret = 37;
			break;
		case (7):
			ret = 40;
			break;
		default :
			break;	
	}
	return ret;
}

/// ************************************************************************
///						for Reset Source Check
///*************************************************************************

/**
  \brief       get chip reset reason
  \param       none
  \return      csi_rsr_e 
*/
csi_rsr_e csi_get_rst_reason(void)
{
	uint32_t wRsr;
	csi_rsr_e eRsr = NO_RST;
	uint8_t i;

	wRsr = csp_rst_rd_st(SYSCON);
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
  \return      none
*/
void csi_sramcheck_set_times(uint16_t wVal)
{
	csp_sramcheck_set_times(SYSCON, wVal);
	
}

/**
  \brief       set chip to reset when sramcheck fail times > preset value
  \param       none
  \return      none
*/
void csi_sramcheck_rst(void)
{

	csp_sramcheck_rst(SYSCON);
	csp_sramcheck_enable(SYSCON, ENABLE);
	
}

/**
  \brief       claim INT when sramcheck fail times > preset value
  \param       callback: user callback function
  \return      none
*/
void csi_sramcheck_int(void)
{

	csp_sramcheck_int(SYSCON);
	csi_vic_enable_irq(SYSCON_IRQn);
	csp_sramcheck_enable(SYSCON, ENABLE);
}

/**
  \brief       disable sramcheck funtion
  \param       none
  \return      none
*/
void csi_sramcheck_disable(void)
{
	csp_sramcheck_enable(SYSCON, DISABLE);
}

/// ************************************************************************
///						for flash check error
///*************************************************************************
/**
  \brief       set flashcheck fail time.
  \param       wVal times
  \return      none
*/
void csi_flashcheck_set_times(uint32_t wVal)
{	
	csp_flashcheck_set_times(SYSCON, wVal);
}

/**
  \brief       set chip to reset when flashcheck times > preset value
  \param       none
  \return      none
*/
void csi_flashcheck_rst(void)
{
	csp_flashcheck_enable(SYSCON, ENABLE);
}

/**
  \brief       disable flashcheck funtion
  \param       none
  \return      none
*/
void csi_flashcheck_disable(void)
{
	csp_flashcheck_enable(SYSCON, DISABLE);
}

/// ************************************************************************
///						for EM clock monitor
///*************************************************************************

/**
  \brief       claim INT and switch sysclk to IMOSC when EMOSC failure detected
  \return      none
*/
void csi_emcm_2_imosc_int(void)
{

	csi_imosc_enable(0);
	csp_emcm_enable(SYSCON, ENABLE);
	csp_emcm_rst_enable(SYSCON, DISABLE);
	
	csp_syscon_int_enable(SYSCON, EMFAIL_INT, ENABLE);
	csi_vic_enable_irq(SYSCON_IRQn);
}

/**
  \brief      rest chip when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_emcm_rst(void)
{	
	csi_imosc_enable(0);
	csp_emcm_enable(SYSCON, ENABLE);
	csp_emcm_rst_enable(SYSCON, ENABLE);
	
}

/**
  \brief       disable EMOSC monitor funtion
  \param       none
  \return      none
*/
void csi_emcm_disable(void)
{
	csp_emcm_enable(SYSCON, DISABLE);
	csi_imosc_disable();
	
}
