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

#ifndef _DRV_SIO_H_
#define _DRV_SIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>

#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif

//sio tx 
/**
 * \enum     csi_sio_idlest_e
 * \brief    SIO idle ouput level
 */
typedef enum{
	SIO_IDLE_Z			= 0,
	SIO_IDLE_H,
	SIO_IDLE_L
}csi_sio_idlest_e;

/**
 * \enum     csi_sio_tdir_e
 * \brief    SIO txbuf bit shift mode
 */
typedef enum{
	SIO_TXDIR_LSB		= 0,		
	SIO_TXDIR_MSB
}csi_sio_tdir_e;

/**
 * \enum     csi_sio_txbuf_data_e
 * \brief    SIO txbuffer data definition
 */
typedef enum
{
	TXBUF_D0     		=	0x00,  
	TXBUF_D1    		=	0x01,      
	TXBUF_DL   			=	0x02, 
	TXBUF_DH			=	0x03
}csi_sio_txbuf_data_e;   

//sio rx
/**
 * \enum     csi_sio_trgedge_e
 * \brief    SIO rx sample edge mode
 */
typedef enum{
	SIO_TRG_RISE		= 0,
	SIO_TRG_FALL,
	SIO_TRG_BOTH,
	SIO_TRG_R
}csi_sio_trgedge_e;

/**
 * \enum     csi_sio_trgmode_e
 * \brief    SIO rx sample trigger mode
 */
typedef enum{
	SIO_TRGMD_DEB		= 0,
	SIO_TRGMD_FIL,
}csi_sio_trgmode_e;

/**
 * \enum     csi_sio_rxpmode_e
 * \brief    SIO rx sample mode
 */
typedef enum{
	SIO_SPMD_EDGE_EN		= 0,
	SIO_SPMD_EDGE_DIS,
}csi_sio_rxpmode_e;

/**
 * \enum     csi_sio_deb_e
 * \brief    SIO rx debounce period
 */
typedef enum{
	SIO_DEB_PER_1		= 0,
	SIO_DEB_PER_2,
	SIO_DEB_PER_3,
	SIO_DEB_PER_4,
	SIO_DEB_PER_5,
	SIO_DEB_PER_6,
	SIO_DEB_PER_7,
	SIO_DEB_PER_8
}csi_sio_deb_e;

/**
 * \enum     csi_sio_extract_e
 * \brief    SIO rx extract
 */
typedef enum{
	SIO_EXTRACT_0	= 0,
	SIO_EXTRACT_1,
	SIO_EXTRACT_2,
	SIO_EXTRACT_3,
	SIO_EXTRACT_4,
	SIO_EXTRACT_5,
	SIO_EXTRACT_6,
	SIO_EXTRACT_7,
	SIO_EXTRACT_8,
	SIO_EXTRACT_9,
	SIO_EXTRACT_10,
	SIO_EXTRACT_11,
	SIO_EXTRACT_12,
	SIO_EXTRACT_13,
	SIO_EXTRACT_14,
	SIO_EXTRACT_15,
	SIO_EXTRACT_16,
	SIO_EXTRACT_17,
	SIO_EXTRACT_18,
	SIO_EXTRACT_19,
	SIO_EXTRACT_20,
	SIO_EXTRACT_21,
	SIO_EXTRACT_22,
	SIO_EXTRACT_23,
	SIO_EXTRACT_24,
	SIO_EXTRACT_25,
	SIO_EXTRACT_26,
	SIO_EXTRACT_27,
	SIO_EXTRACT_28,
	SIO_EXTRACT_29,
	SIO_EXTRACT_30,
	SIO_EXTRACT_31,
	SIO_EXTRACT_HI
}csi_sio_extract_e;

/**
 * \enum     csi_sio_rdir_e
 * \brief    SIO rxbuf shift mode
 */
typedef enum{
	SIO_RXDIR_LSB	=	 0,
	SIO_RXDIR_MSB
}csi_sio_rdir_e;

/**
 * \enum     csi_sio_rdir_e
 * \brief    SIO break detect level
 */
typedef enum{
	SIO_BKLEV_LOW		= 0,
	SIO_BKLEV_HIGH
}csi_sio_bklev_e;


/**
 * \enum     csi_sio_intsrc_e
 * \brief    SIO interrupt source
 */
typedef enum
{
	SIO_INTSRC_NONE     	=	(0x00ul << 0),  
	SIO_INTSRC_TXDNE     	=	(0x01ul << 0),  
	SIO_INTSRC_RXDNE    	=	(0x01ul << 1),      
	SIO_INTSRC_TXBUFEMPT   	=	(0x01ul << 2), 
	SIO_INTSRC_RXBUFFULL	=	(0x01ul << 3), 
	SIO_INTSRC_BREAK		=	(0x01ul << 4), 
	SIO_INTSRC_TIMEOUT		=	(0x01ul << 5)
}csi_sio_intsrc_e; 


/**
 * \enum     csi_sio_state_e
 * \brief    SIO working status
 */
typedef enum {
	SIO_STATE_IDLE		= 0,	//sio idle(rx/tx)
	SIO_STATE_RECV,				//sio receiving 
	SIO_STATE_SEND,				//sio sending 
	SIO_STATE_FULL,				//sio receive complete(full)
	SIO_STATE_DONE,				//sio send complete
	SIO_STATE_ERROR,			//sio send complete
	SIO_STATE_TIMEOUT			//sio send complete
} csi_sio_state_e;

/**
 * \enum     csi_sio_wkmode_e
 * \brief    SIO tx/rx mode
 */
typedef enum{
	//send mode
	SIO_TX_MODE_POLL	=	0,		//polling mode, no interrupt
	SIO_TX_MODE_INT		=	1,		//tx use interrupt mode
	//receive
	SIO_RX_MODE_BUFFULL	=	0,		//rx use interrupt mode(RXBUFFULL)
	SIO_RX_MODE_DONE	=	1		//rx use interrupt mode(RXDONE)
}csi_sio_wkmode_e;
          

/// \struct csi_sio_tx_config_t
/// \brief  sio parameter configuration, open to users  
typedef struct {
	uint8_t			byD0Len;		//D0 send length (1~8)bit, all 0 sequence(out low(0)) 
	uint8_t			byD1Len;		//D1 send length (1~8)bit, all 1 sequence(out high(1)) 
	uint8_t			byDLLen;		//DL send length (1~8)bit
	uint8_t			byDHLen;		//DH send length (1~8)bit
	uint8_t			byDLLsq;		//DL data sequence (0~0xff),send sequence bit0 - bitdatahlen
	uint8_t			byDHHsq;		//DH data sequence (0~0xff),send sequence bit0 - bitdatallen
	uint8_t			byTxDir; 		//sio output dir, LSB OR MSB
	uint8_t			byIdleLev;		//idle sio ouput state	
	uint8_t			byTxCnt;		//sio tx bit count, Mux Num = 256bit(32bytes)	 
	uint8_t			byTxBufLen;		//sio tx buffer length, Max Len = 16bit(2bytes)	
	uint8_t			byInter;		//sio interrupt
	uint8_t			byTxMode;		//sio tx mode
	uint32_t		wTxFreq;		//sio tx frequency 
} csi_sio_tx_config_t;;


/// \struct csi_sio_tx_config_t
/// \brief  sio parameter configuration, open to users  
typedef struct {
	uint8_t			byDebPerLen;		//debounce period length, (1~8)period
	uint8_t			byDebClkDiv;		//debounce clk div,
	uint8_t			byTrgEdge;			//receive samping trigger edge
	uint8_t			byTrgMode;			//receive samping trigger mode
	uint8_t			bySpMode;			//receive samping mode
	uint8_t			bySpBitLen;			//receive samping one bit count length
	uint8_t			bySpExtra;			//receive samping extract select
	uint8_t			byHithr;			//extract high Threshold 
	uint8_t			byRxDir;			//sio receive dir, LSB OR MSB
	uint8_t			byRxCnt;			//sio rx bit count, Mux Num = 256bit(32bytes)	 
	uint8_t			byRxBufLen;			//sio rx buffer length, Max Len = 32bit(4bytes)
	uint8_t			byInter;			//sio interrupt
	uint32_t		wRxFreq;			//sio tx frequency 
} csi_sio_rx_config_t;;

/// \struct csi_sio_transfer_t
/// \brief  sio transport handle, not open to users  
typedef struct {
	
	uint32_t		*pwData;			//transport data buffer
	uint16_t        hwSize;				//transport data size
	uint16_t        hwTranLen;			//transport data size
	uint8_t			byRecvMode;			//sio receive mode
	uint8_t			bySendMode;			//sio send mode
	uint8_t			byRxStat;			//sio receive status
	uint8_t			byTxStat;			//sio send status
	
} csi_sio_trans_t;

extern csi_sio_trans_t g_tSioTran;	


/**
  \brief       configure sio tx
               configure the resources needed for the sio instance
  \param[in]   ptSioBase	pointer of sio register structure
  \param[in]   ptTxCfg    	pointer of sio parameter config structure
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_init(csp_sio_t *ptSioBase, csi_sio_tx_config_t *ptTxCfg);

/**
  \brief       configure sio tx
               configure the resources needed for the sio instance
  \param[in]   ptSioBase	pointer of sio register structure
  \param[in]   ptTxCfg    	pointer of sio parameter config structure
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_init(csp_sio_t *ptSioBase, csi_sio_rx_config_t *ptRxCfg);

/**
  \brief       sio receive break reset config
  \param[in]   ptSioBase	pointer of sio register structure
  \param[in]   eBkLev    	break reset detect level
  \param[in]   byBkCnt    	break reset detect period
  \param[in]   bEnable    	ENABLE/DISABLE
  \return      none
*/
void csi_sio_break_rst(csp_sio_t *ptSioBase, csi_sio_bklev_e eBkLev, uint8_t byBkCnt, bool bEnable);

/**
  \brief       sio receive timeout reset config
  \param[in]   ptSioBase	pointer of sio register structure
  \param[in]   byBkCnt    	break reset detect period
  \param[in]   bEnable    	ENABLE/DISABLE
  \return      none
*/
void csi_sio_timeout_rst(csp_sio_t *ptSioBase, uint8_t byToCnt ,bool bEnable);

/**
  \brief	   send data from sio, this function is polling and interrupt mode     
  \param[in]   ptSioBase	pointer of sio register structure
  \param[in]   pwData    	pointer to buffer with data to send 
  \param[in]   hwSize    	send data size
  \return      error code \ref csi_error_t or data size
*/
int32_t csi_sio_send(csp_sio_t *ptSioBase, const uint32_t *pwData, uint16_t hwSize);

/** 
  \brief 	   set sio receive data buffer
  \param[in]   pwData		pointer of sio transport data buffer
  \param[in]   hwLen		sio transport data length
  \return 	   error code \ref csi_error_t
 */ 
csi_error_t csi_sio_set_buffer(uint32_t *pwData, uint16_t hwLen);

/**
  \brief       receive data to sio transmitter, asynchronism mode
  \param[in]   ptSioBase	pointer of sio register structure
  \return      size of receive data
*/
uint16_t csi_sio_receive(csp_sio_t *ptSioBase);

/** 
  \brief get the status of sio receive 
  \param[in] none
  \return the status of sio receive 
 */ 
csi_sio_state_e csi_sio_get_recv_status(void);

/** 
  \brief clr the status of sio receive 
  \param[in] none
  \return none
 */ 
void csi_sio_clr_recv_status(void);

/** 
  \brief get the status of sio receive 
  \param[in] none
  \return the status of sio receive 
 */ 
//csi_sio_state_e csi_sio_get_send_status(void);

/** 
  \brief clr the status of sio receive 
  \param[in] none
  \return none
 */ 
//void csi_sio_clr_send_status(void);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_sio_H_ */
