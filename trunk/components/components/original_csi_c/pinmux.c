/***********************************************************************//** 
 * \file  pinmux.c
 * \brief  csi pin driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-2 <td>V0.1  <td>ZJY   <td>modify for  110 
 * <tr><td> 2021-1-12 <td>V0.0  <td>WNN   <td>adapt for 102x/003
 * </table>
 * *********************************************************************
*/
#include <stdint.h>
#include <stddef.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include "csp_gpio.h"

/* Private macro-----------------------------------------------------------*/
#define	CSI_PINNAME_CHK(pin_name, err)		\
	do{										\
		if(pin_name > PB05)					\
			return err;						\
	}while(0)

/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief set gpio mux function
 * 
 *  \param[in] pin_name: gpio pin name
 *  \return pointer of pin infor
 */ 
static unsigned int *get_pin_name_addr(pin_name_t pin_name)
{
	static unsigned int pin_infor[2];
	if(pin_name > PA015)
	{
		pin_infor[0] = APB_GPIOB0_BASE;				//PB0
		pin_infor[1] = pin_name - 16;
	}
	else
	{
		pin_infor[0] = APB_GPIOA0_BASE;				//PA0
		pin_infor[1] = pin_name;
	}	
	
	return pin_infor;
}

/** \brief set gpio mux function
 * 
 *  \param[in] pin_name: gpio pin name
 *  \param[in] pin_func: gpio pin function
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_pin_set_mux(pin_name_t pin_name, pin_func_t pin_func)
{
	
    csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;
	//csp_syscon_t *sys_base = (csp_syscon_t *)APB_SYS_BASE;
	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	
	//IO REMAP
	if(pin_func == IOMAP)			
	{
		csp_syscon_t *sys_base = (csp_syscon_t *)APB_SYS_BASE;
		if(pin_name < PA08)
			sys_base->IOMAP0 = (sys_base->IOMAP0 & ~(0x0F << 4*pin_name)) | (pin_name << 4*pin_name);
		else 
		{
			if(pin_name < PB02)
				sys_base->IOMAP1 = (sys_base->IOMAP1 & ~(0x0F << 4*(pin_name - 6))) |  ((pin_name - 6) << 4*(pin_name - 6));
			else 
				sys_base->IOMAP1 = (sys_base->IOMAP1 & ~(0x0F << 4*(pin_name - 18))) |  ((pin_name - 18) << 4*(pin_name - 18));
		}
	}
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name - 16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;	
	
	csp_gpio_mux_set(gpio_base, pin_name , pin_func);
	
    return ret;
}
/** \brief get gpio mux function
 * 
 *  \param[in] pin_name: gpio pin name
 *  \return value of gpio mux
 */ 
pin_func_t csi_pin_get_mux(pin_name_t pin_name)
{
    uint8_t ret = 0x0f;
	csp_gpio_t *gpio_base = NULL;
	
	CSI_PINNAME_CHK(pin_name, (pin_func_t)ret);
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name - 16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
		
	ret = csp_gpio_mux_get(gpio_base, pin_name);
		
    return (pin_func_t)ret;
}
/** \brief set gpio pin input or output mode
 * 
 *  \param[in] pin_name: gpio pin name
 *  \param[in] mode: gpio mode
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_pin_mode(pin_name_t pin_name, csi_gpio_mode_t mode)
{
    csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;
	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name - 16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	csp_gpio_mux_set(gpio_base,pin_name,0x01);				//input
	switch(mode)
	{
		case GPIO_MODE_PULLNONE:
			csp_gpio_pullpighlow_dis(gpio_base, pin_name);
			break;
		case GPIO_MODE_PULLUP:
			csp_gpio_pullhigh_set(gpio_base, pin_name);
			break;
		case GPIO_MODE_PULLDOWN:
			csp_gpio_pulllow_set(gpio_base, pin_name);
			break;
		case GPIO_MODE_OPEN_DRAIN:
			csp_gpio_mux_set(gpio_base,pin_name,0x02);		//output		
			csp_gpio_opendrain_en(gpio_base, pin_name);
			break;
		case GPIO_MODE_PUSH_PULL:
			csp_gpio_mux_set(gpio_base,pin_name,0x02);		//output
			csp_gpio_opendrain_dis(gpio_base, pin_name);
			break;
		default:
			break;
	}
	
    return ret;
}
/** \brief set gpio pin speed
 * 
 *  \param[in] pin_name: gpio pin name
 *  \param[in] speed: gpio pin speed
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_pin_speed(pin_name_t pin_name, csi_pin_speed_t speed)
{
	csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;
	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name -16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	csp_gpio_speed_set(gpio_base, pin_name, (uint8_t)speed);
	
	return ret;
}

/** \brief set gpio pin drive level
 * 
 *  \param[in] pin_name: gpio pin name, defined in soc.h.
 *  \param[in] drive: gpio pin drive level, week/strong = 0/1
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_drive(pin_name_t pin_name, csi_pin_drive_t drive)
{
#if defined(IS_CHIP_102) || defined(IS_CHIP_1022)
	return CSI_ERROR;
#endif
	csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;

	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name -16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	//	
	csp_gpio_drv_set(gpio_base, pin_name, (uint8_t)drive);
		
    return ret;
}

/** \brief set gpio pin input mode
 * 
 *  \param[in] pin_name: gpio pin name, defined in soc.h.
 *  \param[in] input mode:  INPUT_CMOS/INPUT_TTL
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_input_mode(pin_name_t pin_name, csi_pin_input_mode_t InputMd)
{
#if defined(IS_CHIP_102) || defined(IS_CHIP_1021) || defined(IS_CHIP_1022) || defined(IS_CHIP_S003)
	return CSI_ERROR;
#endif
	csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;
	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name -16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	switch (InputMd)
	{
		case (INPUT_TTL2):	csp_gpio_ccm_ttl2(gpio_base, pin_name);
			break;
		case (INPUT_TTL1): 	csp_gpio_ccm_ttl1(gpio_base, pin_name);
			break;
		case (INPUT_CMOS):	csp_gpio_ccm_cmos(gpio_base, pin_name);
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
	return ret;
}
/** \brief set gpio pin input mode
 * 
 *  \param[in] pin_name: gpio pin name, defined in soc.h.
 *  \param[in] outmode:  constant current/normal
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_output_mode(pin_name_t pin_name, csi_pin_out_mode_t outmode)
{
	return CSI_ERROR; 
}
/** \brief config gpio pull up or pull down
 * 
 *  \param[in] pin_name: gpio pin name
 *  \param[in] pull_mode: pull up/down
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_pull_mode(pin_name_t pin_name, csi_pin_pull_t pull_mode)
{
	//csi_error_t ret = CSI_OK;
	csp_gpio_t *gpio_base = NULL;
	
	CSI_PINNAME_CHK(pin_name, CSI_ERROR);
	
	if(pin_name > PA015)
	{
		gpio_base = (csp_gpio_t *)APB_GPIOB0_BASE;
		pin_name = pin_name -16;
	}
	else
		gpio_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	switch(pull_mode)
	{
		case PIN_PULL_UP:
			csp_gpio_pullhigh_set(gpio_base, pin_name);
			break;
		case PIN_PULL_DOWN:
			csp_gpio_pulllow_set(gpio_base, pin_name);
			break;
		case PIN_PULL_DIS:
		default:
			csp_gpio_pullpighlow_dis(gpio_base, pin_name);
			break;
		}
	return CSI_OK;
}


/** \brief get gpio pin num
 * 
 *  \param[in] pin_name: gpio pin name, defined in soc.h.
 *  \return pin num
 */ 
uint8_t csi_pin_get_num(pin_name_t pin_name)
{
    uint8_t ret = 44;
	unsigned int *pin_mess = NULL;
	CSI_PINNAME_CHK(pin_name, (pin_func_t)ret);
	
	pin_mess = get_pin_name_addr(pin_name);
	ret = (uint8_t)pin_mess[1];					//gpio pin number
						
    return ret;
}