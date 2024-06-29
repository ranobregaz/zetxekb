/***********************************************************************//** 
 * \file  spi.c
 * \brief  csi spi driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-28 <td>V0.0  <td>ZJY   <td>initial
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

#include "csp_spi.h"

/* Private macro-----------------------------------------------------------*/

/* externs function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/

/* Private variablesr------------------------------------------------------*/
static csi_spi_mode_t	work_mode	=	SPI_MASTER;
/** \brief spi receive data,interrupt call 
 * 
 *  \param[in] spi: pointer of SPI reg structure.
 *  \return none
 */ 
static void spi_intr_recv_data(csi_spi_t *spi)
{
    csp_spi_t *spi_base  = (csp_spi_t *)HANDLE_REG_BASE(spi); 
	
	if((spi->rx_data == NULL) || (spi->rx_size == 0U))
	{
		
		if(spi->rx_data == NULL)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				csp_spi_get_data(spi_base);								//read fifo 
				spi->rx_size--;
				if(spi->rx_size == 0)
					break;
			}
			
			if(spi->rx_size == 0)
			{
				spi->state.readable = 1U;
				csp_spi_set_int(spi_base, SPI_RXIM_INT | SPI_RTIM_INT, false);		//disable fifo rx int
			}
		}
		else
		{
			if(spi->callback) 
				spi->callback(spi, SPI_EVENT_ERROR, spi->arg);		//error
		}
	}
	else
	{
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)				//16bit(two byte)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				*(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	//receive data
				spi->rx_data += 2;
				spi->rx_size--;
				if(spi->rx_size == 0)
					break;
			}
		}
		else														//8bit( one byte)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				*spi->rx_data = csp_spi_get_data(spi_base);			//receive data
				spi->rx_data++;
				spi->rx_size--;
				if(spi->rx_size == 0)
					break;
			}
		}
		
		if(spi->rx_size == 0)
		{
			spi->state.readable = 1U;
			csp_spi_set_int(spi_base, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int
			
			if(spi->callback)
				spi->callback(spi, (csi_spi_event_t)spi->priv, spi->arg);
		}
	}
}
/** \brief spi send data,interrupt call
 * 
 *  \param[in] spi: pointer of SPI reg structure.
 *  \return none
 */ 
static void spi_intr_send_data(csi_spi_t *spi)
{
    csp_spi_t *spi_base  = (csp_spi_t *)HANDLE_REG_BASE(spi); 
	
	if(spi->tx_data)
	{
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)			//16bit(two bite)
		{
			while(!(csp_spi_get_sr(spi_base) & SPI_TNF));
			csp_spi_set_data(spi_base, (uint16_t)(*(uint16_t *)spi->tx_data));			//send data
			spi->tx_data += 2;
		}
		else													//8bit(one byte)
		{
			while(!(csp_spi_get_sr(spi_base) & SPI_TNF));
			csp_spi_set_data(spi_base,*spi->tx_data);			//send data
			spi->tx_data++;
		}
	}
	else
		csp_spi_set_data(spi_base,0x00);						//send data

	spi->tx_size --;
	
    if (spi->tx_size == 0U) 
	{
        spi->state.writeable = 1U;
		csp_spi_set_int(spi_base, SPI_TXIM_INT, false);			//disable fifo tx int
		
		if((csi_spi_event_t)spi->priv != SPI_EVENT_SEND_RECEIVE_COMPLETE)		//data has been send_received, send end no callback
		{
			if(spi->callback)
				spi->callback(spi, (csi_spi_event_t)spi->priv, spi->arg);
		}
		else
		{
			if(spi->rx_data == NULL)
			{
				if(spi->callback)
					spi->callback(spi, (csi_spi_event_t)spi->priv, spi->arg);
			}
		}
    }
}
/** \brief spi interrupt handle function
 * 
 *  \param[in] args: pointer of spi dev
 *  \return none
 */ 
static void apt_spi_irqhandler(void *args)
{
    csi_spi_t *spi = (csi_spi_t *)args;
    csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	uint32_t status = csp_spi_get_isr(spi_base);
	
	//fifo rx 
	if(status & SPI_RXIM_INT)
	{
		//csp_spi_clr_isr(spi_base, SPI_RXIM_INT);
		//if(work_mode == SPI_MASTER)
			spi_intr_recv_data(spi);
		//else
//		{
//			if(spi->callback)
//				spi->callback(spi, SPI_EVENT_RECEIVE_SLAVE_COMPLE, spi->arg);
//			
//			spi->state.readable = 1;
//		}
			
	}
	//fifo tx 
	if(status & SPI_TXIM_INT)		
	{
		csp_spi_clr_isr(spi_base, SPI_TXIM_INT);
		spi_intr_send_data(spi);
	}
	
	//
//	if(status & SPI_ROTIM_INT)		
//	{
//		csp_spi_clr_isr(spi_base, SPI_ROTIM_INT);
//		if(spi->callback)
//				spi->callback(spi, SPI_EVENT_ERROR_OVERFLOW, spi->arg);
//	}
	
	//
	if(status & SPI_RTIM_INT)		
	{
		csp_spi_clr_isr(spi_base, SPI_RTIM_INT);
		
		for(uint8_t i = 0; i < 3; i++)
		{
			if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)			//16bit(two bite)
			{
				if(csp_spi_get_sr(spi_base) & SPI_RNE)
				{
					*(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	//receive data
					spi->rx_data += 2;				//send data
				}
				else
					break;
			}
			else													//8bit(one byte)
			{
				if(csp_spi_get_sr(spi_base) & SPI_RNE)
				{
					*spi->rx_data = csp_spi_get_data(spi_base);		//receive data
					spi->rx_data++;
				}
				else
					break;
			}
		}
		
		spi->rx_size = 0;
		spi->state.readable = 1U;
		csp_spi_set_int(spi_base, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int
		
		if(spi->callback)
				spi->callback(spi, (csi_spi_event_t)spi->priv, spi->arg);

	}
}
/** \brief initialize spi data structure
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] port_idx: spi index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_init(csi_spi_t *spi, uint32_t idx)
{
	CSI_PARAM_CHK(spi, CSI_ERROR);

	csi_error_t ret = CSI_OK;
    csp_spi_t *spi_base;
	
	if (target_get(DEV_SPI_TAG, idx, &spi->dev) != CSI_OK)
		ret = CSI_ERROR;
    else
	{
		
		spi->state.writeable = 1U;
        spi->state.readable  = 1U;
        spi->state.error     = 0U;
        spi->send            = NULL;
        spi->receive         = NULL;
        spi->send_receive    = NULL;
        spi->rx_dma          = NULL;
        spi->tx_dma          = NULL;
        spi->rx_data         = NULL;
        spi->tx_data         = NULL;
        spi->callback        = NULL;
        spi->arg             = NULL;
		spi->priv			 = (void *)SPI_EVENT_ERROR;
		
		spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
		
		csi_clk_enable(&spi->dev);				//spi peripheral clk en
		csp_spi_default_init(spi_base);			//reset all registers
		csp_spi_dis(spi_base);					//disable spi
		//csp_spi_set_lbm(spi_base, SPI_LBM_1);	//enable spi lbm, dbug
		csp_spi_set_rxifl(spi_base, SPI_RXFIFO_1_2);
		//csp_spi_en(spi_base);					//enable spi
	}
	return ret;
}
/** \brief de-initialize spi interface
 * 
 *  \param[in] spi: SPI handle
 *  \return none
 */
void csi_spi_uninit(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);

    csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	spi->state.writeable = 0U;
	spi->state.readable  = 0U;
	spi->state.error     = 0U;
	spi->send            = NULL;
	spi->receive         = NULL;
	spi->send_receive    = NULL;
	spi->rx_dma          = NULL;
	spi->tx_dma          = NULL;
	spi->rx_data         = NULL;
	spi->tx_data         = NULL;
	spi->callback        = NULL;
	spi->arg             = NULL;
	
	csp_spi_default_init(spi_base);					//reset all registers
	csi_irq_disable((uint32_t)spi->dev.irq_num);	//unregister irq 
    csi_irq_detach((uint32_t)spi->dev.irq_num);
}
/** \brief set spi mode, master or slave
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] mode: master, slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_mode(csi_spi_t *spi, csi_spi_mode_t mode)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    csi_error_t   ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	work_mode = mode;
	
	switch (mode) 			//configure spi mode
	{
        case SPI_MASTER:
            csp_spi_set_mode(spi_base, SPI_MODE_MASTER);
            break;
        case SPI_SLAVE:
            csp_spi_set_mode(spi_base, SPI_MODE_SLAVE);
            break;
        default:
            ret = CSI_ERROR;
            break;
    }
	
	return ret;
}
/** \brief config spi cp format
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] format: spi cp format
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_cp_format(csi_spi_t *spi, csi_spi_cp_format_t format)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    csi_error_t   ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	switch (format)		//configure spi format 
	{
        case SPI_FORMAT_CPOL0_CPHA0:
			csp_spi_set_spo_sph(spi_base, SPI_SPO0_SPH0);
            break;
        case SPI_FORMAT_CPOL0_CPHA1:
			csp_spi_set_spo_sph(spi_base, SPI_SPO0_SPH1);
            break;
        case SPI_FORMAT_CPOL1_CPHA0:
			csp_spi_set_spo_sph(spi_base, SPI_SPO1_SPH0);
            break;
        case SPI_FORMAT_CPOL1_CPHA1:
			csp_spi_set_spo_sph(spi_base, SPI_SPO1_SPH1);
            break;
        default:
            ret = CSI_ERROR;
            break;
    }
	
	return ret;
} 
/** \brief config spi work frequence
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] baud: spi work baud
 *  \return spi config frequency
 */
uint32_t csi_spi_baud(csi_spi_t *spi, uint32_t baud)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(baud, CSI_ERROR);

    uint32_t div;
    uint32_t freq = 0U;
	
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);

	div = (soc_get_pclk_freq() >> 1) / baud;
	
	if(div > 0)
		div --;
	
	csp_spi_set_clk_div(spi_base, div, 2);

	if(div > 0)
		freq = (soc_get_pclk_freq() >> 1) / (div + 1);
	else
		freq = (soc_get_pclk_freq() >> 1) ;
		
    return freq;
}
/** \brief config spi frame length
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] length: frame length
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_frame_len(csi_spi_t *spi, csi_spi_frame_len_t length)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
	
    csi_error_t   ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);

    if ((length < SPI_FRAME_LEN_4) || (length > SPI_FRAME_LEN_16)) 
        ret = CSI_ERROR;
	else 
	{
        csp_spi_set_data_size(spi_base, (length -1));			 //configura data frame width
        //spi->priv = (void *)(length -1);
    }
	
    return ret;
}
/** \brief sending data to spi transmitter(received data is ignored),blocking mode
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_send(csi_spi_t *spi, const void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_spi_t 	*spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	uint32_t count = 0U;
	uint32_t wValue;
	
	if((spi->state.writeable == 0U))						
		return CSI_BUSY;
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)	//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
		spi->tx_size = size >> 1;
	}
	else
		spi->tx_size = size;

	spi->state.writeable = 0U;
	spi->tx_data = (uint8_t *)data;
	csp_spi_en(spi_base);								//enable spi
	
#ifdef	SPI_SEND_TIMEOUT_DIS							//spi timeout disable
	while(spi->tx_size > 0)
	{
		while(!(csp_spi_get_sr(spi_base) & SPI_TNF));	//fifo full? wait; 		
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)	//two byte
		{
			wValue = (uint16_t)(*(uint16_t *)spi->tx_data);
			csp_spi_set_data(spi_base, wValue);			//send data
			spi->tx_data += 2;
			count += 2;
		}
		else											//one byte
		{
			csp_spi_set_data(spi_base, *spi->tx_data);	//send data
			spi->tx_data ++;
			count ++;
		}
		
		spi->tx_size --;
	}
	
	while((csp_spi_get_sr(spi_base) & SPI_BSY));		//wait for transmition finish
#else

	uint32_t wSendStart;
	while(spi->tx_size > 0)
	{
		wSendStart = SPI_SEND_TIMEOUT;
		while(!csp_spi_write_ready(spi_base) && wSendStart --);	//fifo full? wait; 
		if(wSendStart == 0)
			break;
		
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)	//two byte
		{
			wValue = (uint16_t)(*(uint16_t *)spi->tx_data);
			spi->tx_data += 2;
			count += 2;
			csp_spi_set_data(spi_base, wValue);			//send data
		}
		else											//one byte
		{
			csp_spi_set_data(spi_base, *spi->tx_data);	//send data
			spi->tx_data ++;
			count ++;
		}
		spi->tx_size --;
	}
	
	wSendStart = SPI_SEND_TIMEOUT;
	while(csp_spi_busy(spi_base))								//wait for transmition finish
	{
		wSendStart --;
		if(wSendStart == 0)
			break;
	}
#endif

	spi->state.writeable = 1U;
	if(ret >= 0)
		ret = (int32_t)count;
	
	return ret;
}
static csi_error_t apt_spi_send_intr(csi_spi_t *spi, const void *data, uint32_t size)
{
	//csi_error_t ret = CSI_OK;
    csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)	//16bit
		spi->tx_size = size >> 1;
	else
		spi->tx_size = size;
	spi->tx_data = (uint8_t *)data;
	spi->priv = (void *)SPI_EVENT_SEND_COMPLETE;		//spi interrupt status
	
	csp_spi_en(spi_base);								//enable spi
	csp_spi_set_int(spi_base, SPI_TXIM_INT,true);		//enable tx fifo int
	
	return CSI_OK;
}
/** \brief sending data to spi transmitter, non-blocking mode(interrupt mode)
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to send(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
	CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	if((spi->state.writeable == 0U))		// || (spi->state.readable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
		
	if((ret == CSI_OK) && spi->send)
	{
		spi->state.writeable = 0U;
		ret = spi->send(spi, data, size);
	}
	else
		ret = CSI_ERROR;
	
	return ret;
}
/** \brief  receiving data from spi receiver, blocking mode
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_receive(csi_spi_t *spi, void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	uint32_t  count = 0U;
	int32_t   ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	do{
		if((spi->state.readable == 0U))		// || (spi->state.writeable == 0U)) 
		{
            ret = CSI_BUSY;
            break;
        }
		
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)			//16bit
		{
			if (size % sizeof(uint16_t))
				return CSI_ERROR;
		}
		
		spi->state.readable = 0U;
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)			//16bit
			spi->rx_size = size >> 1;
		else
			spi->rx_size = size;
       
		spi->rx_data = (uint8_t *)data;
		csp_spi_en(spi_base);										//enable spi
		
#ifdef SPI_RECV_TIMEOUT_DIS
		while(spi->rx_size)
		{
			//while(!csp_spi_read_ready(spi_base));					
			while(!(csp_spi_get_sr(spi_base) & SPI_RNE));			//recv fifo empty? wait	
			
			if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
			{
				 *(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	
				spi->rx_data += 2;
				count += 2;
			}
			else													//8bit
			{
				*spi->rx_data = csp_spi_get_data(spi_base);			//recv data
				spi->rx_data++;
				count ++;
			}

			spi->rx_size --;
		}
	
		while(csp_spi_busy(spi_base));								//wait spi idle(transmition finish)
#else
		uint32_t wRecvStart = csi_tick_get_ms();
		while(spi->rx_size)
		{
			//while(!csp_spi_read_ready(spi_base))
			while(!(csp_spi_get_sr(spi_base) & SPI_RNE))			//recv fifo empty? wait	
			{
				if((csi_tick_get_ms() - wRecvStart) >= timeout) 
					return count;
			}
			
			if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
			{
				 *(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	
				spi->rx_data += 2;
				count += 2;
			}
			else													//8bit
			{
				*spi->rx_data = csp_spi_get_data(spi_base);			//recv data
				spi->rx_data++;
				count ++;
			}
			
			spi->rx_size --;
			wRecvStart = csi_tick_get_ms();
		}
		
//		wRecvStart = csi_tick_get_ms();
//		while(csp_spi_busy(spi_base))			//wait spi idle(transmition finish)
//		{
//			if((csi_tick_get_ms() - wRecvStart) >= timeout) 
//				break;
//		}
#endif
	}while(0);
	
	spi->state.readable = 1U;
    if (ret >= 0) {
        ret = (int32_t)count;
    }
	
	return ret;
}

static csi_error_t apt_spi_recv_intr(csi_spi_t *spi, void *data, uint32_t size)
{
	//csi_error_t ret = CSI_OK;
    csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
		spi->rx_size = size >> 1;
	else
		spi->rx_size = size;
	
	spi->rx_data = (uint8_t *)data;
	spi->priv = (void *)SPI_EVENT_RECEIVE_COMPLETE;		//spi interrupt status

	csp_spi_set_int(spi_base, SPI_RXIM_INT | SPI_RTIM_INT,true);//enable rx fifo int
	csp_spi_en(spi_base);								//enable spi
	
	return CSI_OK;
}
/** \brief  receiving data from spi receiver, not-blocking mode(interrupt mode)
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data: pointer to buffer with data to send to spi transmitter
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{
	CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	if ((spi->state.readable == 0U))	// || (spi->state.writeable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
	
	if((ret == CSI_OK) && spi->receive)
	{
		spi->state.readable = 0U;
		ret = apt_spi_recv_intr(spi, data,size);
	}
	else
		ret = CSI_ERROR;
	
	return ret;
}
/** \brief  receiving data from spi receiver,blocking mode
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \param[in] timeout: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
int32_t csi_spi_send_receive(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(spi,      CSI_ERROR);
    //CSI_PARAM_CHK(data_out, CSI_ERROR);
    //CSI_PARAM_CHK(data_in,  CSI_ERROR);
    CSI_PARAM_CHK(size,     CSI_ERROR);

	csi_error_t ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	uint32_t count = 0U;
	uint32_t wValue;
	
	do{
		if((spi->state.writeable == 0U) || (spi->state.readable == 0U)) 
		{
            ret = CSI_BUSY;
            break;
        }
		
		if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
		{
			if (size % sizeof(uint16_t))
				return CSI_ERROR;
			
			spi->tx_size = size >> 1;
			spi->rx_size = size >> 1;
		}
		else												//8bit
		{
			spi->tx_size = size;
			spi->rx_size = size;
		}
		
		spi->state.writeable = 0U;
        spi->state.readable  = 0U;
		spi->tx_data = (uint8_t *)data_out;
		spi->rx_data = (uint8_t *)data_in;
		csp_spi_en(spi_base);													//enable spi
		
#ifdef	SPI_SEND_TIMEOUT_DIS
		while((spi->tx_size > 0U) || (spi->rx_size > 0U))
		{
			if(spi->tx_size > 0U)
			{
				//send data
				while(!(csp_spi_get_sr(spi_base) & SPI_TNF));					//fifo full? wait; 
				if(data_out)
				{
					if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)				//16bit
					{
						wValue = (uint16_t)(*(uint16_t *)spi->tx_data);
						csp_spi_set_data(spi_base, wValue);						//send data
						spi->tx_data += 2;
						count += 2;
					}
					else 														//8bit
					{
						csp_spi_set_data(spi_base,*spi->tx_data);				//send data
						spi->tx_data ++;
						count ++;
					}
				}
				else
					csp_spi_set_data(spi_base,0x00);							//send data
				
				spi->tx_size --;	
			}
			
			if(spi->rx_size > 0U)
			{
				//receive data
				while(!(csp_spi_get_sr(spi_base) & SPI_RNE));					//recv fifo empty? wait	
				if(data_in)
				{
					if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)			//16bit
					{
						*(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	//receive data
						spi->rx_data += 2;
					} 
					else														//8bit
					{
						*spi->rx_data  = csp_spi_get_data(spi_base);			//recv data
						spi->rx_data ++;
					}
				}
				else
					csp_spi_get_data(spi_base);									//recv data
					
				spi->rx_size --;
			}
		}
#else
		uint32_t wTimeStart;
		while((spi->tx_size > 0U) || (spi->rx_size > 0U))
		{
			if(spi->tx_size > 0U)
			{
				wTimeStart = SPI_SEND_TIMEOUT;
				while(!csp_spi_write_ready(spi_base) && wTimeStart --);		//send fifo full? wait; 
				if(wTimeStart ==0)
					break;
				
				if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)
				{
					wValue = (uint16_t)(*(uint16_t *)spi->tx_data);
					csp_spi_set_data(spi_base, wValue);						//send data
					spi->tx_data += 2;
					count += 2;
				}
				else
				{
					csp_spi_set_data(spi_base,*spi->tx_data);				//send data
					spi->tx_data ++;
					count ++;
				}
				
				spi->tx_size --;
			}
			
			if(spi->rx_size > 0U)
			{
				wTimeStart = SPI_RECV_TIMEOUT;
				while(!csp_spi_read_ready(spi_base) && wTimeStart --);		//recv fifo empty? wait	
				//while(csp_spi_busy(spi_base) && wTimeStart --);				//wait spi idle(transmition finish)
				if(wTimeStart ==0)
					break;
		
				if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)
				{
					*(uint16_t *)spi->rx_data = csp_spi_get_data(spi_base);	//receive data
					spi->rx_data += 2;
				}
				else
				{
					*spi->rx_data  = csp_spi_get_data(spi_base);			//recv data
					spi->rx_data ++;
				}
			
				spi->rx_size --;
			}
		}
#endif

	}while(0);
	
	while((csp_spi_get_sr(spi_base) & SPI_BSY));		//wait for transmition finish
	
	spi->state.writeable = 1U;
    spi->state.readable  = 1U;

    if (ret >= 0) {
        ret = (int32_t)count;
    }
	
	return ret;
}
static csi_error_t apt_spi_send_receive_intr(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
   // csi_error_t ret = CSI_OK;
    csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);

	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
	{
		spi->tx_size = size >> 1;
		spi->rx_size = size >> 1;
	}
	else
	{
		spi->tx_size = size;
		spi->rx_size = size;
	}
	
	if(data_out)
		spi->tx_data = (uint8_t *)data_out;
	else 
		spi->tx_data = NULL;
	
	if(data_in)
		spi->rx_data = (uint8_t *)data_in;
	else
		spi->rx_data = NULL;
	
	spi->priv = (void *)SPI_EVENT_SEND_RECEIVE_COMPLETE;			//spi interrupt status
	csp_spi_set_int(spi_base, SPI_RXIM_INT | SPI_RTIM_INT,true);	//enable rx fifo int
	csp_spi_set_int(spi_base, SPI_TXIM_INT,true);					//enable tx fifo int
	csp_spi_en(spi_base);											//enable spi
	
	return CSI_OK;
}
/** \brief  receiving data from spi receiver, not-blocking mode
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data_out: pointer to buffer with data to send to spi transmitter
 *  \param[in] data_in: number of data to receive(byte)
 *  \param[in] size: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_receive_async(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    //CSI_PARAM_CHK(data_out, CSI_ERROR);
    //CSI_PARAM_CHK(data_in, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_error_t ret = CSI_OK;
	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	if((spi->state.writeable == 0U) || (spi->state.readable == 0U)) 
        ret = CSI_BUSY;
	
	if(csp_spi_get_data_size(spi_base) == SPI_DSS_16)		//16bit
	{
		if (size % sizeof(uint16_t))
			return CSI_ERROR;
	}
		
	if((ret == CSI_OK) && spi->send_receive) 
	{
		if(data_out)
			spi->state.writeable = 0U;
		else
			spi->state.writeable = 1U;
		if(data_in)
			spi->state.readable  = 0U;
		else
			spi->state.readable  = 1U;
			
		ret = apt_spi_send_receive_intr(spi, data_out, data_in, size);
    } 
	else 
        ret = CSI_ERROR;
		
	return ret;
}
/** \brief  register spi interrupt callback function
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] callback: spi interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_attach_callback(csi_spi_t *spi, void *callback, void *arg)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    spi->callback     = callback;
    spi->arg          = arg;
    spi->send         = apt_spi_send_intr;
    spi->receive      = apt_spi_recv_intr;
    spi->send_receive = apt_spi_send_receive_intr;
	
	csi_irq_attach((uint32_t)spi->dev.irq_num, &apt_spi_irqhandler, &spi->dev);
	csi_irq_enable((uint32_t)spi->dev.irq_num);

    return CSI_OK;
}
/** \brief cancel spi callback function
 * 
 *  \param[in] spi: SPI handle
 *  \return none
 */ 
void csi_spi_detach_callback(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);

    spi->callback     = NULL;
    spi->arg          = NULL;
    spi->send         = NULL;
    spi->receive      = NULL;
    spi->send_receive = NULL;
	
	csi_irq_disable((uint32_t)(spi->dev.irq_num));
    csi_irq_detach((uint32_t)(spi->dev.irq_num));
	
}
csi_error_t csi_spi_link_dma(csi_spi_t *spi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
	/*
    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        tx_dma->parent = spi;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(tx_dma, dw_spi_dma_event_cb, NULL);
            spi->tx_dma = tx_dma;
            spi->send = dw_spi_send_dma;
        } else {
            tx_dma->parent = NULL;
        }
    } else {
        if (spi->tx_dma) {
            csi_dma_ch_detach_callback(spi->tx_dma);
            csi_dma_ch_free(spi->tx_dma);
            spi->tx_dma = NULL;
        }

        if (spi->callback != NULL) {
            spi->send = dw_spi_send_intr;
        } else {
            spi->send = NULL;
        }
    }

    if (ret == CSI_OK) {
        if (rx_dma != NULL) {
            rx_dma->parent = spi;
            ret = csi_dma_ch_alloc(rx_dma, -1, -1);

            if (ret == CSI_OK) {
                csi_dma_ch_attach_callback(rx_dma, dw_spi_dma_event_cb, NULL);
                spi->rx_dma = rx_dma;
                spi->receive = dw_spi_receive_dma;
            } else {
                rx_dma->parent = NULL;
            }
        } else {
            if (spi->rx_dma) {
                csi_dma_ch_detach_callback(spi->rx_dma);
                csi_dma_ch_free(spi->rx_dma);
                spi->rx_dma = NULL;
            }

            if (spi->callback != NULL) {
                spi->receive = dw_spi_receive_intr;
            } else {
                spi->receive = NULL;
            }
        }
    }


    if (ret == CSI_OK) {
        if ((tx_dma != NULL) && (rx_dma != NULL)) {
            spi->send_receive =  dw_spi_send_receive_dma;
        }

        if ((tx_dma == NULL) || (rx_dma == NULL)) {
            spi->send_receive = (spi->callback != NULL) ? dw_spi_send_receive_intr : NULL;
        }
    }
	*/
    return CSI_UNSUPPORTED;
}

/** \brief get the state of spi device
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] state: the state of spi device
 *  \return none
 */ 
csi_error_t csi_spi_get_state(csi_spi_t *spi, csi_state_t *state)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);

    *state = spi->state;
    return CSI_OK;
}
/** \brief set slave select num. only valid for master
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] slave_num: spi slave num
 *  \return none
 */ 
void csi_spi_select_slave(csi_spi_t *spi, uint32_t slave_num)
{
    CSI_PARAM_CHK_NORETVAL(spi);

   // csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	//csp_spi_set_mode(spi_base,SPI_MODE_SLAVE);
	//dw_spi_enable_slave(spi_base, slave_num);
}

/** \brief spi slave receive data
 * 
 *  \param[in] spi: SPI handle
 *  \return none
 */ 
uint16_t csi_spi_receive_slave(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);

	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	return (uint16_t)csp_spi_get_data(spi_base);

}

/** \brief spi slave receive data
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] data_out: data of send
 *  \return none
 */ 
csi_error_t csi_spi_send_slave(csi_spi_t *spi, uint16_t data_out)
{
    CSI_PARAM_CHK_NORETVAL(spi);

	csp_spi_t *spi_base = (csp_spi_t *)HANDLE_REG_BASE(spi);
	
	while(!(csp_spi_get_sr(spi_base) & SPI_TNF));	//fifo full? wait; 	
	csp_spi_set_data(spi_base, data_out);
	
	return CSI_OK;
}
