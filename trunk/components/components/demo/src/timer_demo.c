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
#include <timer.h>
#include <pin.h>

#include "csp.h"
#include "demo.h"
/* externs function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/

/* Private macro-----------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/
typedef enum{
	TIMER_DEMO_TIMING		= 0,	//TIMER timing
	TIMER_DEMO_PWM,					//TIMER pwm
}timer_demo_e;


/** \brief timer test
 * 
 *  \param[in] none
 *  \return error code
 */
int timer_demo(void)
{
	int iRet = 0;
	csi_timer_t tTimer;							//timer Parameter config structure
	timer_demo_e eTmDemo = TIMER_DEMO_TIMING;
	
	switch(eTmDemo)
	{
		case TIMER_DEMO_TIMING:
			
			csi_bt_timer_init(BT0, 1000);		//1000us = 1ms
			csi_bt_open(BT0);					//start  timer
			break;
		case TIMER_DEMO_PWM:
			
			csi_pin_set_mux(PB02, PB02_BT0_OUT);					//PB02 pwm output	
			
			//init timer pwm para
			tTimer.byIdleLevel 	= T_IDLE_HIGH;						//pwm ouput idle level
			tTimer.byStartLevel = T_START_HIGH;						//pwm ouput start level
			tTimer.byDutyCycle 	= 40;								//pwm ouput duty cycle			
			tTimer.wFreq 		= 10000;							//pwm ouput frequency
			tTimer.byInter 		= TIMER_PEND_INT | TIMER_CMP_INT;	//interrupt(PEND and CMP)
			
			csi_bt_pwm_init(BT0, &tTimer);
			csi_bt_open(BT0);									//start  timer
			
			break;
		default:
			break;
	}
	
	return iRet;
}