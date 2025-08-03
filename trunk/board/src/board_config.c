/***********************************************************************//** 
 * \file  baord_config.c
 * \brief  board configuration 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-23 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include "sys_clk.h"
#include "board_config.h"
#include "sys_console.h"

extern int32_t console_init(sys_console_t *handle);

/// system clock configuration parameters to define source, source freq(if selectable), sdiv and pdiv
csi_clk_config_t tClkConfig = 
	//{SRC_HFOSC, HFOSC_48M_VALUE, SCLK_DIV3, PCLK_DIV1, 5556000, 5556000};
	//{SRC_EMOSC, 20000000, SCLK_DIV1, PCLK_DIV2, 5556000, 5556000};
	{SRC_IMOSC, IMOSC_5M_VALUE, SCLK_DIV1, PCLK_DIV2,5556000, 5556000};
	//{SRC_HFOSC, HFOSC_48M_VALUE, SCLK_DIV2, PCLK_DIV1,5556000, 5556000};
	//{SRC_IMOSC, IMOSC_4M_VALUE, SCLK_DIV1, PCLK_DIV1,5556000, 5556000};



sys_console_t console;
__attribute__((weak)) void board_init(void)
{
    //init the console according to board_config.h
	
	console.uart_id = (uint32_t)CONSOLE_IDX;
    console.baudrate = 115200U;
    console.tx.pin = CONSOLE_TXD;   
    console.tx.func = CONSOLE_TXD_FUNC;
 
    console.rx.pin = CONSOLE_RXD;
    console.rx.func = CONSOLE_RXD_FUNC;
    console.uart = NULL;

    console_init(&console);

}

