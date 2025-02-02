/***********************************************************************//** 
 * \file  timer_demo.c
 * \brief  TIMER_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-11 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include <drv/bt.h>
#include <drv/pin.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief bt timer
 * 
 *  \param[in] none
 *  \return error code
 */
int bt_timer_demo(void)
{
	int iRet = 0;
			
	csi_bt_timer_init(BT0, 1000);		//1000us = 1ms
	csi_bt_start(BT0);					//start  timer

	return iRet;
}

/** \brief bt pwm ouput
 * 
 *  \param[in] none
 *  \return error code
 */
int bt_pwm_demo(void)
{
	int iRet = 0;
	csi_bt_pwm_config_t tPwmCfg;								//timer Parameter config structure
	
	csi_pin_set_mux(PB02, PB02_BT0_OUT);						//PB02 pwm output	
	
	//init timer pwm para config
	tPwmCfg.byIdleLevel 	= BT_PWM_IDLE_HIGH;					//pwm ouput idle level
	tPwmCfg.byStartLevel	= BT_PWM_START_HIGH;				//pwm ouput start level
	tPwmCfg.byDutyCycle 	= 40;								//pwm ouput duty cycle			
	tPwmCfg.wFreq 			= 10000;							//pwm ouput frequency
	tPwmCfg.byInter 		= BT_INTSRC_PEND | BT_INTSRC_CMP;	//interrupt(PEND and CMP)
	
	csi_bt_pwm_init(BT0, &tPwmCfg);
	csi_bt_start(BT0);											//start  timer
			
	
	
	return iRet;
}