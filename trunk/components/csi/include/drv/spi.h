/***********************************************************************//** 
 * \file  spi.h
 * \brief  head file of csi spi
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version   <th>Author  <th>Description
 * <tr><td> 2020-8-28 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2021-1-12 <td>V0.1  <td>ZJY     <td>modify
 * <tr><td> 2021-5-31 <td>V0.2  <td>LQ      <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_SPI_H_
#define _DRV_SPI_H_

#include <stdint.h>

#include <drv/common.h>
#include <drv/gpio.h>
#include <drv/dma.h>

#ifdef __cplusplus
extern "C" {
#endif



#define		SPI_MASTER_SEL			//spi mode select(master or slave)
#define		SPI_SYNC_SEL			//spi work mode select(同步 或者异步)

#ifdef SPI_MASTER_SEL
	#define		SPICS_CLR		*(uint32_t *)(0x60002010)=(uint32_t)0x20		//PB0.5低
	#define		SPICS_SET		*(uint32_t *)(0x6000200c)=(uint32_t)0x20	    //PB0.5高
#else
	#define		SPICS_CLR		nop
	#define		SPICS_SET       nop
#endif	
	
/**
 *  \enum    csi_spi_mode_t
 *  \brief   Function mode of spi
 */
typedef enum {
    SPI_MASTER,             ///< SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps
    SPI_SLAVE               ///< SPI Slave  (Output on MISO, Input on MOSI)
} csi_spi_mode_e;

/**
 *  \enum     csi_spi_frame_len_t
 *  \brief    SPI data width (4bit ~ 16bit)
 */

typedef enum {
    SPI_FRAME_LEN_4  = 4,
    SPI_FRAME_LEN_5,
    SPI_FRAME_LEN_6,
    SPI_FRAME_LEN_7,
    SPI_FRAME_LEN_8,
    SPI_FRAME_LEN_9,
    SPI_FRAME_LEN_10,
    SPI_FRAME_LEN_11,
    SPI_FRAME_LEN_12,
    SPI_FRAME_LEN_13,
    SPI_FRAME_LEN_14,
    SPI_FRAME_LEN_15,
    SPI_FRAME_LEN_16
} csi_spi_frame_len_e;

/**
 *  \enum     csi_spi_format_t
 *  \brief    Timing format of spi
 */
typedef enum {
    SPI_FORMAT_CPOL0_CPHA0 = 0,  ///< Clock Polarity 0, Clock Phase 0
    SPI_FORMAT_CPOL0_CPHA1,      ///< Clock Polarity 0, Clock Phase 1
    SPI_FORMAT_CPOL1_CPHA0,      ///< Clock Polarity 1, Clock Phase 0
    SPI_FORMAT_CPOL1_CPHA1,      ///< Clock Polarity 1, Clock Phase 1
} csi_spi_cp_format_e;

/**
 *  \enum     csi_spi_event_t
 *  \brief    Signaled event for user by driver
 */
typedef enum {
    SPI_EVENT_SEND_COMPLETE,           ///< Data Send completed. Occurs after call to csi_spi_send_async to indicate that all the data has been send over
    SPI_EVENT_RECEIVE_COMPLETE,        ///< Data Receive completed. Occurs after call to csi_spi_receive_async to indicate that all the data has been received
    SPI_EVENT_SEND_RECEIVE_COMPLETE,   ///< Data Send_receive completed. Occurs after call to csi_spi_send_receive_async to indicate that all the data has been send_received
    SPI_EVENT_RECEIVE_SLAVE_COMPLE,    ///< Data Receive completed. Occurs after call to csi_spi_receive_async to indicate that all the data has been received
	SPI_EVENT_ERROR_OVERFLOW,          ///< Data overflow: Receive overflow
    SPI_EVENT_ERROR_UNDERFLOW,         ///< Data underflow: Transmit underflow
    SPI_EVENT_ERROR                    ///< Master Mode Fault (SS deactivated when Master).Occurs in master mode when Slave Select is deactivated and indicates Master Mode Fault
} csi_spi_event_t;


typedef struct 
{	
	spi_rxifl_e         eSpiRxFifoLevel;	//(1/8 or 1/4 or 1/2)fifo trigger rx interrupt
	csi_spi_mode_e      eSpiMode;	    	//0:master   1:slave
	csi_spi_cp_format_e eSpiPolarityPhase;  //0:(0 0)   1:(0 1)  2:(1 0) 3:(1 1)
	csi_spi_frame_len_e eSpiFrameLen;       //4-16 bit
	uint32_t            dwSpiBaud;			//spi clk
    uint8_t             *tx_data;      ///< Output data buf
    uint32_t             tx_size;      ///< Output data size specified by user
    uint8_t             *rx_data;      ///< Input  data buf
    uint32_t             rx_size;      ///< Input  data size specified by user
	void (*callback)(csp_spi_t *ptSpiBase, csi_spi_event_t event, void *arg); ///< User callback ,signaled by driver event
    csi_state_t         state;        ///< Peripheral state
    void                *priv;
	void                *arg;          ///< User private param ,passed to user callback
}csi_spi_para_config_t;
extern csi_spi_para_config_t g_tSpiTransmit;

/** \brief initialize spi data structure
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_init(csp_spi_t *ptSpiBase);

/** \brief set spi mode, master or slave
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] mode: master, slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_mode(csp_spi_t *ptSpiBase, csi_spi_mode_e mode);

/** \brief config spi cp format
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] format: spi cp format
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_cp_format(csp_spi_t *ptSpiBase, csi_spi_cp_format_e format);

/** \brief config spi work frequence
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] baud: spi work baud
 *  \return spi config frequency
 */
uint32_t csi_spi_baud(csp_spi_t *ptSpiBase, uint32_t baud);

/** \brief config spi frame length
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] length: frame length
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_frame_len(csp_spi_t *ptSpiBase, csi_spi_frame_len_e length);

/** \brief get the state of spi device
 * 
 *  \param[in] state: the state of spi device
 *  \return none
 */ 
csi_error_t csi_spi_get_state(csi_state_t *state);

/** \brief sending data to spi transmitter(received data is ignored),blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_send(csp_spi_t *ptSpiBase, const void *data, uint32_t size, uint32_t timeout);

/** \brief sending data to spi transmitter, non-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_async(csp_spi_t *ptSpiBase, const void *data, uint32_t size);

/** \brief  receiving data from spi receiver, blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_receive(csp_spi_t *ptSpiBase, void *data, uint32_t size, uint32_t timeout);

/** \brief  receiving data from spi receiver, not-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_receive_async(csp_spi_t *ptSpiBase, void *data, uint32_t size);

/** \brief  receiving data from spi receiver,blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
int32_t csi_spi_send_receive(csp_spi_t *ptSpiBase, const void *data_out, void *data_in, uint32_t size, uint32_t timeout);

/** \brief  receiving data from spi receiver, not-blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_receive_async(csp_spi_t *ptSpiBase, const void *data_out, void *data_in, uint32_t size);

/** \brief  spi parameter struct init
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] callback: after interrupt have done,then callback this function,Tells the user that interrupt processing is over
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_paraconfig_init(csp_spi_t *ptSpiBase, void *callback);

/** \brief spi interrupt callback function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] event: spi event 
 *  \param[in] arg: para
 *  \return none
 */
__attribute__((weak)) void apt_spi_event(csp_spi_t *ptSpiBase, csi_spi_event_t event, void *arg);

//interrupt
/** \brief spi interrupt handle function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */ 
__attribute__((weak)) void spi_irqhandler(csp_spi_t *ptSpiBase);

/** \brief clr spi rx fifo
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */
void csi_spi_clrRx_fifo(csp_spi_t *ptSpiBase);

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */ 
uint16_t csi_spi_receive_slave(csp_spi_t *ptSpiBase);

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: data of send
 *  \return none
 */ 
csi_error_t csi_spi_send_slave(csp_spi_t *ptSpiBase, uint16_t data_out);

//-----------------------------------------------------------------------------------------------------------
//high speed spi function for reference
//-----------------------------------------------------------------------------------------------------------

/** \brief spi_send_receive_1byte
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data :send data buffer pointer
 *  \return the receive data
 */ 
uint8_t spi_send_receive_1byte(csp_spi_t *ptSpiBase,uint8_t data);

/** \brief spi send buff
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data :send data buffer pointer
 *  \param[in] size ：length
 *  \return none
 */ 
void spi_buff_send(csp_spi_t *ptSpiBase,uint8_t *data,uint8_t size);

/** \brief spi send and receive(less than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out :send data buffer pointer
 *  \param[in] data_in :receive data buffer pointer
 *  \param[in] size ：length
 *  \return none
 */ 
void csi_spi_send_receive_x8(csp_spi_t *ptSpiBase, uint8_t *data_out,uint8_t *data_in,uint32_t size);

/** \brief spi send and receive(send equal to 8 bytes or  more than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out :send data buffer pointer 
 *  \param[in] data_in  :send data buffer pointer 
 *  \param[in] size ：length
 *  \return none
 */ 
void csi_spi_send_receive_d8(csp_spi_t *ptSpiBase, uint8_t *data_out,uint8_t *data_in, uint32_t size);
//-------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _DRV_SPI_H_ */
