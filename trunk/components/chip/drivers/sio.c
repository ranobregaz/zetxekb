/***********************************************************************//** 
 * \file  sio.c
 * \brief  csi sio driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-03 <td>V0.0  <td>XB   <td>initial
 * <tr><td> 2021-1-03 <td>V0.0  <td>ZJY   <td>modify
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/sio.h>
#include <drv/irq.h>
#include <drv/pin.h>
#include <drv/tick.h>

/* Private macro------------------------------------------------------*/
#define SIO_RESET_VALUE  (0x00000000)
#define SIO_RX_TIMEOUT		(0x10ff)
#define SIO_TX_TIMEOUT		(0x1FFF)
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/
csi_sio_trans_t g_tSioTran;	


/** \brief sio interrupt handle 
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \return none
 */
void apt_sio_irqhandler(csp_sio_t *ptSioBase)
{
	volatile uint32_t wStatus = csp_sio_get_isr(ptSioBase) & 0x2a;
	
	switch(wStatus)
	{
		case SIO_RXBUFFULL:										
		case SIO_RXDNE:
		
			csp_sio_clr_isr(ptSioBase, SIO_RXDNE | SIO_RXBUFFULL);
			if(NULL == g_tSioTran.pwData || 0 == g_tSioTran.hwSize)
			{
				csp_sio_get_rxbuf(ptSioBase);
				g_tSioTran.byRxStat = SIO_STATE_ERROR;					//receive error
			}
			else
			{
				*g_tSioTran.pwData = csp_sio_get_rxbuf(ptSioBase);		//receive data
				g_tSioTran.hwSize --;
				g_tSioTran.pwData ++;
				g_tSioTran.hwTranLen ++;
				
				if(0 == g_tSioTran.hwSize)						
				{
					g_tSioTran.byRxStat = SIO_STATE_FULL;				//receive complete
				}
			}
			break;
		case SIO_TIMEOUT:
			csp_sio_clr_isr(ptSioBase, SIO_TIMEOUT);
			break;
		case SIO_BREAK:
			csp_sio_clr_isr(ptSioBase, SIO_BREAK);
			break;
		default:
			break;
	}
}
/** \brief init sio register init 
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \return none
 */
static void apt_sio_default_init(csp_sio_t *ptSioBase)
{
	ptSioBase->CR 		= SIO_RESET_VALUE;
	ptSioBase->TXCR0 	= SIO_RESET_VALUE;
	ptSioBase->TXCR1 	= SIO_RESET_VALUE;
	ptSioBase->RXCR0 	= SIO_RESET_VALUE;
	ptSioBase->RXCR1 	= SIO_RESET_VALUE;
	ptSioBase->RXCR2 	= SIO_RESET_VALUE;
	ptSioBase->IMCR 	= SIO_RESET_VALUE;
	ptSioBase->ICR 		= SIO_RESET_VALUE;
}
/** \brief Init sio tx, Initializes the resources needed for the sio instance 
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \param[in] ptTxCfg: pointer of sio parameter config structure
 *  \return error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_init(csp_sio_t *ptSioBase, csi_sio_tx_config_t *ptTxCfg)
{
	uint16_t hwClkDiv;
	
	csi_clk_enable((uint32_t *)ptSioBase);		//sio peripheral clk en
	apt_sio_default_init(ptSioBase);			//sio reg default
	csp_sio_clk_en(ptSioBase);					//enable clk
	csp_sio_set_mode(ptSioBase, SIO_MODE_TX);	//sio tx mode
	
	if(ptTxCfg->wTxFreq > 1)					//tx clk config
	{
		hwClkDiv = soc_get_pclk_freq()/ptTxCfg->wTxFreq - 1;
		csp_sio_set_tx_clkdiv(ptSioBase, hwClkDiv);
	}
	else
		return CSI_ERROR;
	
	//set TX CR0~1 reg
	csp_sio_set_txcr0(ptSioBase, ptTxCfg->byIdleLev, ptTxCfg->byTxDir, ptTxCfg->byTxBufLen - 1, ptTxCfg->byTxCnt - 1);	
	csp_sio_set_d0(ptSioBase, ptTxCfg->byD0Len - 1);					//set d0 clk len
	csp_sio_set_d1(ptSioBase, ptTxCfg->byD1Len - 1);					//set d1 clk len
	csp_sio_set_dl(ptSioBase, ptTxCfg->byDLLen - 1, ptTxCfg->byDLLsq);	//set dl clk len and lsq
	csp_sio_set_dh(ptSioBase, ptTxCfg->byDHLen - 1, ptTxCfg->byDHHsq);	//set dl clk len and hsq
	
	if(ptTxCfg->byInter)	
	{	
		g_tSioTran.bySendMode = SIO_TX_MODE_INT;						//interrupt
		csi_irq_enable((uint32_t*)ptSioBase);							//enable sio irq 
	}
	else
		g_tSioTran.bySendMode = SIO_TX_MODE_POLL;						//polling mode

	return CSI_OK;
}

/** \brief Init sio rx, Initializes the resources needed for the sio instance 
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \param[in] ptRxCfg: pointer of sio parameter config structure
 *  \return error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_init(csp_sio_t *ptSioBase, csi_sio_rx_config_t *ptRxCfg)
{
	uint16_t hwClkDiv;
	
	csi_clk_enable((uint32_t *)ptSioBase);		//sio peripheral clk en
	apt_sio_default_init(ptSioBase);			//sio reg default
	csp_sio_clk_en(ptSioBase);					//enable clk
	csp_sio_set_mode(ptSioBase, SIO_MODE_RX);	//sio rx mode
	
	if(ptRxCfg->wRxFreq > 1)					//tx clk config
	{
		hwClkDiv = soc_get_pclk_freq()/ptRxCfg->wRxFreq - 1;
		csp_sio_set_rx_clkdiv(ptSioBase, hwClkDiv);
	}
	else
		return CSI_ERROR;
	
	//rx receive config
	csp_sio_set_rx_deb(ptSioBase, ptRxCfg->byDebPerLen - 1, ptRxCfg->byDebClkDiv - 1);								//set rx sampling debounce
	csp_sio_set_sample_mode(ptSioBase, ptRxCfg->byTrgEdge, ptRxCfg->byTrgMode, ptRxCfg->bySpMode);					//set rx samping mode
	csp_sio_set_sample(ptSioBase, ptRxCfg->bySpExtra, SIO_ALIGN_EN, ptRxCfg->bySpBitLen - 1, ptRxCfg->byHithr);		//set rx samping control
	csp_sio_set_recv(ptSioBase, ptRxCfg->byRxDir, ptRxCfg->byRxBufLen - 1, ptRxCfg->byRxCnt - 1);					//set receive para
	
	if(ptRxCfg->byInter)	
	{
		if(ptRxCfg->byInter & SIO_INTSRC_RXDNE)							//RXDONE  interrupt				
		{
			if(ptRxCfg->byRxCnt > 32)									//byRxCnt > 32 ,the mode work error
				return CSI_ERROR;
		}
		csp_sio_int_enable(ptSioBase, ptRxCfg->byInter, ENABLE);		//enable sio interrupt
		csi_irq_enable((uint32_t*)ptSioBase);							//enable sio irq 
	}

	return CSI_OK;
}
/** \brief sio receive break reset config
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \param[in] eBkLev: break reset detect level
 *  \param[in] byBkCnt: break reset detect period
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
*/
void csi_sio_break_rst(csp_sio_t *ptSioBase, csi_sio_bklev_e eBkLev, uint8_t byBkCnt, bool bEnable)
{
	csp_sio_set_break(ptSioBase, bEnable, eBkLev, byBkCnt);
}

/** \brief sio receive timeout reset config
 * 
 *  \param[in] ptSioBase: pointer of sio register structure
 *  \param[in] byBkCnt: break reset detect period
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
*/
void csi_sio_timeout_rst(csp_sio_t *ptSioBase, uint8_t byToCnt ,bool bEnable)
{
	csp_sio_set_torst(ptSioBase, bEnable, byToCnt);
}
/** \brief send data from sio, this function is polling mode(sync mode)
 * 
 * \param[in] ptSioBase: pointer of sio register structure
 * \param[in] pwData: pointer to buffer with data to send 
 * \param[in] hwSize: send data size
 * \return error code \ref csi_error_t or receive data size
*/
int32_t csi_sio_send(csp_sio_t *ptSioBase, const uint32_t *pwData, uint16_t hwSize)
{
	uint16_t  i;

	switch(g_tSioTran.bySendMode)
	{
		case SIO_TX_MODE_POLL:											//sio send polling mode
		
			for(i = 0; i < hwSize; i++)									
			{
				csp_sio_set_txbuf(ptSioBase,pwData[i]);
				while(!(csp_sio_get_risr(ptSioBase) & SIO_TXBUFEMPT));	
			}
			while(!(csp_sio_get_risr(ptSioBase) & SIO_TXDNE));
			//csp_sio_clr_isr(ptSioBase, SIO_TXDNE);
			return i;
		case SIO_TX_MODE_INT:											//sio send interrupt mode, unsupport
			return CSI_UNSUPPORTED;
		default:
			return CSI_ERROR;
	}
}
/** \brief set sio receive data buffer and length
 * 
 *  \param[in] pwData: pointer of sio transport data buffer
 *  \param[in] hwLen: sio transport data length
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_sio_set_buffer(uint32_t *pwData, uint16_t hwLen)
{
	if(NULL == pwData || hwLen == 0)
		return CSI_ERROR;
	
	g_tSioTran.pwData 	 = pwData;
	g_tSioTran.hwSize 	 = hwLen;
	g_tSioTran.hwTranLen = 0;
	g_tSioTran.byRxStat  = SIO_STATE_IDLE;
	g_tSioTran.byTxStat  = SIO_STATE_IDLE;
	
	return CSI_OK;
}
/** \brief receive data to sio transmitter, asynchronism mode
 * 
 * \param[in] ptSioBase: pointer of sio register structure
 * \return receive data length
*/
uint16_t csi_sio_receive(csp_sio_t *ptSioBase)
{
	
	if(csi_sio_get_recv_status() == SIO_STATE_FULL)
		return g_tSioTran.hwTranLen;
	else
		return 0;
}
/** \brief get the status of sio receive 
 * 
 *  \param[in] sio: pointer of sio register structure
 *  \return the status of sio receive 
 */ 
csi_sio_state_e csi_sio_get_recv_status(void)
{
	return g_tSioTran.byRxStat;
}
/** \brief clr the status of sio receive 
 * 
 *  \param[in] sio: pointer of sio register structure
 *  \return none
 */ 
void csi_sio_clr_recv_status(void)
{
	g_tSioTran.byRxStat = SIO_STATE_IDLE;
}

/** \brief get the status of sio receive 
 * 
 *  \param[in] sio: pointer of sio register structure
 *  \return the status of sio receive 
 */ 
//csi_sio_state_e csi_sio_get_send_status(void)
//{
//	return g_tSioTran.byTxStat;
//}
/** \brief clr the status of sio receive 
 * 
 *  \param[in] sio: pointer of sio register structure
 *  \return none
 */ 
//void csi_sio_clr_send_status(void)
//{
//	g_tSioTran.byTxStat = SIO_STATE_IDLE;
//}