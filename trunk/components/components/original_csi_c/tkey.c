/***********************************************************************//** 
 * \file  tkey.c
 * \brief  csi tkey driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Vertkeyn  <th>Author  <th>Description
 * <tr><td> 2020-9-03 <td>V0.0  <td>XB   <td>initial
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/tkey.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>
#include "csp_tkey.h"
#include "uart.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

extern uint32_t wTkey;
/** \brief tkey interrupt handle 
 * 
 *  \param[in] args: pointer of tkey dev
 *  \return none
 */
void apt_tkey_irqhandler(void *arg)
{
	csi_tkey_t *tkey		= (csi_tkey_t *)arg;
    csp_tkey_t *tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	uint32_t 	irqnumber = 0;
	irqnumber =csp_tkey_get_risr(tkey_base);
	 if((irqnumber&TKEY_SINDNE)==TKEY_SINDNE)
	{
		csp_tkey_clr_risr(tkey_base,TKEY_SINDNE);
		
	}
	if((irqnumber&TKEY_DNE)==TKEY_DNE)
	{
		wTkey=1;
		csp_tkey_clr_risr(tkey_base,TKEY_DNE);
	}
	if((irqnumber&TKEY_THR)==TKEY_THR)
	{
		csp_tkey_clr_risr(tkey_base,TKEY_THR);
	}
	if((irqnumber&TKEY_FLW)==TKEY_FLW)
	{
		csp_tkey_clr_risr(tkey_base,TKEY_FLW);
	}
	if((irqnumber&TKEY_OVW)==TKEY_OVW)
	{
		csp_tkey_clr_risr(tkey_base,TKEY_OVW);
	}
	if((irqnumber&TKEY_TIME)==TKEY_TIME)
	{
		csp_tkey_clr_risr(tkey_base,TKEY_TIME);
	}
	if (tkey->callback) 
	{
		tkey->callback(tkey, tkey->arg);
	
	}
	
}
/** \brief initialize tkey
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \param[in] port_idx: gpio index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_tkey_init(csi_tkey_t *tkey, uint32_t idx)
{
	CSI_PARAM_CHK(tkey, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	csp_tkey_t *tkey_base;
	
	
	if (target_get(DEV_TKEY_TAG, idx, &tkey->dev) != CSI_OK)
		ret = CSI_ERROR;
    else
	{
		tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
		tkey->callback        = NULL;
		tkey->arg             = NULL;
		csi_clk_enable(&tkey->dev);						//tkey peripheral clk en
		csp_tkey_clk_config(tkey_base,ENABLE,0,0);		//tkey peripheral clk en
		csp_tkey_init(tkey_base);						//tkey init 
		
	}
	return ret;
   
}
/** \brief TKey parameter
 *
 *  \param[in] none
 *  \return none
 */
void csi_tkey_parameter(void)
{
	wTkeyIOEnable  = 0x3 << 7;				 //TK IO使能    第几位对应TOUCH几如： bit0 -->  touch 0      bit16 --> touch16

 	hwTkeyDiv[0]   = 2;	//TK扫描分频1~3  
	hwTkeyDiv[1]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[2]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[3]   = 2;	//TK扫描分频1~3  
	hwTkeyDiv[4]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[5]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[6]   = 2;	//TK扫描分频1~3  
	hwTkeyDiv[7]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[8]   = 2;	//TK扫描分频1~3 
	hwTkeyDiv[9]   = 2;	//TK扫描分频1~3  
	hwTkeyDiv[10] = 2;	//TK扫描分频1~3 
	hwTkeyDiv[11] = 2;	//TK扫描分频1~3 
	hwTkeyDiv[12] = 2;	//TK扫描分频1~3  
	hwTkeyDiv[13] = 2;	//TK扫描分频1~3 
	hwTkeyDiv[14] = 2;	//TK扫描分频1~3 
	hwTkeyDiv[15] = 2;	//TK扫描分频1~3  
	hwTkeyDiv[16] = 2;	//TK扫描分频1~3 

	hwTkeyIcon[0]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[1]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[2]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[3]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[4]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[5]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[6]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[7]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[8]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[9]   = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[10] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[11] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[12] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[13] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[14] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[15] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	hwTkeyIcon[16] = 4;	//TK扫描Icon0~7	//icon减小采样值增大
	
	hwTkeySenprd[0]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[1]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[2]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[3]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[4]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[5]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[6]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[7]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[8]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[9]   = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[10] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[11] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[12] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[13] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[14] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[15] = 70;	//TK扫描周期	//senprd增加采样值增大
	hwTkeySenprd[16] = 70;	//TK扫描周期	//senprd增加采样值增大

	hwTkeyTriggerLevel[0]   = 100;	//门槛值
	hwTkeyTriggerLevel[1]   = 100;	//门槛值
	hwTkeyTriggerLevel[2]   = 100;	//门槛值
	hwTkeyTriggerLevel[3]   = 100;	//门槛值
	hwTkeyTriggerLevel[4]   = 100;	//门槛值
	hwTkeyTriggerLevel[5]   = 100;	//门槛值
	hwTkeyTriggerLevel[6]   = 100;	//门槛值
	hwTkeyTriggerLevel[7]   = 100;	//门槛值
	hwTkeyTriggerLevel[8]   = 100;	//门槛值
	hwTkeyTriggerLevel[9]   = 100;	//门槛值
	hwTkeyTriggerLevel[10] = 100;	//门槛值
	hwTkeyTriggerLevel[11] = 100;	//门槛值
	hwTkeyTriggerLevel[12] = 100;	//门槛值
	hwTkeyTriggerLevel[13] = 100;	//门槛值
	hwTkeyTriggerLevel[14] = 100;	//门槛值
	hwTkeyTriggerLevel[15] = 100;	//门槛值
	hwTkeyTriggerLevel[16] = 100;	//门槛值
	
	byPressDebounceData=2;		//按下去抖1~5
	byReleaseDebounceData=2;	//释放去抖1~5
	byMultiTimesFilter=20;		//倍数滤波>=4
	byTkeyMode=1;				// 1=multi key
	byValidKeyNum=4;			//最大按下有效个数

 	byBaseSpeed=30;									//基线更新速度
 	byTekyLongpressTime=16;					//longpress rebuild time = _TK_longpress_time1*1s  0=disable
 	wTkeyBaseCnt=19999;							//基线更新时间 =  (定时器时间*wTkeyBaseCnt ) 

 	wTkeyPselMode = TK_PSEL_AVDD;		//tk power sel:TK_PSEL_FVR/TK_PSEL_AVDD
 	wTkeyFvrLevel = TK_FVR_4096V;		//FVR level:TK_FVR_2048V/TK_FVR_4096V
 	wTkeyECLevel = TK_EC_3_6V;		//C0 voltage sel:TK_EC_1V/TK_EC_2V/TK_EC_3V/TK_EC_3_6V
 	wTkeyCon0 = 0;	
}


/** \brief de-initialize tkey interface
 * 
 *  \param[in] tkey: Handle of tkey instance
 *  \return none
 */ 
void csi_tkey_uninit(csi_tkey_t *tkey)
{
	CSI_PARAM_CHK_NORETVAL(tkey);
	
	//csp_tkey_t *tkey_base;
    //tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	
	csi_irq_disable((uint32_t)tkey->dev.irq_num);
    csi_irq_detach((uint32_t)tkey->dev.irq_num);
	
}
/** \brief  register tkey interrupt callback function
 * 
 *  \param[in] tkey: Handle of tkey instance
 *  \param[in] callback: tkey interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_tkey_attach_callback(csi_tkey_t *tkey, void *callback, void *arg)
{
    CSI_PARAM_CHK(tkey, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
    tkey->callback = callback;
    tkey->arg = arg;
	
	csi_irq_attach((uint32_t)tkey->dev.irq_num, &apt_tkey_irqhandler,&tkey->dev);
    csi_irq_enable((uint32_t)tkey->dev.irq_num);
	
    return ret;
}
/** \brief cancel tkey callback function
 * 
 *  \param[in] tkey: Handle of tkey instance
 *  \return none
 */ 
void csi_tkey_detach_callback(csi_tkey_t *tkey)
{
	CSI_PARAM_CHK_NORETVAL(tkey);
    
	tkey->callback 	= NULL;
    tkey->arg 		= NULL;
 	
	csi_irq_disable((uint32_t)tkey->dev.irq_num);
    csi_irq_detach((uint32_t)tkey->dev.irq_num);
}


/** \brief tkey interrupt enable/disable
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \param[in] eNewState: ENABLE, DISABLE
 *  \param[in] dwTkeyIMSCRX: interrupt number
 *  \return none
 */
void csi_tkey_set_intr(csi_tkey_t *tkey,bool bNewState,uint32_t wTkeyIMSCRX)
{
	csp_tkey_t *tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	csp_tkey_configinterrupt_cmd(tkey_base,bNewState,wTkeyIMSCRX);
	
}
/** \brief tkey interrupt enable/disable
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \param[in] dwThr: thr value
 *  \return none
 */
void csi_tkey_set_intcsp_tkey_set_thr(csi_tkey_t *tkey, uint32_t wThr)
{
	csp_tkey_t *tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	csp_tkey_set_thr(tkey_base,wThr);
	
}

//apt user
//#define TKEY_UART_DEBUG  			//uart receive dynamic length


#ifdef TKEY_UART_DEBUG
extern csi_uart_t	g_tUart;
#endif

/****************************************************
//TK test main
*****************************************************/
/** \brief tkey test main
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \return none
 */
void csi_tkey_prgm(csi_tkey_t *tkey)
{
	
	csp_tkey_t *tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	csp_tkey_prgm(tkey_base);
	
#ifdef TKEY_UART_DEBUG
	int i=0;
	csi_uart_putc(&g_tUart,0X0D);
	csi_uart_putc(&g_tUart,0X0A);
	for (i=0;i<=16;i++)
	{				
		csi_uart_putc(&g_tUart,hwSamplData0[i]>>8);
		csi_uart_putc(&g_tUart,hwSamplData0[i]&0xff);
		//csi_uart_putc(&g_tUart,hwBaselineData0[i]>>8);
		//csi_uart_putc(&g_tUart,hwBaselineData0[i]&0xff);
		//csi_uart_putc(&g_tUart,hwSamplData1[i]>>8);
		//csi_uart_putc(&g_tUart,hwSamplData1[i]&0xff);
		//csi_uart_putc(&g_tUart,hwBaselineData1[i]>>8);
		//csi_uart_putc(&g_tUart,hwBaselineData1[i]&0xff);
		//csi_uart_putc(&g_tUart,hwSamplData2[i]>>8);
		//csi_uart_putc(&g_tUart,hwSamplData2[i]&0xff);
		//csi_uart_putc(&g_tUart,hwBaselineData2[i]>>8);
		//csi_uart_putc(&g_tUart,hwBaselineData2[i]&0xff);
		
	}
#endif

}

/** \brief TKey sampling program
 *
 *  \param[in] tkey: tkey handle to operate
 *  \return none
 */
void csi_tkey_baseline_prog(csi_tkey_t *tkey)
{
	csp_tkey_t *tkey_base = (csp_tkey_t *)HANDLE_REG_BASE(tkey);
	csp_tkey_baseline_prog(tkey_base);
}














