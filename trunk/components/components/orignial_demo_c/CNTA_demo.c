/***********************************************************************//** 
 * \file  CNTA_demo.c
 * \brief  counterA 示例代码

 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-09-21 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * 
 * \verbatim
 * ETCB的使用方法可以参看
 * 	1、gpta_senario3(): EPT和ADC通过ETCB的联动
 * 	2、ept_senario1()：EXI和GPTA通过ETCB的联动
 * \endverbatim 
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "cnta.h"
#include "gpio.h"
#include "pin.h"
#include "board_config.h"
/* externs function--------------------------------------------------------*/

/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
/* device instance ------------------------------------------------------*/
csi_cnta_t	g_tCnta;
csi_gpio_t	g_tGpioB0;


/** \brief cnta interrupt callback function
 * 
 *  \param cnta_handle: handle timer handle to operate
 *  \param arg: para
 *  \return none
 */
static void user_cnta_event(csi_cnta_t *cnta_handle, void *arg)
{
	csi_gpio_toggle(&g_tGpioB0, GPIO_PIN_MSK);		//toggle PB0.2
	
}


/**
  \brief       counterA 定时中断示例
  \return      csi_error_t
*/
csi_error_t cnta_senario0(void)
{	
	csi_error_t tRet = CSI_OK;
	csi_gpio_init(&g_tGpioB0, 1);								//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	
	csi_pin_set_mux(PA011,PA011_CNTA_BUZ);
	tRet = csi_cnta_init(&g_tCnta, 0);	
	
	tRet =  csi_cnta_attach_callback(&g_tCnta, user_cnta_event, NULL);		
	tRet = csi_cnta_start(&g_tCnta, 5000);		//5ms定时
	
	while(1);
	return tRet;
	
}