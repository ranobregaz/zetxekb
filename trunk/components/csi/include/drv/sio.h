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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SIO_EVENT_SEND_COMPLETE        = 0,		//Send data completed.
    SIO_EVENT_RECEIVE_COMPLETE,          	//Receive data completed.
    SIO_EVENT_TXBUF_EMPTY,           		//TX buffer empty
    SIO_EVENT_RXBUF_FULL,                   //RX buffer full
    SIO_EVENT_BREAK,                  		//Receive break
    SIO_EVENT_TIMEOUT,                    	//Receive timeout
	SIO_EVENT_ERROR							//Event Error
} csi_sio_event_t;

typedef struct csi_sio csi_sio_t;
struct csi_sio {
    csi_dev_t          dev;
    void               (*callback)(csi_sio_t *sio, csi_sio_event_t event, void *arg);
    void               *arg;
	uint32_t           *data;             	//SIO transfer-data buffer
    uint32_t           size;              	//SIO transfer-data size
    void               *priv;
} ;




//sio tx 
typedef enum{
	SIO_IDLE_Z		= 0,
	SIO_IDLE_H,
	SIO_IDLE_L
}csi_sio_idlest_e;

typedef enum{
	SIO_TXDIR_LSB	= 0,		
	SIO_TXDIR_MSB
}csi_sio_tdir_e;

//sio rx
typedef enum{
	SIO_DEBDP1			= 0,
	SIO_DEBDP2,
	SIO_DEBDP3,
	SIO_DEBDP4,
	SIO_DEBDP5,
	SIO_DEBDP6,
	SIO_DEBDP7,
	SIO_DEBDP8
}csi_sio_debdep_e;

typedef enum{
	SIO_BSTSEL_RISE	= 0,
	SIO_BSTSEL_FALL,
	SIO_BSTSEL_BOTH,
	SIO_BSTSEL_R
}csi_sio_bst_e;

typedef enum{
	SIO_TRGMD0		= 0,
	SIO_TRGMD1
}csi_sio_trgmd_e;

typedef enum{
	SIO_RXMD0		= 0,
	SIO_RXMD1
}csi_sio_rmode_e;

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
	SIO_ALN_DIS		= 0,
	SIO_ALN_EN
}csi_sio_align_e;

typedef enum{
	SIO_RXDIR_MSB	= 0,
	SIO_RXDIR_LSB
}csi_sio_rdir_e;

typedef enum{
	SIO_BK_DIS		= 0,
	SIO_BK_EN
}csi_sio_break_e;

typedef enum{
	SIO_BKLVL_LOW		= 0,
	SIO_BKLVL_HIGH
}csi_sio_breaklel_e;

typedef enum{
	SIO_TORST_DIS		= 0,
	SIO_TORST_EN
}csi_sio_torst_e;

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

//sio tx data structure
typedef struct{
	csi_sio_debdep_e	deb_depth;	//Receive Debounce Filter Depth		
	uint8_t				deb_ckdiv; 	//Receive Debounce Filter CLK Div        
}csi_sio_rxdeb_t;

typedef struct{
	csi_sio_bst_e		bstsel;
	csi_sio_trgmd_e 	trgmode;
	csi_sio_rmode_e		rxmode;
	csi_sio_extract_e	extract;
	csi_sio_align_e		alignen;
	csi_sio_rdir_e		rxdir;
	uint8_t				splcnt;			
	uint8_t				hithr;          
	uint8_t				rxbuflen;          
	uint8_t 			rxlen;			
}csi_sio_rxspl_t;

typedef struct{
	csi_sio_break_e		break_en;		
	csi_sio_breaklel_e  break_lvl;       
	uint8_t				break_cnt;		
}csi_sio_rstdete_t;

typedef struct{
	csi_sio_torst_e		torst_en;		
	uint8_t				to_cnt;		
}csi_sio_to_t;

typedef struct{
	csi_sio_rxdeb_t		rxdeb;   
	csi_sio_rxspl_t 	rxsample;			
	csi_sio_rstdete_t	rstdete;  
	csi_sio_to_t		torst;
}csi_sio_rx_t;


/**
  \brief       Init sio ctrl block
               Initializes the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   idx    Index of instance
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_init(csi_sio_t *sio, uint32_t idx);

/**
  \brief       Uninit sio ctrl block
               Stops operation and releases the software resources used by the instance
  \param[in]   sio    Handle of sio instance
  \return      None
*/
void csi_sio_uninit(csi_sio_t *sio);

/**
  \brief       Attach callback to the sio
  \param[in]   sio    sio handle to operate
  \param[in]   cb     Event callback function \ref csi_sio_callback_t
  \param[in]   arg    User private param for event callback
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_attach_callback(csi_sio_t *sio, void *callback, void *arg);

/**
  \brief       Detach callback from the sio
  \param[in]   sio    sio handle to operate
  \return      None
*/
void csi_sio_detach_callback(csi_sio_t *sio);

/**
  \brief       Config sio tx freq
  \param[in]   sio          Handle of sio instance
  \param[in]   tx_clk    	sio tx freq 
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_freq(csi_sio_t *sio, uint32_t tx_clk);

/**
  \brief       Config sio rx freq
  \param[in]   sio          Handle of sio instance
  \param[in]   rx_clk    	sio rx freq 
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_freq(csi_sio_t *sio, uint32_t rx_clk);

/**
  \brief       configure sio tx
               configure the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   sio_tx    Pointer to siotx struct\ref csi_sio_tx_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_tx_init(csi_sio_t *sio, csi_sio_tx_t *sio_tx);


/**
  \brief       configure sio rx
               configure the resources needed for the sio instance
  \param[in]   sio    Handle of sio instance
  \param[in]   sio_rx    Pointer to siorx struct\ref csi_sio_rx_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_rx_init(csi_sio_t *sio, csi_sio_rx_t *sio_rx);

/**
  \brief       send txbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to send data buffer
  \param[in]   size   size of data buffer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_send(csi_sio_t *sio, const void *data, uint32_t size);

/**
  \brief       receive data to sio transmitter, asynchronism mode
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \param[in]   size   size of data buffer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_receive_async(csi_sio_t *sio, void *data, uint32_t size);

/**
  \brief       receive rxbuf
  \param[in]   sio    Handle of sio instance
  \param[in]   data   Pointer to receive data buffer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_sio_receive(csi_sio_t *sio, uint32_t *data);


/**
  \brief       get misr
  \param[in]   sio    Handle of sio instance
  \return      sio->misr 
*/
uint32_t csi_sio_get_isr(csi_sio_t *sio);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_sio_H_ */
