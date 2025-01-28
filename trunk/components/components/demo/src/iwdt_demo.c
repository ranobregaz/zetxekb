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


/** \brief iwdt normal mode
 * 
 *  \param[in] none
 *  \return error code
 */
int iwdt_normal_demo(void)
{
	int iRet = 0;

	csi_iwdt_init(IWDT_TV_1000);			//reset time = 1s
	csi_iwdt_open();						//open iwdt
//	mdelay(500);
//	mdelay(400);
//	csi_wdt_feed(IWDT);						//feed iwdt
//	mdelay(500);
			
	return iRet;
}
/** \brief iwdt working with interrupt
 * 
 *  \param[in] none
 *  \return error code
 */
int iwdt_irq_demo(void)
{
	int iRet = 0;

	csi_iwdt_init(IWDT_TV_1000);					//reset time = 1s
	csi_iwdt_irq_enable(IWDT_INT_4_8, ENABLE);		//iwdt interrupt timer 
	csi_iwdt_open();								//start iwdt
//	mdelay(600);

	return iRet;
}