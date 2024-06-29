/***********************************************************************//** 
 * \file  lvd.h
 * \brief  
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-13 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_LVD_H_
#define _DRV_LVD_H_

#include <drv/common.h>

/// ************************************************************************
///						for LVD module
///*************************************************************************


typedef struct csi_reliability {
    csi_dev_t       dev;
} csi_reliability_t;

typedef struct csi_lvd{
	void (*callback)(void);
}csi_lvd_t;

typedef struct csi_eflash_check{
	void (*callback)(void);
}csi_eflash_check_t;

typedef struct csi_sram_check{
	void (*callback)(void);
}csi_sram_check_t;

typedef struct csi_emcm_check{
	void (*callback)(void);
}csi_emcm_check_t;

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


/**
  \brief       Attach the callback handler to Rlblty
  \pasram[in]   callback    callback function
  \pasram[in]   arg         callback's pasram
  \return      error  code \ref csi_error_t
*/
csi_error_t csi_lvd_attach_callback(void *callback);


/**
  \brief       Get lvd level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_lvd_get_lvdlevel(void);

/**
  \brief       Get lvr level 
  \pasram[in]   none
  \return      lvd    lvd level
*/
uint32_t csi_lvd_get_lvrlevel(void);

/**
  \brief       lvd V risng int enable 
  \pasram     eLvl      lvd level
  \param       callback: user callback function
  \return     none
*/
void csi_lvd_r_enable(clvd_level_e eLvl, void *callback);

/**
  \brief       lvd V risng int enable 
  \pasram     eLvl      lvd level
  \param       callback: user callback function
  \return     none
*/
void csi_lvd_r_enable(clvd_level_e eLvl, void *callback);

/**
  \brief       lvd V risng and falling int enable 
  \pasram[in]   eLvl      lvd level
  \pasram[in]   callback  user callback funtion
  \return     none
*/
void csi_lvd_f_r_enable(clvd_level_e eLvl, void *callback);


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
/// ************************************************************************
///						for Reset Source check
///*************************************************************************

typedef enum{
	PO_RST_POS	= 0,
	LV_RST_POS,
	EXT_RST_POS,
	IWDT_RST_POS = 4,
	EMCM_RST_POS = 6,
	CPU_RST_POS,
	SW_RST_POS,
	CPUFAULT_RST_POS,
	SRAM_RST_POS = 11,
	EFL_RST_POS,
	WWDT_RST_POS,
	NO_RST
}csi_rst_rsr_e;

/**
  \brief       Initialize Virtual Reliablity Interface. Initializes the resources needed for the EPT interface
  \param       none
  \return      error code \ref csi_error_t
*/
csi_error_t csi_reliability_init(void);
/// ************************************************************************
///						for sram error check
///*************************************************************************
/**
  \brief       set sramcheck fail time.
  \param       wVal times
  \return      error code \ref csi_error_t
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
  \param       callback: user callback function
  \return      none
*/
void csi_sramcheck_int(void *callback);

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
  \param       callback: user callback function
  \return      none
*/
void csi_EMCM_2_IMOSC_int(void * callback);

/**
  \brief      rest chip when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_EMCM_rst(void);

/**
  \brief       switch sysclk to IMOSC when EMOSC failure detected
  \param       none
  \return      none
*/
void csi_EMCM_2_IMOSC(void);

/**
  \brief       disable EMOSC monitor funtion
  \param       none
  \return      none
*/
void csi_EMCM_disable(void);

/// ************************************************************************
///						for RSR
///*************************************************************************
/**
  \brief       get chip reset reason
  \param       none
  \return      csi_rst_rsr_e 
*/
csi_rst_rsr_e csi_get_rst_reason(void);

#endif /* _DRV_RLBLTY_H_ */