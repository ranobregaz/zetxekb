/***********************************************************************//** 
 * \file  gpio.c
 * \brief  csi gpio driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/

#include <drv/gpio.h>
#include <drv/irq.h>

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/
const uint32_t  wExiStateMap[] = 
{    
	EXI0_IRQ_MAP, EXI1_IRQ_MAP, EXI2_IRQ_MAP, EXI3_IRQ_MAP, EXI4_IRQ_MAP,
};

const uint8_t  byExiNumMap[] = 
{    
	EXI0_IRQn, EXI1_IRQn, EXI2_IRQn, EXI3_IRQn, EXI4_IRQn,
};

/** \brief config gpio direction
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eDir: pin dir,ouput/input
 *  \return	error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_dir(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_dir_e eDir)
{
	csi_error_t ret = CSI_OK;
	uint32_t wConlrVal;
	uint32_t wConhrVal;
	uint32_t wLrVal = (wPinMask & 0xff);
	uint32_t wHrVal = (wPinMask >> 8) & 0xff;
	uint32_t wConMask = 0;
	
	switch(eDir)
	{
		case GPIO_DIR_INPUT:
		
			if(wLrVal)
			{
				wConMask = ((wLrVal & 0x01)*0xf) | (((wLrVal & 0x02) << 3)*0xf) | (((wLrVal & 0x04) << 6)*0xf) | (((wLrVal & 0x08) << 9)*0xf) |
						(((wLrVal & 0x10) << 12)*0xf) | (((wLrVal & 0x20) << 15)*0xf) | (((wLrVal & 0x40) << 18)*0xf) | (((wLrVal & 0x80) << 21)*0xf); 	
				wConlrVal = (wLrVal & 0x01) | ((wLrVal & 0x02) << 3) | ((wLrVal & 0x04) << 6) | ((wLrVal & 0x08) << 9) |
						((wLrVal & 0x10) << 12) | ((wLrVal & 0x20) << 15) | ((wLrVal & 0x40) << 18) | ((wLrVal & 0x80) << 21); 	
				ptGpioBase->CONLR = (ptGpioBase->CONLR & ~wConMask) | wConlrVal;
			}
			
			if(wHrVal)
			{
				wConMask = ((wHrVal & 0x01)*0xf) | (((wHrVal & 0x02) << 3)*0xf) | (((wHrVal & 0x04) << 6)*0xf) | (((wHrVal & 0x08) << 9)*0xf) |
						(((wHrVal & 0x10) << 12)*0xf) | (((wHrVal & 0x20) << 15)*0xf) | (((wHrVal & 0x40) << 18)*0xf) | (((wHrVal & 0x80) << 21)*0xf); 	
				wConhrVal = (wHrVal & 0x01) | ((wHrVal & 0x02) << 3) | ((wHrVal & 0x04) << 6) | ((wHrVal & 0x08) << 9) |
						((wHrVal & 0x10) << 12) | ((wHrVal & 0x20) << 15) | ((wHrVal & 0x40) << 18) | ((wHrVal & 0x80) << 21); 
				ptGpioBase->CONHR = (ptGpioBase->CONHR & ~wConMask) | wConhrVal;
			}
			
			break;
		
		case GPIO_DIR_OUTPUT:
			if(wLrVal)
			{
				wConMask = ((wLrVal & 0x01)*0xf) | (((wLrVal & 0x02) << 3)*0xf) | (((wLrVal & 0x04) << 6)*0xf) | (((wLrVal & 0x08) << 9)*0xf) |
						(((wLrVal & 0x10) << 12)*0xf) | (((wLrVal & 0x20) << 15)*0xf) | (((wLrVal & 0x40) << 18)*0xf) | (((wLrVal & 0x80) << 21)*0xf); 
				wConlrVal = ((wLrVal & 0x01) << 1) | ((wLrVal & 0x02) << 4) | ((wLrVal & 0x04) << 7) | ((wLrVal & 0x08) << 10) |
						((wLrVal & 0x10) << 13) | ((wLrVal & 0x20) << 16) | ((wLrVal & 0x40) << 19) | ((wLrVal & 0x80) << 22); 	
				ptGpioBase->CONLR = (ptGpioBase->CONLR & ~wConMask) | wConlrVal;
			}
			
			if(wHrVal)
			{
				wConMask = ((wHrVal & 0x01)*0xf) | (((wHrVal & 0x02) << 3)*0xf) | (((wHrVal & 0x04) << 6)*0xf) | (((wHrVal & 0x08) << 9)*0xf) |
						(((wHrVal & 0x10) << 12)*0xf) | (((wHrVal & 0x20) << 15)*0xf) | (((wHrVal & 0x40) << 18)*0xf) | (((wHrVal & 0x80) << 21)*0xf);
				wConhrVal = ((wHrVal & 0x01) << 1) | ((wHrVal & 0x02) << 4) | ((wHrVal & 0x04) << 7) | ((wHrVal & 0x08) << 10) |
						((wHrVal & 0x10) << 13) | ((wHrVal & 0x20) << 16) | ((wHrVal & 0x40) << 19) | ((wHrVal & 0x80) << 22); 
				ptGpioBase->CONHR = (ptGpioBase->CONHR & ~wConMask) | wConhrVal;
			}
			break;

		case GPIO_DIR_GPD:
			if(wLrVal)
			{
				wConMask = ((wLrVal & 0x01)*0xf) | (((wLrVal & 0x02) << 3)*0xf) | (((wLrVal & 0x04) << 6)*0xf) | (((wLrVal & 0x08) << 9)*0xf) |
						(((wLrVal & 0x10) << 12)*0xf) | (((wLrVal & 0x20) << 15)*0xf) | (((wLrVal & 0x40) << 18)*0xf) | (((wLrVal & 0x80) << 21)*0xf); 
				ptGpioBase->CONLR &= ~wConMask;
			}
			
			if(wHrVal)
			{
				wConMask = ((wHrVal & 0x01)*0xf) | (((wHrVal & 0x02) << 3)*0xf) | (((wHrVal & 0x04) << 6)*0xf) | (((wHrVal & 0x08) << 9)*0xf) |
						(((wHrVal & 0x10) << 12)*0xf) | (((wHrVal & 0x20) << 15)*0xf) | (((wHrVal & 0x40) << 18)*0xf) | (((wHrVal & 0x80) << 21)*0xf);
				ptGpioBase->CONHR &= ~wConMask;
			}
			break;
		default:
			ret = CSI_ERROR;
			break;
				
	}
	return ret;
}
/** \brief config gpio pull or output mode
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eMode: pin pullnone/pullup/pulldown
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_pull_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_pull_mode_e eMode)
{
	csi_error_t	ret = CSI_OK;
	uint32_t wPudrVal;
	uint32_t wPullVal = (wPinMask & 0xffff);
	uint32_t wPudrMask = 0;	
	
	switch(eMode)
	{
		case GPIO_PULLNONE:						//pullnone		
			if(wPullVal)
			{
				wPudrMask = ((wPullVal&0x01)*3)|(((wPullVal&0x02)<<1)*3)|(((wPullVal&0x04)<<2)*3)|(((wPullVal&0x08)<<3)*3)|(((wPullVal&0x10)<<4)*3)|(((wPullVal&0x20)<<5)*3)|
					(((wPullVal&0x40)<<6)*3)|(((wPullVal&0x80)<<7)*3)|(((wPullVal&0x100)<<8)*3)|(((wPullVal&0x200)<<9)*3)|(((wPullVal&0x400)<<10)*3)|(((wPullVal&0x800)<<11)*3)|
					(((wPullVal&0x1000)<<12)*3)|(((wPullVal&0x2000)<<13)*3)|(((wPullVal&0x4000)<<14)*3)|(((wPullVal&0x8000)<<15)*3);
				ptGpioBase->PUDR &= ~wPudrMask;
			}
			break;
		case GPIO_PULLUP:						//pulliup
			if(wPullVal)
			{
				wPudrMask = ((wPullVal&0x01)*3)|(((wPullVal&0x02)<<1)*3)|(((wPullVal&0x04)<<2)*3)|(((wPullVal&0x08)<<3)*3)|(((wPullVal&0x10)<<4)*3)|(((wPullVal&0x20)<<5)*3)|
					(((wPullVal&0x40)<<6)*3)|(((wPullVal&0x80)<<7)*3)|(((wPullVal&0x100)<<8)*3)|(((wPullVal&0x200)<<9)*3)|(((wPullVal&0x400)<<10)*3)|(((wPullVal&0x800)<<11)*3)|
					(((wPullVal&0x1000)<<12)*3)|(((wPullVal&0x2000)<<13)*3)|(((wPullVal&0x4000)<<14)*3)|(((wPullVal&0x8000)<<15)*3);
				wPudrVal = (wPullVal&0x01)|((wPullVal&0x02)<<1)|((wPullVal&0x04)<<2)|((wPullVal&0x08)<<3)|((wPullVal&0x10)<<4)|((wPullVal&0x20)<<5)|
					((wPullVal&0x40)<<6)|((wPullVal&0x80)<<7)|((wPullVal&0x100)<<8)|((wPullVal&0x200)<<9)|((wPullVal&0x400)<<10)|((wPullVal&0x800)<<11)|
					((wPullVal&0x1000)<<12)|((wPullVal&0x2000)<<13)|((wPullVal&0x4000)<<14)|((wPullVal&0x8000)<<15);
				ptGpioBase->PUDR = (ptGpioBase->PUDR & ~wPudrMask) | wPudrVal;
			}
			break;
		case GPIO_PULLDOWN:						//pulldown
			if(wPullVal)
			{
				wPudrMask = ((wPullVal&0x01)*3)|(((wPullVal&0x02)<<1)*3)|(((wPullVal&0x04)<<2)*3)|(((wPullVal&0x08)<<3)*3)|(((wPullVal&0x10)<<4)*3)|(((wPullVal&0x20)<<5)*3)|
					(((wPullVal&0x40)<<6)*3)|(((wPullVal&0x80)<<7)*3)|(((wPullVal&0x100)<<8)*3)|(((wPullVal&0x200)<<9)*3)|(((wPullVal&0x400)<<10)*3)|(((wPullVal&0x800)<<11)*3)|
					(((wPullVal&0x1000)<<12)*3)|(((wPullVal&0x2000)<<13)*3)|(((wPullVal&0x4000)<<14)*3)|(((wPullVal&0x8000)<<15)*3);
				wPudrVal = ((wPullVal&0x01) << 1)|((wPullVal&0x02)<<2)|((wPullVal&0x04)<<3)|((wPullVal&0x08)<<4)|((wPullVal&0x10)<<5)|((wPullVal&0x20)<<6)|
					((wPullVal&0x40)<<7)|((wPullVal&0x80)<<8)|((wPullVal&0x100)<<9)|((wPullVal&0x200)<<10)|((wPullVal&0x400)<<11)|((wPullVal&0x800)<<12)|
					((wPullVal&0x1000)<<13)|((wPullVal&0x2000)<<14)|((wPullVal&0x4000)<<15)|((wPullVal&0x8000)<<16);
				ptGpioBase->PUDR = (ptGpioBase->PUDR & ~wPudrMask) | wPudrVal;
			}
			break;
		default:
			ret = CSI_ERROR;
			break;
	}

	return ret;
 }
 
/** \brief config gpio output mode
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eMode: pin open drain/push pull(output mode) 
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_output_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_output_mode_e eMode)
{
	csi_error_t	ret = CSI_OK;
	uint32_t hwOpdVal = (wPinMask & 0xffff);
	
	switch(eMode)
	{
		case GPIO_PUSH_PULL:						//push pull output
			ptGpioBase->OMCR &= ~hwOpdVal;
			break;
		case GPIO_OPEN_DRAIN:						//open drain output
			ptGpioBase->OMCR = (ptGpioBase->OMCR &~hwOpdVal) | hwOpdVal;
			break;
		default:
			ret = CSI_ERROR;
			break;
	}

	return ret;
}
/** \brief config gpio output mode
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eMode: pin input mode; TTL1/TTL2/COMS
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_input_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_input_mode_e eMode)
{
	csi_error_t	ret = CSI_OK;	
	uint32_t wDscrVal;	
	uint32_t wDscrMask = 0;	
	uint32_t wDrvVal = wPinMask & 0xffff;
	uint32_t hwOmcrVal = (wPinMask & 0xffff) << 16;
	
	switch (eMode)
	{
		case (GPIO_INPUT_TTL2):	
			if(wDrvVal)
			{
				wDscrMask = ((wDrvVal&0x01)*3)|(((wDrvVal&0x02)<<1)*3)|(((wDrvVal&0x04)<<2)*3)|(((wDrvVal&0x08)<<3)*3)|(((wDrvVal&0x10)<<4)*3)|(((wDrvVal&0x20)<<5)*3)|
					(((wDrvVal&0x40)<<6)*3)|(((wDrvVal&0x80)<<7)*3)|(((wDrvVal&0x100)<<8)*3)|(((wDrvVal&0x200)<<9)*3)|(((wDrvVal&0x400)<<10)*3)|(((wDrvVal&0x800)<<11)*3)|
					(((wDrvVal&0x1000)<<12)*3)|(((wDrvVal&0x2000)<<13)*3)|(((wDrvVal&0x4000)<<14)*3)|(((wDrvVal&0x8000)<<15)*3);
				wDscrVal = ((wDrvVal&0x01) << 1)|((wDrvVal&0x02)<<2)|((wDrvVal&0x04)<<3)|((wDrvVal&0x08)<<4)|((wDrvVal&0x10)<<5)|((wDrvVal&0x20)<<6)|
					((wDrvVal&0x40)<<7)|((wDrvVal&0x80)<<8)|((wDrvVal&0x100)<<9)|((wDrvVal&0x200)<<10)|((wDrvVal&0x400)<<11)|((wDrvVal&0x800)<<12)|
					((wDrvVal&0x1000)<<13)|((wDrvVal&0x2000)<<14)|((wDrvVal&0x4000)<<15)|((wDrvVal&0x8000)<<16);
				ptGpioBase->DSCR = (ptGpioBase->DSCR & ~wDscrMask) | wDscrVal;
				ptGpioBase->OMCR = (ptGpioBase->OMCR & ~hwOmcrVal) | hwOmcrVal;
			}
			break;
		case (GPIO_INPUT_TTL1): 	
			if(wDrvVal)
			{
				wDscrMask = ((wDrvVal&0x01)*3)|(((wDrvVal&0x02)<<1)*3)|(((wDrvVal&0x04)<<2)*3)|(((wDrvVal&0x08)<<3)*3)|(((wDrvVal&0x10)<<4)*3)|(((wDrvVal&0x20)<<5)*3)|
					(((wDrvVal&0x40)<<6)*3)|(((wDrvVal&0x80)<<7)*3)|(((wDrvVal&0x100)<<8)*3)|(((wDrvVal&0x200)<<9)*3)|(((wDrvVal&0x400)<<10)*3)|(((wDrvVal&0x800)<<11)*3)|
					(((wDrvVal&0x1000)<<12)*3)|(((wDrvVal&0x2000)<<13)*3)|(((wDrvVal&0x4000)<<14)*3)|(((wDrvVal&0x8000)<<15)*3);
				wDscrVal = ((wDrvVal&0x01) << 1)|((wDrvVal&0x02)<<2)|((wDrvVal&0x04)<<3)|((wDrvVal&0x08)<<4)|((wDrvVal&0x10)<<5)|((wDrvVal&0x20)<<6)|
					((wDrvVal&0x40)<<7)|((wDrvVal&0x80)<<8)|((wDrvVal&0x100)<<9)|((wDrvVal&0x200)<<10)|((wDrvVal&0x400)<<11)|((wDrvVal&0x800)<<12)|
					((wDrvVal&0x1000)<<13)|((wDrvVal&0x2000)<<14)|((wDrvVal&0x4000)<<15)|((wDrvVal&0x8000)<<16);
				ptGpioBase->DSCR = (ptGpioBase->DSCR & ~wDscrMask) | wDscrVal;
				ptGpioBase->OMCR &= ~hwOmcrVal;
			}
			break;
		case (GPIO_INPUT_CMOS):	
				wDscrMask = ((wDrvVal&0x01)*3)|(((wDrvVal&0x02)<<1)*3)|(((wDrvVal&0x04)<<2)*3)|(((wDrvVal&0x08)<<3)*3)|(((wDrvVal&0x10)<<4)*3)|(((wDrvVal&0x20)<<5)*3)|
					(((wDrvVal&0x40)<<6)*3)|(((wDrvVal&0x80)<<7)*3)|(((wDrvVal&0x100)<<8)*3)|(((wDrvVal&0x200)<<9)*3)|(((wDrvVal&0x400)<<10)*3)|(((wDrvVal&0x800)<<11)*3)|
					(((wDrvVal&0x1000)<<12)*3)|(((wDrvVal&0x2000)<<13)*3)|(((wDrvVal&0x4000)<<14)*3)|(((wDrvVal&0x8000)<<15)*3);
				ptGpioBase->DSCR &= ~wDscrMask;
			break;
		default:
			ret = CSI_ERROR;
			break;
	}

	return ret;
 }
/** \brief config gpio irq mode
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eTrgEdge: rising edge; falling edge;	both edge;
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_irq_mode(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_irq_mode_e eTrgEdge)
{
	csi_error_t	ret = CSI_OK;
	uint32_t wIgrplVal;
	uint32_t wGrpMask = 0;
	uint32_t wGrplVal = (wPinMask & 0xff);
	uint32_t wGrphVal = (wPinMask >> 8) & 0xff;
	uint32_t wEdgeMask = (wPinMask & 0xffff);
	
	csp_gpio_set_port_irq(ptGpioBase, wPinMask, ENABLE);		//to make exi to be visible by syscon
	
	if(wGrplVal)		//interrupt groupl
	{
		wGrpMask = ((wGrplVal & 0x01)*0x7) | (((wGrplVal & 0x02) << 3)*0x7) | (((wGrplVal & 0x04) << 6)*0x7) | (((wGrplVal & 0x08) << 9)*0x7) |
					(((wGrplVal & 0x10) << 12)*0x7) | (((wGrplVal & 0x20) << 15)*0x7) | (((wGrplVal & 0x40) << 18)*0x7) | (((wGrplVal & 0x80) << 21)*0x7); 
		if((uint32_t)ptGpioBase == APB_GPIOA0_BASE)
			GPIOGRP->IGRPL &= ~wGrpMask;
		else if((uint32_t)ptGpioBase == APB_GPIOB0_BASE)
		{
			wIgrplVal = ((wGrplVal & 0x01) << 1) | ((wGrplVal & 0x02) << 4) | ((wGrplVal & 0x04) << 7) | ((wGrplVal & 0x08) << 10) |
					((wGrplVal & 0x10) << 13) | ((wGrplVal & 0x20) << 16) | ((wGrplVal & 0x40) << 19) | ((wGrplVal & 0x80) << 22); 	
			GPIOGRP->IGRPL = (GPIOGRP->IGRPL & ~wGrpMask) | wIgrplVal;
		}
	}
	
	if(wGrphVal)		//interrupt groupl
	{
		wGrpMask = ((wGrphVal & 0x01)*0x7) | (((wGrphVal & 0x02) << 3)*0x7) | (((wGrphVal & 0x04) << 6)*0x7) | (((wGrphVal & 0x08) << 9)*0x7) |
					(((wGrphVal & 0x10) << 12)*0x7) | (((wGrphVal & 0x20) << 15)*0x7) | (((wGrphVal & 0x40) << 18)*0x7) | (((wGrphVal & 0x80) << 21)*0x7); 
		if((uint32_t)ptGpioBase == APB_GPIOA0_BASE)
			GPIOGRP->IGRPH &= ~wGrpMask;
	}
	
	switch(eTrgEdge)
	{
		case GPIO_IRQ_RISING_EDGE:							//rising edge interrupt mode
			SYSCON->EXIRT |= wEdgeMask;
			SYSCON->EXIFT &= ~wEdgeMask;
			break;
		case GPIO_IRQ_FALLING_EDGE:							//falling edge interrupt mode
			SYSCON->EXIFT |= wEdgeMask;
			SYSCON->EXIRT &= ~wEdgeMask;
			break;
		case GPIO_IRQ_BOTH_EDGE:							//double edge interrupt mode
			SYSCON->EXIFT |= wEdgeMask;
			SYSCON->EXIRT |= wEdgeMask;
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
	return ret;
}
/** \brief gpio irq enable
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] bEnable: true or false
 *  \return none
 */ 
void csi_gpio_port_irq_enable(csp_gpio_t *ptGpioBase, uint32_t wPinMask, bool bEnable)
{
	uint8_t  i,k = 0;
	uint8_t  byIrqbuf[5];
	
	csp_gpio_set_port_irq(ptGpioBase, wPinMask, bEnable);	//GPIO INT enable Control reg(setting IEER)
	csp_exi_set_port_irq(SYSCON,wPinMask, bEnable);			//EXI INT enable
	csp_exi_clr_port_irq(SYSCON,wPinMask);					//clear interrput status before enable irq 
	
	for(i = 0; i < 5; i++)
	{
		if(wPinMask & wExiStateMap[i])
		{
			byIrqbuf[k++] = byExiNumMap[i];
		}
	}
	
	for(i = 0; i < k; i++)
	{
		if(bEnable)
			csi_vic_enable_irq((uint32_t)byIrqbuf[i]);
		else
			csi_vic_disable_irq((uint32_t)byIrqbuf[i]);
	}
}
/** \brief  set gpio toggle
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \return none
 */ 
void csi_gpio_port_toggle(csp_gpio_t *ptGpioBase, uint32_t wPinMask)
{
    CSI_PARAM_CHK_NORETVAL(ptGpioBase);

	uint32_t tmp = csp_gpio_read_output_port(ptGpioBase);
	csp_gpio_write_output_port(ptGpioBase, tmp ^ wPinMask);
}

/** \brief  read gpio status value
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \return value gpio port select wPinMask
 */ 
uint32_t csi_gpio_port_read(csp_gpio_t *ptGpioBase, uint32_t wPinMask)
{
    return csp_gpio_read_input_port(ptGpioBase) & wPinMask;
}
/** \brief  write gpio ouput value
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] ePinVal: gpio output value;
 *  \return none
 */ 
void  csi_gpio_port_write(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_pin_state_e ePinVal)
{
	uint32_t tmp = csp_gpio_read_output_port(ptGpioBase);
    if (ePinVal == 1) 
        csp_gpio_write_output_port(ptGpioBase, tmp | wPinMask);
    else 
        csp_gpio_write_output_port(ptGpioBase, tmp & (~wPinMask));
}

/** \brief  set gpio ouput high
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \return none
 */ 
void  csi_gpio_port_set_high(csp_gpio_t *ptGpioBase, uint32_t wPinMask)
{
	uint32_t tmp = csp_gpio_read_output_port(ptGpioBase);
	csp_gpio_write_output_port(ptGpioBase, tmp | wPinMask);
}
/** \brief  set gpio ouput low
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \return none
 */ 
void  csi_gpio_port_set_low(csp_gpio_t *ptGpioBase, uint32_t wPinMask)
{
	uint32_t tmp = csp_gpio_read_output_port(ptGpioBase);
	csp_gpio_write_output_port(ptGpioBase, tmp & (~wPinMask));
}


