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
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>
#include "csp_sio.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief sio receive data,interrupt call 
 * 
 *  \param[in] sio: pointer of SIO reg structure.
 *  \return none
 */ 
static void sio_intr_recv_data(csi_sio_t *sio)
{
	csp_sio_t *sio_base  = (csp_sio_t *)HANDLE_REG_BASE(sio); 
	
	if((sio->data == NULL) || (sio->size == 0U))
	{
		csp_sio_get_rxbuf(sio_base);
		if(sio->callback) 
			sio->callback(sio, SIO_EVENT_ERROR, sio->arg);	//error
	}
	else
	{
		*sio->data = csp_sio_get_rxbuf(sio_base);
		sio->size --;
		sio->data++ ;
		
		if(sio->size == 0)
		{
			if(sio->callback) 
				sio->callback(sio, SIO_EVENT_RECEIVE_COMPLETE, sio->arg);	
		}
	}
}
/** \brief sio interrupt handle 
 * 
 *  \param[in] args: pointer of sio dev
 *  \return none
 */
void apt_sio_irqhandler(void *args)
{
	csi_sio_t *sio = (csi_sio_t *)args;
    csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	volatile uint32_t status = csp_sio_get_isr(sio_base);
	
	if(status & SIO_RXBUFFULL)
	{
		csp_sio_clr_isr(sio_base, SIO_RXBUFFULL);
		sio_intr_recv_data(sio);
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
	 
	if(status & SIO_TIMEOUT)
	{
		csp_sio_clr_isr(sio_base, SIO_TIMEOUT);
		if(sio->callback) 
			sio->callback(sio, SIO_EVENT_TIMEOUT, sio->arg);	//error
	}
}

/**
  \brief       Init sio ctrl block
               Initializes the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   idx    Index of instance
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_init(csi_sio_t *sio, uint32_t idx)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);

	csi_error_t ret = CSI_OK;
    csp_sio_t *sio_base;
	
	if (target_get(DEV_SIO_TAG, idx, &sio->dev) != CSI_OK)
		ret = CSI_ERROR;
	else
	{	
		sio->size = 0U;
		sio->data = NULL;
		sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
		
		csi_clk_enable(&sio->dev);				//sio peripheral clk en
		csp_sio_def_Init(sio_base); 			//reset sio registers
		csp_sio_clk_en(sio_base);				//enable clk
		
		//csp_sio_set_mode(sio_base,SIO_MODE_TX);	//ensale sio tx
	}
	
	return ret;
}
/** \brief de-initialize sio interface
 * 
 *  \param[in] sio: Handle of sio instance
 *  \return none
 */ 
void csi_sio_uninit(csi_sio_t *sio)
{
	CSI_PARAM_CHK_NORETVAL(sio);
	
	csp_sio_t *sio_base;
    sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	sio->size = 0U;
	sio->data = NULL;
	
	csp_sio_def_Init(sio_base);
	csi_irq_disable((uint32_t)sio->dev.irq_num);
    csi_irq_detach((uint32_t)sio->dev.irq_num);
}
/** \brief  register sio interrupt callback function
 * 
 *  \param[in] sio: Handle of sio instance
 *  \param[in] callback: sio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_sio_attach_callback(csi_sio_t *sio, void *callback, void *arg)
{
    CSI_PARAM_CHK(sio, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
    sio->callback = callback;
    sio->arg = arg;
	
	csi_irq_attach((uint32_t)sio->dev.irq_num, &apt_sio_irqhandler, &sio->dev);
    csi_irq_enable((uint32_t)sio->dev.irq_num);
	
    return ret;
}
/** \brief cancel sio callback function
 * 
 *  \param[in] sio: Handle of sio instance
 *  \return none
 */ 
void csi_sio_detach_callback(csi_sio_t *sio)
{
	CSI_PARAM_CHK_NORETVAL(sio);
    
	sio->callback 	= NULL;
    sio->arg 		= NULL;
 	
	csi_irq_disable((uint32_t)sio->dev.irq_num);
    csi_irq_detach((uint32_t)sio->dev.irq_num);
}

/**
  \brief       Config sio tx kprs
  \param[in]   sio          Handle of sio instance
  \param[in]   tx_freq    	sio tx bit freq 
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_freq(csi_sio_t *sio, uint32_t tx_freq)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	if(tx_freq == 0)
		return CSI_ERROR;
	
	uint32_t cld_div = (soc_get_pclk_freq()/tx_freq - 1);
	csp_sio_set_tx_clkdiv(sio_base, cld_div);
	
	return ret;
}
/**
  \brief       Config sio tx kprs
  \param[in]   sio          Handle of sio instance
  \param[in]   rx_freq    	sio rx bit freq 
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_freq(csi_sio_t *sio, uint32_t rx_freq)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base;
	sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	if(rx_freq == 0)
		return CSI_ERROR;
	
	uint32_t cld_div = (soc_get_pclk_freq()/rx_freq - 1);
	csp_sio_set_rx_clkdiv(sio_base, cld_div);
	
	return ret;
}
/**
  \brief       configure sio tx
               configure the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   sio_tx    Pointer to siotx struct\ref csi_sio_tx_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_init(csi_sio_t *sio, csi_sio_tx_t *sio_tx)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	CSI_PARAM_CHK(sio_tx, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	csp_sio_set_mode(sio_base,SIO_MODE_TX);		//ensale sio tx
	
	csp_sio_set_tx(sio_base,sio_tx->idlest,sio_tx->txdir, sio_tx->txbuflen - 1, sio_tx->txcnt - 1);
	csp_sio_set_d0(sio_base,sio_tx->txdata.d0_len - 1);
	csp_sio_set_d1(sio_base,sio_tx->txdata.d1_len - 1);
	csp_sio_set_dh(sio_base,sio_tx->txdata.dh_len - 1, sio_tx->txdata.dh_hsq);
	csp_sio_set_dl(sio_base,sio_tx->txdata.dl_len - 1, sio_tx->txdata.dl_lsq);
	
	//sio->priv = (void *)sio_tx->txcnt;			
	//csp_sio_set_int(sio_base,SIO_TXDNE|SIO_TXBUFEMPT,true);
	//csp_sio_vic_irq_en();
	
	return ret;
}
/**
  \brief       configure sio rx
               configure the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   sio_rx    Pointer to siorx struct\ref csi_sio_rx_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_init(csi_sio_t *sio, csi_sio_rx_t *sio_rx)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	CSI_PARAM_CHK(sio_rx, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	csp_sio_set_mode(sio_base, SIO_MODE_RX);										//sio rx mode
	csp_sio_set_rxdeb(sio_base, sio_rx->rxdeb.deb_depth, sio_rx->rxdeb.deb_ckdiv);	//rx debounce filter
	csp_sio_set_sample_mode(sio_base, sio_rx->rxsample.bstsel, sio_rx->rxsample.trgmode, sio_rx->rxsample.rxmode);
	csp_sio_set_sample(sio_base, sio_rx->rxsample.extract, sio_rx->rxsample.alignen, sio_rx->rxsample.splcnt -1, sio_rx->rxsample.hithr -1);
	csp_sio_set_rx(sio_base, sio_rx->rxsample.rxdir, sio_rx->rxsample.rxbuflen - 1, sio_rx->rxsample.rxlen - 1);
	csp_sio_set_break(sio_base, sio_rx->rstdete.break_en, sio_rx->rstdete.break_lvl, sio_rx->rstdete.break_cnt);		//rx break set 
	csp_sio_set_torst(sio_base, sio_rx->torst.torst_en, sio_rx->torst.to_cnt);										//rx timeover set
	
	return ret;
}
/**
  \brief       send txbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   val    txbuf value
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_send(csi_sio_t *sio, const void *data, uint32_t size)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	uint32_t *pwSend = (uint32_t *)data;
	int32_t wTransNum = 0 , j = SIO_TX_TIMEOUT ;
	
	wTransNum = csp_sio_send(sio_base, pwSend, size);
	
	while(!(csp_sio_get_risr(sio_base) & SIO_TXDNE) && j--);
	
	if(j == 0)
		return CSI_TIMEOUT;
		
	return wTransNum;
}

/**
  \brief       receive data to sio transmitter, asynchronism mode
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_receive_async(csi_sio_t *sio, void *data, uint32_t size)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	
	sio->data = (uint32_t *)data;
	sio->size = size;
	
	csp_sio_set_int(sio_base, SIO_RXBUFFULL | SIO_TIMEOUT, true);
	
	return ret;
}

/**
  \brief       receive rxbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_receive(csi_sio_t *sio, uint32_t *data)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	CSI_PARAM_CHK(data, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_sio_t *sio_base;
	
	sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	*data = csp_sio_get_rxbuf(sio_base);
	
	return ret;
}


/**
  \brief       get misr
  \param[in]   sio    Handle of sio instance
  \return      sio->misr 
*/
uint32_t csi_sio_get_isr(csi_sio_t *sio)
{
	CSI_PARAM_CHK(sio, CSI_ERROR);
	
	uint32_t ret = CSI_OK;
	csp_sio_t *sio_base;
	
	sio_base = (csp_sio_t *)HANDLE_REG_BASE(sio);
	ret = csp_sio_get_isr(sio_base);
	
	return ret;
}