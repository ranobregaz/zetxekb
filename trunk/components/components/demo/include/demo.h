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
//gpio port demo
extern int gpio_port_ouput_demo(void);
extern int gpio_port_input_demo(void);
extern int gpio_port_irq_demo(void);

//pin demo
extern int pin_output_demo(void);
extern int pin_input_demo(void);
extern int pin_irq_demo(void);

//bt demo
extern int bt_timer_demo(void);
extern int bt_pwm_demo(void);

//iwdt demo
extern int iwdt_normal_demo(void);
extern int iwdt_irq_demo(void);

//etcb demo
int etcb_one_trg_one_demo(void);

//uart demo
//uart send
int uart_char_demo(void);
int uart_send_demo(void);
int uart_send_intr_demo(void);
//uart receive
int uart_receive_demo(void);
int uart_recv_intr_demo(void);
int uart_recv_dynamic_demo(void);
int uart_recv_dynamic_demo1(void);

//lpt demo
extern int lpt_timer_demo(void);
extern int lpt_pwm_demo(void);

//reliability
extern void lvd_demo(void);
extern void lvr_demo(void);
extern void memorycheck_demo(void);
extern void emcm_demo(void);

//ifc demo
void ifc_read(void);
void ifc_program(void);

//rtc_demo
void rtc_set_time_demo(void);
void rtc_alarm_demo(void);
void rtc_timer_demo(void);
void rtc_trgev_demo(void);

//low power demo
void lp_exi_wakeup_demo(void);
#endif