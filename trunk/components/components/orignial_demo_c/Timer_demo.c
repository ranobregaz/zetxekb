/***********************************************************************//** 
 * \file  Timer_demo.c
 * \brief  Basic Timer示例代码
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-14 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include "soc.h"
#include "sys_clk.h"
#include "drv/pin.h"
#include "timer.h"
#include "board_config.h"



/* Private macro-----------------------------------------------------------*/
//#define		TIMER_PWM_SEL			//timer pwm output select

/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/
csi_timer_t	g_tTimer;
csi_gpio_t g_tGpioB0;

/** \brief 用户中断回调函数，在中断中toggle一个IO
 * 
 *  \param[in] timer_handle: handle timer handle to operate
 *  \param[in] arg: user para
 *  \return none
 */
static void apt_timer_event(csi_timer_t *timer_handle, void *arg)
{
	uint32_t wArg = (uint32_t)arg;
	
	/* add user code*/
	if(wArg == 0x01)
	{
		csi_gpio_toggle(&g_tGpioB0, GPIO_PIN_MSK);
	}
	
}

/** \brief Basic timer定时中断里IO 翻转
 *  \param[in] none
 *  \return error code
 */
csi_error_t timer_senario0(void)
{
	csi_error_t tRet = CSI_OK;
	

	uint32_t arg = 0x01;
	
	tRet =  csi_gpio_init(&g_tGpioB0, GPIO_IDX);
	tRet =  csi_pin_set_mux(GPIO_PIN, GPIO_PIN_FUNC);
	
	tRet =  csi_timer_init(&g_tTimer, 0x0);	
	tRet =   csi_timer_attach_callback(&g_tTimer, apt_timer_event,(void *)arg);		
	tRet =  csi_timer_start(&g_tTimer, 10000);		//10ms 翻转一次
	
	while(1);
	return tRet;
}

/** \brief timer PWM输出

 *  \param[in] none
 *  \return error code
 */
csi_error_t timer_senario1(void)
{
	csi_error_t tRet = CSI_OK;	

	csi_pin_set_mux(PB02, PB02_BT0_OUT);			//PB02 pwm output	
	tRet =  csi_timer_init(&g_tTimer, 0x0);		
	tRet =  csi_timer_pwm_init(&g_tTimer,T_START_HIGH,T_IDLE_HIGH);		
	tRet =  csi_timer_pwm_start(&g_tTimer,20000,50);	//20KHz, duty cycle = 50%

	while(1);
	return tRet;
}