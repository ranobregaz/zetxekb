/***********************************************************************//** 
 * \file  pin_demo.c
 * \brief  PIN_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-4-25 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include "sys_clk.h"
#include <drv/gpio.h>
#include <drv/pin.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


/** \brief gpio pin output demo 
 * 
 *  \param[in] none
 *  \return error code
 */
int pin_output_demo(void)
{
	int iRet = 0;
	
	csi_pin_set_mux(PA05,PA05_OUTPUT);			//PA05 配置为输出
	csi_pin_set_high(PA05);						//PA05 输出高
	mdelay(100);								//延时100ms
	csi_pin_set_low(PA05);						//PA05 输出低
	mdelay(100);
	csi_pin_set_high(PA05);						//PA05 输出低
	mdelay(100);
	
	csi_pin_output_mode(PA05, GPIO_OPEN_DRAIN);	//PA05 配置为开漏输出
	csi_pin_set_high(PA05);						
	mdelay(100);
	csi_pin_set_low(PA05);						
	mdelay(100);
	csi_pin_set_high(PA05);						
	mdelay(100);
	
	csi_pin_output_mode(PA05, GPIO_PUSH_PULL);	//PA05 配置为推挽输出
	csi_pin_set_high(PA05);						
	mdelay(100);
	csi_pin_set_low(PA05);						
	mdelay(100);
	csi_pin_set_high(PA05);						
	mdelay(100);
	
	return iRet;
}

/** \brief gpio pin output demo 
 * 
 *  \param[in] none
 *  \return error code
 */
int pin_input_demo(void)
{
	int iRet = 0;
	
	uint32_t wStatus;
	
	csi_pin_set_mux(PA05,PA05_INPUT);			//PA05 配置为输入
	csi_pin_pull_mode(PA05,GPIO_PULLNONE);		//无上下拉
	mdelay(100);
	wStatus = csi_pin_read(PA05);				//PA05 输入状态读取(0/1 = 高/低)
	while(wStatus != 0);
	
	csi_pin_pull_mode(PA05,GPIO_PULLUP);		//上拉
	mdelay(100);
	wStatus = csi_pin_read(PA05);
	while(wStatus != (0x01 << 0x05));
	
	csi_pin_pull_mode(PA05,GPIO_PULLDOWN);		//下拉
	mdelay(100);
	wStatus = csi_pin_read(PA05);
	while(wStatus != 0);
	
	return iRet;
}

/** \brief gpio pin output demo 
 * 
 *  \param[in] none
 *  \return error code
 */
int pin_irq_demo(void)
{
	int iRet = 0;
	
	csi_pin_set_mux(PA05, PA05_INPUT);							//PA05 配置为输入
	csi_pin_pull_mode(PA05, GPIO_PULLUP);						//PA05 上拉
	csi_pin_irq_mode(PA05,EXI_GRP2, GPIO_IRQ_FALLING_EDGE);		//PA05 下降沿产生中断
	csi_pin_irq_enable(PA05, EXI_GRP5, ENABLE);					//PA05 中断使能，选择中断组5			
	
	return iRet;
}