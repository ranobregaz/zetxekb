/***********************************************************************//** 
 * \file  timer.c
 * \brief  csi timer driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/timer.h>
#include <drv/irq.h>
#include <drv/tick.h>

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/

/** \brief initialize timer data structure
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] wTimeOut: the timeout for timer, unit: us
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_bt_timer_init(csp_bt_t *ptTmBase, uint32_t wTimeOut)
{
    CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	CSI_PARAM_CHK(wTimeOut, CSI_ERROR);

	uint32_t wTmLoad;
	uint32_t wClkDiv;
	
	csi_clk_enable((uint32_t *)ptTmBase);									//bt clk enable
	csp_bt_soft_rst(ptTmBase);												//reset bt
	
	wClkDiv = (soc_get_pclk_freq()/100000 * wTimeOut/600000);				//bt clk div value
	if(wClkDiv == 0)
		wClkDiv  = 1;
	wTmLoad = (soc_get_pclk_freq() / (wClkDiv * 20000)) * wTimeOut / 50;	//bt prdr load value
	
	csp_bt_set_cr(ptTmBase, (BT_IMMEDIATE << BT_SHDW_POS) | (BT_CONTINUOUS << BT_OPM_POS) |		//bt work mode
			(BT_PCLKDIV << BT_EXTCKM_POS) | (BT_CNTRLD_EN << BT_CNTRLD_POS) | BT_CLK_EN );
	csp_bt_set_pscr(ptTmBase, (uint16_t)wClkDiv - 1);						//bt clk div	
	csp_bt_set_prdr(ptTmBase, (uint16_t)wTmLoad);							//bt prdr load value
	csp_bt_set_int(ptTmBase, BT_PEND_INT, true);							//enable PEND interrupt
	csi_irq_enable((uint32_t *)ptTmBase);									//enable bt irq
	
    return CSI_OK;
}
/** \brief de-initialize timer interface
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
//void csi_timer_uninit(csi_timer_t *timer)
//{
////    CSI_PARAM_CHK_NORETVAL(timer);
////    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
////
////    memset((void *)timer_base, 0, sizeof(csp_bt_t));
//}
/** \brief start timer
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] timeout_us: the timeout for timer((1us * 1) ->(1us * 65535): 1us ->65.535ms)
 *  \return error code \ref csi_error_t
 */ 
//csi_error_t csi_timer_start(csi_timer_t *timer, uint32_t timeout_us)
//{
//    CSI_PARAM_CHK(timer, CSI_ERROR);
//    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
//    
//	csi_error_t ret = CSI_OK;
////    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
////	uint32_t tmp_load;
////	
////	timer->priv = (void*)(soc_get_pclk_freq()/100000 * timeout_us/600000);
////	if((uint32_t)timer->priv == 0)
////		timer->priv  = (void *)1;
////	
////	tmp_load = (soc_get_pclk_freq() / ((uint32_t)timer->priv * 20000)) * timeout_us / 50;
////	csp_bt_set_timer(timer_base, BT_CLK_EN, BT_IMMEDIATE, BT_CONTINUOUS, BT_PCLKDIV, BT_CNTRLD_EN, (uint32_t)timer->priv - 1);		//timer clk = pclk/24
////	csp_bt_set_prdr(timer_base,(uint16_t)tmp_load);
////	csp_bt_set_cmp(timer_base,(uint16_t)tmp_load >> 1); 		//cmp = padr/2
////	csp_bt_set_int(timer_base, BT_PEND_INT | BT_CMP_INT, true);
////	
////	csp_bt_start(timer_base);
//	
//	return ret;
//}
/** \brief open(start) timer
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_bt_timer_open(csp_bt_t *ptTmBase)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	csp_bt_start(ptTmBase);
	
	return CSI_OK;
}
/** \brief close(stop) timer
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_bt_timer_close(csp_bt_t *ptTmBase)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
    csp_bt_stop(ptTmBase);
	
	return CSI_OK;
}
/** \brief   attach the callback handler to timer
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] callback: timer interrupt callback function
 *  \param[in] arg: callback's param
 *  \return error code \ref csi_error_t
 */ 
//csi_error_t csi_timer_attach_callback(csi_timer_t *timer, void *callback, void *arg)
//{
//    CSI_PARAM_CHK(timer, CSI_ERROR);
//	
////	//csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
////	//csi_timer_intsrc_t int_src = (csi_timer_intsrc_t)arg;
////	
////    timer->callback = callback;
////    timer->arg = arg;
////    csi_irq_attach((uint32_t)timer->dev.irq_num, &apt_timer_irqhandler, &timer->dev);
////    csi_irq_enable((uint32_t)timer->dev.irq_num);
////	//csp_bt_set_int(timer_base, int_src, true);
//	
//    return CSI_OK;
//}
/** \brief detach timer callback handler
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
//void csi_timer_detach_callback(csi_timer_t *timer)
//{
//    CSI_PARAM_CHK_NORETVAL(timer);
//
////    timer->callback = NULL;
////    timer->arg = NULL;
////    csi_irq_disable((uint32_t)timer->dev.irq_num);
////    csi_irq_detach((uint32_t)timer->dev.irq_num);
//}

/** \brief get timer remaining value
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \return none
 */ 
uint32_t csi_timer_get_remaining_value(csp_bt_t *ptTmBase)
{
    CSI_PARAM_CHK(ptTmBase, 0U);
	
    uint32_t wTimeVal = csp_bt_get_prdr(ptTmBase) - csp_bt_get_cnt(ptTmBase);

	return wTimeVal;
}
/** \brief get timer load value
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \return none
 */ 
uint32_t csi_timer_get_load_value(csp_bt_t *ptTmBase)
{
    CSI_PARAM_CHK(ptTmBase, 0U);
	
	return csp_bt_get_prdr(ptTmBase);
}
/** \brief get timer load value
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \return rue: running; false: stopped
 */ 
bool csi_timer_is_running(csp_bt_t *ptTmBase)
{
	CSI_PARAM_CHK(ptTmBase, false);
	 
	if(ptTmBase->RSSR & BT_START)
		return true;
	else 
		return false;
}

/** \brief timer pwm init 
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] ptTmPara: pointer of timer parameter structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_bt_pwm_init(csp_bt_t *ptTmBase, csi_timer_t *ptTmPara)
{
    CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	CSI_PARAM_CHK(ptTmPara, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
	uint32_t wClkDiv;
	uint32_t wCrVal;
	uint32_t wCmpLoad; 
	uint32_t wPrdrLoad; 
	
	if(ptTmPara->wFreq == 0 || ptTmPara->byDutyCycle  == 0 || ptTmPara->byDutyCycle == 100)
		return CSI_ERROR;
	
	csi_clk_enable((uint32_t *)ptTmBase);								//bt clk enable
	csp_bt_soft_rst(ptTmBase);											//reset bt
		
	wClkDiv = (soc_get_pclk_freq() / ptTmPara->wFreq / 60000);			//bt clk div value
	if(wClkDiv == 0)
		wClkDiv = 1;
	
	wPrdrLoad  = (soc_get_pclk_freq() / (wClkDiv * ptTmPara->wFreq));	//prdr load value
	wCmpLoad = wPrdrLoad * ptTmPara->byDutyCycle /100;					//cmp load value
		
	wCrVal = BT_CLK_EN | (BT_IMMEDIATE << BT_SHDW_POS) | (BT_CONTINUOUS << BT_OPM_POS) | (BT_PCLKDIV << BT_EXTCKM_POS) |
				(BT_CNTRLD_EN << BT_CNTRLD_POS) | (ptTmPara->byIdleLevel << BT_IDLEST_POS) | (ptTmPara->byIdleLevel << BT_STARTST_POS);
	csp_bt_set_cr(ptTmBase, wCrVal);									//set bt work mode
	csp_bt_set_pscr(ptTmBase, (uint16_t)wClkDiv - 1);					//bt clk div
	csp_bt_set_prdr(ptTmBase, (uint16_t)wPrdrLoad);						//bt prdr load value
	csp_bt_set_cmp(ptTmBase, (uint16_t)wCmpLoad);						//bt cmp load value
	
	if(ptTmPara->byInter)
	{
		csp_bt_set_int(ptTmBase, ptTmPara->byInter, true);				//enable interrupt
		csi_irq_enable((uint32_t *)ptTmBase);							//enable bt irq
	}
	
	return ret;
}
/** \brief start timer pwm
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] freq: pwm frequency  
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
//csi_error_t csi_timer_pwm_start(csi_timer_t *timer, uint32_t freq, uint32_t duty_cycle) 
//{
//	CSI_PARAM_CHK(timer, CSI_ERROR);
//	
////	//csi_error_t ret = CSI_OK;
////	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
////	
////	uint32_t prdr_load;  	//= (soc_get_pclk_freq() / (24 * freq));
////	uint32_t cmp_load; 		//= prdr_load * duty_cycle /100;
////	
////	if(freq == 0 || duty_cycle == 0 || duty_cycle == 100)
////		return CSI_ERROR;
////		
////	timer->priv = (void*)(soc_get_pclk_freq()/freq/60000);
////	if((uint32_t)timer->priv == 0)
////		timer->priv  = (void *)1;
////	
////	prdr_load  = (soc_get_pclk_freq() / ((uint32_t)timer->priv * freq));
////	cmp_load = prdr_load * duty_cycle /100;
////	
////	csp_bt_set_timer(timer_base, BT_CLK_EN, BT_SHDOW, BT_CONTINUOUS, BT_PCLKDIV, BT_CNTRLD_EN, (uint32_t)timer->priv -1);		//timer clk = pclk/24
////	csp_bt_set_prdr(timer_base,prdr_load);
////	csp_bt_set_cmp(timer_base, cmp_load);
////	csp_bt_set_int(timer_base, BT_PEND_INT, true);
////	csp_bt_start(timer_base);
//	
//	return CSI_OK;
//}
/** \brief  updata timer para and cmp reg value
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] hwPrdr: timer padr reg  value
 *  \param[in] hwCmp: timer cmp reg value
 *  \return none
 */
csi_error_t csi_timer_prdr_cmp_updata(csp_bt_t *ptTmBase, uint16_t hwPrdr, uint16_t hwCmp) 
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	
	ptTmBase->PRDR = hwPrdr;		
	ptTmBase->CMP  = hwCmp;	
	
	return CSI_OK;
}

/** \brief  updata timer pwm duty cycle
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] byDutyCycle: duty cycle(0 -> 100)
 *  \return none
 */
csi_error_t csi_bt_pwm_duty_cycle_updata(csp_bt_t *ptTmBase, uint8_t byDutyCycle) 
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	
	uint32_t wCmpLoad = csp_bt_get_prdr(ptTmBase) * byDutyCycle /100;
	
	csp_bt_set_cmp(ptTmBase, wCmpLoad);
	//csp_bt_updata_en(ptTmBase);
	return CSI_OK;
}

/** \brief  updata timer pwm freq and duty cycle
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] wfreq: pwm frequency  
 *  \param[in] byDutyCycle: pwm duty cycle(0 -> 100)
 *  \return none
 */
csi_error_t csi_bt_pwm_updata(csp_bt_t *ptTmBase, uint32_t wfreq, uint8_t byDutyCycle) 
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	
	uint16_t hwClkDiv = csp_bt_get_pscr(ptTmBase) + 1;
	uint32_t wPrdrLoad  = (soc_get_pclk_freq() / (hwClkDiv * wfreq));
	uint32_t wCmpLoad = wPrdrLoad * byDutyCycle /100;
	
	ptTmBase->PRDR = wPrdrLoad;		
	ptTmBase->CMP  = wCmpLoad;	
	
	return CSI_OK;
}

/** \brief timer sync input evtrg config  
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] eTrgin: timer sync evtrg input channel(0~1)
 *  \param[in] eTrgMode: timer evtrg mode, continuous/once
 *  \param[in] bAutoRearm: auto rearm, ENABLE/DISABLE(true/false)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_set_sync(csp_bt_t *ptTmBase,csi_timer_trgin_e eTrgin, csi_timer_trgmode_e eTrgMode, bool bAutoRearm)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR); 
    
	csi_error_t ret = CSI_OK;
	bt_sync_e eSync = BT_SYNC_EN;
	
	if(eTrgin == TIMER_SYNCEN1)
	{
		eTrgMode = eTrgMode << 1;
		eSync = eSync << 1;
	}
	ptTmBase->CR = (ptTmBase->CR & 0xffff30ff) | (eSync << BT_SYNC_POS) | (BT_SYNCMD_DIS << BT_SYNCCMD_POS) | 
				(eTrgMode << BT_OSTMD_POS) | (bAutoRearm << BT_AREARM_POS);
	//csp_bt_set_sync(ptTmBase, BT_SYNC_EN, BT_SYNCMD_DIS, eTrgMode, bAutoRearm,eTrgin);
	
	return ret;
}
/** \brief rearm timer sync evtrg  
 * 
 *  \param[in] ptTmBase: pointer of timer register structure
 *  \param[in] eTrgin: timer sync evtrg input channel(0~1)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_rearm_sync(csp_bt_t *ptTmBase,csi_timer_trgin_e eTrgin)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
	csp_bt_rearm_sync(ptTmBase, eTrgin);
	
	return ret;
}
/** \brief timer evtrg output config
 * 
 *  \param[in] ptTmBase:pointer of timer register structure
 *  \param[in] byEvtrg: trigger event (byEvtrg =  0)
 *  \param[in] eTrgSrc: BT evtrg source(1~3) 
 *  \param[in] eTrgPrd: event count period
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_set_evtrg(csp_bt_t *ptTmBase, uint8_t byEvtrg, csi_timer_trgsrc_e eTrgSrc, uint8_t eTrgPrd)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
    csi_error_t ret = CSI_OK;

	if (byEvtrg > 0)
		return CSI_ERROR;
	
	if(eTrgSrc == TIMER_TRGSRC_DIS)
		ptTmBase->EVTRG = 0x00;
	else
		ptTmBase->EVTRG =(ptTmBase->EVTRG & (~BT_TRGSRC_MSK)) | eTrgSrc | (BT_TRGOE_EN << BT_TRGOE_POS);
		
	return ret;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/** \brief start timer by sync event
 * 
 *  \param[in] ptTmBase: handle timer handle to operate
 *  \param[in] wTimeOut: tthe timeout for timer, unit: us
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_start_sync(csp_bt_t *ptTmBase, uint32_t wTimeOut)
{
	CSI_PARAM_CHK(ptTmBase, CSI_ERROR);
	CSI_PARAM_CHK(wTimeOut, CSI_ERROR);
	
	uint32_t wTmLoad;
	uint32_t wClkDiv;
	
	csi_clk_enable((uint32_t *)ptTmBase);									//bt clk enable
	csp_bt_soft_rst(ptTmBase);												//reset bt
	
	wClkDiv = (soc_get_pclk_freq()/100000 * wTimeOut/600000);				//bt clk div
	if(wClkDiv == 0)
		wClkDiv  = 1;
	wTmLoad = (soc_get_pclk_freq() / (wClkDiv * 20000)) * wTimeOut / 50;	//bt prdr load value
	
	csp_bt_set_cr(ptTmBase, (BT_IMMEDIATE << BT_SHDW_POS) | (BT_CONTINUOUS << BT_OPM_POS) |		//bt work mode
			(BT_PCLKDIV << BT_EXTCKM_POS) | (BT_CNTRLD_EN << BT_CNTRLD_POS) | BT_CLK_EN );
	csp_bt_set_pscr(ptTmBase, (uint16_t)wClkDiv - 1);						//bt clk div	
	csp_bt_set_prdr(ptTmBase, (uint16_t)wTmLoad);							//bt prdr load value
	csp_bt_set_int(ptTmBase, BT_PEND_INT, true);							//enable PEND interrupt
	csi_irq_enable((uint32_t *)ptTmBase);									//enable bt irq
	
	return CSI_OK;
}