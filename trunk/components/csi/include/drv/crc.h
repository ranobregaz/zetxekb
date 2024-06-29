/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     drv_crc.h
 * @brief    Header File for CRC Driver
 * @version  V1.0
 * @date     02. June 2017
 * @model    crc
 ******************************************************************************/
#ifndef _DRV_CRC_H_
#define _DRV_CRC_H_


#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       Initialize CRC Interface. 1. Initializes the resources needed for the CRC interface 2.registers event callback function
  \param[in]   crc  handle of crc instance
  \param[in]   idx  device id
  \return      \ref csi_error_t
*/
csi_error_t csi_crc_init(void);

void csi_crc_rst(void);

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

uint8_t csi_crc7_be(uint8_t crc, uint8_t* data, uint32_t size);

/**
 * \brief Compute the CRC-8 checksum of a buffer.
 *  init = 0 or 0xff; poly = 0x07 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC8 value
 */

uint8_t csi_crc8(uint8_t crc, uint8_t* data, size_t size);

/**
 * \brief Compute the CRC-8 checksum of a buffer.
 *  init = 0; poly = 0x31 refin = 1 refout = 1 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC8 value
 */

uint8_t csi_crc8_maxim(uint8_t crc, uint8_t* data, size_t size);


/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0 or 0xffff; poly = 0x8005 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16(uint16_t crc, uint8_t* data, uint32_t size);

/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0; poly = 0x1021 refin = 1 refout = 1 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16_ccitt(uint16_t crc, uint8_t* data, uint32_t size);

/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0; poly = 0x3d65 refin = 1 refout = 1 xorout = 0xffff
 * \param[in] init_value      crc init value
 * \param[in] data            data buf to be calculate
 * \param[in] size            data size
 * \return    The computed CRC16 with xorout
 */

uint16_t csi_crc16_dnp(uint16_t init_value, uint8_t* data, uint32_t size);

/**
 * \brief Compute the CRC-16 checksum of a buffer.
 *  init = 0; poly = 0x1021 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC16 without xorout
 */

uint16_t csi_crc16_itu(uint16_t crc, uint8_t* data, uint32_t size);


/**
 * \brief Compute the CRC-32 checksum of a buffer.
 *  init = 0; poly = 0xEDB88320 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC32 without xorout
 */

uint32_t csi_crc32_le(uint32_t crc, uint8_t* data, uint32_t size);

/**
 * \brief Compute the CRC-32 checksum of a buffer.
 *  init = 0; poly = 0x04c11db7 refin = 0 refout = 0 xorout = 0
 * \param[in] crc      crc init value or crc immediate result
 * \param[in] data     data buf to be calculate
 * \param[in] size     data size
 * \return    The computed CRC32 without xorout
 */

uint32_t csi_crc32_be(uint32_t crc, uint8_t* data, uint32_t size);




#ifdef __cplusplus
}
#endif

#endif /* _CSI_CRC_H_ */
