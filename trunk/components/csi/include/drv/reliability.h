/***********************************************************************//** 
 * \file  reliability.h
 * \brief  reliability driver head file, including LVR/LVD, RSR, emcm and memory check
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-27 <td>V2.0 <td>WNN    <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_LVD_H_
#define _DRV_LVD_H_

#include <drv/common.h>

/// ************************************************************************
///						for LVD module
///*************************************************************************

typedef enum{
	LVD_24 = 0,
	LVD_21,
	LVD_27,
	LVD_30,
	LVD_33,
	LVD_36,
	LVD_39,
}clvd_level_e;

typedef enum {
	LVR_19 = 0,
	LVR_22,
	LVR_25,
	LVR_28,
	LVR_31,
	LVR_34,
	LVR_37,
	LVR_40
}clvr_level_e;

typedef enum {
	LVD_INTF = 1,
	LVD_INTR,
	LVD_INTFR
}clvd_pol_e;

/**
  \brief       lvd  int enable  
  \param       ePol      lvd falling/rising/both 
  \param       eLvl      lvd level
  \return      none
*/
void csi_lvd_int_enable(clvd_pol_e ePol, clvd_level_e eLvl);

/**
  \brief       lvd disable  
  \return      none
*/
void csi_lvd_disable(void);

/**
  \brief       Enable LVR
  \param       eLvl LVR level
  \return      error code \ref csi_error_t
*/
void csi_lvr_enable(clvr_level_e eLvl);

/**
  \brief       Disable LVR
  \return      none
*/
void csi_lvr_disable(void);


/**
  \brief       low voltage reset enable 
  \pasram[in]   ptRlblty
  \return      elvl    lvr level
*/
void csi_lvr_enable(clvr_level_e eLvl);


/**
  \brief       Disable LVR
  \return      error code \ref csi_error_t
*/
void csi_lvr_disable(void);

/**
  \brief       Get lvd level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_get_lvdlevel(void);

/**
  \brief       Get lvr level 
  \param       none
  \return      lvd    lvd level
*/
uint32_t csi_get_lvrlevel(void);
/// ************************************************************************
///						for Reset Source check
///*************************************************************************

typedef enum{
	PO_RST_POS	= 0,
	LV_RST_POS,
	EXT_RST_POS,
	IWDT_RST_POS = 4,
	emcm_RST_POS = 6,
	CPU_RST_POS,
	SW_RST_POS,
	CPUFAULT_RST_POS,
	SRAM_RST_POS = 11,
	EFL_RST_POS,
	WWDT_RST_POS,
	NO_RST
}csi_rsr_e;

/**
  \brief       get chip reset reason
  \param       none
  \return      csi_rsr_e 
*/
csi_rsr_e csi_get_rst_reason(void);

/// ************************************************************************
///						for sram error check
///*************************************************************************
/**
  \brief       set sramcheck fail time.
  \param       wVal times
  \return      none
*/
void csi_sramcheck_set_times(uint16_t wVal);

/**
  \brief       set chip to reset when sramcheck times > preset value
  \param       none
  \return      none
*/
void csi_sramcheck_rst(void);

/**
  \brief       claim INT when sramcheck times > preset value
  \param       none
  \return      none
*/
void csi_sramcheck_int(void);

/**
  \brief       disable sramcheck funtion
  \param       none
  \return      none
*/
void csi_sramcheck_disable(void);

/// ************************************************************************
///						for flash error check
///*************************************************************************
/**
  \brief       set flashcheck fail time.
  \param       wVal times
  \return      error code \ref csi_error_t
*/
void csi_flashcheck_set_times(uint32_t wVal);

/**
  \brief       set chip to reset when flashcheck times > preset value
  \param       none
  \return      none
*/
void csi_flashcheck_rst(void);

/**
  \brief       disable flashcheck funtion
  \param       none
  \return      none
*/
void csi_flashcheck_disable(void);

/// ************************************************************************
///						for EM clock monitor
///*************************************************************************
/**
  \brief       claim INT and switch sysclk to IMOSC when EMOSC failure detected
  \return      none
*/
void csi_emcm_2_imosc_int(void);

/**
  \brief      rest chip when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_emcm_rst(void);


/**
  \brief       disable EMOSC monitor funtion
  \param       none
  \return      none
*/
void csi_emcm_disable(void);

/// ************************************************************************
///						for RSR
///*************************************************************************
/**
  \brief       get chip reset reason
  \param       none
  \return      csi_rsr_e 
*/
csi_rsr_e csi_get_rst_reason(void);

#endif /* _DRV_RLBLTY_H_ */