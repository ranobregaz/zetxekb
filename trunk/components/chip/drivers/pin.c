/***********************************************************************//** 
 * \file  pinmux.c
 * \brief  csi pin driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>adapt for 102x/003
 * </table>
 * *********************************************************************
*/
#include <stdint.h>
#include <stddef.h>
#include <drv/pin.h>
#include <drv/pin.h>

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/

/** \brief set gpio mux function
 * 
 *  \param[in] ePinName: gpio pin name
 *  \return pointer of pin infor
 */ 
static unsigned int *apt_get_pin_name_addr(pin_name_e ePinName)
{
	static unsigned int pin_infor[2];
	if(ePinName > PA015)
	{
		pin_infor[0] = APB_GPIOB0_BASE;				//PB0
		pin_infor[1] = ePinName - 16;
	}
	else
	{
		pin_infor[0] = APB_GPIOA0_BASE;				//PA0
		pin_infor[1] = ePinName;
	}	
	
	return pin_infor;
}

/** \brief set gpio interrupt group
 * 
 *  \param[in] ptGpioBase: pointer of gpio register structure
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eExiGrp:	EXI_IGRP0 ~ EXI_IGRP19
 *  \return none
 */ 
static void apt_gpio_intgroup_set(csp_gpio_t *ptGpioBase, uint8_t byPinNum, gpio_igrp_e eExiGrp)
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
static void apt_exi_trg_edge_set(csp_syscon_t *ptSysconBase,gpio_igrp_e eExiGrp, exi_trigger_e eGpioTrg)
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

/** \brief set gpio mux function
 * 
 *  \param[in] ePinName: gpio pin name
 *  \param[in] ePinFunc: gpio pin function
 *  \return enone
 */  
void csi_pin_set_mux(pin_name_e ePinName, pin_func_e ePinFunc)
{
	csp_gpio_t *ptGpioBase = NULL;
	
	//IO REMAP
	if(ePinFunc == IOMAP)			
	{
		csp_syscon_t *ptSysBase = (csp_syscon_t *)APB_SYS_BASE;
		if(ePinName < PA08)
			ptSysBase->IOMAP0 = (ptSysBase->IOMAP0 & ~(0x0F << 4*ePinName)) | (ePinName << 4*ePinName);
		else 
		{
			if(ePinName < PB02)
				ptSysBase->IOMAP1 = (ptSysBase->IOMAP1 & ~(0x0F << 4*(ePinName - 6))) |  ((ePinName - 6) << 4*(ePinName - 6));
			else 
				ptSysBase->IOMAP1 = (ptSysBase->IOMAP1 & ~(0x0F << 4*(ePinName - 18))) |  ((ePinName - 18) << 4*(ePinName - 18));
		}
	}
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName - 16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;	
	
	if(ePinName < 8)
		ptGpioBase->CONLR =(ptGpioBase->CONLR & ~(0xF << 4*ePinName)) | (ePinFunc << 4*ePinName);
	else
		ptGpioBase->CONHR =(ptGpioBase->CONHR & ~(0xF << 4*(ePinName-8))) | (ePinFunc << 4*(ePinName-8));	
	
}
/** \brief get gpio mux function
 * 
 *  \param[in] ePinName: gpio pin name
 *  \return value of gpio mux
 */ 
pin_func_e csi_pin_get_mux(pin_name_e ePinName)
{
    uint8_t ret = 0x0f;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName - 16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
		
	if(ePinName < 8)
		ret = (((ptGpioBase->CONLR) >> 4 * ePinName) & 0x0Ful);
	else
		ret = (((ptGpioBase->CONHR) >> 4 * (ePinName - 8)) & 0x0Ful);
		
    return (pin_func_e)ret;
}
/** \brief set gpio pin pull mode
 * 
 *  \param[in] ePinName: gpio pin name
 *  \param[in] ePullMode: gpio pull mode; pull none/pull up/pull down
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_pin_pull_mode(pin_name_e ePinName, csi_gpio_pull_mode_e ePullMode)
{
    csi_error_t ret = CSI_OK;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName - 16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
		
	switch(ePullMode)
	{
		case GPIO_PULLNONE:
			csp_gpio_pullnone(ptGpioBase, ePinName);			//pull none
			break;
		case GPIO_PULLUP:
			csp_gpio_pullup(ptGpioBase, ePinName);			//pull up
			break;
		case GPIO_PULLDOWN:
			csp_gpio_pulldown(ptGpioBase, ePinName);			//pull down
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
    return ret;
}
/** \brief set gpio pin speed
 * 
 *  \param[in] ePinName: gpio pin name
 *  \param[in] eSpeed: gpio pin speed
 *  \return none
 */  
void csi_pin_speed(pin_name_e ePinName, csi_gpio_speed_e eSpeed)
{
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	csp_gpio_speed_set(ptGpioBase, ePinName, (uint8_t)eSpeed);
	
}

/** \brief set gpio pin drive level
 * 
 *  \param[in] ePinName: gpio pin name, defined in soc.h.
 *  \param[in] eDrive: gpio pin drive level, week/strong = 0/1
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_drive(pin_name_e ePinName, csi_gpio_drive_e eDrive)
{
#if defined(IS_CHIP_102) || defined(IS_CHIP_1022)
	return CSI_ERROR;
#endif
	csi_error_t ret = CSI_OK;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	csp_gpio_drv_set(ptGpioBase, ePinName, (uint8_t)eDrive);
		
    return ret;
}

/** \brief set gpio pin input mode
 * 
 *  \param[in] ePinName: gpio pin name, defined in soc.h.
 *  \param[in] eInputMode:  INPUT_CMOS/INPUT_TTL
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_input_mode(pin_name_e ePinName, csi_gpio_input_mode_e eInputMode)
{
#if defined(IS_CHIP_102) || defined(IS_CHIP_1021) || defined(IS_CHIP_1022) || defined(IS_CHIP_S003)
	return CSI_ERROR;
#endif
	csi_error_t ret = CSI_OK;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
	
	switch (eInputMode)
	{
		case (GPIO_INPUT_TTL2):	csp_gpio_ccm_ttl2(ptGpioBase, ePinName);
			break;
		case (GPIO_INPUT_TTL1): csp_gpio_ccm_ttl1(ptGpioBase, ePinName);
			break;
		case (GPIO_INPUT_CMOS):	csp_gpio_ccm_cmos(ptGpioBase, ePinName);
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
	return ret;
}
/** \brief set gpio pin input mode
 * 
 *  \param[in] ePinName: gpio pin name, defined in soc.h.
 *  \param[in] eOutMode: push-pull/open drain
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_output_mode(pin_name_e ePinName, csi_gpio_output_mode_e eOutMode)
{
	csi_error_t ret = CSI_OK;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;
		
	switch(eOutMode)
	{
		case GPIO_PUSH_PULL:
			csp_gpio_opendrain_dis(ptGpioBase, ePinName);	//push-pull mode
			break;
		case GPIO_OPEN_DRAIN:
			csp_gpio_opendrain_en(ptGpioBase, ePinName);	//open drain mode 
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
	return ret; 
}

/** \brief get gpio pin num
 * 
 *  \param[in] ePinName: gpio pin name, defined in soc.h.
 *  \return pin num
 */ 
uint8_t csi_pin_get_num(pin_name_e ePinName)
{
    uint8_t ret = 44;
	unsigned int *pin_mess = NULL;
	
	pin_mess = apt_get_pin_name_addr(ePinName);
	ret = (uint8_t)pin_mess[1];					//gpio pin number
						
    return ret;
}
/** \brief Get the value of  selected pin 
 *  \param[in] ePinName: gpio pin name, defined in soc.h.
 *  \return According to the bit mask, the corresponding pin status is obtained
*/
uint32_t csi_pin_read(pin_name_e ePinName)
{
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;	
		
	return (csp_gpio_read_input_port(ptGpioBase) & (0x01ul << ePinName));
}
/** \brief config pin irq mode(assign exi group)
 * 
 *  \param[in] ePinName: pin name
 *  \param[in] eExiGrp: exi group; EXI_GRP0 ~EXI_GRP19
 *  \param[in] eTrgEdge: rising edge; falling edge;	both edge;
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_irq_mode(pin_name_e ePinName, csi_exi_grp_e eExiGrp, csi_gpio_irq_mode_e eTrgEdge)
{
	csi_error_t ret = CSI_OK;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;				
		
	csp_gpio_irq_en(ptGpioBase, ePinName);							//enable gpio interrupt 
	apt_gpio_intgroup_set(ptGpioBase,ePinName,eExiGrp);					//interrupt group
	
	if(eTrgEdge >  GPIO_IRQ_BOTH_EDGE)
		ret = CSI_ERROR;
	else
		apt_exi_trg_edge_set(SYSCON,eExiGrp, eTrgEdge);					//interrupt edge
	
	return ret;
}
/** \brief pin irq enable
 * 
 *  \param[in] ePinName: pin mask,0x0001~0xffff
 *  \param[in] bEnable: true or false
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_irq_enable(pin_name_e ePinName, csi_exi_grp_e eExiGrp, bool bEnable)
{
	csp_gpio_t *ptGpioBase = NULL;
	uint32_t byIrqNum = EXI0_IRQn;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;		
		
	csp_gpio_set_port_irq(ptGpioBase, (0x01ul << ePinName), bEnable);	//GPIO INT enable Control reg(setting IEER)
	csp_exi_set_port_irq(SYSCON,(0x01ul << ePinName), bEnable);			//EXI INT enable
	csp_exi_clr_port_irq(SYSCON,(0x01ul << ePinName));					//clear interrput status before enable irq 
	
	switch(eExiGrp)
	{
		case EXI_GRP0:
		case EXI_GRP16:
			byIrqNum = EXI0_IRQn;
			break;
		case EXI_GRP1:
		case EXI_GRP17:
			byIrqNum = EXI1_IRQn;
			break;
		case EXI_GRP2:
		case EXI_GRP3:
		case EXI_GRP18:
		case EXI_GRP19:
			byIrqNum = EXI2_IRQn;
			break;
		default:
			if(eExiGrp < EXI_GRP10)				//group4->9
				byIrqNum = EXI3_IRQn;
			else if(eExiGrp < EXI_GRP16)		//group10->15
				byIrqNum = EXI4_IRQn;
			else
				return CSI_ERROR;
				
			break;
	}
	
	if(bEnable)
		csi_vic_enable_irq(byIrqNum);
	else
		csi_vic_disable_irq(byIrqNum);
	
	return CSI_OK;
}
/** \brief  gpio pin toggle
 * 
 *  \param[in] ePinName: gpio pin name
 *  \return none
 */
void csi_pin_toggle(pin_name_e ePinName)
{
	uint32_t wDat;
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;				
	
	wDat = (ptGpioBase->ODSR >> ePinName) & 0x01;
	if(wDat) 
		ptGpioBase->CODR = (1ul << ePinName);
	else
		ptGpioBase->SODR = (1ul << ePinName);
	
}

/** \brief  gpio pin set high(output = 1)
 * 
 *  \param[in] ePinName: gpio pin name
 *  \return none
 */
void csi_pin_set_high(pin_name_e ePinName)
{
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;			
	
	csp_gpio_set_high(ptGpioBase, (uint8_t)ePinName);
}

/** \brief   gpio pin set low(output = 0)
 * 
 *  \param[in] ePinName: gpio pin name
 *  \return none
 */
void csi_pin_set_low(pin_name_e ePinName)
{
	csp_gpio_t *ptGpioBase = NULL;
	
	if(ePinName > PA015)
	{
		ptGpioBase = (csp_gpio_t *)APB_GPIOB0_BASE;
		ePinName = ePinName -16;
	}
	else
		ptGpioBase = (csp_gpio_t *)APB_GPIOA0_BASE;				
	
	csp_gpio_set_low(ptGpioBase, (uint8_t)ePinName);
}
/** \brief  set exi as trigger Event(EV0~5) 
 *  \param[in] byTrgOut: output Event select(TRGOUT0~5)
 *  \param[in] eExiTrgSrc: event source (TRGSRC_EXI0~19)
 *  \param[in] byTrgPrd: accumulated EXI events to output trigger 
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_exi_set_evtrg(uint8_t byTrgOut, csi_exi_trgsrc_e eExiTrgSrc, uint8_t byTrgPrd)
{
	
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