/***********************************************************************//** 
 * \file  csp_coret.c
 * \brief  CORET description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include "csp_coret.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief coret reg default init
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return none
 */   
void csp_coret_default_init(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CSR=CORET_CSR_RST;
	ptCoretBase->CORET_RVR=CORET_RVR_RST;
	ptCoretBase->CORET_CVR=CORET_CVR_RST;
	ptCoretBase->CORET_CALIB=CORET_CALIB_RST;
}
/** \brief coret VIC interrupt enable
 * 
 *  \param[in] none
 *  \return  none
 */   
void csp_coret_int_en(void)
{
    //ck801_base->CORET_CVR = 0x0;				// Clear counter and flag
	NVIC_EnableIRQ(CORET_IRQn);
}
/** \brief coret VIC interrupt disable
 * 
 *  \param[in] none
 *  \return  none
 */  
void csp_coret_int_dis(void)
{
	NVIC_DisableIRQ(CORET_IRQn);
}
/** \brief coret VIC interrupt wakeup enable
 * 
 *  \param[in] none
 *  \return  none
 */  
void csp_coret_wakeup_en(void)
{
	NVIC_SetWakeupIRQ(CORET_IRQn);
}
/** \brief coret VIC interrupt wakeup disable
 * 
 *  \param[in] none
 *  \return  none
 */  
void  csp_coret_wakeup_dis(void)
{
	NVIC_ClearWakeupIRQ(CORET_IRQn); 
}
/** \brief CORET start 
 * 
 *  Coret count start
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */  
 /*
void csp_coret_start(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CSR |= 0x01;
}
*/
/** \brief CORET stop 
 * 
 *  Coret count stop
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */ 
 /*
void csp_coret_stop(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CSR &= 0xfffffffe;
}
*/
/** \brief CORET clk source select 
 * 
 *  Coret clk source select, clk = coreclk/8
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */ 
/*
void csp_coret_clksrc_div8(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CSR &= 0xfffffffb;
}
*/
/** \brief CORET clk source select
 * 
 *  Coret clk source select, clk = coreclk
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */ 
/*
void csp_coret_clksrc_div1(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CSR |= 0x04;
}
*/
/** \brief CORET count interrupt enable
 * 
 *  Coret tick interrupt enable
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */
/* 
void csp_coret_tickint_en(csp_coret_t *ptCoretBase)
{
    ptCoretBase->CORET_CSR |= 0x02;
}
*/
/** \brief CORET count interrupt disable
 * 
 *  Coret tick interrupt disenable
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */
/* 
void csp_coret_tickint_dis(csp_coret_t *ptCoretBase)
{
   ptCoretBase->CORET_CSR &= 0xfffffffd;
}
*/
/** \brief CORET count val reload
 * 
 *  Coret count value reload
 * 
 *  \param[in] ptCoretBase: pointer of CORET reg structure.
 *  \return  none
 */ 
 /*
void csp_coret_reload(csp_coret_t *ptCoretBase)
{
	ptCoretBase->CORET_CVR = 0x00;				// Clear counter and flag
}
*/
/******************* (C) COPYRIGHT 2020 APT Chip *****END OF FILE****/

