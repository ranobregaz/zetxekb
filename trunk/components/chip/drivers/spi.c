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

csi_spi_transmit_t g_tSpiTransmit; 
/** \brief init spi gpio 
 * 
 *  \param[in] eMode:master or slave
 *  \return none
 */ 
static void apt_spi_gpio_init(csi_spi_mode_e eMode)
{
	if(SPI_MASTER == eMode)
	{
		uint32_t wPinMask = 0x01 << (PB05-16);							//pb0_5 control nss
		csi_gpio_port_dir(GPIOB0, wPinMask, GPIO_DIR_OUTPUT);			//gpio_port as output 
	}
	else
	{
		csi_pin_set_mux(PB05,PB05_SPI_NSS);
	}
	csi_pin_set_mux(PB04,PB04_SPI_SCK);								//PB04 = SPI_SCK
	csi_pin_set_mux(PA015,PA015_SPI_MISO);							//PA15 = SPI_MISO
	csi_pin_set_mux(PA014,PA014_SPI_MOSI);							//PA14 = SPI_MOSI
	SPICS_SET;													    //NSS init high	
}

/** \brief apt_spi_int_set 
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] eSpiInt:spi interrupt source 
 *  \return none
 */ 
static void apt_spi_int_set(csp_spi_t *ptSpiBase,spi_int_e eSpiInt)
{
	if(eSpiInt != SPI_NONE_INT)//收发使用中断设置
	{
		csi_irq_enable((uint32_t *)ptSpiBase);
		//csp_spi_set_int(ptSpiBase, eSpiInt,true);
	}
	else//不使用中断则关闭
	{
		csi_irq_disable((uint32_t *)ptSpiBase);
	}
}

/** \brief initialize spi data structure
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] ptSpiCfg: user spi parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_init(csp_spi_t *ptSpiBase,csi_spi_config_t *ptSpiCfg)
{
	csi_error_t tRet = CSI_OK;
	
	apt_spi_gpio_init(ptSpiCfg->eSpiMode);			      	  //端口初始化

	csi_clk_enable((uint32_t *)ptSpiBase);				       //打开时钟
	csp_spi_default_init(ptSpiBase);					       //寄存器初始值复位
	csp_spi_dis(ptSpiBase);								       //关闭spi
	csp_spi_set_rxifl(ptSpiBase, ptSpiCfg->eSpiRxFifoLevel);   //设置接收fifo阈值
	csi_spi_mode(ptSpiBase, ptSpiCfg->eSpiMode);			   //主从机
	csi_spi_cp_format(ptSpiBase, ptSpiCfg->eSpiPolarityPhase); //极性和相位设置	
	csi_spi_frame_len(ptSpiBase, ptSpiCfg->eSpiFrameLen);	   //格式帧长度设置
	csi_spi_baud(ptSpiBase, ptSpiCfg->dwSpiBaud);              //通信速率
	apt_spi_int_set(ptSpiBase,(spi_int_e)(ptSpiCfg->byInter)); //中断配置
	csi_spi_Internal_variables_init(ptSpiCfg->eSpiRxFifoLevel,ptSpiCfg->byInter);//内部使用，客户无需更改			
							
	csp_spi_en(ptSpiBase);							           //打开spi
	
	return tRet;
}

/** \brief uninit spi data structure
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_uninit(csp_spi_t *ptSpiBase)
{
	csi_error_t tRet = CSI_OK;
	
	csi_clk_disable((uint32_t *)ptSpiBase);	
	csi_irq_disable((uint32_t *)ptSpiBase);
	csp_spi_default_init(ptSpiBase);
	csi_spi_Internal_variables_init(SPI_RXFIFO_1_2,SPI_NONE_INT);
	
	return tRet;	
}
/** \brief set spi mode, master or slave
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] eMode: master, slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_mode(csp_spi_t *ptSpiBase, csi_spi_mode_e eMode)
{
    csi_error_t   tRet = CSI_OK;
	
	switch (eMode) 			//configure spi mode
	{
        case SPI_MASTER:
            csp_spi_set_mode(ptSpiBase, SPI_MODE_MASTER);
            break;
        case SPI_SLAVE:
            csp_spi_set_mode(ptSpiBase, SPI_MODE_SLAVE);
            break;
        default:
            tRet = CSI_ERROR;
            break;
    }	
	return tRet;
}

/** \brief config spi cp format
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] eFormat: spi cp format
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_cp_format(csp_spi_t *ptSpiBase, csi_spi_cp_format_e eFormat)
{
    csi_error_t   ret = CSI_OK;
	switch (eFormat)		//configure spi format 
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
 *  \param[in] wBaud: spi work baud
 *  \return spi config frequency
 */
uint32_t csi_spi_baud(csp_spi_t *ptSpiBase, uint32_t wBaud)
{
    uint32_t wDiv;
    uint32_t wFreq = 0U;

	wDiv = (soc_get_pclk_freq() >> 1) / wBaud;//baud = FPCLK/ CPSDVR / (1 + SCR))
	
	if(wDiv > 0)
		wDiv --;
	
	csp_spi_set_clk_div(ptSpiBase, wDiv, 2);

	if(wDiv > 0)
		wFreq = (soc_get_pclk_freq() >> 1) / (wDiv + 1);
	else
		wFreq = (soc_get_pclk_freq() >> 1) ;
		
    return wFreq;
}

/** \brief config spi frame length
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] eLength: frame length
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_frame_len(csp_spi_t *ptSpiBase, csi_spi_frame_len_e eLength)
{	
    csi_error_t   tRet = CSI_OK;

    if ((eLength < SPI_FRAME_LEN_4) || (eLength > SPI_FRAME_LEN_16)) 
        tRet = CSI_ERROR;
	else 
	{
        csp_spi_set_data_size(ptSpiBase, (eLength -1));			 //configura data frame width
    }
    return tRet;
}

/** \brief get the tState of spi device
 * 
 *  \param[in] ptState: the tState of spi device
 *  \return none
 */ 
csi_error_t csi_spi_get_state(csi_state_t *ptState)
{
    *ptState = g_tSpiTransmit.tState;
    return CSI_OK;
}

/** \brief sending data to spi transmitter(received data is ignored),blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to send(byte)
 *  \param[in] wTimeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_send(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize, uint32_t wTimeout)
{
	uint32_t wCount = 0U;
	
	if((g_tSpiTransmit.tState.writeable == 0U))						
		return CSI_BUSY;
	
	g_tSpiTransmit.byTxSize = wSize;
	g_tSpiTransmit.tState.writeable = 0U;
	g_tSpiTransmit.pbyTxData = (uint8_t *)pData;
	csp_spi_en(ptSpiBase);								//enable spi
	
	uint32_t wSendStart;
	while(g_tSpiTransmit.byTxSize > 0)
	{
		wSendStart = SPI_SEND_TIMEOUT;
		while(!csp_spi_write_ready(ptSpiBase) && wSendStart --);	//fifo full? wait; 
		if(wSendStart == 0)
		{
			break;
		}
		csp_spi_set_data(ptSpiBase, *g_tSpiTransmit.pbyTxData);	//send data
		g_tSpiTransmit.pbyTxData ++;
		wCount ++;
		g_tSpiTransmit.byTxSize --;
	}
	
	wSendStart = SPI_SEND_TIMEOUT;
	while(csp_spi_busy(ptSpiBase))								//wait for transmition finish
	{
		wSendStart --;
		if(wSendStart == 0)
			break;
	}

	g_tSpiTransmit.tState.writeable = 1U;

	return wCount;
}

/** \brief sending data to spi transmitter, non-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to send(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_async(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize)
{	
	csi_error_t tRet = CSI_OK;
	
	if((g_tSpiTransmit.tState.writeable == 0U))		// || (g_tSpiTransmit.tState.readable == 0U)) 
        tRet = CSI_BUSY;
		
	if(tRet == CSI_OK)
	{
		g_tSpiTransmit.tState.writeable = 0U;
		g_tSpiTransmit.byTxSize = wSize;
		g_tSpiTransmit.pbyTxData = (uint8_t *)pData;
		csp_spi_en(ptSpiBase);											//enable spi
		csp_spi_set_int(ptSpiBase, g_tSpiTransmit.byInter,true);		//enable tx fifo int
		//csp_spi_set_int(ptSpiBase, SPI_TXIM_INT,true);		//enable tx fifo int
	}
	else
	{
		tRet = CSI_ERROR;
	}
	
	return tRet;
}

/** \brief  receiving data from spi receiver, blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pData: pointer to buffer with data to receive
 *  \param[in] wSize: number of data to receive(byte)
 *  \param[in] wTimeout: unit in mini-second
 *  \return return the num of data or  return Error code
 */
int32_t csi_spi_receive(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize, uint32_t wTimeout)
{	
	uint32_t  wCount = 0U;
	int32_t   iRet = CSI_OK;
	
	do{
		if((g_tSpiTransmit.tState.readable == 0U))		// || (g_tSpiTransmit.tState.readable == 0U)) 
		{
            iRet = CSI_BUSY;
            break;
        }
		
		g_tSpiTransmit.tState.readable = 0U;
		g_tSpiTransmit.byRxSize = wSize;
       
		g_tSpiTransmit.pbyRxData = (uint8_t *)pData;
		csp_spi_en(ptSpiBase);										//enable spi
		
		uint32_t wTimeStart;
		while(g_tSpiTransmit.byRxSize)
		{
			wTimeStart = SPI_RECV_TIMEOUT;
			while(!csp_spi_read_ready(ptSpiBase) && wTimeStart --);		//recv fifo empty? wait	
			if(wTimeStart ==0)
			{
				break;
			}
			
			*g_tSpiTransmit.pbyRxData = csp_spi_get_data(ptSpiBase);			//recv data
			g_tSpiTransmit.pbyRxData++;
			wCount ++;
			
			g_tSpiTransmit.byRxSize --;
		}
	}while(0);
	
	g_tSpiTransmit.tState.readable = 1U;
    if (iRet >= 0) 
	{
        iRet = (int32_t)wCount;
    }
	
	return iRet;
}

/** \brief  receiving data from spi receiver, not-blocking mode(interrupt mode)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pData: pointer to buffer with data to send to spi transmitter
 *  \param[in] wSize: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_receive_async(csp_spi_t *ptSpiBase, void *pData, uint32_t wSize)
{	
	csi_error_t tRet = CSI_OK;
	
	if ((g_tSpiTransmit.tState.readable == 0U))	// || (g_tSpiTransmit.tState.readable == 0U)) 
	{
		tRet = CSI_BUSY;
	}
	
	if(tRet == CSI_OK)
	{
		g_tSpiTransmit.tState.readable = 0U;
		g_tSpiTransmit.byRxSize = wSize;
		g_tSpiTransmit.pbyRxData = (uint8_t *)pData;
		csp_spi_set_int(ptSpiBase, g_tSpiTransmit.byInter,true);
		//csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT,true);//enable rx fifo int
		csp_spi_en(ptSpiBase);
	}
	else
		tRet = CSI_ERROR;
	
	return tRet;
}

/** \brief  receiving data from spi receiver,blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pDataout: pointer to buffer with data to send to spi transmitter
 *  \param[in] pDatain: number of data to receive(byte)
 *  \param[in] wSize: number of data to receive(byte)
 *  \param[in] wTimeout: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
int32_t csi_spi_send_receive(csp_spi_t *ptSpiBase, void *pDataout, void *pDatain, uint32_t wSize, uint32_t wTimeout)
{
	csi_error_t ret = CSI_OK;
	uint32_t wCount = 0U;
	
	do{
		if((g_tSpiTransmit.tState.writeable == 0U) || (g_tSpiTransmit.tState.readable == 0U)) 
		{
            ret = CSI_BUSY;
            break;
        }
		
		g_tSpiTransmit.byTxSize = wSize;
		g_tSpiTransmit.byRxSize = wSize;
		
		g_tSpiTransmit.tState.writeable = 0U;
        g_tSpiTransmit.tState.readable  = 0U;
		g_tSpiTransmit.pbyTxData = (uint8_t *)pDataout;
		g_tSpiTransmit.pbyRxData = (uint8_t *)pDatain;
		csp_spi_en(ptSpiBase);													//enable spi
		
		uint32_t wTimeStart;
		while((g_tSpiTransmit.byTxSize > 0U) || (g_tSpiTransmit.byRxSize > 0U))
		{
			if(g_tSpiTransmit.byTxSize > 0U)
			{
				wTimeStart = SPI_SEND_TIMEOUT;
				while(!csp_spi_write_ready(ptSpiBase) && wTimeStart --);		//send fifo full? wait; 
				if(wTimeStart ==0)
				{
					break;
				}
				
				csp_spi_set_data(ptSpiBase,*g_tSpiTransmit.pbyTxData);				//send data
				g_tSpiTransmit.pbyTxData ++;
				wCount ++;
				g_tSpiTransmit.byTxSize --;
			}
			
			if(g_tSpiTransmit.byRxSize > 0U)
			{
				wTimeStart = SPI_RECV_TIMEOUT;
				while(!csp_spi_read_ready(ptSpiBase) && wTimeStart --);		//recv fifo empty? wait	
				if(wTimeStart ==0)
				{
					break;
				}
		
				*g_tSpiTransmit.pbyRxData  = csp_spi_get_data(ptSpiBase);			//recv data
				g_tSpiTransmit.pbyRxData ++;
				g_tSpiTransmit.byRxSize --;
			}
		}

	}while(0);
	
	while((csp_spi_get_sr(ptSpiBase) & SPI_BSY));		//wait for transmition finish
	
	g_tSpiTransmit.tState.writeable = 1U;
    g_tSpiTransmit.tState.readable  = 1U;

    if (ret >= 0) {
        ret = (int32_t)wCount;
    }
	
	return ret;
}

/** \brief  receiving data from spi receiver, not-blocking mode
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pDataout: pointer to buffer with data to send to spi transmitter
 *  \param[in] pDatain: number of data to receive(byte)
 *  \param[in] wSize: number of data to receive(byte)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_spi_send_receive_async(csp_spi_t *ptSpiBase, void *pDataout, void *pDatain, uint32_t wSize)
{
    csi_error_t tRet = CSI_OK;
	
	if((g_tSpiTransmit.tState.writeable == 0U) || (g_tSpiTransmit.tState.readable == 0U))
	{ 
        tRet = CSI_BUSY;
	}
		
	if(tRet == CSI_OK) 
	{
		if(pDataout)
			g_tSpiTransmit.tState.writeable = 0U;
		else
			g_tSpiTransmit.tState.writeable = 1U;
		if(pDatain)
			g_tSpiTransmit.tState.readable  = 0U;
		else
			g_tSpiTransmit.tState.readable  = 1U;
			
		g_tSpiTransmit.byTxSize = wSize;
		g_tSpiTransmit.byRxSize = wSize;

		if(pDataout)
			g_tSpiTransmit.pbyTxData = (uint8_t *)pDataout;
		else 
			g_tSpiTransmit.pbyTxData = NULL;

		if(pDatain)
			g_tSpiTransmit.pbyRxData = (uint8_t *)pDatain;
		else
			g_tSpiTransmit.pbyRxData = NULL;

		csp_spi_set_int(ptSpiBase, g_tSpiTransmit.byInter,true);
		//csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT,true);	//enable rx fifo int
		//csp_spi_set_int(ptSpiBase, SPI_TXIM_INT,true);					//enable tx fifo int
		csp_spi_en(ptSpiBase);	
    } 
	else 
        tRet = CSI_ERROR;
		
	return tRet;
}

/** \brief  transmission variables init ,user not change it
 * 
 *  \param[in] eRxLen:rx fifo length
 *  \param[in] byInter:interrupt source
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_spi_Internal_variables_init(spi_rxifl_e eRxLen,uint8_t byInter)
{
	g_tSpiTransmit.pbyRxData =NULL;
	g_tSpiTransmit.byRxSize =0;
	g_tSpiTransmit.pbyTxData =NULL;
	g_tSpiTransmit.byTxSize =0;
	g_tSpiTransmit.byRxFifoLength = (uint8_t)eRxLen;
	g_tSpiTransmit.byInter = byInter;
	g_tSpiTransmit.tState.writeable = 1;
	g_tSpiTransmit.tState.readable  = 1;
	g_tSpiTransmit.tState.error = 0;
	
    return CSI_OK;
}

/** \brief clr spi rx fifo
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */
void csi_spi_clr_rxfifo(csp_spi_t *ptSpiBase)
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
 *  \param[in] hwDataout: data of send
 *  \return none
 */ 
csi_error_t csi_spi_send_slave(csp_spi_t *ptSpiBase, uint16_t hwDataout)
{
	while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));	//send fifo not full: write 	
	csp_spi_set_data(ptSpiBase, hwDataout);
	
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
	if((g_tSpiTransmit.pbyRxData == NULL) || (g_tSpiTransmit.byRxSize == 0U))
	{
		
		if(g_tSpiTransmit.pbyRxData == NULL)
		{
			for(uint8_t byIdx = 0; byIdx < g_tSpiTransmit.byRxFifoLength; byIdx++)
			{
				csp_spi_get_data(ptSpiBase);								//read fifo 
				g_tSpiTransmit.byRxSize--;
				if(g_tSpiTransmit.byRxSize == 0)
				{
					break;	
				}
			}
			
			if(g_tSpiTransmit.byRxSize == 0)
			{
				g_tSpiTransmit.tState.readable = 1U;
				csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);		//disable fifo rx int
			}
		}
		else
		{
			csi_spi_clr_rxfifo(SPI0);
			csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);		//disable fifo rx int
		}
	}
	else
	{
		for(uint8_t byIdx = 0; byIdx < g_tSpiTransmit.byRxFifoLength; byIdx++)
		{
			*g_tSpiTransmit.pbyRxData = csp_spi_get_data(ptSpiBase);			//receive data
			g_tSpiTransmit.pbyRxData++;
			g_tSpiTransmit.byRxSize--;
			if(g_tSpiTransmit.byRxSize == 0)
			{
				break;
			}
		}
		
		if(g_tSpiTransmit.byRxSize == 0)
		{
			g_tSpiTransmit.tState.readable = 1U;
			csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int
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
	if(g_tSpiTransmit.pbyTxData)
	{
		while(!(csp_spi_get_sr(ptSpiBase) & SPI_TNF));
		csp_spi_set_data(ptSpiBase,*g_tSpiTransmit.pbyTxData);			//send data
		g_tSpiTransmit.pbyTxData++;
	}
	else
		csp_spi_set_data(ptSpiBase,0x00);						//send data

	g_tSpiTransmit.byTxSize --;
	
    if (g_tSpiTransmit.byTxSize <= 0U) 
	{
        g_tSpiTransmit.tState.writeable = 1U;
		SPICS_SET;
		csp_spi_set_int(ptSpiBase, SPI_TXIM_INT, false);			//disable fifo tx int
    }
}

/** \brief spi interrupt handle weak function
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \return none
 */ 
__attribute__((weak)) void spi_irqhandler(csp_spi_t *ptSpiBase)
{	
	uint32_t wStatus = csp_spi_get_isr(ptSpiBase);
	#ifndef SPI_SYNC_SEL
		uint8_t receive_data[4];
	#endif
	//fifo rx 
	if(wStatus & SPI_RXIM_INT)
	{
		//for reference
		#ifdef SPI_MASTER_SEL
			#ifndef SPI_SYNC_SEL
				//apt_spi_intr_recv_data(ptSpiBase);//when use"csi_spi_send_receive_async"function need open
				for(uint8_t byIdx = 0; byIdx < g_tSpiTransmit.byRxFifoLength; byIdx++)
				{
					
					while(!(csp_spi_get_sr(SPI0) & SPI_RNE));	//receive not empty:read
					receive_data[byIdx] = csp_spi_get_data(SPI0);						
				}
			#endif
		#else
			#ifndef SPI_SYNC_SEL
				for(uint8_t byIdx = 0; byIdx < g_tSpiTransmit.byRxFifoLength; byIdx++)
				{
					receive_data[byIdx] = csi_spi_receive_slave(SPI0);
					csi_spi_send_slave(SPI0, receive_data[byIdx]);
				}
			#endif
		#endif
	}
	//fifo tx 
	if(wStatus & SPI_TXIM_INT)		
	{
		//for reference
		apt_spi_intr_send_data(ptSpiBase);
	}
	
	//fifo overflow
	if(wStatus & SPI_ROTIM_INT)
	{	
		//for reference
		csi_spi_clr_rxfifo(ptSpiBase);
		csp_spi_clr_isr(ptSpiBase, SPI_ROTIM_INT);
	}
	
	//fifo rx timeout
	if(wStatus & SPI_RTIM_INT)		
	{	
		//for reference
		csp_spi_clr_isr(ptSpiBase, SPI_RTIM_INT);
		
		for(uint8_t byIdx = 0; byIdx < g_tSpiTransmit.byRxFifoLength-1; byIdx++)
		{
			
			if(csp_spi_get_sr(ptSpiBase) & SPI_RNE)
			{
				*g_tSpiTransmit.pbyRxData = csp_spi_get_data(ptSpiBase);		//receive data
				g_tSpiTransmit.pbyRxData++;
			}
			else
			{
				break;
			}
			
		}		
		g_tSpiTransmit.byRxSize = 0;
		g_tSpiTransmit.tState.readable = 1U;
		csp_spi_set_int(ptSpiBase, SPI_RXIM_INT | SPI_RTIM_INT, false);			//disable fifo rx int

	}
}

//-----------------------------------------------------------------------------------------------------------
//high speed spi function for reference
//-----------------------------------------------------------------------------------------------------------

/** \brief spi_send_receive_1byte
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] byReceiveData :send data buffer pointer
 *  \return the receive data
 */ 
uint8_t spi_send_receive_1byte(csp_spi_t *ptSpiBase,uint8_t byData)
{
	uint8_t byReceiveData = 0;
	uint32_t wTimeStart = SPI_SEND_TIMEOUT;
	 
	wTimeStart = SPI_SEND_TIMEOUT;
	while( ( !((uint32_t)(ptSpiBase->SR) & SPI_TNF) ) && (wTimeStart --) ){;} //send not full:write
	ptSpiBase->DR = byData; //send data

	wTimeStart = SPI_SEND_TIMEOUT;
	while( (!((uint32_t)(ptSpiBase->SR) & SPI_RNE) ) && (wTimeStart --) ){;}  //receive not empty:read
	byReceiveData = ptSpiBase->DR;//receive data
	
	return byReceiveData;

}


/** \brief spi send buff(this funtion will ignore the receive)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pbyData :send data buffer pointer
 *  \param[in] bySize ：length
 *  \return none
 */ 
void spi_buff_send(csp_spi_t *ptSpiBase,uint8_t *pbyData,uint8_t bySize)
{
	uint8_t byIcount = 0;
	uint32_t wTimeStart = SPI_SEND_TIMEOUT;
	
	for(byIcount = 0;byIcount <bySize;byIcount++)
	{
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( !((uint32_t)(ptSpiBase->SR) & SPI_TNF) ) && (wTimeStart --) ){;}  //send not full:write
		ptSpiBase->DR = *(pbyData+byIcount);
	}
	
	wTimeStart = SPI_SEND_TIMEOUT;//ensure spi process is finish
	while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} //not busy:finish

}

/** \brief spi send and receive(less than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pbyDataOut :send data buffer pointer
 *  \param[in] pbyDataIn :receive data buffer pointer
 *  \param[in] wSize ：length
 *  \return none
 */ 
void csi_spi_send_receive_x8(csp_spi_t *ptSpiBase, uint8_t *pbyDataOut,uint8_t *pbyDataIn,uint32_t wSize)//小于等于八个的发送接收,这种场景通常适合用来发送指令，读取状态。（大块的数据读取不合适）
{	
		uint8_t byCount = 0;
	    uint32_t wTimeStart = SPI_SEND_TIMEOUT;
		
		csi_spi_clr_rxfifo(ptSpiBase);
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;} 
		ptSpiBase->CR1 &= ~SPI_SSE_MSK;
		for(byCount = 0;byCount<wSize;byCount++)
		{
			ptSpiBase->DR = pbyDataOut[byCount];
		}
		ptSpiBase->CR1 |= (SPI_EN << SPI_SSE_POS);//SPI使能
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( (uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;} 
		
		pbyDataIn[0]=ptSpiBase->DR;
		pbyDataIn[1]=ptSpiBase->DR;
		pbyDataIn[2]=ptSpiBase->DR;
		pbyDataIn[3]=ptSpiBase->DR;
		pbyDataIn[4]=ptSpiBase->DR;
		pbyDataIn[5]=ptSpiBase->DR;
		pbyDataIn[6]=ptSpiBase->DR;
		pbyDataIn[7]=ptSpiBase->DR;
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ( (uint32_t)(ptSpiBase->SR) & SPI_BSY ) && (wTimeStart --) ){;}
}

/** \brief spi send and receive(send equal to 8 bytes or  more than eight bytes)
 * 
 *  \param[in] ptSpiBase: SPI handle
 *  \param[in] pbyDataOut :send data buffer pointer 
 *  \param[in] pbyDataIn  :send data buffer pointer 
 *  \param[in] wSize ：length
 *  \return none
 */ 
void csi_spi_send_receive_d8(csp_spi_t *ptSpiBase, uint8_t *pbyDataOut,uint8_t *pbyDataIn, uint32_t wSize)//大于八个的发送和读
{
		uint8_t byTxsize = wSize;
		uint32_t wTimeStart = SPI_SEND_TIMEOUT;
#if 1
		uint8_t byOutidx = 0;
		uint8_t byIdx;
		uint8_t byRemainder = 0;
		uint8_t byZheng = 0;
		uint8_t byLast8Times = 0;
		uint8_t byLastTxBuff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

		csi_spi_clr_rxfifo(ptSpiBase);

		byZheng = (wSize >> 3);
		byRemainder = wSize & 0x07;
		byLast8Times = byZheng << 3;//position
		
		memcpy((void *)byLastTxBuff,(void *)&pbyDataOut[byLast8Times], byRemainder);
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 

		while(byTxsize >=8U)
		{
			ptSpiBase->DR = pbyDataOut[byOutidx];
			ptSpiBase->DR = pbyDataOut[byOutidx+1];  //csp_spi_set_data(spi_base,*spi->pbyTxData);	//send data
			ptSpiBase->DR = pbyDataOut[byOutidx+2];
			ptSpiBase->DR = pbyDataOut[byOutidx+3];
			ptSpiBase->DR = pbyDataOut[byOutidx+4];
			ptSpiBase->DR = pbyDataOut[byOutidx+5];
			ptSpiBase->DR = pbyDataOut[byOutidx+6];
			ptSpiBase->DR = pbyDataOut[byOutidx+7];
	
			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 		

			byTxsize -= 8;
			pbyDataIn[byOutidx] = ptSpiBase->DR;
			pbyDataIn[byOutidx+1] = ptSpiBase->DR;
			pbyDataIn[byOutidx+2] = ptSpiBase->DR;
			pbyDataIn[byOutidx+3] = ptSpiBase->DR;
			pbyDataIn[byOutidx+4] = ptSpiBase->DR;
			pbyDataIn[byOutidx+5] = ptSpiBase->DR;
			pbyDataIn[byOutidx+6] = ptSpiBase->DR;
			pbyDataIn[byOutidx+7] = ptSpiBase->DR;
			
			byOutidx += 8;	
		}
		
	if(byRemainder != 0){	
		if(byRemainder == 1)
		{
			ptSpiBase->DR = byLastTxBuff[0];
		}
		else if(byRemainder == 2)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];
		}
		else if(byRemainder == 3)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];
		}
		else if(byRemainder == 4)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];ptSpiBase->DR = byLastTxBuff[3];
		}
		else if(byRemainder == 5)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];ptSpiBase->DR = byLastTxBuff[3];
			ptSpiBase->DR = byLastTxBuff[4];
		}
		else if(byRemainder == 6)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];ptSpiBase->DR = byLastTxBuff[3];
			ptSpiBase->DR = byLastTxBuff[4];ptSpiBase->DR = byLastTxBuff[5];
		}
		else if(byRemainder == 7)
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];ptSpiBase->DR = byLastTxBuff[3];
			ptSpiBase->DR = byLastTxBuff[4];ptSpiBase->DR = byLastTxBuff[5];ptSpiBase->DR = byLastTxBuff[6];
		}
		else
		{
			ptSpiBase->DR = byLastTxBuff[0];ptSpiBase->DR = byLastTxBuff[1];ptSpiBase->DR = byLastTxBuff[2];ptSpiBase->DR = byLastTxBuff[3];
			ptSpiBase->DR = byLastTxBuff[4];ptSpiBase->DR = byLastTxBuff[5];ptSpiBase->DR = byLastTxBuff[6];ptSpiBase->DR = byLastTxBuff[7];
		}
		
		wTimeStart = SPI_SEND_TIMEOUT;
		while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 

		for(byIdx=0;byIdx<byRemainder;byIdx++)		//read buffer data
			pbyDataIn[byOutidx+byIdx] = ptSpiBase->DR;		
	}

#else 	
		csi_spi_clrRx_fifo(ptSpiBase);
		byTxsize = wSize;

		while(byTxsize > 0U)
		{
			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 
			ptSpiBase->DR = *pbyDataOut;  //csp_spi_set_data(spi_base,*spi->pbyTxData);	//send data

			wTimeStart = SPI_SEND_TIMEOUT;
			while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 
		
			byTxsize --;
			*dpbyDataIn = ptSpiBase->DR;
			pbyDataOut++;
			pbyDataIn ++;
			
		}
	wTimeStart = SPI_SEND_TIMEOUT;
	while( ((uint32_t)(ptSpiBase->SR) & SPI_BSY) && (wTimeStart --) ){;} 	

#endif 	

}