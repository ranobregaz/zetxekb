/***********************************************************************//** 
 * \file  iwdt_demo.c
 * \brief  IWDT_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY    <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include "iwdt.h"
#include "pin.h"
#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
typedef enum{
	IWDT_DEMO_NORMAL	= 0,	//IWDT normal mode
	IWDT_DEMO_INTER				//IWDT interrupt mode
}iwdt_demo_e;

/** \brief timer test
 * 
 *  \param[in] none
 *  \return error code
 */
int iwdt_demo(void)
{
	int iRet = 0;

	iwdt_demo_e eIwdtDemo = IWDT_DEMO_INTER;
	
	switch(eIwdtDemo)
	{
		case IWDT_DEMO_NORMAL:
		
			csi_wdt_init(IWDT, IWDT_TV_1000);				//reset time = 1s
			csi_wdt_open(IWDT);								//start iwdt
//			mdelay(500);
//			mdelay(400);
//			csi_wdt_feed(IWDT);								//feed iwdt
//			mdelay(500);
			
			break;
		case IWDT_DEMO_INTER:
		
			csi_wdt_init(IWDT, IWDT_TV_1000);				//reset time = 2s
			csi_wdt_irq_enable(IWDT,IWDT_INT_4_8, ENABLE);	//iwdt interrupt timer 
			csi_wdt_open(IWDT);								//start iwdt
//			mdelay(600);
			break;
		default:
			break;
	}
	
	return iRet;
}