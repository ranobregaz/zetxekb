/***********************************************************************//** 
 * \file  cnta.h
 * \brief header file for cnta driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version   <th>Author  <th>Description
 * <tr><td> 2020-9-09 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2021-5-27 <td>V0.1  <td>LQ      <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_CNTA_H_
#define _DRV_CNTA_H_

#include <drv/common.h>
#include <csp_cnta.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \struct csi_conta_timer_config_t
/// \brief  conta timer parameter configuration
typedef struct {
	cnta_ckdiv_e eClkDiv;  //时钟分频
	cnta_mode_e  eRunMode; //单次还是重复
	uint8_t     byInter;   //int source
	uint32_t	wTime;	   //周期（us）
} csi_conta_timer_config_t;

/// \struct csi_conta_pwm_config_t
/// \brief  conta pwm parameter configuration
typedef struct {
	cnta_ckdiv_e eClkDiv;    //时钟分频选择
	uint8_t		byStartLevel;//计数器pwm初始电平
	uint8_t		byStopLevel; //计数器pwm结束电平
	uint8_t     byInter;     //int source
	uint8_t		byDutyCycle; //占空比(0-100)
	uint32_t	wFreq;	     //频率（hz）	
} csi_conta_pwm_config_t;

typedef enum
{
	POLAR_LOW	= 0,
    POLAR_HIGH,       
	STOP_LOW,	
    STOP_HIGH   	    
}csi_cnta_pwmlev_t;

typedef enum
{
	PWM_CARRIER		= 0,
    PWM_ENVELOPE    
}csi_cnta_pwmout_t;

typedef enum
{
	MATCH_CARRIERON_DIS	= 0,
	MATCH_CARRIERON_CLR, 
	MATCH_CARRIERON_SET       
}csi_cnta_tcmatch_e;

typedef enum
{
	PEND_CARRIERON_DIS	= 0,
	PEND_CARRIERON_CLR, 
	PEND_CARRIERON_SET      
}csi_cnta_tcpend_e;

typedef enum
{
	CNTA_SW_EN			= (0x01ul)  
}csi_cnta_sw_updata_e;

typedef enum
{
	CNTA_HW_DIS			= (0x00ul),
	CNTA_HW_TCMATCH		= (0x01ul), 
	CNTA_HW_TCPEND		= (0x02ul)       
}csi_cnta_hw_updata_e;

/** \brief initialize cnta data structure
 * 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \param[in] ptContaTimerCfg:point of timer parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_init(csp_cnta_t *ptCntaBase,csi_conta_timer_config_t *ptContaTimerCfg);

/** \brief start cnta
 * 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \return none
 */ 
void csi_cnta_start(csp_cnta_t *ptCntaBase);

/**
  \brief       Stop cnta
  \param[in]   ptCntaBase    pointer of cnta register structure
  \return      None
*/
void csi_cnta_stop(csp_cnta_t *ptCntaBase);

/**
  \brief       Get cnta datah value
  \param[in]   ptCntaBase    pointer of cnta register structure
  \return      cnta datah load value
*/
uint32_t csi_cnta_get_datah_value(csp_cnta_t *cnta);

/** \brief get cnta datal load value
 * 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \return cnta datal load value
 */ 
uint32_t csi_cnta_get_datal_value(csp_cnta_t *ptCntaBase);

/** \brief cnta pwm init 
 * 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \param[in] ptContaPwmCfg:point of pwm parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_pwm_init(csp_cnta_t *ptCntaBase,csi_conta_pwm_config_t *ptContaPwmCfg);

/** \brief updata cnta pwm freq para: (datah and datal value)
 * 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \param[in] hwDatah: rem ouput high time long 
 *  \param[in] hwDatal: rem ouput low time long  
 *  \param[in] eUpdata: data reg soft updata control
 *  \return none
 */
void csi_cnta_pwm_para_updata(csp_cnta_t *ptCntaBase, uint16_t hwDatah, uint16_t hwDatal, csi_cnta_sw_updata_e eUpdata);

/** \brief set cnta tc sync 
 *  \param[in] ptCntaBase: pointer of cnta register structure
 *  \param[in] tcpend_rem: carrieron control of tc pend end 
 *  \param[in] tcmatch_rem: carrieron control of tc pulse match 
 *  \param[in] hw_updata: cadata reg auto updata control
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_bt0_sync(csp_cnta_t *ptCntaBase, csi_cnta_tcpend_e tcpend_rem, csi_cnta_tcmatch_e tcmatch_rem,csi_cnta_hw_updata_e hw_updata);
//---------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TIMER_H_ */