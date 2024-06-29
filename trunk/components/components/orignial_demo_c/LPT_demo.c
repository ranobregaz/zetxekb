/***********************************************************************//** 
 * \file  LPT_demo.c
 * \brief  low power counter 示例代码

 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-1 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * 
 * \verbatim

 * \endverbatim 
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "lpt.h"
#include "gpio.h"
#include "pin.h"
#include "board_config.h"
/* externs function--------------------------------------------------------*/

/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
/* device instance ------------------------------------------------------*/
csi_lpt_t	g_tLpt;
csi_gpio_t	g_tGpioB0;

void user_lpt_handler(csi_lpt_t *ptLpt, void *arg)
{
	csi_gpio_toggle(&g_tGpioB0, GPIO_PIN_MSK);		//toggle PB0.2
}

/**
  \brief       LPT PWM输出示例
  \return      csi_error_t
*/
csi_error_t lpt_senario0(void)
{
	csi_error_t tRet = CSI_OK;
	csi_gpio_init(&g_tGpioB0, 1);								//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	csi_pin_set_mux(PA09, PA09_LPT_OUT);						//将PA09设为LPT_OUT
	
	csi_lpt_init(&g_tLpt, 0);
	tRet = csi_lpt_pwm_init(&g_tLpt, LPT_POL_LOW, LPT_IDLE_Z);	//初始低电平，停止时highZ
	
	tRet = csi_lpt_pwm_start(&g_tLpt, LPT_CLK_ISCLK, 1000, 70); //使用ISOSC，fre = 1KHz, ducy = 70%
	
	while(1);
	return tRet;
}

/**
  \brief       LPT 定时中断示例
  \return      csi_error_t
*/
csi_error_t lpt_senario1(void)
{
	csi_error_t tRet = CSI_OK;
	csi_gpio_init(&g_tGpioB0, 1);								//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	csi_pin_set_mux(PA09, PA09_LPT_OUT);						//将PA09设为LPT_OUT
	
	csi_lpt_init(&g_tLpt, 0);
	csi_lpt_attach_callback(&g_tLpt,user_lpt_handler, (void *)LPT_INT_PEND);
	tRet = csi_lpt_start(&g_tLpt, LPT_CLK_ISCLK, 200);			//使用ISOSC，200ms定时
		
	while(1);
	return tRet;
}

