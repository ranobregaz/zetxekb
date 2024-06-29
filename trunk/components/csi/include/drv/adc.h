/***********************************************************************//** 
 * \file  adc.h
 * \brief  csi adc driver head file
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/
#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

#include <stdint.h>
#include <stdbool.h>

#include <drv/common.h>


#ifdef __cplusplus
extern "C" {
#endif

/****** ADC Event *****/
typedef enum {
    ADC_EVENT_CONVERT_COMPLETE = 0,      ///< All data convert completed
    ADC_EVENT_CONVERT_HALF_DONE,         ///< Convert half done
    ADC_EVENT_ERROR,                      ///< Converted data has not been read before the new conversion result is load to the data register
	ADC_EVENT_CMP,
} csi_adc_event_t;

typedef struct csi_adc csi_adc_t;
struct csi_adc {
    csi_dev_t                dev;        ///< Hw-device info
    void (*callback)(csi_adc_t *adc, csi_adc_event_t event, void *arg);  ///< User callback ,signaled by driver event
    void                    *arg;        ///< User private param ,passed to user callback
    uint32_t                *data;       ///< Data buf
    uint32_t                 num;        ///< Data size (depth)
    csi_error_t             (*start)(csi_adc_t *adc);  ///< Start function
    csi_error_t             (*stop)(csi_adc_t *adc);   ///< Stop function
    csi_state_t             state;       ///< current state
    void                    *priv;		 ///< channels that are used 
};

typedef enum{
	ADCVERF_VDD_VSS 		= (0x00ul ),
	ADCVREF_VREFP_VSS		= (0x01ul ),
	ADCVREF_FVR2048_VSS		= (0x02ul ),
	ADCVREF_FVR4096_VSS		= (0x03ul ),
	ADCVREF_INTVREF_VSS		= (0x04ul ),
	ADCVREF_VDD_VREFN		= (0x08ul ),
	ADCVREF_VREFP_VREFN		= (0x09ul ),
	ADCVREF_FVR2048_VREFN	= (0x0aul ),
	ADCVREF_FVR4096_VREFN	= (0x0bul ),
	ADCVREF_INTVREF_VREFN	= (0x0cul )
}csi_adc_vref_t;

typedef enum{
	ADCIN0		= 0ul,
	ADCIN1,
	ADCIN2,
	ADCIN3,
	ADCIN4,
	ADCIN5,
	ADCIN6,
	ADCIN7,
	ADCIN8,
	ADCIN9,
	ADCIN10,
	ADCIN11,
	ADCIN12,
	ADCIN13,
	ADCIN14,
	ADCIN15,
	ADCIN16,
	ADCIN17,
	ADCIN_INTVREF	= 0x1cul,
	ADCIN_1_4VDD,
	ADCIN_VSS
}csi_adc_ch_t;


typedef enum{
	ADC_SYNCEN0			= 0,
	ADC_SYNCEN1,		
	ADC_SYNCEN2,	
	ADC_SYNCEN3,	
	ADC_SYNCEN4,		
	ADC_SYNCEN5  	

}csi_adc_trgin_t;

typedef enum{
	ADC_TRG_CONT		= 0,
	ADC_TRG_OT					
}csi_adc_trgmode_t;

typedef enum {
	ADCSYNC_NONE		= 0,
	ADCSYNC_SWTGR		,
	ADCSYNC_IN0		,
	ADCSYNC_IN1		,
	ADCSYNC_IN2		,
	ADCSYNC_IN3		,
	ADCSYNC_IN4		,
	ADCSYNC_IN5		
}csi_adc_syncsrc_t;

typedef enum{
	ADC_TRGSRC_NONE		= 0,
	ADC_TRGSRC_EOC,		
	ADC_TRGSRC_READY,	
	ADC_TRGSRC_OVR,	
	ADC_TRGSRC_CMP0H,		
	ADC_TRGSRC_CMP0L,		
	ADC_TRGSRC_CMP1H,		
	ADC_TRGSRC_CMP1L,	
	ADC_TRGSRC_SEQEND0,	
	ADC_TRGSRC_SEQEND1,
	ADC_TRGSRC_SEQEND2,	
	ADC_TRGSRC_SEQEND3,
	ADC_TRGSRC_SEQEND4,	
	ADC_TRGSRC_SEQEND5,
	ADC_TRGSRC_SEQEND6,	
	ADC_TRGSRC_SEQEND7,
	ADC_TRGSRC_SEQEND8,	
	ADC_TRGSRC_SEQEND9,
	ADC_TRGSRC_SEQEND10,	
	ADC_TRGSRC_SEQEND11,
	ADC_TRGSRC_SEQEND12,	
	ADC_TRGSRC_SEQEND13,
	ADC_TRGSRC_SEQEND14,	
	ADC_TRGSRC_SEQEND15
}csi_adc_trgsrc_t;

typedef enum {
	ADC_CMP_R = 0,
	ADC_CMP_F,
}csi_adc_cmp_dir_t;

typedef enum{
	ADC_INT_EOC		= 0,
	ADC_INT_READY		= 1,
	ADC_INT_OVR		= 2,
	ADC_INT_CMP0H		= 4,
	ADC_INT_CMP0L		= 5,
	ADC_INT_CMP1H		= 6,
	ADC_INT_CMP1L		= 7,
	
	ADC_INT_SEQ0		= 16,			//SEQX0-15     
	ADC_INT_SEQ1  	= 17,     
	ADC_INT_SEQ2  	= 18,     
	ADC_INT_SEQ3  	= 19,     
	ADC_INT_SEQ4  	= 20,     
	ADC_INT_SEQ5  	= 21,     
	ADC_INT_SEQ6 	 	= 22,     
	ADC_INT_SEQ7  	= 23,     
	ADC_INT_SEQ8  	= 24,     
	ADC_INT_SEQ9  	= 25,     
	ADC_INT_SEQ10  	= 26,     
	ADC_INT_SEQ11   	= 27,     
	ADC_INT_SEQ12   	= 28,     
	ADC_INT_SEQ13   	= 29,     
	ADC_INT_SEQ14   	= 30,     
	ADC_INT_SEQ15   	= 31,
}csi_adc_int_t;

typedef enum{
	ADC_FVR2048 = 0,
	ADC_FVR4096
}csi_adc_fvrsel_e;


/**
  \brief       Initialize adc Interface. Initialize the resources needed for the adc interface
  \param[in]   adc    ADC handle to operate
  \param[in]   idx    ADC index
  \return      Return adc handle if successful
*/
csi_error_t csi_adc_init(csi_adc_t *adc, uint32_t idx);

/**
  \brief       De-initialize adc Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle    ADC handle to operate
  \return      None 
*/
void csi_adc_uninit(csi_adc_t *adc);

/**
  \brief       Set adc receive buffer
  \param[in]   adc    Handle adc handle to operate
  \param[in]   num    The receive data length
  \return      Error code
*/
csi_error_t csi_adc_set_buffer(csi_adc_t *adc, uint32_t *data, uint32_t num);

/**
  \brief       Start adc
  \param[in]   handle    ADC handle to operate
  \return      Error code
*/
csi_error_t csi_adc_start(csi_adc_t *adc);

/**
  \brief       Enable dma or interrupt, and start adc conversion
  \param[in]   handle    ADC handle to operate
  \return      Error code
*/
csi_error_t csi_adc_start_async(csi_adc_t *adc);

/**
  \brief       Stop adc
  \param[in]   handle    ADC handle to operate
  \return      Error code
*/
csi_error_t csi_adc_stop(csi_adc_t *adc);

/**
  \brief       Disable dma or interrupt, and stop adc conversion
  \param[in]   handle    ADC handle to operate
  \return      Error code
*/
csi_error_t csi_adc_stop_async(csi_adc_t *adc);

/**
  \brief       ADC channel enable
  \param[in]   adc          Handle ADC handle to operate
  \param[in]   ch_id        ADC channel id
  \param[in]   is_enable    true->enable, false->disable
  \return      Error code
*/
#ifdef CSI_TEST
	csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable);
#else
	csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable, csi_adc_syncsrc_t eSrc);
#endif

/**
  \brief       Set the ADC channel sampling time
  \param[in]   adc          Handle ADC handle to operate
  \param[in]   ch_id        ADC channel id
  \param[in]   clock_num    Channel sampling clock number
  \return      Error code
*/
csi_error_t csi_adc_channel_sampling_time(csi_adc_t *adc, uint8_t ch_id, uint16_t clock_num);

/**
  \brief       Set the ADC controller sampling time
  \param[in]   adc          Handle ADC handle to operate
  \param[in]   clock_num    ADC controller sampling clock number
  \return      Error code
*/
csi_error_t csi_adc_sampling_time(csi_adc_t *adc, uint16_t clock_num);

/**
  \brief       Enable the continue mode of ADC
  \param[in]   adc          Handle ADC handle to operate
  \param[in]   is_enable    true->enable, false->disable
  \return      Error code
*/
csi_error_t csi_adc_continue_mode(csi_adc_t *adc, bool is_enable);

/**
  \brief       Set ADC frequence division
  \param[in]   adc    Handle ADC handle to operate
  \param[in]   div    The division of frequence
  \return      The actual config frequency
*/
uint32_t csi_adc_freq_div(csi_adc_t *adc, uint32_t div);

/**
  \brief       Receiving data from ADC receiver
  \param[in]   handle    ADC handle to operate
  \return      If read successful, this function shall return the result of convert value
               otherwise, the function shall return error code
*/
int32_t csi_adc_read(csi_adc_t *adc);

/**
  \brief       Receiving data from ADC receiver
  \param[in]   handle    ADC handle to operate
  \param[in]   ch    ADC seq ch
  \return      If read successful, this function shall return the result of convert value
               otherwise, the function shall return error code
*/
int32_t csi_adc_read_channel(csi_adc_t *adc, uint32_t ch);

/**
  \brief       Get ADC state
  \param[in]   adc      ADC handle to operate
  \param[in]   state    ADC state
  \return      Error code
*/
csi_error_t csi_adc_get_state(csi_adc_t *adc, csi_state_t *state);

/**
  \brief       Attach the callback handler to adc
  \param[in]   adc         Operate handle
  \param[in]   callback    callback function
  \param[in]   arg         User can define it by himself as callback's param
  \return      Error code
*/
csi_error_t csi_adc_attach_callback(csi_adc_t *adc, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   uart    Operate handle
  \return      None 
*/
void        csi_adc_detach_callback(csi_adc_t *adc);

/**
  \brief       Enable adc power manage
  \param[in]   adc    ADC handle to operate
  \return      Error code
*/
csi_error_t csi_adc_enable_pm(csi_adc_t *adc);

/**
  \brief       Disable adc power manage
  \param[in]   adc    ADC handle to operate
  \return      None 
*/
void csi_adc_disable_pm(csi_adc_t *adc);

/** \brief set adc vref
  \param[in] adc: ADC handle to operate
  \param[in] adc_vref: source of adc reference voltage
  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_set_vref(csi_adc_t *adc, csi_adc_vref_t src_vref);

/** 
   \brief rearm adc sync 
   \param[in] adc: ADC handle to operate
   \param[in] adc_trgin: adc evtrg input channel(0~5)
   \return error code \ref csi_error_t
 */
csi_error_t csi_adc_rearm_sync(csi_adc_t *adc, csi_adc_trgin_t adc_trgin);

/** 
   \brief set adc sync evtrg 
   \param[in] adc: ADC handle to operate
   \param[in] adc_trgin: adc sync evtrg input channel(0~5)
   \param[in] trg_mode: adc evtrg mode, continuous/once
   \param[in] trg_delay: adc input evtrg delay
   \return error code \ref csi_error_t
 */
csi_error_t csi_adc_set_sync(csi_adc_t *adc, csi_adc_trgin_t adc_trgin, csi_adc_trgmode_t trg_mode, uint8_t trg_delay);

/** 
   \brief set adc evtrg output
   \param[in] adc: ADC handle to operate
   \param[in] adc_trgsrc: adc evtrg source(0~23) 
   \param[in] trg_out: adc evtrg out select(0~1)
   \return error code \ref csi_error_t
 */
csi_error_t csi_adc_set_evtrg(csi_adc_t *adc, uint8_t trg_out, csi_adc_trgsrc_t adc_trgsrc);

/** \brief clear adc INT status
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] wSeQEndInt:  SEQ_END[] INT
 */
void csi_adc_clr_status(csi_adc_t *adc, uint32_t wSeQEndInt);

/** \brief enable/disable adc INT status
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] wInt:  INT
 */
void csi_adc_int_enable(csi_adc_t *adc, uint32_t wInt, bool bEnable);

 /** \brief adc cmp1 config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp1 data
 *  \param[in] byCmpChnl: cmp1 chnl(adc seqx)
 *  \return none
 */
 void csi_adc_set_cmp1(csi_adc_t *adc, uint8_t byCmpChnl, csi_adc_cmp_dir_t dir,  uint32_t wCmpData);
 
 /** \brief adc cmp0 config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp0 data
 *  \param[in] byCmpChnl: cmp0 chnl(adc seqx)
 *  \return none
 */
 void csi_adc_set_cmp0(csi_adc_t *adc, uint8_t byCmpChnl, csi_adc_cmp_dir_t dir,  uint32_t wCmpData);
 
 /** \brief fvr output config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] csi_adc_fvrsel_e: FVR output level select
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
 */
 void csi_adc_fvrout_enable(csi_adc_t *adc, csi_adc_fvrsel_e eLvl, bool bEnable);
 
 /** \brief buffer output(1V) config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
 */
 void csi_adc_bufout_enable(csi_adc_t *adc, bool bEnable);
 
 
#ifdef __cplusplus
}
#endif

#endif /* _DRV_ADC_H_ */
