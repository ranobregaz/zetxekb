/***********************************************************************//** 
 * \file  GPIO_demo.c
 * \brief  GPIO示例代码
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-12 <td>V0.1  <td>WNN   <td>增加中断的demo
 * </table>
 * *********************************************************************
*/
/* include ----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "soc.h"
#include "pin.h"
#include "board_config.h"
#include "iostring.h"


/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/
csi_gpio_t g_tGpioB0;			//如果使用GPIOB0中的IO，需要先实例化设备


/**
  \brief       GPIO 输出示例		   
  \return      csi_error_t
*/
void GPIO_senario0(void)
{
	uint32_t n = 100000;
	csi_gpio_init(&g_tGpioB0, 1);	//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	
	
	while(n--) {
		udelay(20);					//延时大概20us
		csi_gpio_toggle(&g_tGpioB0, GPIO_PIN_MSK);
	}
	

	while(1) {
		mdelay(1);					//延时大概1ms
		csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_HIGH);
		mdelay(1);
		csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_LOW);
	}
}

/**
  \brief       GPIO 强驱输出示例	   
  \return      csi_error_t
*/
void GPIO_senario1(void)
{
	
	csi_gpio_init(&g_tGpioB0, 1);	//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	
	csi_pin_drive(GPIO_PIN, PIN_DRIVE_LV1);
	
	csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_HIGH);	//对应电气参数中的VOH参数
	csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_LOW);		//对应电气参数中的VOL参数
	while(1);
}

/**
  \brief       GPIO 输出速度设置示例	   
  \return      csi_error_t
*/
void GPIO_senario2(void)
{
	
	csi_gpio_init(&g_tGpioB0, 1);	//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出
	
	csi_pin_speed(GPIO_PIN, PIN_SPEED_LV1);				//通常情况下建议工作于PIN_SPEED_LV0（默认），特殊场合可以尝试PIN_SPEED_LV1	
	
	while(1) {
		udelay(20);					//延时大概1ms
		csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_HIGH);
		mdelay(20);
		csi_gpio_write(&g_tGpioB0, GPIO_PIN_MSK, GPIO_PIN_LOW);
	}
}


/**
  \brief       GPIO输入示例：设置不同的逻辑判断电平。 只有1023支持这个功能。	   
  \return      none
*/
void GPIO_senario3(void)
{
	csi_gpio_init(&g_tGpioB0, 1);	//初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PULLNONE);				//将GPIO_PIN(见board_config.h）设为输入，无上下拉
	
	csi_pin_input_mode(GPIO_PIN, INPUT_TTL1);				//TTL1 逻辑判断电平参数 见电器特性章节
	csi_pin_input_mode(GPIO_PIN, INPUT_CMOS);				//CMOS 逻辑判断电平参数 见电器特性章节
	csi_pin_input_mode(GPIO_PIN, INPUT_TTL2);				//TTL2 逻辑判断电平参数 见电器特性章节
	
	while(1);
}

/**
  \brief       GPIO中断示例。PB0.1下降沿中断, PB0.3下降沿中断	   
  \return      none
*/

void apt_gpiob0_irq(csi_gpio_t *tGpiob0, uint32_t wBitMsk, void * arg)
{
	if (wBitMsk & (0x1<<1))
		my_printf("PB01 INT\n");
	if (wBitMsk & (0x1<<3))
		my_printf("PB03 INT\n");
}
void GPIO_senario4(void)
{
	csi_gpio_init(&g_tGpioB0, 1);  //初始化设备，1为GPIOB0的设备号，芯片资源请参考devices.c
	
	csi_gpio_dir(&g_tGpioB0, 0x1<<1 | 0x1<<3, GPIO_DIRECTION_INPUT);
	csi_gpio_irq_mode_ex(&g_tGpioB0, 0x1<<1, EXI_GRP1, GPIO_IRQ_MODE_FALLING_EDGE);		// 将PB0.1视为 EXI1，下降沿触发中断
	csi_gpio_irq_mode_ex(&g_tGpioB0, 0x1<<3, EXI_GRP3, GPIO_IRQ_MODE_FALLING_EDGE);		// 将PB0.3视为 EXI3，下降沿触发中断
	csi_gpio_irq_enable(&g_tGpioB0, 0x1<<1| 0x1<<3, ENABLE);
	csi_gpio_attach_callback(&g_tGpioB0, apt_gpiob0_irq, (void *)(0x1<<1|0x1<<3));
	
	while(1);
}