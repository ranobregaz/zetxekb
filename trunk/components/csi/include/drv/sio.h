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

//typedef enum {
//    SIO_EVENT_SEND_COMPLETE        = 0,		//Send data completed.
//    SIO_EVENT_RECEIVE_COMPLETE,          	//Receive data completed.
//    SIO_EVENT_TXBUF_EMPTY,           		//TX buffer empty
//    SIO_EVENT_RXBUF_FULL,                   //RX buffer full
//    SIO_EVENT_BREAK,                  		//Receive break
//    SIO_EVENT_TIMEOUT,                    	//Receive timeout
//	SIO_EVENT_ERROR							//Event Error
//} csi_sio_event_t;
//
//typedef struct csi_sio csi_sio_t;
//struct csi_sio {
//    csi_dev_t          dev;
//    void               (*callback)(csi_sio_t *sio, csi_sio_event_t event, void *arg);
//    void               *arg;
//	uint32_t           *data;             	//SIO transfer-data buffer
//    uint32_t           size;              	//SIO transfer-data size
//    void               *priv;
//} ;

//sio tx 
typedef enum{
	SIO_IDLE_Z			= 0,
	SIO_IDLE_H,
	SIO_IDLE_L
}csi_sio_idlest_e;

typedef enum{
	SIO_TXDIR_LSB		= 0,		
	SIO_TXDIR_MSB
}csi_sio_tdir_e;

//sio rx
typedef enum{
	SIO_TRG_RISE		= 0,
	SIO_TRG_FALL,
	SIO_TRG_BOTH,
	SIO_TRG_R
}csi_sio_trgedge_e;

typedef enum{
	SIO_TRGMD_DEB		= 0,
	SIO_TRGMD_FIL,
}csi_sio_trgmode_e;

typedef enum{
	SIO_SPMD_EDGE		= 0,
	SIO_SPMD_EDGE_NO,
}csi_sio_rxpmode_e;

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

typedef enum{
	SIO_RXDIR_MSB	=	 0,
	SIO_RXDIR_LSB
}csi_sio_rdir_e;

//typedef enum{
//	SIO_BK_DIS		= 0,
//	SIO_BK_EN
//}csi_sio_bken_e;

typedef enum{
	SIO_BKLEV_LOW		= 0,
	SIO_BKLEV_HIGH
}csi_sio_bklev_e;

//typedef enum{
//	SIO_TORST_DIS		= 0,
//	SIO_TORST_EN
//}csi_sio_torst_e;

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
	SIO_STATE_IDLE		= 0,	//uart idle(rx/tx)
	SIO_STATE_RECV,				//uart receiving 
	SIO_STATE_SEND,				//uart sending 
	SIO_STATE_FULL,				//uart receive complete(full)
	SIO_STATE_DONE				//uart send complete
} csi_sio_state_e;



//sio tx data structure
typedef struct{
	uint8_t	d0_len;				//D0 send length  [1 - 8]bit
	uint8_t	d1_len;				//D1 send length  [1 - 8]bit
	uint8_t	dh_len;				//DH send length  [1 - 8]bit
	uint8_t	dl_len;				//DL send length  [1 - 8]bit
	uint8_t	dh_hsq;				//DH data sequence [0x00 - 0xff],send sequence bit0 - bitdatahlen
	uint8_t	dl_lsq;				//DL data sequence [0x00 - 0xff],send sequence bit0 - bitdatallen
}csi_sio_txdata_t;		

typedef struct{
	csi_sio_txdata_t txdata;		
	csi_sio_idlest_e idlest;	//idle sio putout state	     		
	csi_sio_tdir_e	 txdir;		//sio putout dir LSB OR MSB			
	uint8_t	txbuflen;			//sio tx buffer length, Max Len = 32bit(4bytes)		   
	uint8_t	txcnt;				//sio tx bit count, Mux Num = 256bit(32bytes)			
}csi_sio_tx_t;                  

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
	uint32_t		wTxFreq;		//sio tx frequency 
} csi_sio_tx_config_t;;

//sio tx data structure
//typedef struct{
//	csi_sio_debdep_e	deb_depth;	//Receive Debounce Filter Depth		
//	uint8_t				deb_ckdiv; 	//Receive Debounce Filter CLK Div        
//}csi_sio_rxdeb_t;

//typedef struct{
//	csi_sio_bst_e		bstsel;
//	csi_sio_trgmd_e 	trgmode;
//	csi_sio_rmode_e		rxmode;
//	csi_sio_extract_e	extract;
//	csi_sio_align_e		alignen;
//	csi_sio_rdir_e		rxdir;
//	uint8_t				splcnt;			
//	uint8_t				hithr;          
//	uint8_t				rxbuflen;          
//	uint8_t 			rxlen;			
//}csi_sio_rxspl_t;
//
//typedef struct{
//	csi_sio_break_e		break_en;		
//	csi_sio_breaklel_e  break_lvl;       
//	uint8_t				break_cnt;		
//}csi_sio_rstdete_t;
//
//typedef struct{
//	csi_sio_torst_e		torst_en;		
//	uint8_t				to_cnt;		
//}csi_sio_to_t;
//
//typedef struct{
//	csi_sio_rxdeb_t		rxdeb;   
//	csi_sio_rxspl_t 	rxsample;			
//	csi_sio_rstdete_t	rstdete;  
//	csi_sio_to_t		torst;
//}csi_sio_rx_t;


/// \struct csi_sio_tx_config_t
/// \brief  sio parameter configuration, open to users  
typedef struct {
	
	uint8_t			byBreakEn;
	uint8_t			byBreakLev;
	uint8_t			byBreakCnt;
	uint8_t			byToRstCnt;
	uint8_t			byToCnt;
} csi_sio_rst_t;

typedef struct {
	uint8_t			byDebPerLen;		//debounce period length, (1~8)period
	uint8_t			byDebClkDiv;		//debounce clk div,
	uint8_t			byTrgEdge;			//receive samping trigger edge
	uint8_t			byTrgMode;			//receive samping trigger mode
	uint8_t			bySpMode;			//receive samping mode
	uint8_t			bySpBitCnt;			//receive samping one bit count length
	uint8_t			bySpExtra;			//receive samping extract select
	uint8_t			byHithr;			//extract high Threshold 
	uint8_t			byRxDir;			//sio receive dir, LSB OR MSB
	uint8_t			byRxCnt;			//sio rx bit count, Mux Num = 256bit(32bytes)	 
	uint8_t			byRxBufLen;			//sio rx buffer length, Max Len = 32bit(4bytes)
	uint8_t			byInter;			//sio interrupt
	uint32_t		wRxFreq;			//sio tx frequency 
	//csi_sio_rst_t	tRxRst;				//receive reset handle
} csi_sio_rx_config_t;;

/// \struct csi_uart_transfer_t
/// \brief  uart transport handle, not open to users  
typedef struct {
	
	uint32_t		*pwData;			//transport data buffer
	uint8_t         bySize;				//transport data size
	uint8_t         byTranLen;			//transport data size
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
  \brief       send txbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to send data buffer
  \param[in]   size   size of data buffer
  \return      error code \ref csi_error_t
*/
uint8_t csi_sio_send(csp_sio_t *ptSioBase, const void *pData, uint8_t bySize);

/** 
  \brief set sio transport data buffer
  \param[in] pwData: pointer of sio transport data buffer
  \param[in] hwLen: sio transport data length
  \return error code \ref csi_error_t
 */ 
csi_error_t csi_sio_set_buffer(uint32_t *pwData, uint8_t byLen);

/**
  \brief       receive data to sio transmitter, asynchronism mode
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \param[in]   size   size of data buffer
  \return      error code \ref csi_error_t
*/
uint8_t csi_sio_receive_async(csp_sio_t *sio, void *data);

/**
  \brief       receive rxbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_sio_receive(csi_sio_t *sio, uint32_t *data);


/**
  \brief       get misr
  \param[in]   sio    Handle of sio instance
  \return      sio->misr 
*/
//uint32_t csi_sio_get_isr(csi_sio_t *sio);

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

#ifdef __cplusplus
}
#endif

#endif /* _DRV_sio_H_ */
