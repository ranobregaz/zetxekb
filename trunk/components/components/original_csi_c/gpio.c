/***********************************************************************//** 
 * \file  gpio.c
 * \brief  csi gpio driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2020-1- 10<td>V0.1  <td>WNN   <td> fix s_byGpioNum = 16
 * <tr><td> 2020-1- 22<td>V0.2  <td>WNN   <td> add debounce
 * </table>
 * *********************************************************************
*/
//#include <csi_config.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include "csp_gpio.h"
#include "csp_syscon.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
const uint32_t  wExiStateMap[] = 
{    
	EXI0_IRQ_MAP, EXI1_IRQ_MAP, EXI2_IRQ_MAP, EXI3_IRQ_MAP, EXI4_IRQ_MAP,
};

const uint8_t  byExiNumMap[] = 
{    
	EXI0_IRQn, EXI1_IRQn, EXI2_IRQn, EXI3_IRQn, EXI4_IRQn,
};

static uint8_t s_byGpioNum = 16;		//GPIO(A0/B0) pin num

/** \brief gpio interrupt handle function
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] port_idx: gpio index
 *  \return error code \ref csi_error_t
 */ 
static void apt_gpio_irqhandler(void *args)
{
	volatile uint32_t wBitMask;
    csi_gpio_t *handle = (csi_gpio_t *)args;
	
    wBitMask = csp_exi_get_port_irq(SYSCON_REG_BASE);
	//handle->arg = (void *)wBitMask;
	
   
 if (handle->callback) {
        handle->callback(handle, wBitMask, handle->arg);
	}
	
	csp_exi_clr_port_irq(SYSCON_REG_BASE,wBitMask);		//clear all interrput 
	
}
/** \brief initialize gpio data structure
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] port_idx: gpio index
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_gpio_init(csi_gpio_t *gpio, uint32_t port_idx)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (target_get(DEV_EXI_TAG, port_idx, &gpio->dev) != CSI_OK) {
        ret = CSI_ERROR;
    }

	s_byGpioNum = 16;

    return ret;
}
/** \brief de-initialize gpio interface
 * 
 *  \param[in] gpio: GPIO port handle
 *  \return error code \ref csi_error_t
 */ 
void csi_gpio_uninit(csi_gpio_t *gpio)
{
    CSI_PARAM_CHK_NORETVAL(gpio);
	
//	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
//	csp_gpio_default_Init(reg);
	
	memset(gpio, 0, sizeof(csi_gpio_t));
} 
/** \brief config gpio direction
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] dir: pin dir,ouput/input
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_dir(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_dir_t dir)
{
	CSI_PARAM_CHK(gpio, CSI_ERROR);
	
	uint8_t byDirData = 0x00, i;
	csi_error_t ret = CSI_OK;
	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
	for(i = 0; i < s_byGpioNum; i++)
	{
		if(pin_mask & 0x01)
		{
			switch(dir)
			{
				case GPIO_DIRECTION_INPUT:
					byDirData = 0x01;
					break;
				case GPIO_DIRECTION_OUTPUT:
					byDirData = 0x02;
					break;
				case GPIO_DIRECTION_GPD:
					byDirData = 0x00;
					break;
				default:
					ret = CSI_UNSUPPORTED;
					break;
			}
			csp_gpio_mux_set(reg, i, byDirData);
		}
		pin_mask = (pin_mask >> 1);
	}
	return ret;
}
/** \brief config gpio pull up or pull down
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] pull_mode: pull up/down
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_pudr(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_pull_t pull_mode)
{
	CSI_PARAM_CHK(gpio, CSI_ERROR);
	
	uint8_t i;
	csi_error_t	ret = CSI_OK;
	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
	for(i = 0; i < s_byGpioNum; i++)
	{
		if(pin_mask & 0x01)
		{
			switch(pull_mode)
			{
				case GPIO_PULL_UP:
					csp_gpio_pullhigh_set(reg, i);
					break;
				case GPIO_PULL_DOWN:
					csp_gpio_pulllow_set(reg, i);
					break;
				case GPIO_PULL_DIS:
				default:
					csp_gpio_pullpighlow_dis(reg, i);
					break;
			}
		}
		pin_mask = (pin_mask >> 1);
	}
	return ret;
}
/** \brief config gpio input or output mode
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] mode: pin dir,ouput/input
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_mode_t mode)
{
	CSI_PARAM_CHK(gpio, CSI_ERROR);

	uint8_t i;		//uint8_t byDir = 0x01, i;
    csi_error_t	ret = CSI_OK;
	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
	//csp_exi_set_port_irq(pin_mask,false);			// EXI's SYSCON switch off
	for(i = 0; i < s_byGpioNum; i++)
	{
		if(pin_mask & 0x01)
		{
			switch(mode)
			{
				case GPIO_MODE_PULLNONE:			//悬空输入			
					csp_gpio_pullpighlow_dis(reg, i);
					break;
				case GPIO_MODE_PULLUP:				//上拉输入
					csp_gpio_pullhigh_set(reg, i);
					break;
				case GPIO_MODE_PULLDOWN:			//下拉输入
					csp_gpio_pulllow_set(reg, i);
					break;
				case GPIO_MODE_OPEN_DRAIN:			//开漏输出
					//byDir = 0x02;
					csp_gpio_opendrain_en(reg, i);
					break;
				case GPIO_MODE_PUSH_PULL:			//推挽输出
					//byDir = 0x02;
					csp_gpio_opendrain_dis(reg, i);
					break;
				default:
					ret = CSI_UNSUPPORTED;
					break;
			}
			//csp_gpio_mux_set(reg, i, byDir);	
		}
		pin_mask = (pin_mask >> 1);
	}
	return ret;
 }
/** \brief config gpio irq mode
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] mode: rising edge; falling edge;	both edge;
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_irq_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_irq_mode_t mode)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
	
	uint8_t i;
    csi_error_t	ret = CSI_OK;
	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
	csp_gpio_set_port_irq(reg, pin_mask, ENABLE);				//to make exi to be visible by syscon
	
	for(i = 0; i < s_byGpioNum; i++)
	{
		if(pin_mask & 0x01)
		{
			csp_gpio_intgroup_set(reg,i,i);
			switch(mode)
			{
				case GPIO_IRQ_MODE_RISING_EDGE:				//rising edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,i, EXI_EDGE_IRT);	//trig edg
					break;
				case GPIO_IRQ_MODE_FALLING_EDGE:			//falling edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,i, EXI_EDGE_IFT);	//trig edg
					break;
				case GPIO_IRQ_MODE_BOTH_EDGE:				//double edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,i, EXI_EDGE_BOTH);	//trig edg
					break;
				case GPIO_IRQ_MODE_LOW_LEVEL:				//low level
				case GPIO_IRQ_MODE_HIGH_LEVEL:				//high level
				default:
					ret = CSI_UNSUPPORTED;
					break;
			}
		}
		pin_mask = (pin_mask >> 1);
	}
	return ret;
}
/** \brief gpio irq enable
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] enable: true or false
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_irq_enable(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
	
	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
	csp_gpio_set_port_irq(reg, pin_mask, enable);				//IO INT enable Control reg(setting IEER)
	csp_exi_set_port_irq(SYSCON_REG_BASE,pin_mask, enable);		//EXI INT enable

	return CSI_OK;
}

/** \brief  set gpio toggle
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \return none
 */ 
void csi_gpio_toggle(csi_gpio_t *gpio, uint32_t pin_mask)
{
    CSI_PARAM_CHK_NORETVAL(gpio);
	
    csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);

	uint32_t tmp = csp_gpio_read_output_port(reg);
	csp_gpio_write_output_port(reg, tmp ^ pin_mask);
}

/** \brief  read gpio status value
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \return error code \ref csi_error_t
 */ 
 uint32_t csi_gpio_read(csi_gpio_t *gpio, uint32_t pin_mask)
{
    csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
	
    return csp_gpio_read_input_port(reg) & pin_mask;
}
/** \brief  write gpio ouput value
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] pin_mask: pin mask,0x0001~0xffff
 *  \param[in] value: gpio output value
 *  \return error code \ref csi_error_t
 */ 
void  csi_gpio_write(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_pin_state_t value)
{
    CSI_PARAM_CHK_NORETVAL(gpio);

   csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);

	uint32_t tmp = csp_gpio_read_output_port(reg);
    if (value == 1) {
        csp_gpio_write_output_port(reg, tmp | pin_mask);
    } else {
        csp_gpio_write_output_port(reg, tmp & (~pin_mask));
    }
}
/** \brief  attach gpio callback handler
 * 
 *  \param[in] gpio: GPIO port handle
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t  csi_gpio_attach_callback(csi_gpio_t *gpio, void *callback, void *arg)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
	
	uint8_t  i,k = 0;
	uint8_t  byIrqbuf[5];
	uint32_t wPinMask = (uint32_t )arg;
	
	gpio->priv = (void *)wPinMask;
	
	csp_exi_clr_port_irq(SYSCON_REG_BASE,wPinMask);		//clear interrput status before enable irq 
	gpio->callback = callback;
    gpio->arg      = arg;
	
	for(i = 0; i < 5; i++)
	{
		if(wPinMask & wExiStateMap[i])
		{
			byIrqbuf[k++] = byExiNumMap[i];
		}
	}
	
	for(i = 0; i < k; i++)
	{
		gpio->dev.irq_num = byIrqbuf[i];
		csi_irq_attach((uint32_t)gpio->dev.irq_num,&apt_gpio_irqhandler, &gpio->dev);	
		csi_irq_enable((uint32_t)gpio->dev.irq_num);
	}
	
	return CSI_OK;
}
/** \brief  detach gpio callback handler
 * 
 *  \param[in] gpio: GPIO port handle
 *  \return none
 */ 
void csi_gpio_detach_callback(csi_gpio_t *gpio)
{
    CSI_PARAM_CHK_NORETVAL(gpio);
	
	uint8_t  i,k = 0;
	uint8_t  byIrqbuf[5];
	uint32_t wPinMask = (uint32_t)gpio->priv;
   
	gpio->callback = NULL;
    gpio->arg      = NULL;
	gpio->priv	   = NULL;
	
	for(i = 0; i < 5; i++)
	{
		if(wPinMask & wExiStateMap[i])
		{
			byIrqbuf[k++] = byExiNumMap[i];
		}
	}
	
	for(i = 0; i < k; i++)
	{
		gpio->dev.irq_num = byIrqbuf[i];
		csi_irq_disable((uint32_t)gpio->dev.irq_num);	//unregister irq 
		csi_irq_detach((uint32_t)gpio->dev.irq_num);
	}
}


/** \brief  set EXI as trigger Event(EV0~5) 
 *  \param[in] trg_out: output Event select(TRGOUT0~5)
 *  \param[in] exi_trgsrc: event source (TRGSRC_EXI0~19)
 *  \param[in] trg_prd: accumulated EXI events to output trigger 
 *  \return none
 */ 
void csi_exi_set_evtrg(uint8_t trg_out, csi_exi_trgsrc_t exi_trgsrc, uint8_t trg_prd)
{
	csp_syscon_t *ptSysconBase = (csp_syscon_t *)APB_SYS_BASE;
 
	trg_prd &= 0xf;
	csp_syscon_set_evtrg_src(ptSysconBase, trg_out, exi_trgsrc);
	csp_syscon_set_evtrg_prd(ptSysconBase, trg_out, trg_prd);
	csp_syscon_evtrg_enable(ptSysconBase, trg_out, ENABLE);
}


/** \brief  gpio external interrupt setting that supports extanded IO
 * 
 *  \param[in] byPinNum: gpio pin num (0-15 valid)
 *  \param	eGrp: exi group:EXI_GRP0 - 19
 *  \param[in] pin_trg: gpio pin int trigger mode: PIN_TRG_IRT/PIN_TRG_IFT/PIN_TRG_BOTH
 *  \param[in] exi_ctrl: gpio EXI enable/disable control, true/false
 *  \return error code \ref csi_error_t
 */
 
csi_error_t csi_gpio_irq_mode_ex(csi_gpio_t *gpio,uint8_t pin_mask, csi_exi_grp_e eGrp, csi_gpio_irq_mode_t pin_trg)
{
	
	csi_error_t	ret = CSI_OK;
	uint8_t i;
	csp_gpio_t *gpio_base = NULL; 
	gpio_base = (csp_gpio_t *)HANDLE_REG_BASE(gpio);	
	
	csp_gpio_set_port_irq(gpio_base, pin_mask, ENABLE);				//to make exi to be visible by syscon
	
	for(i = 0; i < 19; i++)
	{
		if(pin_mask & 0x01)
		{
			csp_gpio_intgroup_set(gpio_base,i,eGrp);
			switch(pin_trg)
			{
				case GPIO_IRQ_MODE_RISING_EDGE:				//rising edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,eGrp, EXI_EDGE_IRT);	//trig edg
					break;
				case GPIO_IRQ_MODE_FALLING_EDGE:			//falling edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,eGrp, EXI_EDGE_IFT);	//trig edg
					break;
				case GPIO_IRQ_MODE_BOTH_EDGE:				//double edge interrupt mode
					csp_exi_trigger_set(SYSCON_REG_BASE,eGrp, EXI_EDGE_BOTH);	//trig edg
					break;
				case GPIO_IRQ_MODE_LOW_LEVEL:				//low level
				case GPIO_IRQ_MODE_HIGH_LEVEL:				//high level
				default:
					ret = CSI_UNSUPPORTED;
					break;
			}
		}
		pin_mask = (pin_mask >> 1);
	}
		
	return ret;
}

/** \brief Set debounce of gpio when gpio configed as input
 *  
 *  \param[in]   gpio        GPIO port handle
 *  \param[in]   pin_mask    Pin mask need to be set
 *  \param[in]   enbale      0: disable   1:enable
 *  \return      Error code
*/
csi_error_t csi_gpio_debounce(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
	return CSI_UNSUPPORTED;
}