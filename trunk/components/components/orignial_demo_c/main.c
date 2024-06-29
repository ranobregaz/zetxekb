/***********************************************************************//** 
 * \file  main.c
 * \brief  the main entry of the application;
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include "sys_clk.h"
#include "demo.h"
#include "iostring.h"
//#include "csp.h"

/* externs function--------------------------------------------------------*/
extern csi_error_t ept_senario1(void);
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


extern void system_init(void);
extern void board_init(void);


int main()
{
	system_init();
	board_init();
	soc_clo_config(CLO_HCLK, CLO_DIV4, PA02);	//PA02 输出HCLK/4
	my_printf("Hello~~~~~~~~\n");				//确保在工程设置compiler tab下，define了 DBG_PRINT2PC=1;
	//RTC_senario();
	//UART_senario0();
	//adc_senario0();
	//adc_senario1();
	eflash_senario1();
	//eflash_senario2();
	//eflash_senario3();
	//eflash_senario4();
	//eflash_test();
	//timer_senario0();
	//timer_senario1();
	//ept_senario0();
	//ept_senario1();
	//ept_senario2();
	//ept_senario3();
	//ept_senario4();
	//crc_senario();
	//GPIO_senario0();
	//GPIO_senario1();
	//GPIO_senario2();
	//GPIO_senario3();
	//GPIO_senario4();
	//gpta_senario0();
	//gpta_senario1();
	//gpta_senario2();
	//gpta_senario3();
	//gpta_senario4();
	//cnta_senario0();
	//lpt_senario0();
	//lpt_senario1();
	//wwdt_senario();
	//etcb_srnario0();
	//lowpower_senario();
	while(1);
	
    return 0;
}
