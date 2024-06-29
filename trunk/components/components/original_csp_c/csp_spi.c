/***********************************************************************//** 
 * \file  csp_spi.c
 * \brief  SPI description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-27 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include "csp_spi.h"
#include "csp_common.h"


/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief spi reg default init
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return none
 */ 
void csp_spi_default_init(csp_spi_t *ptSpiBase)
{
	ptSpiBase->CR0	 = SPI_CR0_RST;
    ptSpiBase->CR1   = SPI_CR1_RST;
    //ptSpiBase->DR    = SPI_DR_RST;
    //ptSpiBase->SR    = SPI_SR_RST;
    ptSpiBase->CPSR  = SPI_CPSR_RST;
    ptSpiBase->IMSCR = SPI_IMSCR_RST;
   //ptSpiBase->RISR  = SPI_RISR_RST;
    //ptSpiBase->MISR  = SPI_MISR_RST;
    ptSpiBase->ICR	 = SPI_ICR_RST; 
}
/** \brief spi interrupt config
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] eSpiInt: int num 
 *  \param[in] bEnable: enable,disable
 *  \return none
 */ 
void csp_spi_set_int(csp_spi_t *ptSpiBase,spi_int_e eSpiInt,bool bEnable)
{
	if(bEnable)
		ptSpiBase->IMSCR |= eSpiInt;
	else
		ptSpiBase->IMSCR &= ~eSpiInt;

}
/** \brief spi speed set
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] wScr: serial clock Rate, SPI_SCR(wScr)
 *  \param[in] wCpsdvsr: clock prescale devisor, SPI_CPSDVSR(wCpsdvsr)
 *  \return none 
 */ 
void csp_spi_set_clk_div(csp_spi_t *ptSpiBase,uint8_t wScr,uint8_t wCpsdvsr)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & (~SPI_SCR_MSK)) | SPI_SCR(wScr);
	ptSpiBase->CPSR  = wCpsdvsr;	
}
/** \brief spi master mode config
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] eSize: data bit size 
 *  \param[in] eSpoH: spiclk polarity and spiclk Phase 
 *  \param[in] eLbm: loopback mode 	
 *  \param[in] eRxfifo: rx int FIFO level 
 *  \return none
 */ 
void csp_spi_set_master(csp_spi_t *ptSpiBase,spi_data_size_e eSize,spi_spo_h_e eSpoH, spi_rxifl_e eRxfifo, spi_lbm_e eLbm)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & ~(SPI_DSS_MSK | SPI_SPO_H_MSK)) | (eSize << SPI_DSS_POS) | (eSpoH << SPI_SPO_H_POS);
	ptSpiBase->CR1 = (ptSpiBase->CR1 & ~(SPI_LBM_MSK | SPI_MODE_MSK | SPI_RXIFL_MSK)) | (eLbm << SPI_LBM_POS) | (eRxfifo << SPI_RXIFL_POS);
}
/** \brief spi slave mode config
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] eSize: data bit size 
 *  \param[in] eSph: spiclk Phase 
 *  \param[in] eRxfifo: rx int FIFO level 
 *  \param[in] eSod: serial clock Rate
 *  \return none
 */ 
void csp_spi_set_slave(csp_spi_t *ptSpiBase,spi_data_size_e eSize,spi_spo_h_e eSph, spi_rxifl_e eRxfifo, spi_sod_e eSod)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & ~(SPI_DSS_MSK | SPI_SPO_H_MSK)) | (eSize << SPI_DSS_POS) | (eSph << SPI_SPO_H_POS);
	ptSpiBase->CR1 = (ptSpiBase->CR1 & ~(SPI_MODE_MSK | SPI_SOD_MSK | SPI_RXIFL_MSK)) | (SPI_MODE_SLAVE << SPI_MODE_POS) | 
					(eSod << SPI_SOD_POS) | (eRxfifo << SPI_RXIFL_POS);
}
/** \brief spi read ready?
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return ready, TRUE; no ready, FALSE
 */ 
uint8_t csp_spi_read_ready(csp_spi_t *ptSpiBase)
{
	if(csp_spi_get_sr(ptSpiBase) & SPI_RNE)
		return TRUE;			//no empty
	else
		return FALSE;			//empty
}
/** \brief spi write ready?
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return ready, TRUE; no ready, FALSE
 */ 
uint8_t csp_spi_write_ready(csp_spi_t *ptSpiBase)
{	
//	volatile uint32_t wTnf = csp_spi_get_sr(ptSpiBase);
//	wTnf &= SPI_TNF;
//	if(wTnf)
	if(csp_spi_get_sr(ptSpiBase) & SPI_TNF)
		return TRUE;			//no full
	else
		return FALSE;			//full
}
/** \brief spi busy?
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return busy, TRUE; idle, FALSE
 */ 
uint8_t csp_spi_busy(csp_spi_t *ptSpiBase)
{
	if(csp_spi_get_sr(ptSpiBase) & SPI_BSY)
		return TRUE;			//busy
	else
		return FALSE;			//idle
}
/** \brief spi write byte
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] hwData: data of write
 *  \return none
 */ 
void csp_spi_write_byte(csp_spi_t *ptSpiBase,uint16_t hwData)
{
	while(!csp_spi_write_ready(ptSpiBase));			//fifo full? wait; 
	
	csp_spi_set_data(ptSpiBase,hwData);				//send data
	
	while(csp_spi_busy(ptSpiBase));					//wait spi idle(transmition finish)
}
/** \brief spi read byte
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] hwWdata: data of write
 *  \return data of read
 */ 
uint16_t csp_spi_read_byte(csp_spi_t *ptSpiBase,uint16_t hwWdata)
{
	uint16_t hwRdata;
	
	while(!csp_spi_write_ready(ptSpiBase));			//send fifo full? wait; 
	csp_spi_set_data(ptSpiBase,hwWdata);			//send data
	
	while(csp_spi_busy(ptSpiBase));					//wait spi idle(send or receive finish)
	hwRdata = csp_spi_get_data(ptSpiBase);			//recv data

	return hwRdata;
}

/** \brief spi read write byte
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] hwWdata: data of write
 *  \return data of read
 */ 
int16_t csp_spi_read_write_byte(csp_spi_t *ptSpiBase,uint16_t hwWdata)
{
	uint16_t hwRdata;
	
#ifdef	SPI_TIMEOUT_DIS								//disable spi timeout	
	while(!csp_spi_write_ready(ptSpiBase));			//send fifo full? wait; 
	csp_spi_set_data(ptSpiBase,hwWdata);			//send data
	
	while(csp_spi_busy(ptSpiBase));					//wait spi idle(send or receive finish)
	//while(!csp_spi_read_ready(ptSpiBase));			//read fifo full? wait; 
	hwRdata = csp_spi_get_data(ptSpiBase);			//recv data
	
	
#else
	uint16_t hwTimeOut = 0;
	while(!csp_spi_write_ready(ptSpiBase))			//send fifo full? wait; 
	{
		hwTimeOut ++;
		if(hwTimeOut > SPI_RXTX_TIMEOUT)
			return -1;
	}
	csp_spi_set_data(ptSpiBase,hwWdata);			//send data
	hwTimeOut = 0;
	
	while(csp_spi_busy(ptSpiBase))					//wait spi idle(send or receive finish)
	//while(!csp_spi_read_ready(ptSpiBase))			//read fifo full? wait;  
	{
		hwTimeOut ++;
		if(hwTimeOut > SPI_RXTX_TIMEOUT)
			return -1;
	}
	hwRdata = csp_spi_get_data(ptSpiBase);			//recv data
#endif

	return hwRdata;
}
/** \brief send data of spi by DMA
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] pbySend: pointer to buffer data of SPI transmitter.
 *  \param[in] wSize: number of data to send (byte).
 *  \param[in] byDmaChnl: channel of DMA(0 -> 3)
 *  \return  none
 */
//void csp_spi_send_dma(csp_spi_t *ptSpiBase, const void *pbySend, uint32_t wSize, uint8_t byDmaChnl)
//{
//	csp_dma_ch_config_t tConfig;
//	
//	//dma work mode
//	tConfig.eSrcLinc 	= DMA_LINC_INC;
//	tConfig.eSrcHinc 	= DMA_HINC_CONST;
//	tConfig.eDetLinc 	= DMA_LINC_CONST;
//	tConfig.eDetHinc 	= DMA_HINC_CONST;
//	tConfig.eDsizeWidth = DMA_DSIZE_BY;
//	tConfig.eReloadEn 	= DMA_RELOAD_DIS;
//	tConfig.eTranMode 	= DMA_SMODE_ONCE;
//	tConfig.eTsizeMult	= DMA_TSIZE_NOR;
//	csp_dma_ch_config(DMA, byDmaChnl, &tConfig);		
//	
//	//etcb dma channel 
//	//csp_pcer0_clk_en(SYSCON, ETCB_SYS_CLK);							
//	//wj_etb_enable(ETCB);
//	wj_etb_ch_one_trig_one_config(ETCB, (WJ_ETB_DMA_CH0 + byDmaChnl - 3), WJ_ETB_SPI0_TXSRC, WJ_ETB_DMA_SYNCCH0 + byDmaChnl, 0);
//	wj_etb_ch_dma_en(ETCB, WJ_ETB_DMA_CH0 + byDmaChnl);
//	
//	csp_spi_set_txdma(ptSpiBase, SPI_TDMA_EN, SPI_TDMA_FIFO_NFULL);
//	csp_spi_en(ptSpiBase);			//enable spi
//	csp_dma_ch_start(DMA, DMA_REQ_HW, byDmaChnl, (void *)pbySend, (void *)&(ptSpiBase->DR) , wSize);
//
//}
/** \brief receive data of spi by DMA
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] pbyRecv: pointer to buffer data of SPI receive.
 *  \param[in] wSize: number of data to receive (byte).
 *  \param[in] byDmaChnl: channel of DMA(0 -> 3)
 *  \return  error code \ref csi_error_t
 */
//void csp_spi_recv_dma(csp_spi_t *ptSpiBase, void *pbyRecv, uint32_t wSize, uint8_t byDmaChnl)
//{
//	csp_dma_ch_config_t tConfig;
//	
//	tConfig.eSrcLinc 	= DMA_LINC_CONST;
//	tConfig.eSrcHinc 	= DMA_HINC_CONST;
//	tConfig.eDetLinc 	= DMA_LINC_INC;
//	tConfig.eDetHinc 	= DMA_HINC_INC;
//	tConfig.eDsizeWidth = DMA_DSIZE_BY;
//	tConfig.eReloadEn 	= DMA_RELOAD_DIS;
//	tConfig.eTranMode 	= DMA_SMODE_ONCE;
//	tConfig.eTsizeMult	= DMA_TSIZE_NOR;
//	csp_dma_ch_config(DMA, byDmaChnl, &tConfig);			//dma work mode
//	
//	//etcb dma channel
//	//csp_pcer0_clk_en(SYSCON, ETCB_SYS_CLK);							
//	//wj_etb_enable(ETCB);
//	wj_etb_ch_one_trig_one_config(ETCB, (WJ_ETB_DMA_CH0 + byDmaChnl - 3), WJ_ETB_SPI0_RXSRC, WJ_ETB_DMA_SYNCCH0 + byDmaChnl, 0);
//	wj_etb_ch_dma_en(ETCB, WJ_ETB_DMA_CH0 + byDmaChnl);
//	
//	csp_spi_set_rxdma(ptSpiBase, SPI_RDMA_EN, SPI_RDMA_FIFO_NSPACE);
//	csp_spi_en(ptSpiBase);			//enable spi
//	csp_dma_ch_start(DMA, DMA_REQ_HW, byDmaChnl,(void *)&(ptSpiBase->DR), (uint32_t *)pbyRecv, wSize);	
//}

/** \brief send and receive data of spi by DMA
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \param[in] pbySend: pointer to buffer data of SPI send.
 *  \param[in] pbyRecv: pointer to buffer data of SPI receive.
 *  \param[in] wSize: number of data to receive (byte).
 *  \param[in] byDmaChnl: channel of DMA(0 -> 3)
 *  \return  error code \ref csi_error_t
 */
//void csp_spi_send_recv_dma(csp_spi_t *ptSpiBase, const void *pbySend, void *pbyRecv, uint32_t wSize, uint8_t byDmaChnl)
//{
//	csp_dma_ch_config_t tConfig0, tConfig1;
//	
//	//dma work mode, spi receive
//	tConfig0.eSrcLinc 	= DMA_LINC_CONST;
//	tConfig0.eSrcHinc 	= DMA_HINC_CONST;
//	tConfig0.eDetLinc 	= DMA_LINC_INC;
//	tConfig0.eDetHinc 	= DMA_HINC_INC;
//	tConfig0.eDsizeWidth = DMA_DSIZE_BY;
//	tConfig0.eReloadEn 	= DMA_RELOAD_DIS;
//	tConfig0.eTranMode 	= DMA_SMODE_ONCE;
//	tConfig0.eTsizeMult	= DMA_TSIZE_NOR;
//	
//	//dma work mode, spi send
//	tConfig1.eSrcLinc 	= DMA_LINC_INC;
//	tConfig1.eSrcHinc 	= DMA_HINC_CONST;
//	tConfig1.eDetLinc 	= DMA_LINC_CONST;
//	tConfig1.eDetHinc 	= DMA_HINC_CONST;
//	tConfig1.eDsizeWidth = DMA_DSIZE_BY;
//	tConfig1.eReloadEn 	= DMA_RELOAD_DIS;
//	tConfig1.eTranMode 	= DMA_SMODE_ONCE;
//	tConfig1.eTsizeMult	= DMA_TSIZE_NOR;
//	csp_dma_ch_config(DMA, byDmaChnl, &tConfig0);			//dma work mode
//	csp_dma_ch_config(DMA, byDmaChnl+1, &tConfig1);	
//	
//	//etcb dma channel , receive
//	//csp_pcer0_clk_en(SYSCON, ETCB_SYS_CLK);							
//	//wj_etb_enable(ETCB);
//	wj_etb_ch_one_trig_one_config(ETCB, (WJ_ETB_DMA_CH0 + byDmaChnl - 3), WJ_ETB_SPI0_TXSRC, WJ_ETB_DMA_SYNCCH0 + byDmaChnl, 0);
//	wj_etb_ch_dma_en(ETCB, WJ_ETB_DMA_CH0 + byDmaChnl);
//	
//	//etcb dma channel, send
//	wj_etb_ch_one_trig_one_config(ETCB, (WJ_ETB_DMA_CH1 + byDmaChnl - 3), WJ_ETB_SPI0_RXSRC, WJ_ETB_DMA_SYNCCH1 + byDmaChnl, 0);
//	wj_etb_ch_dma_en(ETCB, WJ_ETB_DMA_CH1 + byDmaChnl);
//	
//	csp_spi_set_rxdma(ptSpiBase, SPI_RDMA_EN, SPI_RDMA_FIFO_NSPACE);
//	csp_spi_set_txdma(ptSpiBase, SPI_TDMA_EN, SPI_TDMA_FIFO_NFULL);
//	csp_spi_en(ptSpiBase);			//enable spi
//	csp_dma_ch_start(DMA, DMA_REQ_HW, byDmaChnl,(void *)&(ptSpiBase->DR), (uint32_t *)pbyRecv, wSize);		//reveive 
//	csp_dma_ch_start(DMA, DMA_REQ_HW, byDmaChnl+1, (void *)pbySend, (void *)&(ptSpiBase->DR) , wSize);		//send
//}