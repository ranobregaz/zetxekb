/***********************************************************************//** 
 * \file  SPI_demo.c
 * \brief  SPI示例代码。
 * w25q16jvsiq(32 block == 512 sector == 2M Byte)
 * 1 page = 256 bytes
 * 1 sector = 16 page (4KB)
 * 1 block = 16 sector (64KB)
 * erasable(1sector,1/2block,1block,chip)
 * Device ID: (M7->M0 = 0xEF), (ID7->ID0 = 0x14)
 * Spi Mode:  MODE0 or MODE3
 * 
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version    <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2021-05-31 <td>V0.1  <td>LQ      <td>modify
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "spi.h"
#include "pin.h"
#include <iostring.h>

/** \brief spi sync mode send buff
 * 
 *  \param[in] none
 *  \return error code
 */
 csi_error_t spi_sync_sendbuff(void)
{
	int iRet = 0;
	uint8_t byData[8] = {1,2,3,4,5,6,7,8};
	csi_spi_config_t t_SpiConfig;  //spi初始化参数配置结构体
	
	t_SpiConfig.eSpiMode = SPI_MASTER;						//作为主机
	t_SpiConfig.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA1; //clk空闲电平为0，相位为在第二个边沿采集数据
	t_SpiConfig.eSpiFrameLen = SPI_FRAME_LEN_8;             //帧数据长度为8bit
	t_SpiConfig.dwSpiBaud = 12000000; 						//通讯速率12兆			
    t_SpiConfig.eSpiRxFifoLevel = SPI_RXFIFO_1_2;  			//接收fifo中断阈值，1/2*8=4个
	t_SpiConfig.byInter = (uint8_t)SPI_NONE_INT;			//初始配置无中断

	iRet = csi_spi_init(SPI0,&t_SpiConfig);			
	if(iRet < 0)
	{
		return -1;
	}	
	my_printf("the spi sync send count is:%d!\n",8);
	mdelay(500);

	while(1)
	{
		SPICS_CLR;
		//csi_spi_send(SPI0,byData,8,1);
		spi_buff_send(SPI0,byData,8);
		SPICS_SET;
		mdelay(100);
		nop;
	}
	return iRet;
}

/** \brief spi async mode send buff
 * 
 *  \param[in] none
 *  \return error code
 */
 csi_error_t spi_async_sendbuff(void)
{
	int tRet = 0;
	uint8_t bySendData[8] = {9,10,11,12,13,14,15,16};
	csi_spi_config_t t_SpiConfig;  //spi初始化参数配置结构体
	
	t_SpiConfig.eSpiMode = SPI_MASTER;						//作为主机
	t_SpiConfig.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA1; //clk空闲电平为0，相位为在第二个边沿采集数据
	t_SpiConfig.eSpiFrameLen = SPI_FRAME_LEN_8;             //帧数据长度为8bit
	t_SpiConfig.dwSpiBaud = 12000000; 						//通讯速率12兆			
    t_SpiConfig.eSpiRxFifoLevel = SPI_RXFIFO_1_2;  			//接收fifo中断阈值，1/2*8=4个
	t_SpiConfig.byInter = (uint8_t)SPI_TXIM_INT;			//发送使用中断	  	
	tRet = csi_spi_init(SPI0,&t_SpiConfig);	
	if(tRet < 0)
	{
		return -1;
	}
	
	my_printf("the spi async send count is:%d!\n",8);	
	mdelay(500);	

	while(1)
	{
		SPICS_CLR;
		tRet = csi_spi_send_async(SPI0, (void *) bySendData, 8);//send first
		//SPICS_SET;  //SPICS_SET when send interrupt complete
		mdelay(100);		
		nop;
	}
	return tRet;
}

/** \brief spi speed test
 * 
 *  \param[in] none
 *  \return error code
 */
csi_error_t spi_sync_test_speed(void)
{
	int iRet = 0;
	uint8_t bySendData[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	uint8_t byReceData[17] = {0};
	csi_spi_config_t t_SpiConfig;  //spi初始化参数配置结构体
	
	t_SpiConfig.eSpiMode = SPI_MASTER;						//作为主机
	t_SpiConfig.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA1; //clk空闲电平为0，相位为在第二个边沿采集数据
	t_SpiConfig.eSpiFrameLen = SPI_FRAME_LEN_8;             //帧数据长度为8bit
	t_SpiConfig.dwSpiBaud = 12000000; 						//通讯速率12兆			
    t_SpiConfig.eSpiRxFifoLevel = SPI_RXFIFO_1_2;  			//接收fifo中断阈值，1/2*8=4个	 
	t_SpiConfig.byInter = (uint8_t)SPI_NONE_INT;			//初始配置无中断 
	iRet = csi_spi_init(SPI0,&t_SpiConfig);				
	if(iRet < 0)
	{
		return -1;
	}	
	my_printf("the spi test speed end!\n");		
	mdelay(500);

	while(1)
	{
		SPICS_CLR;
		csi_spi_send_receive_x8(SPI0,bySendData,byReceData,7);//23.38us
		SPICS_SET;
		
		SPICS_CLR;
		csi_spi_send_receive(SPI0, (void*)bySendData, (void *)byReceData, 7, 1);//53.72 us
		SPICS_SET;
		
		SPICS_CLR;
		csi_spi_send_receive_d8(SPI0,bySendData,byReceData,7);//(29.08 us #if 1)
		SPICS_SET;
		
		SPICS_CLR;
		csi_spi_send_receive(SPI0, (void*)bySendData, (void *)byReceData, 17, 1);//117.74 us
		SPICS_SET;
		
		SPICS_CLR;
		csi_spi_send_receive_d8(SPI0,bySendData,byReceData,17);//(40.22)
		SPICS_SET;
		mdelay(100);
		nop;
	}
	return iRet;
}

/** \brief spi master or slave test
 * 
 *  \param[in] none
 *  \return error code
 */
//模式一：主机同步发，同步收；  从机同步收发
//模式二：主机同步发，同步收；  从机异步收发
//模式三：主机同步发，异步收；  从机同步收发
//模式四：主机同步发，异步收；  从机异步收发
//模式五：主机异步发，异步收；  从机同步收发
//模式六：主机异步发，异步收；  从机异步收发
//主从机常用模式：模式一，模式四
 csi_error_t spi_sync_async_master_slave(void)
{
	int iRet = 0;
	uint8_t bySendData[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	uint8_t byReceData[17] = {0};
	csi_spi_config_t t_SpiConfig;  //spi初始化参数配置结构体
	
	#ifdef SPI_MASTER_SEL
		t_SpiConfig.eSpiMode = SPI_MASTER;					//作为主机
	#else
		t_SpiConfig.eSpiMode = SPI_SLAVE;					//作为从机
	#endif
	t_SpiConfig.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA1; //clk空闲电平为0，相位为在第二个边沿采集数据
	t_SpiConfig.eSpiFrameLen = SPI_FRAME_LEN_8;             //帧数据长度为8bit
	t_SpiConfig.dwSpiBaud = 2000000; 						//通讯速率2兆			
    t_SpiConfig.eSpiRxFifoLevel = SPI_RXFIFO_1_2;  			//接收fifo中断阈值，1/2*8=4个
	t_SpiConfig.byInter = (uint8_t)SPI_NONE_INT;			//初始配置无中断	
	iRet = csi_spi_init(SPI0,&t_SpiConfig);				
	if(iRet < 0)
	{
		return -1;
	}
	
#ifdef SPI_MASTER_SEL	//spi master mode		
	my_printf("the spi master mode!\n");
	mdelay(500);
	#ifdef SPI_SYNC_SEL
		while(1)
		{
			SPICS_CLR;
			csi_spi_send_receive(SPI0, bySendData, byReceData, 16, 1);//265.18 us
			SPICS_SET;
			mdelay(100);
			nop;
		}
	#else
		while(1)
		{
			SPICS_CLR;
			if(SPI_NONE_INT == t_SpiConfig.byInter)
				csi_spi_send(SPI0, bySendData, 16, 1);//265.18 us
			else if(SPI_TXIM_INT == t_SpiConfig.byInter)
				csi_spi_send_async(SPI0, bySendData, 16);
			else if( (SPI_TXIM_INT | SPI_RXIM_INT) == t_SpiConfig.byInter)
				csi_spi_send_receive_async(SPI0, bySendData,byReceData,16);
			mdelay(100);
			nop;
		}	
	#endif
#else
	my_printf("the spi slave mode!\n");
	#ifdef SPI_SYNC_SEL
		while(1)
		{
			while( (uint32_t)(SPI0->SR) & SPI_RNE )	
			{
				byReceData[0] = csi_spi_receive_slave(SPI0);
				csi_spi_send_slave(SPI0, byReceData[0]);
			}
			nop;
		}
	#else
		while(1)
		{
			nop;
		}
	#endif
#endif	
	return iRet;
}

//----------------------------------------------------------------------------------------------
//w25q16jvsiq(32 block == 512 sector == 2M Byte)
// 1 page = 256 bytes
// 1 sector = 16 page (4KB)
// 1 block = 16 sector (64KB)

//erasable(1sector,1/2block,1block,chip)


#define		PAGE_SIZE				256
//flash cmd
#define		WREN_CMD				0x06		//write enable
#define		WRDI_CMD				0x04		//write disable
#define		RDSR0_CMD				0x05		//Read Status0
#define		RDSR1_CMD				0x35		//Read Status1
#define		WRSR_CMD				0x01		//Write Status 
#define		READ_CMD				0x03		//ReadData
#define		FREAD_CMD				0x0B		//FastReadData  
#define		PGPRO_CMD				0x02		//PageProgram
#define		FPGPRO_CMD				0x32		//Fast PageProgram
#define		SECTORERASE_CMD			0x20		//setor erase
#define		BKERASE0_CMD			0xD8		//block erase0(64k)
#define		BKERASE1_CMD			0x52		//block erase1(32k)
#define		CHIPERASE_CMD			0xc7		//chip erase
#define		RDDEVICEID_CMD			0x90		//read chip ID ReadElectricManufacturerDeviceID 
#define		RDCHIPID_CMD			0x9F		//read chip ID
	

static uint8_t byWrBuf[16] = {26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11};
volatile static uint8_t byRdBuf[16];
	
//flash status reg
//BIT7    6     5     4     3     2     1     0
//SPR0    SEC   TB   BP2    BP1  BP0   WEL   BUSY
//SPR0:Status Protect
//SEC: Sector/Block Protect
//TB:  TOP/BOTTOM Block Protect
//BP3, BP2,BP1,BP0:FLASH Block Protect
//WEL: Write Enable Latch
//BUSY:Busy flage（1 busy：0 idle）
//defualt:0x00

/** \brief flash write enable 
 * 
 *  \param[in] none
 *  \return none
 */
static void SPI_flash_write_enable(void)
{
	uint8_t byCmd = WREN_CMD;		//write enable cmd = 0x06
	
	SPICS_CLR;
	csi_spi_send_receive(SPI0, (void *)&byCmd, NULL, 1, 2);	
	SPICS_SET;
}

/** \brief flash read status reg
 * 
 *  \param[in] none
 *  \return flash status
 */
//BIT7    6     5     4     3     2     1     0
//SPR0    SEC   TB   BP2    BP1  BP0   WEL   BUSY
//SPR0:Status Protect
//SEC: Sector/Block Protect
//TB:  TOP/BOTTOM Block Protect
//BP3, BP2,BP1,BP0:FLASH Block Protect
//WEL: Write Enable Latch
//BUSY:Busy flage（1 busy：0 idle）
//defualt:0x00

uint8_t SPI_flash_read_status(void)
{
	uint8_t bySend[2] = {RDSR0_CMD, 0x00};		//read status cmd0 = 0x05
	uint8_t byRecv[2];

	SPICS_CLR;
	csi_spi_send_receive(SPI0, (void *)bySend, (void *)byRecv, 2, 2);
	SPICS_SET;
	
	return byRecv[1];
}
/** \brief flash wait idle
 * 
 *  \param[in] none
 *  \return none
 */
void SPI_flash_wait_busy(void)
{
	while((SPI_flash_read_status() & 0x01) == 0x01);	// 1: busy, 0: idle
}

/** \brief flash write status 
 * 
 *  \param[in] byStatus: flash status of write
 *  \return none
 */
//SPR0,TB,BP2,BP1,BP0(bit 7,5,4,3,2) 
void SPI_flash_write_Status(uint8_t byStatus)
{
	uint8_t bySend[2] = {WRSR_CMD, byStatus};		//write status cmd = 0x01

	SPI_flash_write_enable();		//write enable cmd
	SPICS_CLR;
	csi_spi_send_receive(SPI0, (void *)bySend, NULL, 2, 2);
	SPICS_SET;
	SPI_flash_wait_busy();
}
/** \brief flash read chip id
 * 
 *  \param[in] none
 *  \return chip id (chip id = 0xef13)
 */
uint32_t SPI_flash_read_id(void)
{
	uint32_t hwId = 0x00;
	uint8_t bySend[6] = {RDDEVICEID_CMD, 0, 0, 0};
	uint8_t byRecv[2];
	
	SPICS_CLR; 
	csi_spi_send_receive(SPI0, (void *)bySend, NULL, 4, 2);	//send read id cmd and three bytes addr	
	csi_spi_send_receive(SPI0, NULL, (void *)byRecv, 2, 2);	//read id value; id value = 0xef14
	hwId = ((byRecv[0] << 8) |  byRecv[1]);
	SPICS_SET;
   
	return hwId;
}
/** \brief flash chip erase
 * 
 *  \param[in] none
 *  \return none
 */
void SPI_flash_chip_erase(void)
{
	uint8_t byCmd = CHIPERASE_CMD;
	
	SPI_flash_write_enable();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive(SPI0, (void *)&byCmd, NULL, 1, 2);		//send chip erase cmd
	SPICS_SET;
	SPI_flash_wait_busy();
}
/** \brief flash sector erase (sector = 4k bytes)
 * 
 *  \param[in] wAddr: flash sector addr
 *  \return none
 */
void SPI_flash_sector_erase(uint32_t wAddr)
{
	wAddr = wAddr & 0xfff000;
	uint8_t bySend[4] = {SECTORERASE_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};
	
	SPI_flash_write_enable();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive(SPI0, (void *)bySend, NULL, 4, 2);		//send sector erase cmd and data three bytes addr 
	SPICS_SET;
	SPI_flash_wait_busy();
}

/** \brief flash read bytes
 * 
 *  \param[in] pbyBuf: pointer of read data buf 
 *  \param[in] wAddr: start addr of read data
 *  \param[in] hwNum: length of read data
 *  \return none
 */
void SPI_flash_read_bytes(uint8_t *pbyBuf, uint32_t wAddr, uint16_t hwNum)
{
	uint8_t bySend[4] = {READ_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};
	
	SPICS_CLR; 
	csi_spi_send_receive(SPI0, (void *)bySend, NULL, 4, 2);		//send read bytes cmd and data three bytes addr 
	csi_spi_send_receive(SPI0, NULL,(void *)pbyBuf, hwNum, 2);		//read hwNum bytes 
	SPICS_SET;
}

/** \brief flash write enable 
 * 
 *  \param[in] pbyBuf: pointer of write data buf 
 *  \param[in] wAddr: start addr of write data
 *  \param[in] hwNum: length of write data
 *  \return none
 */
void SPI_flash_write_bytes(uint8_t *pbyBuf, uint32_t wAddr, uint16_t hwNum)
{
	uint8_t bySend[4] = {PGPRO_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};

	SPI_flash_write_enable();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive(SPI0, (void *)bySend, NULL, 4, 2);		//send write bytes cmd and data three bytes addr 
	csi_spi_send_receive(SPI0, (void *)pbyBuf, NULL, hwNum, 2);	//write hwNum bytes
	SPICS_SET;
	SPI_flash_wait_busy();
}

/** \brief spi示例代码
 * 
 *  \param[in] none
 *  \return error code
 */
csi_error_t spi_w25q16jvsiq_write_read(void)
{
	int iRet = 0;	
	csi_spi_config_t t_SpiConfig;  //spi初始化参数配置结构体
	
	t_SpiConfig.eSpiMode = SPI_MASTER;						//作为主机
	t_SpiConfig.eSpiPolarityPhase = SPI_FORMAT_CPOL0_CPHA0; //clk空闲电平为0，相位为在第二个边沿采集数据
	t_SpiConfig.eSpiFrameLen = SPI_FRAME_LEN_8;             //帧数据长度为8bit
	t_SpiConfig.dwSpiBaud = 12000000; 						//通讯速率12兆			
    t_SpiConfig.eSpiRxFifoLevel = SPI_RXFIFO_1_2;  			//接收fifo中断阈值，1/2*8=4个
	t_SpiConfig.byInter = (uint8_t)SPI_NONE_INT;			//初始配置无中断	  
	iRet = csi_spi_init(SPI0,&t_SpiConfig);							
	if(iRet < 0)
	{
		return -1;
	}
	my_printf("the spi w25q16 test!\n");	
	mdelay(200);
	iRet = SPI_flash_read_id();					//read chip id, chip id = 0xef14
	
	if(iRet == 0xef14)
	{
	
		//iRet = SPI_flash_read_status();		//read status reg
		
		SPI_flash_sector_erase(0x1000);			//erase sector 1; start addr = 0x1000

		SPI_flash_read_bytes((uint8_t *)byRdBuf, 0x1000, 16);	//read data = 0xff
	
		SPI_flash_write_bytes(byWrBuf,0x1000,16);				//write 16 bytes 
		
		SPI_flash_read_bytes((uint8_t *)byRdBuf, 0x1000, 16);	//read 16 bytes, read bytes = write bytes
		
		iRet = SPI_flash_read_id();					//read chip id, chip id = 0xef14
	}
	while(1)
	{
		nop;
	}
	return iRet;
}