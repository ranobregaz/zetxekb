/***********************************************************************//** 
 * \file  spi.c
 * \brief  csi spi driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-28 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-5-31 <td>V0.1 <td>LQ     <td>modify
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/spi.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>
#include <iostring.h>
#include <uart.h>

#include "csp_spi.h"
/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/

static csi_spi_mode_e	work_mode	=	SPI_MASTER;
csi_spi_send_receive_t g_tSpiTransmit;

/** \brief init spi gpio 
 * 
 *  \param[in] none
 *  \return none
 */ 
static void apt_spi_gpio_init(void)
{
	#ifdef SPI_MASTER_SEL
		uint32_t wPinMask = 0x01 << (PB05-16);							//pb0_5 control nss
		csi_gpio_port_dir(GPIOB0, wPinMask, GPIO_DIR_OUTPUT);			//gpio_port as output 
	#else
		csi_pin_set_mux(PB05,PB05_SPI_NSS);
	#endif
	csi_pin_set_mux(PB04,PB04_SPI_SCK);								//PB04 = SPI_SCK
	csi_pin_set_mux(PA015,PA015_SPI_MISO);							//PA15 = SPI_MISO
	csi_pin_set_mux(PA014,PA014_SPI_MOSI);							//PA14 = SPI_MOSI
	SPICS_SET;														//NSS init high	
}

/** \brief initialize spi data structure
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_init(csp_spi_t *ptSpiBase)
{
	csi_error_t tRet = CSI_OK;
	
	apt_spi_gpio_init();
	csi_spi_paraconfig_init(ptSpiBase, apt_spi_event);
	
	csi_clk_enable((uint32_t *)ptSpiBase);				//spi peripheral clk enable
	csp_spi_default_init(ptSpiBase);					//reset all registers
	csp_spi_dis(ptSpiBase);								//disable spi
	csp_spi_set_rxifl(ptSpiBase, g_tSpiTransmit.eSpiRxFifoLevel);
	csi_spi_mode(ptSpiBase, g_tSpiTransmit.eSpiMode);	
	csi_spi_cp_format(ptSpiBase, g_tSpiTransmit.eSpiPolarityPhase);	
	csi_spi_frame_len(ptSpiBase, g_tSpiTransmit.eSpiFrameLen);	
	csi_spi_baud(ptSpiBase, g_tSpiTransmit.dwSpiBaud);
	//csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT,true);//enable rx fifo int,when as slave use
	csp_spi_en(ptSpiBase);							//enable spi
	
	return tRet;
}

/** \brief set spi mode, master or slave
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] mode: master, slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_mode(csp_spi_t *ptSpiBase, csi_spi_mode_e mode)
{
    csi_error_t   ret = CSI_OK;
	work_mode = mode;
	
	switch (mode) 			//configure spi mode
	{
        case SPI_MASTER:
            csp_spi_set_mode(ptSpiBase, SPI_MODE_MASTER);
            break;
        case SPI_SLAVE:
            csp_spi_set_mode(ptSpiBase, SPI_MODE_SLAVE);
            break;
        default:
            ret = CSI_ERROR;
            break;
    }	
	return ret;
}

/** \brief config spi cp format
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] format: spi cp format
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_cp_format(csp_spi_t *ptSpiBase, csi_spi_cp_format_e format)
{
    csi_error_t   ret = CSI_OK;
	switch (format)		//configure spi format 
	{
        case SPI_FORMAT_CPOL0_CPHA0:
			csp_spi_set_spo_sph(ptSpiBase, SPI_SPO0_SPH0);
            break;
        case SPI_FORMAT_CPOL0_CPHA1:
			csp_spi_set_spo_sph(ptSpiBase, SPI_SPO0_SPH1);
            break;
        case SPI_FORMAT_CPOL1_CPHA0:
			csp_spi_set_spo_sph(ptSpiBase, SPI_SPO1_SPH0);
            break;
        case SPI_FORMAT_CPOL1_CPHA1:
			csp_spi_set_spo_sph(ptSpiBase, SPI_SPO1_SPH1);
            break;
        default:
            ret = CSI_ERROR;
            break;
    }	
	return ret;
} 

/** \brief config spi work frequence
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] baud: spi work baud
 *  \return spi config frequency
 */
uint32_t csi_spi_baud(csp_spi_t *ptSpiBase, uint32_t baud)
{
    uint32_t div;
    uint32_t freq = 0U;

	div = (soc_get_pclk_freq() >> 1) / baud;//baud = FPCLK/ CPSDVR / (1 + SCR))
	
	if(div > 0)
		div --;
	
	csp_spi_set_clk_div(ptSpiBase, div, 2);

	if(div > 0)
		freq = (soc_get_pclk_freq() >> 1) / (div + 1);
	else
		freq = (soc_get_pclk_freq() >> 1) ;
		
    return freq;
}

/** \brief config spi frame length
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] length: frame length
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_frame_len(csp_spi_t *ptSpiBase, csi_spi_frame_len_e length)
{	
    csi_error_t   ret = CSI_OK;

    if ((length < SPI_FRAME_LEN_4) || (length > SPI_FRAME_LEN_16)) 
        ret = CSI_ERROR;
	else 
	{
        csp_spi_set_data_size(ptSpiBase, (length -1));			 //configura data frame width
    }
    return ret;
}

/** \brief get the state of spi device
 * 
 *  \param[in] state: the state of spi device
 *  \return none
 */ 
csi_error_t csi_spi_get_state(csi_state_t *state)
{
    *state = g_tSpiTransmit.state;
    return CSI_OK;
}

/** \brief sending data to spi transmitter(received data is ignored),blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_send(csp_spi_t *ptSpiBase, const void *data, uint32_t size, uint32_t timeout)
{
	csi_error_t ret = CSI_OK;
	uint32_t count = 0U;
	uint32_t wValue;
	
	if((g_tSpiTransmit.state.writeable == 0U))						
		return CSI_BUSY;
	
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)	//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
		g_tSpiTransmit.tx_size = size >> 1;
	}
	else
		g_tSpiTransmit.tx_size = size;

	g_tSpiTransmit.state.writeable = 0U;
	g_tSpiTransmit.tx_data = (uint8_t *)data;
	csp_spi_en(ptSpiBase);								//enable spi
	
#ifdef	SPI_SEND_TIMEOUT_DIS							//spi timeout disable
	while(g_tSpiTransmit.tx_size > 0)
	{
		while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));	//fifo full? wait; 		
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)	//two byte
		{
			wValue = (uint16_t)(*(uint16_t *)g_tSpiTransmit.tx_data);
			csp_spi_set_data(ptSpiBase, wValue);			//send data
			g_tSpiTransmit.tx_data += 2;
			count += 2;
		}
		else											//one byte
		{
			csp_spi_set_data(ptSpiBase, *g_tSpiTransmit.tx_data);	//send data
			g_tSpiTransmit.tx_data ++;
			count ++;
		}
		
		g_tSpiTransmit.tx_size --;
	}
	
	while((csp_spi_get_sr(ptSpiBase) & SPI_BSY));		//wait for transmition finish
#else
	uint32_t wSendStart;
	while(g_tSpiTransmit.tx_size > 0)
	{
		wSendStart = SPI_SEND_TIMEOUT;
		while(!csp_spi_write_ready(ptSpiBase) && wSendStart --);	//fifo full? wait; 
		if(wSendStart == 0)
			break;
		
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)	//two byte
		{
			wValue = (uint16_t)(*(uint16_t *)g_tSpiTransmit.tx_data);
			g_tSpiTransmit.tx_data += 2;
			count += 2;
			csp_spi_set_data(ptSpiBase, wValue);			//send data
		}
		else											//one byte
		{
			csp_spi_set_data(ptSpiBase, *g_tSpiTransmit.tx_data);	//send data
			g_tSpiTransmit.tx_data ++;
			count ++;
		}
		g_tSpiTransmit.tx_size --;
	}
	
	wSendStart = SPI_SEND_TIMEOUT;
	while(csp_spi_busy(ptSpiBase))								//wait for transmition finish
	{
		wSendStart --;
		if(wSendStart == 0)
			break;
	}
#endif

	g_tSpiTransmit.state.writeable = 1U;
	if(ret >= 0)
	{
		ret = (int32_t)count;
	}
	
	return ret;
}

/** \brief apt_spi_send_intr
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \size: number of data to send(byte)
 *  \return return the num of data or  return Error code
 */
static csi_error_t apt_spi_send_intr(csp_spi_t *ptSpiBase, const void *data, uint32_t size)
{
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)	//16bit
		g_tSpiTransmit.tx_size = size >> 1;
	else
		g_tSpiTransmit.tx_size = size;
		
	g_tSpiTransmit.tx_data = (uint8_t *)data;
	g_tSpiTransmit.priv = (void *)SPI_EVENT_SEND_COMPLETE;		//spi interrupt status
	csp_spi_en(ptSpiBase);								//enable spi
	csp_spi_set_int(ptSpiBase, SPI_TXIM_INT,true);		//enable tx fifo int
	
	return CSI_OK;
}

/** \brief sending data to spi transmitter, non-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_async(csp_spi_t *ptSpiBase, const void *data, uint32_t size)
{	
	csi_error_t ret = CSI_OK;
	
	if((g_tSpiTransmit.state.writeable == 0U))		// || (g_tSpiTransmit.state.readable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
		
	if(ret == CSI_OK)
	{
		g_tSpiTransmit.state.writeable = 0U;
		ret = apt_spi_send_intr(ptSpiBase, data, size);
	}
	else
		ret = CSI_ERROR;
	
	return ret;
}

/** \brief  receiving data from spi receiver, blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to receive
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_receive(csp_spi_t *ptSpiBase, void *data, uint32_t size, uint32_t timeout)
{	
	uint32_t  count = 0U;
	int32_t   ret = CSI_OK;
	
	do{
		if((g_tSpiTransmit.state.readable == 0U))		// || (g_tSpiTransmit.state.readable == 0U)) 
		{
            ret = CSI_BUSY;
            break;
        }
		
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)			//16bit
		{
			if (size % sizeof(uint16_t))
				return CSI_ERROR;
		}
		
		g_tSpiTransmit.state.readable = 0U;
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)			//16bit
			g_tSpiTransmit.rx_size = size >> 1;
		else
			g_tSpiTransmit.rx_size = size;
       
		g_tSpiTransmit.rx_data = (uint8_t *)data;
		csp_spi_en(ptSpiBase);										//enable spi
		
#ifdef SPI_RECV_TIMEOUT_DIS
		while(g_tSpiTransmit.rx_size)
		{
			//while(!csp_spi_read_ready(ptSpiBase));					
			while(!(csp_spi_get_sr(ptSpiBase) & SPI_RNE));			//recv fifo empty? wait	
			
			if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
			{
				 *(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	
				g_tSpiTransmit.rx_data += 2;
				count += 2;
			}
			else													//8bit
			{
				*g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);			//recv data
				g_tSpiTransmit.rx_data++;
				count ++;
			}

			g_tSpiTransmit.rx_size --;
		}
	
		while(csp_spi_busy(ptSpiBase));								//wait spi idle(transmition finish)
#else
		uint32_t wRecvStart = csi_tick_get_ms();
		while(g_tSpiTransmit.rx_size)
		{
			//while(!csp_spi_read_ready(ptSpiBase))
			while(!(csp_spi_get_sr(ptSpiBase) & SPI_RNE))			//recv fifo empty? wait	
			{
				if((csi_tick_get_ms() - wRecvStart) >= timeout) 
					return count;
			}
			
			if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
			{
				 *(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	
				g_tSpiTransmit.rx_data += 2;
				count += 2;
			}
			else													//8bit
			{
				*g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);			//recv data
				g_tSpiTransmit.rx_data++;
				count ++;
			}
			
			g_tSpiTransmit.rx_size --;
			wRecvStart = csi_tick_get_ms();
		}
#endif
	}while(0);
	
	g_tSpiTransmit.state.readable = 1U;
    if (ret >= 0) {
        ret = (int32_t)count;
    }
	
	return ret;
}

/** \brief apt_spi_recv_intr
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to receive
 *  \size: number of data to receive(byte)
 *  \return return the num of data or  return Error code
 */
static csi_error_t apt_spi_recv_intr(csp_spi_t *ptSpiBase, void *data, uint32_t size)
{
	csi_error_t ret = CSI_OK;
    
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
		g_tSpiTransmit.rx_size = size >> 1;
	else
		g_tSpiTransmit.rx_size = size;
	
	g_tSpiTransmit.rx_data = (uint8_t *)data;
	g_tSpiTransmit.priv = (void *)SPI_EVENT_RECEIVE_COMPLETE;		//spi interrupt status

	csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT,true);//enable rx fifo int
	csp_spi_en(ptSpiBase);								//enable spi
	
	return ret;
}

/** \brief  receiving data from spi receiver, not-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_receive_async(csp_spi_t *ptSpiBase, void *data, uint32_t size)
{	
	csi_error_t ret = CSI_OK;
	
	if ((g_tSpiTransmit.state.readable == 0U))	// || (g_tSpiTransmit.state.readable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
	
	if(ret == CSI_OK)
	{
		g_tSpiTransmit.state.readable = 0U;
		ret = apt_spi_recv_intr(ptSpiBase, data,size);
	}
	else
		ret = CSI_ERROR;
	
	return ret;
}

/** \brief  receiving data from spi receiver,blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
int32_t csi_spi_send_receive(csp_spi_t *ptSpiBase, const void *data_out, void *data_in, uint32_t size, uint32_t timeout)
{
	csi_error_t ret = CSI_OK;
	uint32_t count = 0U;
	uint32_t wValue;
	
	do{
		if((g_tSpiTransmit.state.writeable == 0U) || (g_tSpiTransmit.state.readable == 0U)) 
		{
            ret = CSI_BUSY;
            break;
        }
		
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
		{
			if (size % sizeof(uint16_t))
				return CSI_ERROR;
			
			g_tSpiTransmit.tx_size = size >> 1;
			g_tSpiTransmit.rx_size = size >> 1;
		}
		else												//8bit
		{
			g_tSpiTransmit.tx_size = size;
			g_tSpiTransmit.rx_size = size;
		}
		
		g_tSpiTransmit.state.writeable = 0U;
        g_tSpiTransmit.state.readable  = 0U;
		g_tSpiTransmit.tx_data = (uint8_t *)data_out;
		g_tSpiTransmit.rx_data = (uint8_t *)data_in;
		csp_spi_en(ptSpiBase);													//enable spi
		
#ifdef	SPI_SEND_TIMEOUT_DIS
		while((g_tSpiTransmit.tx_size > 0U) || (g_tSpiTransmit.rx_size > 0U))
		{
			if(g_tSpiTransmit.tx_size > 0U)
			{
				//send data
				while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));					//fifo full? wait; 
				if(data_out)
				{
					if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)				//16bit
					{
						wValue = (uint16_t)(*(uint16_t *)g_tSpiTransmit.tx_data);
						csp_spi_set_data(ptSpiBase, wValue);						//send data
						g_tSpiTransmit.tx_data += 2;
						count += 2;
					}
					else 														//8bit
					{
						csp_spi_set_data(ptSpiBase,*g_tSpiTransmit.tx_data);				//send data
						g_tSpiTransmit.tx_data ++;
						count ++;
					}
				}
				else
					csp_spi_set_data(ptSpiBase,0x00);							//send data
				
				g_tSpiTransmit.tx_size --;	
			}
			
			if(g_tSpiTransmit.rx_size > 0U)
			{
				//receive data
				while(!(csp_spi_get_sr(ptSpiBase) & SPI_RNE));					//recv fifo empty? wait	
				if(data_in)
				{
					if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)			//16bit
					{
						*(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	//receive data
						g_tSpiTransmit.rx_data += 2;
					} 
					else														//8bit
					{
						*g_tSpiTransmit.rx_data  = csp_spi_get_data(ptSpiBase);			//recv data
						g_tSpiTransmit.rx_data ++;
					}
				}
				else
					csp_spi_get_data(ptSpiBase);									//recv data
					
				g_tSpiTransmit.rx_size --;
			}
		}
#else
		uint32_t wTimeStart;
		while((g_tSpiTransmit.tx_size > 0U) || (g_tSpiTransmit.rx_size > 0U))
		{
			if(g_tSpiTransmit.tx_size > 0U)
			{
				wTimeStart = SPI_SEND_TIMEOUT;
				while(!csp_spi_write_ready(ptSpiBase) && wTimeStart --);		//send fifo full? wait; 
				if(wTimeStart ==0)
					break;
				
				if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)
				{
					wValue = (uint16_t)(*(uint16_t *)g_tSpiTransmit.tx_data);
					csp_spi_set_data(ptSpiBase, wValue);						//send data
					g_tSpiTransmit.tx_data += 2;
					count += 2;
				}
				else
				{
					csp_spi_set_data(ptSpiBase,*g_tSpiTransmit.tx_data);				//send data
					g_tSpiTransmit.tx_data ++;
					count ++;
				}
				
				g_tSpiTransmit.tx_size --;
			}
			
			if(g_tSpiTransmit.rx_size > 0U)
			{
				wTimeStart = SPI_RECV_TIMEOUT;
				while(!csp_spi_read_ready(ptSpiBase) && wTimeStart --);		//recv fifo empty? wait	
				//while(csp_spi_busy(ptSpiBase) && wTimeStart --);				//wait spi idle(transmition finish)
				if(wTimeStart ==0)
					break;
		
				if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)
				{
					*(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	//receive data
					g_tSpiTransmit.rx_data += 2;
				}
				else
				{
					*g_tSpiTransmit.rx_data  = csp_spi_get_data(ptSpiBase);			//recv data
					g_tSpiTransmit.rx_data ++;
				}
			
				g_tSpiTransmit.rx_size --;
			}
		}
#endif

	}while(0);
	
	while((csp_spi_get_sr(ptSpiBase) & SPI_BSY));		//wait for transmition finish
	
	g_tSpiTransmit.state.writeable = 1U;
    g_tSpiTransmit.state.readable  = 1U;

    if (ret >= 0) {
        ret = (int32_t)count;
    }
	
	return ret;
}

/** \brief  apt_spi_send_receive_intr
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
static csi_error_t apt_spi_send_receive_intr(csp_spi_t *ptSpiBase, const void *data_out, void *data_in, uint32_t size)
{
    csi_error_t ret = CSI_OK;
	
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
	{
		g_tSpiTransmit.tx_size = size >> 1;
		g_tSpiTransmit.rx_size = size >> 1;
	}
	else
	{
		g_tSpiTransmit.tx_size = size;
		g_tSpiTransmit.rx_size = size;
	}
	
	if(data_out)
		g_tSpiTransmit.tx_data = (uint8_t *)data_out;
	else 
		g_tSpiTransmit.tx_data = NULL;
	
	if(data_in)
		g_tSpiTransmit.rx_data = (uint8_t *)data_in;
	else
		g_tSpiTransmit.rx_data = NULL;
	
	g_tSpiTransmit.priv = (void *)SPI_EVENT_SEND_RECEIVE_COMPLETE;	//spi interrupt status
	csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT,true);	//enable rx fifo int
	csp_spi_set_int(ptSpiBase, SPI_TXIM_INT,true);					//enable tx fifo int
	csp_spi_en(ptSpiBase);											//enable spi
	
	return ret;
}

/** \brief  receiving data from spi receiver, not-blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_receive_async(csp_spi_t *ptSpiBase, const void *data_out, void *data_in, uint32_t size)
{
    csi_error_t ret = CSI_OK;
	
	if((g_tSpiTransmit.state.writeable == 0U) || (g_tSpiTransmit.state.readable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
		
	if(ret == CSI_OK) 
	{
		if(data_out)
			g_tSpiTransmit.state.writeable = 0U;
		else
			g_tSpiTransmit.state.writeable = 1U;
		if(data_in)
			g_tSpiTransmit.state.readable  = 0U;
		else
			g_tSpiTransmit.state.readable  = 1U;
			
		ret = apt_spi_send_receive_intr(ptSpiBase, data_out, data_in, size);
    } 
	else 
        ret = CSI_ERROR;
		
	return ret;
}

/** \brief  register spi interrupt callback function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] callback: after interrupt have done,then callback this function,Tells the user that interrupt processing is over(weak func),if not use
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_paraconfig_init(csp_spi_t *ptSpiBase, void *callback)
{
	g_tSpiTransmit.eSpiRxFifoLevel	 = SPI_RXFIFO_1_2; 
	
	#ifdef SPI_MASTER_SEL
		g_tSpiTransmit.eSpiMode		 = SPI_MASTER;
	#else
		g_tSpiTransmit.eSpiMode		 = SPI_SLAVE;
	#endif
	
	g_tSpiTransmit.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA1 ;
	g_tSpiTransmit.eSpiFrameLen 	 = SPI_FRAME_LEN_8;
	g_tSpiTransmit.dwSpiBaud 		 = 1000000;
	g_tSpiTransmit.rx_data =NULL;
	g_tSpiTransmit.rx_size =0;
	g_tSpiTransmit.tx_data =NULL;
	g_tSpiTransmit.tx_size =0;
	g_tSpiTransmit.state.writeable = 1;
	g_tSpiTransmit.state.readable  = 1;
	g_tSpiTransmit.state.error = 0;
		
	
	#ifdef SPI_SYNC_SEL
		g_tSpiTransmit.callback      = NULL;
		g_tSpiTransmit.arg           = NULL;
		g_tSpiTransmit.priv			 = NULL;
		csi_irq_disable((uint32_t *)ptSpiBase);
	#else		
		g_tSpiTransmit.callback      = callback;
		g_tSpiTransmit.arg           = (void *)(0x00000002);
		g_tSpiTransmit.priv			 = (void *) SPI_EVENT_ERROR;
		csi_irq_enable((uint32_t *)ptSpiBase);
	#endif

    return CSI_OK;
}

/** \brief spi interrupt callback function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] event: spi event 
 *  \param[in] arg: para
 *  \return none
 */
__attribute__((weak)) void apt_spi_event(csp_spi_t *ptSpiBase, csi_spi_event_t event, void *arg)
{
	switch(event)
	{
		case SPI_EVENT_SEND_COMPLETE:			//send complete		
			 //my_printf("the callback async send  complete!\n");	
			break;
		case SPI_EVENT_RECEIVE_COMPLETE:		//receive complete
			 //my_printf("the callback async receive complete!\n");
			break;
		case SPI_EVENT_SEND_RECEIVE_COMPLETE:	//send receive complete
			// my_printf("the callback async send_receive complete!\n");
			break;
		case SPI_EVENT_ERROR:					//
			 //my_printf("the callback async error!\n");
			break;
		default:
			break;
	}
}

/** \brief clr spi rx fifo
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */
void csi_spi_clrRx_fifo(csp_spi_t *ptSpiBase)
{
	uint8_t byIndex=8;
	while(byIndex--)
	{
		ptSpiBase->DR;//fifo deep is 8,read 8 times is clear rx fifo
	}
}

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */ 
uint16_t csi_spi_receive_slave(csp_spi_t *ptSpiBase)
{
	while(!(csp_spi_get_sr(ptSpiBase) & SPI_RNE));	//receive not empty:read
	return csp_spi_get_data(ptSpiBase);
}

/** \brief spi slave receive data
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out: data of send
 *  \return none
 */ 
csi_error_t csi_spi_send_slave(csp_spi_t *ptSpiBase, uint16_t data_out)
{
	while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));	//send fifo not full: write 	
	csp_spi_set_data(ptSpiBase, data_out);
	
	return CSI_OK;
}
//------------------------------------------------------------------------------------------
//interrupt process,just for reference
//------------------------------------------------------------------------------------------

/** \brief spi receive data,interrupt call 
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return none
 */ 
static void apt_spi_intr_recv_data(csp_spi_t *ptSpiBase)
{
	if((g_tSpiTransmit.rx_data == NULL) || (g_tSpiTransmit.rx_size == 0U))
	{
		
		if(g_tSpiTransmit.rx_data == NULL)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				csp_spi_get_data(ptSpiBase);								//read fifo 
				g_tSpiTransmit.rx_size--;
				if(g_tSpiTransmit.rx_size == 0)
					break;
			}
			
			if(g_tSpiTransmit.rx_size == 0)
			{
				g_tSpiTransmit.state.readable = 1U;
				csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);		//disable fifo rx int
			}
		}
		else
		{
			if(g_tSpiTransmit.callback) 
				g_tSpiTransmit.callback(ptSpiBase, SPI_EVENT_ERROR, g_tSpiTransmit.arg);		//error
		}
	}
	else
	{
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)				//16bit(two byte)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				*(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	//receive data
				g_tSpiTransmit.rx_data += 2;
				g_tSpiTransmit.rx_size--;
				if(g_tSpiTransmit.rx_size == 0)
					break;
			}
		}
		else														//8bit( one byte)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				*g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);			//receive data
				g_tSpiTransmit.rx_data++;
				g_tSpiTransmit.rx_size--;
				if(g_tSpiTransmit.rx_size == 0)
					break;
			}
		}
		
		if(g_tSpiTransmit.rx_size == 0)
		{
			g_tSpiTransmit.state.readable = 1U;
			csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int
			
			if(g_tSpiTransmit.callback)
				g_tSpiTransmit.callback(ptSpiBase, (csi_spi_event_t)g_tSpiTransmit.priv, g_tSpiTransmit.arg);
		}
	}
}

/** \brief spi send data,interrupt call
 * 
 *  \param[in] ptSpiBase: pointer of SPI reg structure.
 *  \return none
 */ 
static void apt_spi_intr_send_data(csp_spi_t *ptSpiBase)
{	
	if(g_tSpiTransmit.tx_data)
	{
		if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)			//16bit(two bite)
		{
			while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));
			csp_spi_set_data(ptSpiBase, (uint16_t)(*(uint16_t *)g_tSpiTransmit.tx_data));			//send data
			g_tSpiTransmit.tx_data += 2;
		}
		else													//8bit(one byte)
		{
			while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));
			csp_spi_set_data(ptSpiBase,*g_tSpiTransmit.tx_data);			//send data
			g_tSpiTransmit.tx_data++;
		}
	}
	else
		csp_spi_set_data(ptSpiBase,0x00);						//send data

	g_tSpiTransmit.tx_size --;
	
    if (g_tSpiTransmit.tx_size == 0U) 
	{
        g_tSpiTransmit.state.writeable = 1U;
		csp_spi_set_int(ptSpiBase, SPI_TXIM_INT, false);			//disable fifo tx int
		
		if((csi_spi_event_t)g_tSpiTransmit.priv != SPI_EVENT_SEND_RECEIVE_COMPLETE)		//data has been send_received, send end no callback
		{
			if(g_tSpiTransmit.callback)
				g_tSpiTransmit.callback(ptSpiBase, (csi_spi_event_t)g_tSpiTransmit.priv, g_tSpiTransmit.arg);
		}
		else
		{
			if(g_tSpiTransmit.rx_data == NULL)
			{
				if(g_tSpiTransmit.callback)
					g_tSpiTransmit.callback(ptSpiBase, (csi_spi_event_t)g_tSpiTransmit.priv, g_tSpiTransmit.arg);
			}
		}
    }
}

/** \brief spi interrupt handle weak function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */ 
__attribute__((weak)) void spi_irqhandler(csp_spi_t *ptSpiBase)
{	
	uint32_t status = csp_spi_get_isr(ptSpiBase);
	
	//fifo rx 
	if(status & SPI_RXIM_INT)
	{
		//You can replace it with your own function handling,The following is for reference only
		apt_spi_intr_recv_data(ptSpiBase);
	}
	//fifo tx 
	if(status & SPI_TXIM_INT)		
	{
		//You can replace it with your own function handling,The following is for reference only
		apt_spi_intr_send_data(ptSpiBase);
	}
	
	//fifo overflow
	if(status & SPI_ROTIM_INT)
	{	
		//You can replace it with your own function handling,The following is for reference only
		csi_spi_clrRx_fifo(ptSpiBase);
		csp_spi_clr_isr(ptSpiBase, SPI_ROTIM_INT);
	}
	
	//fifo rx timeout
	if(status & SPI_RTIM_INT)		
	{	
		//You can replace it with your own function handling,The following is for reference only
		csp_spi_clr_isr(ptSpiBase, SPI_RTIM_INT);
		
		for(uint8_t i = 0; i < 3; i++)
		{
			if(csp_spi_get_data_size(ptSpiBase) == SPI_DSS_16)			//16bit(two bite)
			{
				if(csp_spi_get_sr(ptSpiBase) & SPI_RNE)
				{
					*(uint16_t *)g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);	//receive data
					g_tSpiTransmit.rx_data += 2;				//send data
				}
				else
					break;
			}
			else													//8bit(one byte)
			{
				if(csp_spi_get_sr(ptSpiBase) & SPI_RNE)
				{
					*g_tSpiTransmit.rx_data = csp_spi_get_data(ptSpiBase);		//receive data
					g_tSpiTransmit.rx_data++;
				}
				else
					break;
			}
		}
		
		g_tSpiTransmit.rx_size = 0;
		g_tSpiTransmit.state.readable = 1U;
		csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int
		
		if(g_tSpiTransmit.callback)
				g_tSpiTransmit.callback(ptSpiBase, (csi_spi_event_t)g_tSpiTransmit.priv, g_tSpiTransmit.arg);

	}
}

//-----------------------------------------------------------------------------------------------------------
//high speed spi function for reference
//-----------------------------------------------------------------------------------------------------------

/** \brief spi_send_receive_1byte
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data :send data buffer pointer
 *  \return the receive data
 */ 
uint8_t spi_send_receive_1byte(csp_spi_t *ptSpiBase,uint8_t data)
{
	uint8_t receive_data = 0;
	uint32_t wTimeStart = SPI_SEND_TIMEOUT;
	 
	wTimeStart = SPI_SEND_TIMEOUT;
	while( ( !((uint32_t)(ptSpiBase->SR) & SPI_TNF) ) && (wTimeStart --) ){;} //send not full:write
	ptSpiBase->DR = data; //send data

	wTimeStart = SPI_SEND_TIMEOUT;
	while( (!((uint32_t)(ptSpiBase->SR) & SPI_RNE) ) && (wTimeStart --) ){;}  //receive not empty:read
	receive_data = ptSpiBase->DR;//receive data
	
	return receive_data;

}


/** \brief spi send buff(this funtion will ignore the receive)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data :send data buffer pointer
 *  \param[in] size ：length
 *  \return none
 */ 
void spi_buff_send(csp_spi_t *ptSpiBase,uint8_t *data,uint8_t size)
{
	uint8_t byIcount = 0;
	uint32_t wTimeStart = SPI_SEND_TIMEOUT;
	
	for(byIcount = 0;byIcount <size;byIcount++)
	{
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( !((uint32_t)(ptSpiBase->SR) & SPI_TNF) ) && (wTimeStart --) ){;}  //send not full:write
		ptSpiBase->DR = *(data+byIcount);
	}
	
	wTimeStart = SPI_SEND_TIMEOUT;//ensure spi process is finish
	while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} //not busy:finish

}

/** \brief spi send and receive(less than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out :send data buffer pointer
 *  \param[in] data_in :receive data buffer pointer
 *  \param[in] size ：length
 *  \return none
 */ 
void csi_spi_send_receive_x8(csp_spi_t *ptSpiBase, uint8_t *data_out,uint8_t *data_in,uint32_t size)//小于等于八个的发送接收,这种场景通常适合用来发送指令，读取状态。（大块的数据读取不合适）
{	
		uint8_t byCount = 0;
	    uint32_t wTimeStart = SPI_SEND_TIMEOUT;
		
		csi_spi_clrRx_fifo(ptSpiBase);
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;} 
		ptSpiBase->CR1 &= ~SPI_SSE_MSK;
		for(byCount = 0;byCount<size;byCount++)
		{
			ptSpiBase->DR = data_out[byCount];
		}
		ptSpiBase->CR1 |= (SPI_EN << SPI_SSE_POS);//SPI使能
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( (uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;} 
		
		//for(byCount = 0;byCount<size;byCount++)
		//{
			//data_out[byCount]=ptSpiBase->DR;
		//}
		data_in[0]=ptSpiBase->DR;
		data_in[1]=ptSpiBase->DR;
		data_in[2]=ptSpiBase->DR;
		data_in[3]=ptSpiBase->DR;
		data_in[4]=ptSpiBase->DR;
		data_in[5]=ptSpiBase->DR;
		data_in[6]=ptSpiBase->DR;
		data_in[7]=ptSpiBase->DR;
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( (uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;}
}

/** \brief spi send and receive(send equal to 8 bytes or  more than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] data_out :send data buffer pointer 
 *  \param[in] data_in  :send data buffer pointer 
 *  \param[in] size ：length
 *  \return none
 */ 
void csi_spi_send_receive_d8(csp_spi_t *ptSpiBase, uint8_t *data_out,uint8_t *data_in, uint32_t size)//大于八个的发送和读
{
		uint8_t tx_size = size;
		uint32_t wTimeStart = SPI_SEND_TIMEOUT;
#if 1
		uint8_t out_idx = 0;
		uint8_t i;
		uint8_t remainder = 0;
		uint8_t zheng_num = 0;
		uint8_t last_8_times = 0;
		uint8_t last_tx_buff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

		csi_spi_clrRx_fifo(ptSpiBase);

		zheng_num = (size >> 3);
		remainder = size & 0x07;
		last_8_times = zheng_num << 3;//position
		/*if(remainder != 0){			
			for(i=0;i<remainder;i++)
				last_tx_buff[i] = data_out[last_8_times+i];
		}*/
		memcpy((void *)last_tx_buff,(void *)&data_out[last_8_times], remainder);
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 

		while(tx_size >=8U)
		{
			ptSpiBase->DR = data_out[out_idx];
			ptSpiBase->DR = data_out[out_idx+1];  //csp_spi_set_data(spi_base,*spi->tx_data);	//send data
			ptSpiBase->DR = data_out[out_idx+2];
			ptSpiBase->DR = data_out[out_idx+3];
			ptSpiBase->DR = data_out[out_idx+4];
			ptSpiBase->DR = data_out[out_idx+5];
			ptSpiBase->DR = data_out[out_idx+6];
			ptSpiBase->DR = data_out[out_idx+7];
	
			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 		

			tx_size -= 8;
			data_in[out_idx] = ptSpiBase->DR;
			data_in[out_idx+1] = ptSpiBase->DR;
			data_in[out_idx+2] = ptSpiBase->DR;
			data_in[out_idx+3] = ptSpiBase->DR;
			data_in[out_idx+4] = ptSpiBase->DR;
			data_in[out_idx+5] = ptSpiBase->DR;
			data_in[out_idx+6] = ptSpiBase->DR;
			data_in[out_idx+7] = ptSpiBase->DR;
			
			out_idx += 8;	
		}
		
	if(remainder != 0){	
		if(remainder == 1)
		{
			ptSpiBase->DR = last_tx_buff[0];
		}
		else if(remainder == 2)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];
		}
		else if(remainder == 3)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];
		}
		else if(remainder == 4)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];ptSpiBase->DR = last_tx_buff[3];
		}
		else if(remainder == 5)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];ptSpiBase->DR = last_tx_buff[3];
			ptSpiBase->DR = last_tx_buff[4];
		}
		else if(remainder == 6)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];ptSpiBase->DR = last_tx_buff[3];
			ptSpiBase->DR = last_tx_buff[4];ptSpiBase->DR = last_tx_buff[5];
		}
		else if(remainder == 7)
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];ptSpiBase->DR = last_tx_buff[3];
			ptSpiBase->DR = last_tx_buff[4];ptSpiBase->DR = last_tx_buff[5];ptSpiBase->DR = last_tx_buff[6];
		}
		else
		{
			ptSpiBase->DR = last_tx_buff[0];ptSpiBase->DR = last_tx_buff[1];ptSpiBase->DR = last_tx_buff[2];ptSpiBase->DR = last_tx_buff[3];
			ptSpiBase->DR = last_tx_buff[4];ptSpiBase->DR = last_tx_buff[5];ptSpiBase->DR = last_tx_buff[6];ptSpiBase->DR = last_tx_buff[7];
		}
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 

		for(i=0;i<remainder;i++)		//read buffer data
			data_in[out_idx+i] = ptSpiBase->DR;		
	}

#else 	
		csi_spi_clrRx_fifo(ptSpiBase);
		tx_size = size;

		while(tx_size > 0U)
		{
			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 
			ptSpiBase->DR = *data_out;  //csp_spi_set_data(spi_base,*spi->tx_data);	//send data

			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 
		
			tx_size --;
			*data_in = ptSpiBase->DR;
			data_out ++;
			data_in ++;
			
		}
	wTimeStart = SPI_SEND_TIMEOUT;
	while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 	

#endif 	

}