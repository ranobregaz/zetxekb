/***********************************************************************//** 
 * \file  crc.c
 * \brief  CRC driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-28 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include <drv/common.h>
#include <drv/crc.h>
#include "csp_common.h"
#include "csp_crc.h"

#define CRC_BASE_ADDRESS (csp_crc_t *)AHB_CRC_BASE
/**
  \brief       Initialize CRC Interface. 1. Initializes the resources needed for the CRC interface 2.registers event callback function
  \param[in]   crc  handle of crc instance
  \param[in]   idx  device id
  \return      \ref csi_error_t
*/
csi_error_t csi_crc_init(void)
{
	
    csi_error_t ret = CSI_OK;  
	csp_crc_clk_enable(CRC_BASE_ADDRESS, ENABLE);
	csp_crc_rst(CRC_BASE_ADDRESS);
	
	return ret;
}


void csi_crc_rst(void)
{
	csp_crc_rst(CRC_BASE_ADDRESS);
}

/**
 * \brief Compute the CRC-7 checksum of a buffer.
 *
 * See JESD84-A441.  Used by the MMC protocol.  Uses 0x09 as the
 * polynomial with no reflection.  The CRC is left
 * justified, so bit 7 of the result is bit 6 of the CRC.
 * init = 0; poly = 0x09 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 *
 * \return    The computed CRC7 value
 */

uint8_t csi_crc7_be(uint8_t crc, uint8_t* data, uint32_t size)
{
	return CSI_UNSUPPORTED;
}

/**
 * \brief Compute the CRC-8 checksum of a buffer.
 *  init = 0 or 0xff; poly = 0x07 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC8 value
 */

uint8_t csi_crc8(uint8_t crc, uint8_t* data, size_t size)
{
	return CSI_UNSUPPORTED;
}

/**
 * \brief Compute the CRC-8 checksum of a buffer.
 *  init = 0; poly = 0x31 refin = 1 refout = 1 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC8 value
 */

uint8_t csi_crc8_maxim(uint8_t crc, uint8_t* data, size_t size)
{
	return CSI_UNSUPPORTED;
}

/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0 or 0xffff; poly = 0x8005 refin = 1 refout = 1 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16(uint16_t crc, uint8_t* data, uint32_t size)
{
	uint32_t i;
	

	csp_crc_set_poly(CRC_BASE_ADDRESS, 2);
	csp_crc_refin_enable(CRC_BASE_ADDRESS, ENABLE);
	csp_crc_refout_enable(CRC_BASE_ADDRESS, ENABLE);
	csp_crc_xorout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_set_seed(CRC_BASE_ADDRESS, crc);
	for (i=0; i<size; i++)
	{
		*(uint8_t *)(AHB_CRC_BASE + 0x14 + (i%4)) = *data;
		data ++;
	}
	return ((uint16_t)csp_crc_get_result(CRC_BASE_ADDRESS));
}



/**
 * \brief Compute the CRC-16/CCITT checksum of a buffer.
 *  init = 0; poly = 0x1021 refin = 1 refout = 1 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16_ccitt( uint16_t crc, uint8_t *data, uint32_t size)
{
	uint32_t i;
	
	csp_crc_set_poly(CRC_BASE_ADDRESS, 0);
	csp_crc_refin_enable(CRC_BASE_ADDRESS, ENABLE);
	csp_crc_refout_enable(CRC_BASE_ADDRESS, ENABLE);
	csp_crc_xorout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_xorin_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_set_seed(CRC_BASE_ADDRESS, crc);
	for (i=0; i<size; i++)
	{
		*(uint8_t *)(AHB_CRC_BASE + 0x14 + (i%4)) = *data;
		data ++;
	}
	return ((uint16_t)csp_crc_get_result(CRC_BASE_ADDRESS));
}

/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0; poly = 0x3d65 refin = 1 refout = 1 xorout = 0xffff
 * \param[in] init_value      crc init value
 * \param[in] data            data buf to be calculate
 * \param[in] size            data size
 * \return    The computed CRC16 with xorout
 */

uint16_t csi_crc16_dnp(uint16_t init_value, uint8_t* data, uint32_t size)
{
	return CSI_UNSUPPORTED;
}


/**
 * \brief Compute the CRC-16 XMODEM checksum of a buffer.
 *  init = 0; poly = 0x1021 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16_itu(uint16_t crc, uint8_t* data, uint32_t size)
{
	uint32_t i;
	
  
	csp_crc_set_poly(CRC_BASE_ADDRESS, 0);
	csp_crc_refin_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_refout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_xorout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_set_seed(CRC_BASE_ADDRESS, crc);
	for (i=0; i<size; i++)
	{
		*(uint8_t *)(AHB_CRC_BASE + 0x14 + (i%4)) = *data;
		data ++;
	}
	return ((uint16_t)csp_crc_get_result(CRC_BASE_ADDRESS));
}


/**
 * \brief Compute the CRC-32 checksum of a buffer.
 *  init = 0; poly = 0xEDB88320 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC32 without xorout
 */
uint32_t csi_crc32_le(uint32_t crc, uint8_t* data, uint32_t size)
{
	return CSI_UNSUPPORTED;
}


/**
 * \brief Compute the CRC-32 checksum of a buffer.
 *  init = 0; poly = 0x04c11db7 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC32 without xorout
 */
uint32_t csi_crc32_be(uint32_t crc, uint8_t* data, uint32_t size)
{
	uint32_t i;
	
	

	csp_crc_set_poly(CRC_BASE_ADDRESS, 3);
	csp_crc_refin_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_refout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_xorout_enable(CRC_BASE_ADDRESS, DISABLE);
	csp_crc_set_seed(CRC_BASE_ADDRESS, crc);
	for (i=0; i<size; i++)
	{
		*(uint8_t *)(AHB_CRC_BASE + 0x14 + (i%4)) = *data;
		data++;
	}
	return (csp_crc_get_result(CRC_BASE_ADDRESS));
}




