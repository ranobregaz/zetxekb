/***********************************************************************//** 
 * \file  lpt_demo.c
 * \brief  LPT_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-27 <td>V0.0 <td>YYM     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include <lpt.h>
#include <pin.h>
#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief lpt timer
 * 
 *  \param[in] none
 *  \return error code
 */
int lpt_timer_demo(void)
{
	int iRet = 0;
	
	csi_lpt_timer_init(LPT,LPT_CLK_ISCLK,200);       //初始化lpt,选用内部超低功耗时钟,定时200ms
	csi_lpt_irq_enable(LPT,LPT_MATCH_INT,ENABLE);	 //选用MATCH中断 
	csi_lpt_start(LPT);	                             //启动lpt
	
	return iRet;	
}

/** \brief lpt pwm ouput
 * 
 *  \param[in] none
 *  \return error code
 */
int lpt_pwm_demo(void)
{
	int iRet = 0;
	
	csi_lpt_pwm_config_t tLptPwmCfg;  	
	csi_pin_set_mux(PA09, PA09_LPT_OUT);						 //将PA09设为LPT_OUT
	
	tLptPwmCfg.eClksrc = LPT_CLK_ISCLK;                          //PWM 时钟选择
	tLptPwmCfg.eStartpol = LPT_POL_LOW;                          //初始低电平
	tLptPwmCfg.eIdlepol = LPT_IDLE_Z;                            //停止时highZ
	tLptPwmCfg.byCycle = 70;                                     //PWM 输出占空比(0~100)	
	tLptPwmCfg.wFreq = 1000;                                     //PWM 输出频率
	if(csi_lpt_pwm_init(LPT, &tLptPwmCfg) == CSI_OK){            //初始化lpt
		csi_lpt_start(LPT);                                      //启动lpt
	}	
	
	return iRet;	
}

