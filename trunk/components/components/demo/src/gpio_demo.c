/***********************************************************************//** 
 * \file  gpio_demo.c
 * \brief  GPIO_DEMO description and static inline functions at register level 
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
#include <gpio.h>
#include <pin.h>
#include "csp.h"

#include "demo.h"
/* externs function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/

/* Private macro-----------------------------------------------------------*/
#define		PINMASK_PA00		(0x01ul << 0)	
#define		PINMASK_PA01		(0x01ul << 1)
#define		PINMASK_PA02		(0x01ul << 2)	
#define		PINMASK_PA05		(0x01ul << 5)
#define		PINMASK_PA09		(0x01ul << 4)	
#define		PINMASK_PA010		(0x01ul << 10)
#define		PINMASK_PA013		(0x01ul << 13)

#define		PINMASK_PB01		(0x01ul << 1)
#define		PINMASK_PB02		(0x01ul << 2)
#define		PINMASK_PB05		(0x01ul << 5)
/* Private variablesr------------------------------------------------------*/
typedef enum{
	GPIO_DEMO_OUPUT		= 0,		//gpio output
	GPIO_DEMO_INPUT,				//gpio input
	GPIO_DEMO_INTER					//gpio interrupt
}gpio_demo_e;

/** \brief gpio port output demo 
 * 
 *  \param[in] none
 *  \return error code
 */
int gpio_port_ouput_demo(void)
{
	int iRet = 0;
	uint32_t wPinMask = PINMASK_PA00 | PINMASK_PA02;
	
	csi_gpio_port_dir(GPIOA0, wPinMask, GPIO_DIR_OUTPUT);			//gpio_port as output 
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 0);						//gpio_port low
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	
	csi_gpio_port_output_mode(GPIOA0, wPinMask, GPIO_OPEN_DRAIN);	//open drain of ouput
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 0);						//gpio_port low
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	
	csi_gpio_port_output_mode(GPIOA0, wPinMask, GPIO_PUSH_PULL);	//push pull of output
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 0);						//gpio_port low
	mdelay(100);
	csi_gpio_port_write(GPIOA0, wPinMask, 1);						//gpio_port high
	mdelay(100);
	
	return iRet;
}

/** \brief gpio port output demo 
 * 
 *  \param[in] none
 *  \return error code
 */
int gpio_port_input_demo(void)
{
	int iRet = 0;
	uint32_t wStatus;
	uint32_t wPinMask = PINMASK_PA00 | PINMASK_PA02;
	
	csi_gpio_port_dir(GPIOA0, wPinMask, GPIO_DIR_INPUT);			//gpio_port as output 
	csi_gpio_port_pull_mode(GPIOA0, wPinMask, GPIO_PULLNONE);		//pullnone
	mdelay(100);
	wStatus = csi_gpio_port_read(GPIOA0,wPinMask);
	while(wStatus != 0);
	
	csi_gpio_port_pull_mode(GPIOA0, wPinMask, GPIO_PULLUP);			//pullup
	mdelay(100);
	wStatus = csi_gpio_port_read(GPIOA0,wPinMask);
	while(wStatus != wPinMask);
	
	csi_gpio_port_pull_mode(GPIOA0, wPinMask, GPIO_PULLDOWN);		//pulldown
	mdelay(100);
	wStatus = csi_gpio_port_read(GPIOA0,wPinMask);
	while(wStatus != 0);
	
	return iRet;
}

 /** \brief gpio port interrupt
 * 
 *  \param[in] none
 *  \return error code
 */
 
int gpio_port_irq_demo(void)
{
	int iRet = 0;
	uint32_t wPinMask = PINMASK_PA00 | PINMASK_PA02 | PINMASK_PA05; //PA00/PA02/PA05 	

	csi_gpio_port_dir(GPIOA0, wPinMask, GPIO_DIR_INPUT);			//input
	csi_gpio_port_pull_mode(GPIOA0, wPinMask, GPIO_PULLUP);			//pullup
	csi_gpio_port_irq_mode(GPIOA0,wPinMask,GPIO_IRQ_FALLING_EDGE);	//falling edge
	csi_gpio_port_irq_enable(GPIOA0,wPinMask,ENABLE);				//enable gpio interrupt
	
//	csi_pin_set_mux(PA02,GPIO_PIN_INPUT);
//	csi_pin_pull_mode(PA02,GPIO_PULLUP);
//	csi_pin_irq_mode(PA02,EXI_GRP2, GPIO_IRQ_FALLING_EDGE);
//	csi_pin_irq_enable(PA02,EXI_GRP2, ENABLE);

	
	return iRet;
}