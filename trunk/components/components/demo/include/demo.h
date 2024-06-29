/***********************************************************************//** 
 * \file  demo.h
 * \brief  demo description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-11 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DEMO_H
#define _DEMO_H

/* Includes ------------------------------------------------------------------*/
extern int gpio_demo(void);
extern int uart_demo(void);
extern int timer_demo(void);
extern int iwdt_demo(void);

#endif