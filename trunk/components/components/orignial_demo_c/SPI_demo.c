/***********************************************************************//** 
 * \file  SPI_demo.c
 * \brief  SPI示例代码。
 * 互联设备 w25x80 信息：
 * size = 32768 pages; page = 256 bytes; size = 32768 * 256 = 8mbytes
 * sector(扇区) = 16 pages = 4Kbytes; 
 * block(块) = 128 pages = 32kbytes; block(块) = 256pages = 64kbytes
 * size = 2048 setor; size = 256 block(32k); size = 128 block(64k);
 * Device ID: (M7->M0 = 0xEF), (ID7->ID0 = 0x13)
 * Spi Mode:  MODE0 or MODE3
 * 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "spi.h"
#include "pin.h"
/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
/* device instance---------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
#define		SPI_MASTER_SEL			//spi mode select(master or slave)
#define		SPI_SYNC_SEL			//spi work mode select(同步 或者异步)

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

#define		SPICS_CLR		csi_gpio_write(&g_tGpioB0, 0x1<<0x4, GPIO_PIN_LOW);		//PB0.5低
#define		SPICS_SET		csi_gpio_write(&g_tGpioB0, 0x1<<0x4, GPIO_PIN_HIGH);	//PB0.5高


/* Private variablesr------------------------------------------------------*/
csi_gpio_t g_tGpioB0;
csi_spi_t	g_tSpiMaster;
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)&byCmd, NULL, 1, 2);	
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, (void *)byRecv, 2, 2);
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, NULL, 2, 2);
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, NULL, 4, 2);	//send read id cmd and three bytes addr	
	csi_spi_send_receive(&g_tSpiMaster, NULL, (void *)byRecv, 2, 2);	//read id value; id value = 0xef13
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)&byCmd, NULL, 1, 2);		//send chip erase cmd
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, NULL, 4, 2);		//send sector erase cmd and data three bytes addr 
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, NULL, 4, 2);		//send read bytes cmd and data three bytes addr 
	csi_spi_send_receive(&g_tSpiMaster, NULL,(void *)pbyBuf, hwNum, 2);		//read hwNum bytes 
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
	csi_spi_send_receive(&g_tSpiMaster, (void *)bySend, NULL, 4, 2);		//send write bytes cmd and data three bytes addr 
	csi_spi_send_receive(&g_tSpiMaster, (void *)pbyBuf, NULL, hwNum, 2);	//write hwNum bytes
	SPICS_SET;
	SPI_flash_wait_busy();
}


/** \brief flash write enable, async mode
 * 
 *  \param[in] none
 *  \return none
 */
static void SPI_flash_write_enable_async(void)
{
	uint8_t byCmd = WREN_CMD;		//write enable cmd = 0x06
	
	SPICS_CLR;
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)&byCmd, NULL, 1);	
	SPICS_SET;
}
/** \brief flash read status reg, async mode
 * 
 *  \param[in] none
 *  \return flash status
 */
uint8_t SPI_flash_read_status_async(void)
{
	uint8_t bySend[2] = {RDSR0_CMD, 0x00};		//read status cmd0 = 0x05
	uint8_t byRecv[2];

	SPICS_CLR;
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, (void *)byRecv, 2);
	SPICS_SET;
	
	return byRecv[1];
}
/** \brief flash wait idle, async mode
 * 
 *  \param[in] none
 *  \return none
 */
void SPI_flash_wait_busy_async(void)
{
	while((SPI_flash_read_status_async() & 0x01) == 0x01);	// 1: busy, 0: idle
}
/** \brief flash write status, async mode 
 * 
 *  \param[in] byStatus: flash status of write.
 *  \return none
 */
//SPR0,TB,BP2,BP1,BP0(bit 7,5,4,3,2) 
void SPI_flash_write_Status_async(uint8_t byStatus)
{
	uint8_t bySend[2] = {WRSR_CMD, byStatus};		//write status cmd = 0x01

	SPI_flash_write_enable_async();		//write enable cmd
	SPICS_CLR;
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, NULL, 2);
	SPICS_SET;
	SPI_flash_wait_busy_async();
}

/** \brief flash read chip id, async mode
 * 
 *  \param[in] none
 *  \return chip id (chip id = 0xef13)
 */
uint32_t SPI_flash_read_id_async(void)
{
	uint32_t hwId = 0x00;
	uint8_t bySend[6] = {RDDEVICEID_CMD, 0, 0, 0};
	uint8_t byRecv[2];
	
	SPICS_CLR; 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, NULL, 4);	//send read id cmd and three bytes addr	
	csi_spi_send_receive_async(&g_tSpiMaster, NULL, (void *)byRecv, 2);	//read id value; id value = 0xef13
	hwId = ((byRecv[0] << 8) |  byRecv[1]);
	SPICS_SET;
   
	return hwId;
}

/** \brief flash chip erase, async mode
 * 
 *  \param[in] none
 *  \return none
 */
void SPI_flash_chip_erase_async(void)
{
	uint8_t byCmd = CHIPERASE_CMD;
	
	SPI_flash_write_enable_async();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)&byCmd, NULL, 1);		//send chip erase cmd
	SPICS_SET;
	SPI_flash_wait_busy_async();
}
/** \brief flash sector erase (sector = 4k bytes), async mode
 * 
 *  \param[in] wAddr: flash sector addr
 *  \return none
 */
void SPI_flash_sector_erase_async(uint32_t wAddr)
{
	wAddr = wAddr & 0xfff000;
	uint8_t bySend[4] = {SECTORERASE_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};
	
	SPI_flash_write_enable_async();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, NULL, 4);		//send sector erase cmd and data three bytes addr 
	SPICS_SET;
	SPI_flash_wait_busy_async();
}
/** \brief flash read bytes, sync mode
 * 
 *  \param[in] pbyBuf: pointer of read data buf 
 *  \param[in] wAddr: start addr of read data
 *  \param[in] hwNum: length of read data
 *  \return none
 */
void SPI_flash_read_bytes_async(uint8_t *pbyBuf, uint32_t wAddr, uint16_t hwNum)
{
	uint8_t bySend[4] = {READ_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};
	
	SPICS_CLR; 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, NULL, 4);		//send read bytes cmd and data three bytes addr 
	csi_spi_send_receive_async(&g_tSpiMaster, NULL,(void *)pbyBuf, hwNum);	//read hwNum bytes 
	SPICS_SET;
}

/** \brief flash write enable, sync mode 
 * 
 *  \param[in] pbyBuf: pointer of write data buf 
 *  \param[in] wAddr: start addr of write data
 *  \param[in] hwNum: length of write data
 *  \return none
 */
void SPI_flash_write_bytes_async(uint8_t *pbyBuf, uint32_t wAddr, uint16_t hwNum)
{
	uint8_t bySend[4] = {PGPRO_CMD, (wAddr >> 16), (wAddr >> 8), wAddr};

	SPI_flash_write_enable_async();		//write enable
	SPICS_CLR; 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)bySend, NULL, 4);		//send write bytes cmd and data three bytes addr 
	csi_spi_send_receive_async(&g_tSpiMaster, (void *)pbyBuf, NULL, hwNum);	//write hwNum bytes
	SPICS_SET;
	SPI_flash_wait_busy_async();
}

/** \brief spi interrupt callback function
 * 
 *  \param[in] spi: SPI handle
 *  \param[in] event: adc event 
 *  \param[in] arg: para
 *  \return none
 */
void apt_spi_event(csi_spi_t *spi, csi_spi_event_t event, void *arg)
{
	//uint32_t wArg = (uint32_t)arg;
	/* add user code*/
	switch(event)
	{
		case SPI_EVENT_SEND_COMPLETE:			//send complete					
			break;
		case SPI_EVENT_RECEIVE_COMPLETE:		//receive complete
			break;
		case SPI_EVENT_SEND_RECEIVE_COMPLETE:		//receive complete
			break;
		case SPI_EVENT_ERROR:					//
		
			break;
		default:
			break;
	}
}
/** \brief spi示例代码
 * 
 *  \param[in] none
 *  \return error code
 */
csi_error_t spi_senario(void)
{
	int iRet = 0;
	
	csi_gpio_init(&g_tGpioB0, 1);
	csi_pin_set_mux(PB05,PIN_FUNC_OUTPUT);		//NSS用PB05模拟
	
	csi_pin_set_mux(PB04,PB04_SPI_SCK);			//PB04 = SPI_SCK
	csi_pin_set_mux(PA015,PA015_SPI_MISO);		//PA15 = SPI_MISO
	csi_pin_set_mux(PA014,PA014_SPI_MOSI);		//PA14 = SPI_MOSI
	SPICS_SET;
	
#ifdef SPI_MASTER_SEL	//spi master mode
	
	#ifdef SPI_SYNC_SEL	//spi sync mode
	
		iRet = csi_spi_init(&g_tSpiMaster,0);			
		iRet = csi_spi_mode(&g_tSpiMaster, SPI_MASTER);	
		iRet = csi_spi_cp_format(&g_tSpiMaster, SPI_FORMAT_CPOL1_CPHA1);	
		iRet = csi_spi_frame_len(&g_tSpiMaster, SPI_FRAME_LEN_8);	
		iRet = csi_spi_baud(&g_tSpiMaster, 1000000);
		if(iRet < 0)
			return -1;
		
		iRet = SPI_flash_read_id();					//read chip id, chip id = 0xef13
		
		if(iRet == 0xef13)
		{
		
			//iRet = SPI_flash_read_status();		//read status reg
			
			SPI_flash_sector_erase(0x1000);			//erase sector 1; start addr = 0x1000
	
			SPI_flash_read_bytes((uint8_t *)byRdBuf, 0x1000, 16);	//read data = 0xff
		
			SPI_flash_write_bytes(byWrBuf,0x1000,16);				//write 16 bytes 
			
			SPI_flash_read_bytes((uint8_t *)byRdBuf, 0x1000, 16);	//read 16 bytes, read bytes = write bytes
			
			iRet = SPI_flash_read_id();					//read chip id, chip id = 0xef13
		}
		
	#else				//spi async mode
	
		uint32_t arg = 0x02;
		
		iRet = csi_spi_init(&g_tSpiMaster,0);		
	
		iRet = csi_spi_attach_callback(&g_tSpiMaster,apt_spi_event, (void *) arg);		
	
		iRet = csi_spi_mode(&g_tSpiMaster, SPI_MASTER);		
	
		iRet = csi_spi_cp_format(&g_tSpiMaster, SPI_FORMAT_CPOL1_CPHA1);		
	
		iRet = csi_spi_frame_len(&g_tSpiMaster, SPI_FRAME_LEN_8);		
	
		iRet = csi_spi_baud(&g_tSpiMaster, 12000000);
		if(iRet < 0)
			return -1;
		
		iRet = SPI_flash_read_id_async();
		
		if(iRet == 0xef13)
		{
			//iRet = SPI_flash_read_status_async();
		
			SPI_flash_sector_erase_async(0x2000);						//erase sector 1; start addr = 0x1000
	
			SPI_flash_read_bytes_async((uint8_t *)byRdBuf, 0x2000, 16);	//read data = 0xff
		
			SPI_flash_write_bytes_async(byWrBuf,0x2000,16);				//write 16 bytes
			
			SPI_flash_read_bytes_async((uint8_t *)byRdBuf, 0x2000, 16);	//read 16 bytes, read bytes = write bytes
		}
		
	#endif

#else					//spi slave mode	

#endif

	return iRet;
}