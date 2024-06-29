/***********************************************************************//** 
 * \file  csp_sio.c
 * \brief  SIO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-18 <td>V0.1  <td>ZJY   <td>initial
 * <tr><td> 2021-1-4 <td>V0.1  <td>ZJY   <td>mofify
 * </table>
 * *********************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "csp_sio.h"
#include "csp_common.h"

/*************************************************************/
//SIO RESET CLEAR ALL REGISTER
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void csp_sio_def_Init(csp_sio_t *ptSioBase)
{
	ptSioBase->CR 		= SIO_RESET_VALUE;
	ptSioBase->TXCR0 	= SIO_RESET_VALUE;
	ptSioBase->TXCR1 	= SIO_RESET_VALUE;
	//ptSioBase->TXBUF 	= SIO_RESET_VALUE;
	ptSioBase->RXCR0 	= SIO_RESET_VALUE;
	ptSioBase->RXCR1 	= SIO_RESET_VALUE;
	ptSioBase->RXCR2 	= SIO_RESET_VALUE;
	ptSioBase->IMCR 	= SIO_RESET_VALUE;
	ptSioBase->ICR 		= SIO_RESET_VALUE;
}
/** \brief sio interrupt enable/disable
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eSioInt: int num
 *  \param[in] byEnable: enable/disable
 *  \return none
 */ 
void csp_sio_set_int(csp_sio_t *ptSioBase,sio_int_e eSioInt, uint8_t byEnable)
{
	if(byEnable)
		ptSioBase->IMCR |= eSioInt;
	else
		ptSioBase->IMCR &= ~eSioInt;

}
/** \brief set sio DH sequence
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eDhBit: DH sequence length
 *  \param[in] byDhSq: DH sequence
 *  \return none
 */ 
void csp_sio_set_dh(csp_sio_t *ptSioBase, sio_lenob_e eDhBit, uint8_t byDhSq)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1 & ~(SIO_LENOBH_MSK | SIO_HSQ_MSK));
	ptSioBase->TXCR1 |= (eDhBit << SIO_LENOBH_POS) | SIO_HSQ(byDhSq);
}
/** \brief set sio DL sequence
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eDhBit: DH sequence length
 *  \param[in] byDhSq: DH sequence
 *  \return none
 */ 
void csp_sio_set_dl(csp_sio_t *ptSioBase, sio_lenob_e eDlBit, uint8_t byDlSq)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1 & ~(SIO_LENOBL_MSK | SIO_LSQ_MSK));
	ptSioBase->TXCR1 |= (eDlBit << SIO_LENOBL_POS) | SIO_LSQ(byDlSq);
}
/** \brief sio tx init
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] tpSioTx: pointer of tx init parameter
 *  \return none
 */ 
void csp_sio_tx_init(csp_sio_t *ptSioBase, csp_sio_tx_t *tpSioTx)
{
	csp_sio_clk_en(ptSioBase);												//enable clk
	csp_sio_set_mode(ptSioBase, SIO_MODE_TX);								//sio tx mode
	csp_sio_set_tx_clkdiv(ptSioBase, HFOSC_48M_VALUE/tpSioTx->wTxclk);		//Ftxclk = 25000,t=40us
	csp_sio_set_tx(ptSioBase,tpSioTx->eIdlest, tpSioTx->eTdir, tpSioTx->byTxLen - 1, tpSioTx->byTxCnt - 1);	//set TXCR0
	csp_sio_set_d0(ptSioBase, (tpSioTx->byD0Ticks - 1));					//D0 length = byD0Tick*Ftxclk 
	csp_sio_set_d1(ptSioBase, (tpSioTx->byD1Ticks - 1));					//D1 length = byD0Tick*Ftxclk
	csp_sio_set_dh(ptSioBase, (tpSioTx->byDHTicks - 1), 0b00011110);			//DH length = byDHTick*Ftxclk
	csp_sio_set_dl(ptSioBase, (tpSioTx->byDLTicks - 1), 0b00010000);			//DL length = byDLTick*Ftxclk
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
/** \brief sio send data 
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] byPdata: pointer of tx data buffer 
 *  \param[in] byLen: sebd data length
 *  \return none
 */ 
uint32_t csp_sio_send(csp_sio_t *ptSioBase, uint32_t *byPdata, uint8_t byLen)
{
	uint32_t i, j = SIO_TX_TIMEOUT;
	for(i = 0; i < byLen; i++)
	{
		csp_sio_set_txbuf(ptSioBase,byPdata[i]);
		while(!(csp_sio_get_risr(ptSioBase) & SIO_TXBUFEMPT) && j --);
		if(j == 0)
			return i;
		delay_ums(1);
	}
	return i;
}
/** \brief set sio sample mode 
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eBst: sample edge select
 *  \param[in] eTrgMode: sample trigger mode
 *  \param[in] eRMode: sample mode
 *  \return none
 */ 
void csp_sio_set_sample_mode(csp_sio_t *ptSioBase,sio_bstsel_e eBst, sio_trgmode_e eTrgMode, sio_rmode_e eRMode)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~(SIO_BSTSEL_MSK | SIO_TRGMODE_MSK | SIO_RMODE_MSK));
	ptSioBase->RXCR0 |= (eBst << SIO_BSTSEL_POS) | (eTrgMode << SIO_TRGMODE_POS) | (eRMode << SIO_RMODE_POS);
}
/** \brief sio sample set 
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eExtract: sample extract 
 *  \param[in] eAlign: sample overtime length
 *  \param[in] bySplCnt: BIT sample length
 *  \param[in] byHihr: sample extract high
 *  \return none
 */ 
void csp_sio_set_sample(csp_sio_t *ptSioBase, sio_extract_e eExtract ,sio_align_e eAlign, uint8_t bySplCnt, uint8_t byHihr)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~(SIO_SPLCNT_MSK | SIO_EXTRACT_MSK | SIO_HITHR_MSK | SIO_ALIGNEN_MSK));
	ptSioBase->RXCR0 |= (eExtract << SIO_EXTRACT_POS) | (eAlign << SIO_ALIGNEN_POS)| SIO_SPLCNT(bySplCnt) | SIO_HITHR(byHihr);
	//ptSioBase->RXCR2= (ptSioBase->RXCR2 & ~SIO_TOCNT_MSK) | SIO_TOCNT(byTocnt);
}
/** \brief sio set rx 
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eRdir: receive data dir
 *  \param[in] byBuflen: receive data buffer length
 *  \param[in] byRxCnt: receive data length 
 *  \return none
 */ 
void csp_sio_set_rx(csp_sio_t *ptSioBase,sio_rdir_e eRdir, uint8_t byBuflen, uint8_t byRxCnt)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~SIO_RDIR_MSK) | (eRdir << SIO_RDIR_POS);
	ptSioBase->RXCR1 = (ptSioBase->RXCR1 & ~(SIO_BUFCNT_MSK | SIO_RXCNT_MSK)) | SIO_BUFCNT(byBuflen) | SIO_RXCNT(byRxCnt) ;
}
/** \brief sio set break; 
 * 
 *  \param[in] ptSioBase: pointer of SIO reg structure
 *  \param[in] eRdir: receive data dir
 *  \param[in] byRxCnt: receive data length 
 *  \param[in] byBuflen: receive data buffer length
 *  \return none
 */ 
void csp_sio_set_break(csp_sio_t *ptSioBase, sio_break_e eBreak, sio_breaklel_e eBkLvl,uint8_t byBkCnt)
{
	ptSioBase->RXCR2 = (ptSioBase->RXCR2 & ~(SIO_BREAKEN_MSK | SIO_BREAKLVL_MSK | SIO_BREAKCNT_MSK));
	ptSioBase->RXCR2 |= eBreak | (eBkLvl << SIO_BREAKLVL_POS) | SIO_BREAKCNT(byBkCnt);
}