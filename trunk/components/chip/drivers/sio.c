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

/** \brief sio receive data,interrupt call 
 * 
 *  \param[in] sio: pointer of SIO reg structure.
 *  \return none
 */ 
//static void sio_intr_recv_data(csi_sio_t *sio)
//{
//	csp_sio_t *sio_base  = (csp_sio_t *)HANDLE_REG_BASE(sio); 
//	
//	if((sio->data == NULL) || (sio->size == 0U))
//	{
//		csp_sio_get_rxbuf(sio_base);
//		if(sio->callback) 
//			sio->callback(sio, SIO_EVENT_ERROR, sio->arg);	//error
//	}
//	else
//	{
//		*sio->data = csp_sio_get_rxbuf(sio_base);
//		sio->size --;
//		sio->data++ ;
//		
//		if(sio->size == 0)
//		{
//			if(sio->callback) 
//				sio->callback(sio, SIO_EVENT_RECEIVE_COMPLETE, sio->arg);	
//		}
//	}
//}
/** \brief sio interrupt handle 
 * 
 *  \param[in] args: pointer of sio dev
 *  \return none
 */
void apt_sio_irqhandler(csp_sio_t *ptSioBase)
{
//	csi_sio_t *sio = (csi_sio_t *)args;
//    csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	volatile uint32_t wStatus = csp_sio_get_isr(ptSioBase);
	
	if(wStatus & SIO_RXBUFFULL)
	{
		csp_sio_clr_isr(ptSioBase, SIO_RXBUFFULL);
		//sio_intr_recv_data(sio);
		
		if(NULL == g_tSioTran.pwData || 0 == g_tSioTran.bySize)
		{
			csp_sio_get_rxbuf(ptSioBase);
		}
		else
		{
			*g_tSioTran.pwData = csp_sio_get_rxbuf(ptSioBase);	//receive data
			g_tSioTran.bySize --;
			g_tSioTran.pwData ++;
			
			if(0 == g_tSioTran.bySize)						
			{
				g_tSioTran.byRxStat = SIO_STATE_FULL;			//receive complete
			}
		}
	}
	
	/*
	if(status & SIO_TXDNE)
	{
		csp_sio_clr_isr(sio_base, SIO_TXDNE);
	}
	
	if(status & SIO_RXDNE)
	{
		csp_sio_clr_isr(sio_base, SIO_RXDNE);
	}
	 */
	 
	if(wStatus & SIO_TIMEOUT)
	{
		csp_sio_clr_isr(ptSioBase, SIO_TIMEOUT);
	}
}

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
static void delay_ums(unsigned int t)
{
	volatile unsigned int i,j ,k=0;
	j = 20* t;
	for ( i = 0; i < j; i++ )
	{
		k++;
	}
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
	csp_sio_set_txcr0(ptSioBase, ptTxCfg->byIdleLev, ptTxCfg->byTxDir, ptTxCfg->byTxBufLen, ptTxCfg->byTxCnt);	
	csp_sio_set_d0(ptSioBase, ptTxCfg->byD0Len - 1);					//set d0 clk len
	csp_sio_set_d1(ptSioBase, ptTxCfg->byD1Len - 1);					//set d1 clk len
	csp_sio_set_dl(ptSioBase, ptTxCfg->byDLLen - 1, ptTxCfg->byDLLsq);	//set dl clk len and lsq
	csp_sio_set_dh(ptSioBase, ptTxCfg->byDHLen - 1, ptTxCfg->byDHHsq);	//set dl clk len and hsq
	
	if(ptTxCfg->byInter)	
	{
		csp_sio_int_enable(ptSioBase, ptTxCfg->byInter, ENABLE);		//enable sio interrupt
		csi_irq_enable((uint32_t*)ptSioBase);							//enable sio irq 
	}

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
	csp_sio_set_mode(ptSioBase, SIO_MODE_TX);	//sio tx mode
	
	if(ptRxCfg->wRxFreq > 1)					//tx clk config
	{
		hwClkDiv = soc_get_pclk_freq()/ptRxCfg->wRxFreq - 1;
		csp_sio_set_rx_clkdiv(ptSioBase, hwClkDiv);
	}
	else
		return CSI_ERROR;
	
	//rx receive config
	csp_sio_set_rx_deb(ptSioBase, ptRxCfg->byDebPerLen, ptRxCfg->byDebClkDiv);										//set rx sampling debounce
	csp_sio_set_sample_mode(ptSioBase, ptRxCfg->byTrgEdge, ptRxCfg->byTrgMode, ptRxCfg->bySpMode);					//set rx samping mode
	csp_sio_set_sample(ptSioBase, ptRxCfg->bySpExtra, SIO_ALIGN_EN, ptRxCfg->bySpBitCnt-1, ptRxCfg->byHithr);		//set rx samping control
	csp_sio_set_recv(ptSioBase, ptRxCfg->byRxDir, ptRxCfg->byRxBufLen, ptRxCfg->byRxCnt);							//set receive para
	
	//rx receive break and timeout config
	//csp_sio_set_break(ptSioBase,ptRxCfg->tRxRst.byBreakEn,ptRxCfg->tRxRst.byBreakLev,ptRxCfg->tRxRst.byBreakCnt);	//set rx break reset
	//csp_sio_set_torst(ptSioBase, ptRxCfg->tRxRst.byToCnt, ptRxCfg->tRxRst.byToRstCnt);							//set rx timeout reset
	
	if(ptRxCfg->byInter)	
	{
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
/**
  \brief       send txbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   val    txbuf value
  \return      error code \ref csi_error_t
*/
uint8_t csi_sio_send(csp_sio_t *ptSioBase, const void *pData, uint8_t bySize)
{
	uint8_t  i;
	uint32_t *pwSend = (uint32_t *)pData;
	uint32_t wTimeout = SIO_TX_TIMEOUT ;
	
	if(NULL == pData || 0 == bySize)
		return 0;
	
	for(i = 0; i < bySize; i++)
	{
		csp_sio_set_txbuf(ptSioBase,pwSend[i]);
		while(!(csp_sio_get_risr(ptSioBase) & SIO_TXBUFEMPT) && wTimeout --);
		if(0 == wTimeout)
			return i;
		delay_ums(1);
	}
	
	return i;
}
/** \brief set sio transport data buffer
 * 
 *  \param[in] pwData: pointer of sio transport data buffer
 *  \param[in] byLen: sio transport data length
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_sio_set_buffer(uint32_t *pwData, uint8_t byLen)
{
	if(NULL == pwData || byLen == 0)
		return CSI_ERROR;
	
	g_tSioTran.pwData = pwData;
	g_tSioTran.bySize = byLen;
	g_tSioTran.byRxStat = SIO_STATE_IDLE;
	g_tSioTran.byTxStat = SIO_STATE_IDLE;
	
	return CSI_OK;
}
/**
  \brief       receive data to sio transmitter, asynchronism mode
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
uint8_t csi_sio_receive_async(csp_sio_t *ptSioBase, void *pData)
{
	
	if(csi_sio_get_recv_status() == SIO_STATE_FULL)
	{
		csi_sio_clr_recv_status();
		memcpy(g_tSioTran.pwData, (uint32_t *)pData, g_tSioTran.byTranLen);
		return g_tSioTran.byTranLen;
	}
	else
		return 0;
}
/**
  \brief       receive rxbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_sio_receive(csi_sio_t *sio, uint32_t *data)
//{
//	CSI_PARAM_CHK(sio, CSI_ERROR);
//	CSI_PARAM_CHK(data, CSI_ERROR);
//	
//	csi_error_t ret = CSI_OK;
//	csp_sio_t *sio_base;
//	
//	sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
//	*data = csp_sio_get_rxbuf(sio_base);
//	
//	return ret;
//}
/**
  \brief       get misr
  \param[in]   sio    Handle of sio instance
  \return      sio->misr 
*/
//uint32_t csi_sio_get_isr(csi_sio_t *sio)
//{
//	CSI_PARAM_CHK(sio, CSI_ERROR);
//	
//	uint32_t ret = CSI_OK;
//	csp_sio_t *sio_base;
//	
//	sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
//	ret = csp_sio_get_isr(sio_base);
//	
//	return ret;
//}

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