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
	
	csi_pin_set_mux(PA05,PIN_OUTPUT);			//PA05 as output
	csi_pin_set_high(PA05);						//PA05 ouput high
	mdelay(100);
	csi_pin_set_low(PA05);						//PA05 low
	mdelay(100);
	csi_pin_set_high(PA05);						//PA05 high
	mdelay(100);
	
	csi_pin_output_mode(PA05, GPIO_OPEN_DRAIN);	//open drain of ouput
	csi_pin_set_high(PA05);						//PA05 high
	mdelay(100);
	csi_pin_set_low(PA05);						//PA05 low
	mdelay(100);
	csi_pin_set_high(PA05);						//PA05 high
	mdelay(100);
	
	csi_pin_output_mode(PA05, GPIO_PUSH_PULL);	//push pull of output
	csi_pin_set_high(PA05);						//gpio_port high
	mdelay(100);
	csi_pin_set_low(PA05);						//gpio_port low
	mdelay(100);
	csi_pin_set_high(PA05);						//gpio_port high
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
	
	csi_pin_set_mux(PA05,PIN_INPUT);			//PA05 as input
	csi_pin_pull_mode(PA05,GPIO_PULLNONE);		//pullnone
	mdelay(100);
	wStatus = csi_pin_read(PA05);
	while(wStatus != 0);
	
	csi_pin_pull_mode(PA05,GPIO_PULLUP);		//pullup
	mdelay(100);
	wStatus = csi_pin_read(PA05);
	while(wStatus != (0x01 << 0x05));
	
	csi_pin_pull_mode(PA05,GPIO_PULLDOWN);		//pulldown
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
	
	csi_pin_set_mux(PA05, PIN_INPUT);							//PA05 as input
	csi_pin_pull_mode(PA05, GPIO_PULLUP);						//pullup
	csi_pin_irq_mode(PA05,EXI_GRP2, GPIO_IRQ_FALLING_EDGE);		//falling edge 
	csi_pin_irq_enable(PA05,EXI_GRP5, ENABLE);					//irq enable,select EXI group5 				
	
	return iRet;
}