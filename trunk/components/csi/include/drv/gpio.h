/***********************************************************************//** 
 * \file  gpio.c
 * \brief  csi gpio driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2020-1- 10<td>V0.1  <td>WNN   <td> tiny change
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_GPIO_H_
#define _DRV_GPIO_H_

#include <drv/common.h>
#include <stdint.h>
#include <stdbool.h>

#include "csp.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum     csi_gpio_speed_e
 * \brief    GPIO speed define
 */
typedef enum {
    GPIO_SPEED_LV0 	= 0U,
    GPIO_SPEED_LV1,
} csi_gpio_speed_e;

/**
 * \enum     csi_gpio_drive_e
 * \brief    GPIO speed define
 */
typedef enum {
    GPIO_DRIVE_LV0 	= 0U,
    GPIO_DRIVE_LV1,
} csi_gpio_drive_e;

/**
 * \enum     csi_gpio_dir_e
 * \brief    GPIO dir define
 */
typedef enum {
	GPIO_DIR_GPD		= 0,
    GPIO_DIR_INPUT,    					//GPIO as input
    GPIO_DIR_OUTPUT             		//GPIO as output
} csi_gpio_dir_e;

/**
 * \enum     csi_gpio_pin_state_e
 * \brief    GPIO pin state define
 */
typedef enum {
    GPIO_PIN_LOW 		= 0,           //GPIO low level
    GPIO_PIN_HIGH,                     //GPIO high level
} csi_gpio_pin_state_e;

/**
 * \enum     csi_gpio_pull_mode_e
 * \brief    GPIO pull mode define
 */
typedef enum {
    GPIO_PULLNONE		= 0,    		//Pull none 
    GPIO_PULLUP,                  		//Pull up 
    GPIO_PULLDOWN,               		//Pull down 
} csi_gpio_pull_mode_e;	

/**
 * \enum     csi_gpio_out_mode_e
 * \brief    GPIO output mode define
 */
typedef enum {
	GPIO_PUSH_PULL 		= 0,			//push-pull output
	GPIO_OPEN_DRAIN,					//open drain output
} csi_gpio_output_mode_e;

/**
 * \enum     csi_gpio_input_mode_e
 * \brief    GPIO input mode define
 */
typedef enum {
	GPIO_INPUT_CMOS 	= 0U,
	GPIO_INPUT_TTL1,
	GPIO_INPUT_TTL2
}csi_gpio_input_mode_e;

/**
 * \enum     csi_gpio_irq_mode_e
 * \brief    GPIO irq triger type
 */
typedef enum {
    GPIO_IRQ_RISING_EDGE  = 0,    		//Interrupt mode for rising edge
    GPIO_IRQ_FALLING_EDGE,        		//Interrupt mode for falling edge
    GPIO_IRQ_BOTH_EDGE,          		//Interrupt mode for both edge
} csi_gpio_irq_mode_e;

/**
 * \enum     csi_exi_grp_e
 * \brief    GPIO exi group
 */
typedef enum{
	EXI_GRP0 = 0,
	EXI_GRP1,
	EXI_GRP2,
	EXI_GRP3,
	EXI_GRP4,
	EXI_GRP5,
	EXI_GRP6,
	EXI_GRP7,
	EXI_GRP8,
	EXI_GRP9,
	EXI_GRP10,
	EXI_GRP11,
	EXI_GRP12,
	EXI_GRP13,
	EXI_GRP14,
	EXI_GRP15,
	EXI_GRP16,
	EXI_GRP17,
	EXI_GRP18,
	EXI_GRP19,
}csi_exi_grp_e;

/**
 * \enum     csi_exi_trgsrc_e
 * \brief    GPIO exi trigger source
 */
typedef enum {
	TRGSRC_EXI0 = 0,
	TRGSRC_EXI1,
	TRGSRC_EXI2,
	TRGSRC_EXI3,
	TRGSRC_EXI4,
	TRGSRC_EXI5,
	TRGSRC_EXI6,
	TRGSRC_EXI7,
	TRGSRC_EXI8,
	TRGSRC_EXI9,
	TRGSRC_EXI10,
	TRGSRC_EXI11,
	TRGSRC_EXI12,
	TRGSRC_EXI13,
	TRGSRC_EXI14,
	TRGSRC_EXI15,
	TRGSRC_EXI16,
	TRGSRC_EXI17,
	TRGSRC_EXI18,
	TRGSRC_EXI19, 
}csi_exi_trgsrc_e;

/**
 * \enum     csi_exi_status_e
 * \brief    GPIO exi status bit
 */
typedef enum {
	STATUS_EXI0 	= (0x01ul << 0),
    STATUS_EXI1		= (0x01ul << 1),      
	STATUS_EXI2 	= (0x01ul << 2),
    STATUS_EXI3		= (0x01ul << 3), 
	STATUS_EXI4 	= (0x01ul << 4),
    STATUS_EXI5		= (0x01ul << 5),      
	STATUS_EXI6 	= (0x01ul << 6),
    STATUS_EXI7		= (0x01ul << 7), 
	STATUS_EXI8 	= (0x01ul << 8),
    STATUS_EXI9		= (0x01ul << 9),      
	STATUS_EXI10 	= (0x01ul << 10),
    STATUS_EXI11	= (0x01ul << 11), 
	STATUS_EXI12 	= (0x01ul << 12),
    STATUS_EXI13	= (0x01ul << 13),      
	STATUS_EXI14 	= (0x01ul << 14),
    STATUS_EXI15	= (0x01ul << 15),
	STATUS_EXI16	= (0x01ul << 16),      
	STATUS_EXI17 	= (0x01ul << 17),
    STATUS_EXI18	= (0x01ul << 18),
	STATUS_EXI19	= (0x01ul << 19)
}csi_exi_status_e;


/**
  \brief       Config pin direction
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \param[in]   eDir         \ref csi_gpio_dir_e
  \return      Error code
*/
csi_error_t csi_gpio_port_dir(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_dir_e eDir);

/**
  \brief       Config pin pull mode
  \param[in]   ptGpioBase	GPIO port handle
  \param[in]   wPinMask		Pin mask need to be set
  \param[in]   eMode        \ref csi_gpio_pull_mode_e
  \return      Error code
*/
csi_error_t csi_gpio_port_pull_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_pull_mode_e eMode);

/**
  \brief       Config pin output mode
  \param[in]   ptGpioBase	GPIO port handle
  \param[in]   wPinMask		Pin mask need to be set
  \param[in]   eMode        \ref csi_gpio_output_mode_e
  \return      Error code
*/
csi_error_t csi_gpio_port_output_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_output_mode_e eMode);

/** 
  \brief config gpio input mode
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask		pin mask,0x0001~0xffff
  \param[in]   eMode		\ref csi_gpio_output_mode_e
  \return 	   error code 	\ref csi_error_t
 */ 
csi_error_t csi_gpio_port_input_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_input_mode_e eMode);

/**
  \brief       Config gpio irq params
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \param[in]   eTrgEdge      Interrupt trigger mode \ref csi_gpio_irq_mode_e
  \return      Error code
*/
csi_error_t csi_gpio_port_irq_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_irq_mode_e eTrgEdge);

/**
  \brief       Enable or disable gpio pin interrupt
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \param[in]   bEnable      0:disable  1:enable
  \return      Error code
*/
void csi_gpio_port_irq_enable(csp_gpio_t *ptGpioBase, uint32_t wPinMask, bool bEnable);

//csi_error_t csi_gpio_debonce(csi_gpio_t *gpio, uint32_t pin_mask, bool enable);
/**
  \brief       Set one or zero to the selected pin mask
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \param[in]   ePinVal      Value to be set \ref csi_gpio_pin_state_e
  \return      None
*/
void  csi_gpio_port_write(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_pin_state_e ePinVal);

/**
  \brief       Set high(1) selected pin mask
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \return      None
*/
void  csi_gpio_port_set_high(csp_gpio_t *ptGpioBase, uint32_t wPinMask);

/**
  \brief       Set low(0) selected pin mask
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \return      None
*/
void  csi_gpio_port_set_low(csp_gpio_t *ptGpioBase, uint32_t wPinMask);

/**
  \brief       Toggle output gpio value,ex.if previous value is 1, then output 0
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask    	Pin mask need to be set
  \return      None
*/
void csi_gpio_port_toggle(csp_gpio_t *ptGpioBase, uint32_t wPinMask);

/**
  \brief       Get the value of  selected GPIO pin mask
  \param[in]   ptGpioBase	pointer of gpio register structure
  \param[in]   wPinMask		Pin mask need to be set
  \return      According to the bit mask, the corresponding pin status is obtained
*/
uint32_t csi_gpio_port_read(csp_gpio_t *ptGpioBase, uint32_t wPinMask);


#ifdef __cplusplus

}
#endif

#endif /* _DRV_GPIO_H_ */
