/***********************************************************************//** 
 * \file  csp_gpio.c
 * \brief  GPIO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2020-12-20 <td>V0.1  <td>ZJY   <td>modify for 110
 * <tr><td> 2021-1-12 <td>V0.2  <td>ZJY   <td> adapt for 102x/003
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include "csp_gpio.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief gpio reg default init
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \return none
 */  
void csp_gpio_default_Init(csp_gpio_t *ptGpioBase)
{
    ptGpioBase->CONLR = GPIO_RESET_VALUE;                     
    ptGpioBase->CONHR = GPIO_RESET_VALUE;
    ptGpioBase->WODR  = GPIO_RESET_VALUE; 
    ptGpioBase->SODR  = GPIO_RESET_VALUE; 
    ptGpioBase->CODR  = GPIO_RESET_VALUE; 
	//ptGpioBase->ODSR  =	GPIO_RESET_VALUE;
	//ptGpioBase->PSDR  =	GPIO_RESET_VALUE;
	ptGpioBase->FLTEN = 0xffff;
    ptGpioBase->PUDR  = GPIO_RESET_VALUE; 
    ptGpioBase->DSCR  = GPIO_RESET_VALUE;
	ptGpioBase->OMCR  = GPIO_RESET_VALUE;
    ptGpioBase->IECR  = GPIO_RESET_VALUE;
}  

/** \brief gpio interrupt group register default init
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \return none
 */  
void csp_igrp_default_Init(csp_igrp_t *ptIgrpBase)
{
    ptIgrpBase->IGRPL  = GPIO_RESET_VALUE;
	ptIgrpBase->IGRPH  = GPIO_RESET_VALUE;
	ptIgrpBase->IGREX  = GPIO_RESET_VALUE;
	ptIgrpBase->IO_CLKEN = 0xf;
} 
/** \brief gpio mux register config
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] eByte: 0,PIN0~PIN7; 1,PIN8~P15
 *  \param[in] wVal: the value of mux register
 *  \return none
 */ 
void csp_gpio_mux_init(csp_gpio_t *ptGpioBase,gpio_byte_e eByte, uint32_t wVal)
{
    if (eByte == 0)
    {
        ptGpioBase->CONLR = wVal;
    }
    else if(eByte == 1)
    {
        ptGpioBase->CONHR = wVal;
    }
}
/** \brief set gpio mux function 
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \param[in] byMux: the value of mux function
 *  \return none
 */ 
int csp_gpio_mux_set(csp_gpio_t *ptGpioBase,uint8_t byPinNum,uint8_t byMux)
{
	if (byPinNum > 15 || byMux > 12 ) return -1;
	if(byPinNum < 8)
		ptGpioBase->CONLR =(ptGpioBase->CONLR & ~(0xF << 4*byPinNum)) | (byMux << 4*byPinNum);
	else
		ptGpioBase->CONHR =(ptGpioBase->CONHR & ~(0xF << 4*(byPinNum-8))) | (byMux << 4*(byPinNum-8));	
	return 0;
}
/** \brief get gpio mux function 
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \return value of pin mux function
 */ 
int csp_gpio_mux_get(csp_gpio_t *ptGpioBase,uint8_t byPinNum)
{
	uint32_t wRegVal;
	
	if (byPinNum > 15) 
		return -1;
	if(byPinNum < 8)
		wRegVal = (((ptGpioBase->CONLR) >> 4 * byPinNum) & 0x0Ful);
	else
		wRegVal = (((ptGpioBase->CONHR) >> 4 * (byPinNum - 8)) & 0x0Ful);
		
	return wRegVal;
}
/** \brief set gpio output or input 
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eDir: INPUT,OUTPUT
 *  \return none
 */ 
void csp_gpio_dir_set(csp_gpio_t *ptGpioBase, uint8_t byPinNum,gpio_dir_e eDir)
{
    if(byPinNum < 8)
    {	
		if (eDir)
			ptGpioBase->CONLR =(ptGpioBase->CONLR & ~(0x0Ful << 4*byPinNum)) | (0x01ul << 4*byPinNum);
        else
			ptGpioBase->CONLR =(ptGpioBase->CONLR & ~(0x0Ful << 4*byPinNum)) | (0x02ul << 4*byPinNum);
    }
    else if (byPinNum < 16)
    {	
		if (eDir)
			ptGpioBase->CONHR =(ptGpioBase->CONHR & ~(0x0Ful << 4*(byPinNum-8))) | (0x01ul << 4*(byPinNum-8));
        else
			ptGpioBase->CONHR =(ptGpioBase->CONHR & ~(0x0Ful << 4*(byPinNum-8))) | (0x02ul << 4*(byPinNum-8));
    }
}
/** \brief disable gpio IO function disable
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \return none
 */ 
void csp_gpio_inputoutput_dis(csp_gpio_t *ptGpioBase,uint8_t byPinNum)
{
    //uint32_t data_temp;
    if(byPinNum < 8)
    {
		ptGpioBase->CONLR = (ptGpioBase->CONLR & ~(0x0Ful << 4*byPinNum));
    }
    else if (byPinNum<16)
    {
		ptGpioBase->CONHR =(ptGpioBase->CONHR & ~(0x0Ful << 4*(byPinNum-8)));
    }
}
/** \brief set gpio IO port interrupt
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] wValue: 0x01~0xffff
 *  \param[in] bEnable: true or false
 *  \return none
 */ 
void csp_gpio_set_port_irq(csp_gpio_t *ptGpioBase, uint32_t wValue, bool bEnable)
{
	if(bEnable)
		ptGpioBase->IEER = wValue;
	else
		ptGpioBase->IEDR = wValue;
}
/** \brief set gpio exi port interrupt
 * 
 *  \param[in] ptSysconBase: pionter of SYSCON reg structure.
 *  \param[in] wValue: 0x01 - 0xfffff
 *  \param[in] bEnable: true or false
 *  \return none
 */ 
void csp_exi_set_port_irq(csp_syscon_t *ptSysconBase,uint32_t wValue,bool bEnable)
{
   	if(bEnable)
	{
		ptSysconBase->EXIER = wValue;				//EXI4 interrupt enable
		while(!(ptSysconBase->EXIMR & wValue));		//Check EXI4 is enabled or not
		ptSysconBase->EXICR = wValue;				//Clear EXI4 status bit
	}
	else
		ptSysconBase ->EXIDR = wValue;
}

/** \brief gpio bank get
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \return GRP_GPIOA0 or GRP_GPIOB0
 */ 
gpio_group_e get_gpio_exi_intbank(csp_gpio_t *ptGpioBase)
{
	gpio_group_e eIoGroup;
	switch((uint32_t)ptGpioBase)
	{
		case APB_GPIOA0_BASE:
			eIoGroup = GRP_GPIOA0;
			break;
		case APB_GPIOB0_BASE:
			eIoGroup = GRP_GPIOB0;
			break;
		default:
			eIoGroup = GRP_GPIOA0;
			break;
	}
	return eIoGroup;
}
/** \brief set gpio interrupt group
 * 
 *  \param[in] ptGpioBase: pointer of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eExiGrp: EXI_IGRP0 ~ EXI_IGRP19
 *  \return none
 */ 
void csp_gpio_intgroup_set(csp_gpio_t *ptGpioBase, uint8_t byPinNum, gpio_igrp_e eExiGrp)
{
	uint32_t byMaskShift,byMask;
	gpio_group_e eIoGroup;
	csp_igrp_t *ptIgrpBase  = (csp_igrp_t*)APB_IGRP_BASE;
	
	eIoGroup = get_gpio_exi_intbank(ptGpioBase);
	
	if(eExiGrp < EXI_IGRP16)
	{
		if(byPinNum < 8)
		{
			byMaskShift = (byPinNum << 2);
			byMask = ~(0x0Ful << byMaskShift);
			ptIgrpBase->IGRPL = ((ptIgrpBase->IGRPL) & byMask) | (eIoGroup << byMaskShift);
		}
		else if(byPinNum < 16)
		{
			byMaskShift = ((byPinNum-8) << 2);
			byMask = ~(0x0Ful << byMaskShift);
			ptIgrpBase->IGRPH = ((ptIgrpBase->IGRPH) & byMask) | (eIoGroup << byMaskShift);
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
				ptIgrpBase->IGREX = ((ptIgrpBase->IGREX) & byMask) | (byPinNum << byMaskShift);
			}
		}
		else
		{
			if(eExiGrp > EXI_IGRP17)
			{
				byMaskShift = (eExiGrp - EXI_IGRP16) << 2;
				byMask = ~(0x0Ful << byMaskShift);
				ptIgrpBase->IGREX = ((ptIgrpBase->IGREX) & byMask) | (byPinNum << byMaskShift);
			}
		}
	}	
}
/** \brief set gpio exi interrupt trigger 
 * 
 *  \param[in] ptSysconBase: pionter of SYSCON reg structure.
 *  \param[in] eExiGrp: EXI_IGRP0~EXI_IGRP19
 *  \param[in] eGpioTrig: EXI_IRT,EXI_IFT,
 *  \return none
 */ 
void csp_exi_trigger_set(csp_syscon_t *ptSysconBase,gpio_igrp_e eExiGrp, exi_trigger_e eGpioTrig)
{
	uint32_t wPinMsak = (0x01ul << eExiGrp);
	
	ptSysconBase->EXIRT &= (~wPinMsak);					//trig edg
	ptSysconBase->EXIFT &= (~wPinMsak);
	
	switch(eGpioTrig)
	{
		case EXI_EDGE_IRT:
			ptSysconBase->EXIRT |= wPinMsak;
			break;
		case EXI_EDGE_IFT:
			ptSysconBase->EXIFT |= wPinMsak;
			break;
		case EXI_EDGE_BOTH:
			ptSysconBase->EXIRT |= wPinMsak;
			ptSysconBase->EXIFT |= wPinMsak;
			break;
		default:
			//ptSysconBase->EXIFT |= (0x01 << byPinNum);
			break;
	}
}
/** \brief get gpio exi IRQ number 
 * 
 *  \param[in] eExiGrp: EXI_IGRP0 ~ EXI_IGRP19
 *  \return EXI IRQ number 
 */ 
uint32_t get_gpio_exi_intnum(gpio_igrp_e eExiGrp)
{
	uint32_t wIntNum  = EXI0_IRQn;	
	switch(eExiGrp){
		case 0:
		case 16: 
			wIntNum  = EXI0_IRQn;
			break;
		case 1:
		case 17:
			wIntNum  = EXI1_IRQn;
			break;
		case 2:
		case 3:
		case 18:
		case 19:
			wIntNum  = EXI2_IRQn;
			break;
		default:
			if(eExiGrp < 10)
				wIntNum  = EXI3_IRQn;
			else if(eExiGrp < 16)
				wIntNum  = EXI4_IRQn;
		break;
	}
	return wIntNum;
}
/** \brief set gpio exi 
 *
 *  \param[in] ptGpioBase: pionter of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eExiGrp: EXI_IGRP0 ~ EXI_IGRP19
 *  \param[in] eGpioTrig: interrupt trigger mode
 *  \param[in] bExiEn: gpio interrupt enable/disable, true or false
 *  \return none 
 */ 
void csp_gpio_exi_config(csp_gpio_t *ptGpioBase,uint8_t byPinNum,gpio_igrp_e eExiGrp,exi_trigger_e eGpioTrig, bool bExiEn)
{
	volatile uint32_t wIntNum;

	if(byPinNum > 15 || eExiGrp > EXI_IGRP19)
		return;
	
	wIntNum = get_gpio_exi_intnum(eExiGrp);
	
	csp_exi_int_dis(SYSCON_REG_BASE,eExiGrp);					//exi disable
	csp_gpio_pullhigh_set(ptGpioBase,byPinNum);					//pull high
	csp_gpio_mux_set(ptGpioBase,byPinNum,0x01);					//input
	csp_gpio_int_en(ptGpioBase, byPinNum);						//set gpio IEER	
	csp_gpio_intgroup_set(ptGpioBase,byPinNum,eExiGrp);			//set int group
	
	csp_exi_trigger_set(SYSCON_REG_BASE,eExiGrp, eGpioTrig);	//trig edg
	
	if(bExiEn)
	{
		NVIC_ClearPendingIRQ(wIntNum);
		NVIC_DisableIRQ(wIntNum);
		NVIC_EnableIRQ(wIntNum);
	
		csp_exi_clr_pending(SYSCON_REG_BASE,eExiGrp);			//clear exi pending
		csp_exi_int_en(SYSCON_REG_BASE,eExiGrp);				//exi enable
	}
	else
	{
		NVIC_DisableIRQ(wIntNum);
		csp_exi_int_en(SYSCON_REG_BASE,eExiGrp);				//exi disable
	}
}

/** \brief set gpio output value
 *
 *  \param[in] ptGpioBase:  pionter of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \param[in] eState: 0,low; 1,high
 *  \return none 
 */ 
void csp_gpio_set_value(csp_gpio_t *ptGpioBase,uint8_t byPinNum,gpio_pin_state_e eState)
{
    if(eState == PIN_LOW)

        ptGpioBase->CODR = (1ul << byPinNum);

	else
		ptGpioBase->SODR = (1ul << byPinNum);
}
/** \brief Reverse gpio output value
 *
 *  \param[in] ptGpioBase:  pionter of GPIO reg structure.
 *  \param[in] byPinNum: pin0~15
 *  \return none 
 */ 
void csp_gpio_toggle(csp_gpio_t *ptGpioBase,uint8_t byPinNum)
{
	uint32_t wDat = 0;
	wDat = (ptGpioBase->ODSR >> byPinNum) & 0x01;
	if(wDat) 
		ptGpioBase->CODR = (1ul << byPinNum);
	else
		ptGpioBase->SODR = (1ul << byPinNum);
}

/******************* (C) COPYRIGHT 2020 APT Chip *****END OF FILE****/
