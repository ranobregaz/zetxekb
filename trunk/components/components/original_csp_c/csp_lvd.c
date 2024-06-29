/***********************************************************************//** 
 * \file  csp_lvd.c
 * \brief  LVD description and static inline functions at register level
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-13 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include "csp_lvd.h"


 void csp_lvd_int_config(csp_syscon_t *ptSysconBase, bool bEnable)
 {
	if (bEnable)
	{
		csp_syscon_int_enable(ptSysconBase, LVD_INT, 1);
		NVIC_EnableIRQ(SYSCON_IRQn);
	} 
	else
	{
		csp_syscon_int_enable(ptSysconBase, LVD_INT, 0);
		NVIC_DisableIRQ(SYSCON_IRQn);
	}
 }
 
 uint32_t csp_lvd_get_lvdlevel(csp_syscon_t *ptSysconBase)
{
	uint32_t ret = 24;
	switch ((ptSysconBase->LVDCR & LVD_LV_MSK) >> 8)
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

uint32_t csp_lvd_get_lvrlevel(csp_syscon_t *ptSysconBase)
{
	uint32_t ret = 19;
	switch ((ptSysconBase->LVDCR & LVR_LV_MSK) >> LVR_LV_POS)
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


