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



#define		SPI_MASTER_SEL			//主机或者从机
#define		SPI_SYNC_SEL			//同步或者异步

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
} csi_spi_event_e;


typedef struct 
{	
	spi_rxifl_e         eSpiRxFifoLevel;	//(1/8 or 1/4 or 1/2)fifo trigger rx interrupt
	csi_spi_mode_e      eSpiMode;	    	//0:master   1:slave
	csi_spi_cp_format_e eSpiPolarityPhase;  //0:(0 0)   1:(0 1)  2:(1 0) 3:(1 1)
	csi_spi_frame_len_e eSpiFrameLen;       //4-16 bit
	uint32_t            dwSpiBaud;			//spi clk
	uint8_t             byInter;            //int source
}csi_spi_config_t;

typedef struct
{
	uint8_t             *pbyTxData;      ///< Output data buf
	uint8_t             *pbyRxData;      ///< Input  data buf
    uint8_t             byTxSize;        ///< Output data size specified by user
    uint8_t             byRxSize;        ///< Input  data size specified by user
	uint8_t             byRxFifoLength;  ///< receive fifo length
	uint8_t             byInter;  		 ///< interrupt
    csi_state_t         tState;          ///< Peripheral state
}csi_spi_transmit_t;
extern csi_spi_transmit_t g_tSpiTransmit; 

/** \brief initialize spi data structure
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] ptSpiCfg: user spi parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_init(csp_spi_t *ptSpiBase,csi_spi_config_t *ptSpiCfg);

/** \brief uninit spi data structure
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_uninit(csp_spi_t *ptSpiBase);

/** \brief set spi mode, master or slave
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] eMode: master, slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_mode(csp_spi_t *ptSpiBase, csi_spi_mode_e eMode);

/** \brief config spi cp format
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] eFormat: spi cp format
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_cp_format(csp_spi_t *ptSpiBase, csi_spi_cp_format_e eFormat);

/** \brief config spi work frequence
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] wBaud: spi work baud
 *  \return spi config frequency
 */
uint32_t csi_spi_baud(csp_spi_t *ptSpiBase, uint32_t wBaud);

/** \brief config spi frame length
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] eLength: frame length
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_frame_len(csp_spi_t *ptSpiBase, csi_spi_frame_len_e eLength);

/** \brief get the state of spi device
 * 
 *  \param[in] ptState: the state of spi device
 *  \return none
 */ 
csi_error_t csi_spi_get_state(csi_state_t *ptState);

/** \brief sending data to spi transmitter(received data is ignored),blocking mode
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to send(byte)
 *  \param[in] wTimeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_send(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize, uint32_t wTimeout);

/** \brief sending data to spi transmitter, non-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to send(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_async(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize);

/** \brief  receiving data from spi receiver, blocking mode
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pData: pointer to buffer with data to receive
 *  \param[in] wSize: number of data to receive(byte)
 *  \param[in] wTimeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_receive(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize, uint32_t wTimeout);

/** \brief  receiving data from spi receiver, not-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_receive_async(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize);

/** \brief  receiving data from spi receiver,blocking mode
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pDataout: pointer to buffer with data to send to spi transmitter
 *  \param[in] pDatain: number of data to receive(byte)
 *  \param[in] wSize: number of data to receive(byte)
 *  \param[in] wTimeout: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
int32_t csi_spi_send_receive(csp_spi_t *ptSpiBase, void *pDataout, void *pDatain, uint32_t wSize, uint32_t wTimeout);

/** \brief  receiving data from spi receiver, not-blocking mode
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pDataout: pointer to buffer with data to send to spi transmitter
 *  \param[in] pDatain: number of data to receive(byte)
 *  \param[in] wSize: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_receive_async(csp_spi_t *ptSpiBase, void *pDataout, void *pDatain, uint32_t wSize);

/** \brief  transmission variables init ,user not change it
 * 
 *  \param[in] eRxLen:rx fifo length
 *  \param[in] byInter:interrupt source
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_Internal_variables_init(spi_rxifl_e eRxLen,uint8_t byInter);

//interrupt
/** \brief spi interrupt handle function
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \return none
 */ 
__attribute__((weak)) void spi_irqhandler(csp_spi_t *ptSpiBase);

/** \brief clr spi rx fifo
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \return none
 */
void csi_spi_clr_rxfifo(csp_spi_t *ptSpiBase);

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \return none
 */ 
uint16_t csi_spi_receive_slave(csp_spi_t *ptSpiBase);

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] hwDataout: data of send
 *  \return none
 */ 
csi_error_t csi_spi_send_slave(csp_spi_t *ptSpiBase, uint16_t hwDataout);

//-----------------------------------------------------------------------------------------------------------
//high speed spi function for reference
//-----------------------------------------------------------------------------------------------------------

/** \brief spi_send_receive_1byte
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] byReceiveData :send data buffer pointer
 *  \return the receive data
 */ 
uint8_t spi_send_receive_1byte(csp_spi_t *ptSpiBase,uint8_t byData);

/** \brief spi send buff(this funtion will ignore the receive)
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pbyData :send data buffer pointer
 *  \param[in] bySize ：length
 *  \return none
 */ 
void spi_buff_send(csp_spi_t *ptSpiBase,uint8_t *pbyData,uint8_t bySize);

/** \brief spi send and receive(equal to 8 or less than eight bytes)
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pbyDataOut :send data buffer pointer
 *  \param[in] pbyDataIn :receive data buffer pointer
 *  \param[in] wSize ：length
 *  \return none
 */ 
void csi_spi_send_receive_x8(csp_spi_t *ptSpiBase, uint8_t *pbyDataOut,uint8_t *pbyDataIn,uint32_t wSize);

/** \brief spi send and receive(equal to 8 bytes or  more than eight bytes)
 * 
 *  \param[in] ptSpiBase: pointer of spi register structure
 *  \param[in] pbyDataout :send data buffer pointer 
 *  \param[in] pbyDatain  :send data buffer pointer 
 *  \param[in] wSize ：length
 *  \return none
 */ 
void csi_spi_send_receive_d8(csp_spi_t *ptSpiBase, uint8_t *pbyDataout,uint8_t *pbyDatain, uint32_t wSize);
//-------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _DRV_SPI_H_ */
