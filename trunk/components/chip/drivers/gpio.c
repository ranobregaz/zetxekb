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
//#include <csi_config.h>
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

//static uint8_t s_byPinNum = 16;			//GPIO(A0/B0) pin num

/** \brief initialize gpio data structure
 * 
 *  \param[in] ptGpioBase: GPIO port handle
 *  \param[in] port_idx: gpio index
 *  \return error code \ref csi_error_t
 */  
//csi_error_t csi_gpio_init(csp_gpio_t *ptGpioBase, uint32_t port_idx)
//{
//    //CSI_PARAM_CHK(gpio, CSI_ERROR);
//
//    csi_error_t ret = CSI_OK;
//
////    if (target_get(DEV_EXI_TAG, port_idx, &gpio->dev) != CSI_OK) {
////        ret = CSI_ERROR;
////    }
////
////	s_byGpioNum = 16;
//
//    return ret;
//}
/** \brief de-initialize gpio interface
 * 
 *  \param[in] gpio: GPIO port handle
 *  \return error code \ref csi_error_t
 */ 
//void csi_gpio_uninit(csp_gpio_t *ptGpioBase)
//{
//   // CSI_PARAM_CHK_NORETVAL(gpio);
//	
////	csp_gpio_t *reg = (csp_gpio_t *)HANDLE_REG_BASE(gpio);
////	csp_gpio_default_Init(reg);
////	memset(gpio, 0, sizeof(csi_gpio_t));
//} 
/** \brief config gpio direction
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] wPinMask: pin mask,0x0001~0xffff
 *  \param[in] eDir: pin dir,ouput/input
 *  \return	error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_dir(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_dir_e eDir)
{
	CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
	
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
	CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
	
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
	CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
	
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
	CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
	
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
/** \brief set gpio interrupt group
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eExiGrp:	EXI_IGRP0 ~ EXI_IGRP19
 *  \return none
 */ 
void gpio_intgroup_set(csp_gpio_t *ptGpioBase, uint8_t byPinNum, gpio_igrp_e eExiGrp)
{
	uint32_t byMaskShift,byMask;
	gpio_group_e eIoGroup = GRP_GPIOA0;
	
	switch((uint32_t)ptGpioBase)
	{
		case APB_GPIOA0_BASE:
			eIoGroup = GRP_GPIOA0;
			break;
		case APB_GPIOB0_BASE:
			eIoGroup = GRP_GPIOB0;
			break;
		default:
			break;
	}
	
	if(eExiGrp < EXI_IGRP16)
	{
		if(byPinNum < 8)
		{
			byMaskShift = (byPinNum << 2);
			byMask = ~(0x0Ful << byMaskShift);
			GPIOGRP->IGRPL = ((GPIOGRP->IGRPL) & byMask) | (eIoGroup << byMaskShift);
		}
		else if(byPinNum < 16)
		{
			byMaskShift = ((byPinNum-8) << 2);
			byMask = ~(0x0Ful << byMaskShift);
			GPIOGRP->IGRPH = ((GPIOGRP->IGRPH) & byMask) | (eIoGroup << byMaskShift);
		}
	}
	else if(eExiGrp <= EXI_IGRP19)
	{
		
		if(GRP_GPIOA0 == eIoGroup)
		{
			if(eExiGrp < EXI_IGRP18)
			{
				byMaskShift = (eExiGrp - EXI_IGRP16) << 2;
				byMask = ~(0x0Ful << byMaskShift);
				GPIOGRP->IGREX = ((GPIOGRP->IGREX) & byMask) | (byPinNum << byMaskShift);
			}
		}
		else
		{
			if(eExiGrp > EXI_IGRP17)
			{
				byMaskShift = (eExiGrp - EXI_IGRP16) << 2;
				byMask = ~(0x0Ful << byMaskShift);
				GPIOGRP->IGREX = ((GPIOGRP->IGREX) & byMask) | (byPinNum << byMaskShift);
			}
		}
	}	
}
/** \brief set gpio exi interrupt trigger 
 * 
 *  \param[in] ptSysconBase: pionter of SYSCON reg structure.
 *  \param[in] eExiGrp: EXI_IGRP0~EXI_IGRP19
 *  \param[in] eGpioTrg: EXI_IRT,EXI_IFT,
 *  \return none
 */ 
void exi_trg_edge_set(csp_syscon_t *ptSysconBase,gpio_igrp_e eExiGrp, exi_trigger_e eGpioTrg)
{
	uint32_t wPinMsak = (0x01ul << eExiGrp);
	
	ptSysconBase->EXIRT &= (~wPinMsak);					//trig edg
	ptSysconBase->EXIFT &= (~wPinMsak);
	
	switch(eGpioTrg)
	{
		case EXI_EDGE_IRT:
			ptSysconBase->EXIRT |= wPinMsak;
			ptSysconBase->EXIFT &= ~wPinMsak;
			break;
		case EXI_EDGE_IFT:
			ptSysconBase->EXIFT |= wPinMsak;
			ptSysconBase->EXIRT &= ~wPinMsak;
			break;
		case EXI_EDGE_BOTH:
			ptSysconBase->EXIRT |= wPinMsak;
			ptSysconBase->EXIFT |= wPinMsak;
			break;
		default:
			break;
	}
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
    CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
	
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
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpio_port_irq_enable(csp_gpio_t *ptGpioBase, uint32_t wPinMask, bool bEnable)
{
    CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);

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

	return CSI_OK;
}

/** \brief config gpio irq mode(assign exi group)
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] byPinNum: pin num,0~15
 *  \param[in] eExiGrp: exi group; EXI_GRP0 ~EXI_GRP19
 *  \param[in] eTrgEdge: rising edge; falling edge;	both edge;
 *  \return error code \ref csi_error_t
 */ 
//csi_error_t csi_gpio_irq_mode_ex(csp_gpio_t *ptGpioBase,uint8_t byPinNum, csi_exi_grp_e eExiGrp, csi_gpio_irq_mode_e eTrgEdge)
//{
//    CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
//	
//	csi_error_t	ret = CSI_OK;
//    
//	csp_gpio_set_port_irq(ptGpioBase,(0x01ul << byPinNum), ENABLE);	//to make exi to be visible by syscon
//	gpio_intgroup_set(ptGpioBase,byPinNum,eExiGrp);					//interrupt group
//	switch(eTrgEdge)
//	{
//		case GPIO_IRQ_RISING_EDGE:									//rising edge interrupt mode
//			exi_trg_edge_set(SYSCON,eExiGrp, EXI_EDGE_IRT);			//trig edg
//			break;
//		case GPIO_IRQ_FALLING_EDGE:									//falling edge interrupt mode
//			exi_trg_edge_set(SYSCON,eExiGrp, EXI_EDGE_IFT);			//trig edg
//			break;
//		case GPIO_IRQ_BOTH_EDGE:									//double edge interrupt mode
//			exi_trg_edge_set(SYSCON,eExiGrp, EXI_EDGE_BOTH);		//trig edg
//			break;
//		//case GPIO_IRQ_LOW_LEVEL:									//low level
//		//case GPIO_IRQ_HIGH_LEVEL:									//high level
//		default:
//			ret = CSI_UNSUPPORTED;
//			break;
//	}
//	return ret;
//}

/** \brief gpio irq enable (assign exi group)
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] byPinNum: pin num,0~15
 *  \param[in] bEnable: true or false
 *  \return error code \ref csi_error_t
 */ 
//csi_error_t csi_gpio_irq_enable_ex(csp_gpio_t *ptGpioBase, uint8_t byPinNum, csi_exi_grp_e eExiGrp, bool bEnable)
//{
//    CSI_PARAM_CHK(ptGpioBase, CSI_ERROR);
//	
//	uint8_t  byIrqNum;
//	
//	if(eExiGrp > EXI_GRP19)
//		return CSI_ERROR;
//		
//	csp_gpio_set_port_irq(ptGpioBase,(0x01ul << byPinNum), bEnable);	//GPIO INT enable Control reg(setting IEER)
//	csp_exi_set_port_irq(SYSCON,(0x01ul << eExiGrp), bEnable);		//EXI INT enable
//	csp_exi_clr_port_irq(SYSCON,(0x01ul << eExiGrp));				//clear interrput status before enable irq 
//	
//	switch(eExiGrp)
//	{
//		case EXI_GRP0:
//		case EXI_GRP16:
//			byIrqNum = EXI0_IRQn; 
//			break;
//		case EXI_GRP1:
//		case EXI_GRP17:
//			byIrqNum = EXI1_IRQn; 
//			break;
//		case EXI_GRP2:
//		case EXI_GRP3:
//		case EXI_GRP18:
//		case EXI_GRP19:
//			byIrqNum = EXI2_IRQn; 
//			break;
//		default:
//			if(eExiGrp < 10)
//				byIrqNum = EXI3_IRQn; 
//			else 
//				byIrqNum = EXI4_IRQn; 
//			break;
//	}
//	
//	if(bEnable)
//		csi_vic_enable_irq((uint32_t)byIrqNum);
//	else
//		csi_vic_disable_irq((uint32_t)byIrqNum);
//
//	return CSI_OK;
//}

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
 *  \return error code \ref csi_error_t
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
 *  \return error code \ref csi_error_t
 */ 
void  csi_gpio_port_write(csp_gpio_t *ptGpioBase, uint32_t wPinMask, csi_gpio_pin_state_e ePinVal)
{
    CSI_PARAM_CHK_NORETVAL(ptGpioBase);

	uint32_t tmp = csp_gpio_read_output_port(ptGpioBase);
    if (ePinVal == 1) 
        csp_gpio_write_output_port(ptGpioBase, tmp | wPinMask);
    else 
        csp_gpio_write_output_port(ptGpioBase, tmp & (~wPinMask));
}

/** \brief  set EXI as trigger Event(EV0~5) 
 *  \param[in] byTrgOut: output Event select(TRGOUT0~5)
 *  \param[in] eExiTrgSrc: event source (TRGSRC_EXI0~19)
 *  \param[in] byTrgPrd: accumulated EXI events to output trigger 
 *  \return none
 */ 
csi_error_t csi_exi_set_evtrg(uint8_t byTrgOut, csi_exi_trgsrc_e eExiTrgSrc, uint8_t byTrgPrd)
{
	//csi_error_t	ret = CSI_OK;
	
	byTrgPrd &= 0xf;
	
	//set evtrg source 
	if (byTrgOut < 4 && eExiTrgSrc < 16)	
	{
		SYSCON -> EVTRG = (SYSCON -> EVTRG & ~(TRG_SRC0_3_MSK(byTrgOut))) | (eExiTrgSrc << TRG_SRC0_3_POS(byTrgOut));
		
		if(byTrgPrd)		//set evtrg period
		{
			SYSCON -> EVTRG |= TRG_EVCNT_CLR_MSK(byTrgOut);		//clear TRG EVxCNT
			SYSCON -> EVPS = (SYSCON -> EVPS & ~(TRG_EVPRD_MSK(byTrgOut)))| ((byTrgPrd - 1) << TRG_EVPRD_POS(byTrgOut));
		}
	}
	else if (byTrgOut < 6 && eExiTrgSrc > 15) 
		SYSCON -> EVTRG = (SYSCON -> EVTRG & ~(TRG_SRC4_5_MSK(byTrgOut)))| ((eExiTrgSrc-16) << TRG_SRC4_5_POS(byTrgOut));
	else
		return CSI_ERROR;
	
	//evtrg output event enable
	SYSCON -> EVTRG = (SYSCON -> EVTRG & ~(ENDIS_ESYNC_MSK(byTrgOut))) | (ENABLE << ENDIS_ESYNC_POS(byTrgOut));
	
//	csp_syscon_set_evtrg_src(SYSCON, byTrgOut, eExiTrgSrc);
//	csp_syscon_set_evtrg_prd(SYSCON, byTrgOut, byTrgPrd);
//	csp_syscon_evtrg_enable(SYSCON, byTrgOut, ENABLE);
	
	return CSI_OK;
}

/** \brief Set debounce of gpio when gpio configed as input
 *  
 *  \param[in] ptGpioBase: GPIO port handle
 *  \param[in] wPinMask:Pin mask need to be set
 *  \param[in] bEnable: 0: disable   1:enable
 *  \return Error code
*/
//csi_error_t csi_gpio_debounce(csp_gpio_t *ptGpioBase, uint32_t wPinMask, bool bEnable)
//{
//	return CSI_UNSUPPORTED;
//}
