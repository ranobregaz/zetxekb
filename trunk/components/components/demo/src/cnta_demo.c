/***********************************************************************//** 
 * \file  cnta_demo.c
 * \brief  counterA 示例代码

 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version    <th>Author  <th>Description
 * <tr><td> 2020-09-21 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2020-05-27 <td>V0.1  <td>LQ      <td>modify
 * </table>
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


/**
  \brief       counterA 定时中断示例
  \return      int
*/
int cnta_senario0(void)
{	
	int iRet = 0;
	uint32_t wPinMask = (0X01 << PA01);//pa0_1
	csi_gpio_port_dir(GPIOA0, wPinMask, GPIO_DIR_OUTPUT);			//gpio_port as output 
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high,then toggle it in cnta interrupt
	
	csi_pin_set_mux(PA011,PA011_CNTA_BUZ);				   			//set counter output pin
	csi_cnta_timer_init(CA0,CNTA_CK_DIV8,CNTA_REPEAT_MODE);
	csi_cnta_timer_start(CA0,1000);		   							//1000us
	while(1)
	{
		nop;
	}
	return iRet;
}

/**
  \brief       counterA pwm示例
  \return      int
*/
int cnta_senario1(void)
{		
	int iRet = 0;
	
	csi_pin_set_mux(PA011,PA011_CNTA_BUZ);//set counter output pin	
	//csi_pin_set_mux(PA05,PA05_CNTA_BUZ);//set counter output pin	
	//csi_pin_set_mux(PB01,PB01_CNTA_BUZ);//set counter output pin	
	csi_cnta_pwm_init(CA0, CNTA_CK_DIV8 ,PWM_CARRIER ,POLAR_LOW, STOP_LOW);
	csi_cnta_pwm_start(CA0, 38000, 33);//fre:38000hz,duty:33%
	while(1)
	{
		nop;
	}
	return iRet;
}