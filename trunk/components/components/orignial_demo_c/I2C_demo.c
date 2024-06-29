/***********************************************************************//** 
 * \file  i2c_example.c
 * \brief  I2C_EXAMPLE description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-01-19 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include "soc.h"
#include "sys_clk.h"
#include <drv/rlblty.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/iic.h>
#include "csp_syscon.h"
#include "csp_gpio.h"
#include "csp_syscon.h"
#include "csp_i2c.h"
#include "csp_uart.h"
#include "csp.h"
#include "ip_test.h"
/* externs function--------------------------------------------------------*/

/* Private macro-----------------------------------------------------------*/
#define		I2C_SYNC_MODE				//sync/async	
#define		I2C_MASTER_MODE				//master/slave
//#define		I2C_WRITE_MODE					//iic write/read(send/receive)
//EEPROM opration		
//#define		I2C_MEM_SEL					//iic EEPROM select(24c04)

/* at24c04  information ---------------------------------------------------*/
//size: size = 512 bytes = 2 block = 32 pages;
//block: 1 block =  16 pages = 256 bytes 
//page: 1 page = 16 bytes  
//block addr : 0x00,0x01,0x02,0x03
//addr: 10bits; high 2bits = (P1P0 = block addr) + low 8bits; 
//device addr: 0x50; 
//handle addr: 0x50 + low 8bits; 0x51 + low 8bits;

/* Private variablesr------------------------------------------------------*/
csi_iic_t g_tIic;

uint8_t byI2cSBuf[18] = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
uint8_t byI2cRbuf[18];

/** \brief iic interrupt callback function
 * 
 *  \param[in] iic: pointer of IIC reg structure.
 *  \param[in] event: adc event 
 *  \param[in] arg: para
 *  \return none
 */
static void apt_iic_event(csi_iic_t *iic, csi_iic_event_t event, void *arg)
{
	uint32_t wArg = (uint32_t)arg;					//主机时，为用户传入的参数；从机时为接收或者发送的数据长度
	
	/* add user code*/
	switch(event)
	{
		case IIC_EVENT_SEND_COMPLETE:				//主机/从机发送完成	
			if(wArg)
				 wArg =1;
			break;
		case IIC_EVENT_RECEIVE_COMPLETE:			//主机/从机接收完成	
			if(wArg)
				 wArg =1;
			break;
		/*
		case IIC_EVENT_SLAVE_RECEIVE_ADDR_CMD:		//从机接收到主机改变方向访问（主机发送命令/地址，然后读取数据）
			if(wArg)
			{
				switch(byI2cSBuf[0])
				{
					case 0x00:
						byI2cSBuf[0] = 0x30;
						byI2cSBuf[1] = 0x31;
						byI2cSBuf[2] = 0x32;
						byI2cSBuf[3] = 0x33;
						break;
					case 0x01:
						byI2cSBuf[0] = 0x40;
						byI2cSBuf[1] = 0x41;
						byI2cSBuf[2] = 0x42;
						byI2cSBuf[3] = 0x43;
						break;
					case 0x02:
						byI2cSBuf[0] = 0x50;
						byI2cSBuf[1] = 0x51;
						byI2cSBuf[2] = 0x52;
						byI2cSBuf[3] = 0x53;
						break;
					case 0x03:
						byI2cSBuf[0] = 0x60;
						byI2cSBuf[1] = 0x61;
						byI2cSBuf[2] = 0x62;
						byI2cSBuf[3] = 0x63;
						break;
				}
			}
			break;
		*/
		case IIC_EVENT_ERROR:				
			break;
		default:
			break;
	}
}

/** \brief i2c operation
 * 
 *  \param[in] none
 *  \return none
 */ 
int i2c_operation(void)
{
	
	int iRet = 0;
	csi_pin_set_mux(PA19, PA19_I2C0_SCL);			//SCL
	csi_pin_set_mux(PA110,PA110_I2C0_SDA);			//SDA
	
#ifdef	I2C_SYNC_MODE					//I2C同步模式(同步模式支持主机，不支持从机，从机推荐异步模式)				
	
		iRet = csi_iic_init(&g_tIic,0);								//初始化I2C设备结构体，设备号0					
		CSI_CHECK_RETURN(iRet);
		
		iRet = csi_iic_mode(&g_tIic, IIC_MODE_MASTER);				//配置主机模式
		CSI_CHECK_RETURN(iRet);
		
		iRet = csi_iic_addr_mode(&g_tIic, IIC_ADDRESS_7BIT);		//配置7BIT地址模式（7/10BIT addr）
		CSI_CHECK_RETURN(iRet);
		
		iRet = csi_iic_speed(&g_tIic, IIC_BUS_SPEED_FAST);			//配置I2C速度模式（100/400k）	
		CSI_CHECK_RETURN(iRet);
	
	#ifdef	I2C_MEM_SEL					//I2C 读写EEPROM
		
		#ifdef I2C_WRITE_MODE			//写				
			iRet = csi_iic_mem_send(&g_tIic, 0x50, 0x00, IIC_MEM_ADDR_SIZE_8BIT, (void *)byI2cSBuf, 16, 3);		//EEPROM 写
		#else									
			iRet = csi_iic_mem_receive(&g_tIic, 0x50, 0x00, IIC_MEM_ADDR_SIZE_8BIT, (void *)byI2cRbuf, 16, 3);	//EEPROM 读
		#endif
		
	#endif
	
	while(1)
	{
		nop;
		nop;
		
		if(fgetc((FILE *) -1) == 0x06)		//串口控制, 串口控制主机发送读写命令（接收到数据0x06）	
		{
			#ifdef I2C_WRITE_MODE	
				iRet = csi_iic_master_send(&g_tIic, 0x51, (void *)byI2cSBuf, 16, 3);		//发送
			#else
				//uint8_t byAddr[2] = {2,1};
				iRet = csi_iic_master_receive(&g_tIic, 0x50, (void *)byI2cRbuf, 16, 3);		//接收
				//iRet = csi_iic_master_send_receive(&g_tIic, 0x51, byAddr, 1, (void *)byI2cRbuf, 16, 3);	//非标准接口函数，扩展	
			#endif
			fputc(0x02,(FILE *) -1);
		}
	}
	
#else									//I2C异步模式（中断）
	
	#ifdef	I2C_MASTER_MODE				//I2C主机模式
		
		uint32_t arg = 0x02;
		iRet = csi_iic_init(&g_tIic,0);										//初始化I2C设备结构体，设备号0	
		CSI_CHECK_RETURN(iRet);

		iRet = csi_iic_attach_callback(&g_tIic, apt_iic_event, (void *)arg);//注册I2C用户回调函数(中断模式)
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_mode(&g_tIic, IIC_MODE_MASTER);						//配置主机模式
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_addr_mode(&g_tIic, IIC_ADDRESS_7BIT);				//配置7BIT地址模式（7/10BIT addr）
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_speed(&g_tIic, IIC_BUS_SPEED_FAST);					//配置I2C速度模式（100/400k）	
		CSI_CHECK_RETURN(iRet);
		
	#else								//I2C从机模式
		
		iRet = csi_iic_init(&g_tIic,0);										//初始化I2C设备结构体，设备号0	
		CSI_CHECK_RETURN(iRet);

		iRet = csi_iic_attach_callback(&g_tIic, apt_iic_event, (void *)0);	//注册I2C用户回调函数(中断模式)
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_mode(&g_tIic, IIC_MODE_SLAVE);						//配置从机模式
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_addr_mode(&g_tIic, IIC_ADDRESS_7BIT);				//配置7BIT地址模式（7/10BIT addr）
		CSI_CHECK_RETURN(iRet);
	
		iRet = csi_iic_speed(&g_tIic, IIC_BUS_SPEED_FAST);					//配置I2C速度模式（100/400k）	
		CSI_CHECK_RETURN(iRet);
			
		iRet = csi_iic_own_addr(&g_tIic, 0x51);								//配置I2C从机地址（自己的地址）							
		CSI_CHECK_RETURN(iRet);
			
	
		#ifdef	I2C_WRITE_MODE			
			iRet = csi_iic_slave_send_async(&g_tIic, (void *)byI2cSBuf, 1000);		//从机发送数据，即从机接收到主机读取数据命令(主机读取数据)
		#else							
			iRet = csi_iic_slave_receive_async(&g_tIic, (void *)byI2cRbuf, 1000);	//从机接收数据。即从机接收到主机发送数据命令(主机发送数据)
		#endif
			
	#endif
	
	
	while(1)
	{
		nop;
		nop;
	#ifdef	I2C_MASTER_MODE					//I2C主机模式
		if(fgetc((FILE *) -1) == 0x06)		//串口控制主机发送读写命令（接收到数据0x06）	
		{
			#ifdef	I2C_WRITE_MODE	
				iRet = csi_iic_master_send_async(&g_tIic, 0x51, (const void *)byI2cSBuf, 10);		//主机写		
			#else
				iRet = csi_iic_master_receive_async(&g_tIic, 0x51, (void *)byI2cRbuf, 10);			//主机读
			#endif
			fputc(0x03,(FILE *) -1);
		}
	#endif
	
	}
	
#endif

	
	return iRet;
}