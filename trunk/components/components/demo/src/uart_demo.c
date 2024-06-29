/***********************************************************************//** 
 * \file  uart_demo.c
 * \brief  GPIO_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-08 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include "sys_clk.h"
#include <gpio.h>
#include "uart.h"
#include "tst_io.h"

/* externs function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/

/* Private macro-----------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/
char rxbuf[64];

/** \brief test init
 * 
 *  \param[in] none
 *  \return none
 */
void test_init(void){
	for(int i=0; i<64; i++) {
		rxbuf[i] =0;
	}
	tst_io_init();
}
/** \brief test init
 * 
 *  \param[in] none
 *  \return none
 */
void uart_test(void){
	uint32_t len =0;
	tst_io_send("hello world\n",sizeof("hello world\n"));
	while(1){
		len = tst_io_rcv(rxbuf,1);
		if(len!=0){
			tst_io_send(rxbuf,len);
		}
	}
}


int uart_demo(void)
{
	test_init();		//uart init
	uart_test();		//uart test
	
	return 0;
}
