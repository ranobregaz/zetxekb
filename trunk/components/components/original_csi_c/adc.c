/***********************************************************************//** 
 * \file  adc.c
 * \brief  csi adc driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/
//#include <csi_config.h>
#include <drv/adc.h>
#include <drv/irq.h>
#include <drv/clk.h>
#include <sys_clk.h>
#include "csp_common.h"
#include "drv/gpio.h"

#include "csp_adc.h"
/* Private macro-----------------------------------------------------------*/
#define VOID_P_ADD(p, val) do{ uint8_t *temp = (uint8_t *)p; temp += val; p = (void *)temp; }while(0);
#define VOID_P_DEC(p, val) do{ uint8_t *temp = (uint8_t *)p; temp -= val; p = (void *)temp; }while(0);

/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

// ADC Average/
typedef struct {
    adc_cnt_e	eAvgCnt;
	adc_avg_e 	eAvgSel;
    bool	eAvgEn;
} csi_adc_average_t;

const csi_adc_average_t g_tAdcAvg[1] = { 
	
	{CVCNT_1, AVGSEL_1, DISABLE}
};

static uint8_t s_byBufLen = 0x00;
uint8_t g_byAdcDepth;

/** \brief adc interrupt handle function
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] args: dev of adc
 *  \return none
 */ 


static void apt_adc_irqhandler(void *args)
{
	csi_adc_t *adc		= (csi_adc_t *)args;
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
    uint8_t i;
	uint32_t wChnlNUm = (uint32_t)adc->priv;
	
	uint32_t wIntSt = csp_adc_get_imr_all(adc_base) & csp_adc_get_status_all(adc_base);
	
	if(wIntSt & 0xf0 ){
		if (adc->callback) 
		{
			adc->callback(adc, ADC_EVENT_CMP, (void*)(wIntSt>>4));
			if (wIntSt & 0x10)
				csp_adc_clr_status(adc_base, 4);
			if (wIntSt & 0x20)
				csp_adc_clr_status(adc_base, 5);
			if (wIntSt & 0x40)
				csp_adc_clr_status(adc_base, 6);
			if (wIntSt & 0x80)
				csp_adc_clr_status(adc_base, 7);
			
		}
	}
		
	if(adc->data != NULL)
	{
		if(adc->num > 0)
		{
			for(i = 0; i < wChnlNUm; i++)
			{
				if(csp_adc_get_status(adc_base,ADC12_SEQ(i)))
				{
					*(adc->data + i*s_byBufLen + adc->num- 1) = csp_adc_get_data(adc_base, i);
					csp_adc_clr_status(adc_base,ADC12_SEQ(i));
				}
			}
			adc->num -- ;
		}
		
		if(adc->num == 0)
		{
			if (adc->callback) 
			{
				adc->callback(adc, ADC_EVENT_CONVERT_COMPLETE, adc->arg);
				adc->state.readable = 1U;
			}
		}
	}
}
/** \brief adc interrupt start config 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */ 
static csi_error_t apt_adc_start_intr(csi_adc_t *adc)
{
	//uint8_t i;
	uint32_t wChnlNUm = (uint32_t)adc->priv;					//channel num
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	if(wChnlNUm == 0)
		return CSI_ERROR;
	
	/*for(i = 0; i < wChnlNUm; i++)
	{
		csp_adc_set_int(adc_base, ADC12_SEQ(i), true);			//enable seq int
	}*/
	
 	csp_adc_set_int(adc_base, ADC12_SEQ(wChnlNUm-1), true);			//enable last seq int
	
	csp_adc_wait_ready(adc_base);		//adc ready ok?
	csp_adc_start(adc_base);			//start adc
	return CSI_OK;
}
/** \brief adc interrupt stop config 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */ 
 static csi_error_t apt_adc_stop_intr(csi_adc_t *adc)
{
    uint8_t i;
	uint32_t wChnlNUm = (uint32_t)adc->priv;					//channel num
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	if(wChnlNUm == 0)
		return CSI_ERROR;
	
	for(i = 0; i < wChnlNUm; i++)
	{
		csp_adc_set_int(adc_base, ADC12_SEQ(i), false);			//disable seq int
	}
	
	if(csp_adc_get_status(adc_base, ADC12_CTCVS))				//continuous mode
		csp_adc_stop(adc_base);									//stop adc
	
	return CSI_OK;
}
/** \brief initialize adc data structure
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] port_idx: gpio index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_init(csi_adc_t *adc, uint32_t idx)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
    csi_error_t ret = CSI_OK;
	csp_adc_t *adc_base;
	
	adc->priv = 0U;			//clr adc seq num
	
	if (target_get(DEV_ADC_TAG, idx, &adc->dev) != CSI_OK)
		ret = CSI_ERROR;
    else
	{
		adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
		
		adc->state.writeable = 1U;
		adc->state.readable  = 1U;
		adc->state.error     = 0U;
		adc->callback        = NULL;
		adc->arg             = NULL;
		adc->data            = NULL;
		adc->start           = NULL;
		adc->stop            = NULL;
	
		csi_clk_enable(&adc->dev);						//adc peripheral clk en
		csp_adc_set_clk(adc_base,ENABLE);				//ADC CLK ENABLE
		csp_adc_swrst(adc_base);						//reset all registers 
		csp_adc_set_clk(adc_base,ENABLE);				//ADC CLK ENABLE
		csp_adc_set_resolution(adc_base,ADC12_12BIT);		//12BIT AD
		//csp_adc_set_vref(adc_base,VERF_VDD_VSS);		//ADC VREF
		csp_adc_en(adc_base);							//enable  adc
	}
	return ret;
}

/** \brief de-initialize adc interface
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return none
 */ 
void csi_adc_uninit(csi_adc_t *adc)
{
	//CSI_PARAM_CHK_NORETVAL(adc);
	if(adc == NULL)
		return ;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csi_irq_detach((uint32_t)adc->dev.irq_num);	//unregister irq 
	
	csp_adc_swrst(adc_base);					//reset all registers 
	//csp_adc_dis(adc_base);						//disable  adc
}

/** \brief set adc value buf
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] data: pointer of adc value buf
 *  \param[in] num: adc value buf length
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_set_buffer(csi_adc_t *adc, uint32_t *data, uint32_t num)
{
    csi_error_t ret = CSI_OK;
//    CSI_PARAM_CHK(adc, CSI_ERROR);
//    CSI_PARAM_CHK(data, CSI_ERROR);
//    CSI_PARAM_CHK(num, CSI_ERROR);
//	
	if(adc == NULL || data ==NULL)
		return CSI_ERROR;

    if (num == 0U) {
        ret = CSI_ERROR;
    }else{
        adc->data = data;
        adc->num = num;
		s_byBufLen = adc->num;
        ret = CSI_OK;
    }
    return ret;
}
/** \brief start adc sample,synchronism mode
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_start(csi_adc_t *adc)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
		
	uint32_t byCurrNums;
    csi_error_t   ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	byCurrNums = (uint32_t)adc->priv;
	
	do{
       
        if (adc->state.readable == 0U || adc->state.writeable == 0U) 		//rx buffer not full or last conversion existed 
		{
            ret = CSI_BUSY;
            break;
		}
	
		if(0 == byCurrNums)
		{
			ret = CSI_ERROR;
			break;
		}
		else
		{
			if(csp_adc_get_status(adc_base, ADC12_CTCVS))	//continuous mode
				adc->state.writeable = 0U;
				
			csp_adc_wait_ready(adc_base);					//adc ready ok?
			csp_adc_start(adc_base);						//start adc
		}
	}while(0);
	
	return ret;
}
/** \brief stop adc sample
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_adc_stop(csi_adc_t *adc)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
	
    csi_error_t   ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	if(csp_adc_get_status(adc_base, ADC12_CTCVS))			//continuous mode
		csp_adc_stop(adc_base);								//stop adc
	
	adc->state.readable  = 1U;
    adc->state.writeable = 1U;
    adc->data = NULL;
    //adc->num = 0U;
	
	return ret;
}
/** \brief enable adc sample channel 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] ch_id: channel num
 *  \param[in] is_enable: true->enable, false->disable
 *  \param		eSrc:  sync src
 *  \return error code \ref csi_error_t
 */ 
#ifdef CSI_TEST
csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

	uint32_t byCurrNums;
    csi_error_t ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	byCurrNums = (uint32_t)adc->priv;
	if(is_enable)
	{
		VOID_P_ADD(adc->priv, 1U);
		csp_adc_set_seqx(adc_base,byCurrNums,ch_id, g_tAdcAvg[0].eAvgCnt,g_tAdcAvg[0].eAvgSel,g_tAdcAvg[0].eAvgEn, ADCSYNC_SWTGR);
		csp_adc_set_seq_num(adc_base,(uint32_t)adc->priv);	
	}
	else
	{
		if(byCurrNums > 0)
			VOID_P_DEC(adc->priv, 1U);
	}

	return ret;
}
#else
csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable, csi_adc_syncsrc_t eSrc)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
		
	uint32_t byCurrNums;
    csi_error_t ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	byCurrNums = (uint32_t)adc->priv;
	if(is_enable)
	{
		VOID_P_ADD(adc->priv, 1U);
		csp_adc_set_seqx(adc_base,byCurrNums,ch_id, g_tAdcAvg[0].eAvgCnt,g_tAdcAvg[0].eAvgSel,g_tAdcAvg[0].eAvgEn, eSrc);
		csp_adc_set_seq_num(adc_base,(uint32_t)adc->priv);	
	}
	else
	{
		if(byCurrNums > 0)
			VOID_P_DEC(adc->priv, 1U);
	}

	return ret;
}
#endif

/** \brief set adc sample time,;adc clk period num  
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] clock_num: clock_num > 3 && clock_num <= 0xff; sample time = 16 + clock_num
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_sampling_time(csi_adc_t *adc, uint16_t clock_num)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
    csi_error_t ret = CSI_OK;
	
	if(clock_num == 0)
		 ret = CSI_ERROR;
	else
	{
		if(clock_num < 3)
			clock_num = 3;
			
		csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
		csp_adc_hold_cycle(adc_base, clock_num);
	}
	
	return ret;
}
/** \brief set adc channel sample time,;adc clk period num  
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] ch_id: ADC channel id
 *  \param[in] clock_num: clock_num > 3 && clock_num <= 0xff; sample time = 16 + clock_num
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_channel_sampling_time(csi_adc_t *adc, uint8_t ch_id, uint16_t clock_num)
{
	
	return CSI_UNSUPPORTED;
}
/** \brief set adc clk,;adc clk = pclk/div
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] div: Frequency division num; div: 1~62
 *  \return adc clk
 */ 
uint32_t csi_adc_freq_div(csi_adc_t *adc, uint32_t div)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csp_adc_set_clk_div(adc_base,div);	
	div = csp_adc_get_clk_div(adc_base);
	return  soc_get_pclk_freq()/div;		
}
/** \brief get adc clk 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return adc clk
 */
uint32_t csi_adc_get_freq(csi_adc_t *adc)
{
   // CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
    uint32_t div;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	div = csp_adc_get_clk_div(adc_base);
	return  soc_get_pclk_freq()/div;
}
/** \brief set adc sample mode, continue/one time
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] is_enable: enable/disable
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_adc_continue_mode(csi_adc_t *adc, bool is_enable)
{
    //CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);

    if (is_enable)
		csp_adc_set_conv_mode(adc_base, ADC12_CONTCV);
	else 
		csp_adc_set_conv_mode(adc_base, ADC12_OTCV);

    return CSI_OK;
}
/** \brief get adc state
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] state: csi adc state
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_adc_get_state(csi_adc_t *adc, csi_state_t *state)
{
   // CSI_PARAM_CHK(adc, CSI_ERROR);
   if(adc == NULL)
		return CSI_ERROR;
    *state = adc->state;
    return CSI_OK;
}

/** \brief get adc value of seq channel
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] ch: channel of seq
 *  \return error code \ref csi_error_t
 */
int32_t csi_adc_read_channel(csi_adc_t *adc,uint32_t ch)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
		
	uint32_t wTimeOut = 0;
	csi_error_t  ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	do{
		if(csp_adc_get_status(adc_base, ADC12_SEQ(ch)))
		{
			ret = csp_adc_get_data(adc_base, ch);
			csp_adc_clr_status(adc_base,ADC12_SEQ(ch));	//clr ch
			break;
		}
		else
		{
			wTimeOut ++;
			if(wTimeOut > 0xffff)
			{
				ret = CSI_TIMEOUT;
				break;
			}
		}
	}while(1);
	
	return ret;
}
/** \brief get adc value 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] ch: channel of seq
 *  \return error code \ref csi_error_t
 */
int32_t csi_adc_read(csi_adc_t *adc)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	//csi_error_t  ret = CSI_OK;
	return CSI_UNSUPPORTED;
}
 /** \brief  register adc interrupt callback function
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_attach_callback(csi_adc_t *adc, void *callback, void *arg)
{
//    CSI_PARAM_CHK(adc, CSI_ERROR);
//    CSI_PARAM_CHK(callback, CSI_ERROR);
	
	if(adc == NULL )
		return CSI_ERROR;
	
    adc->callback = callback;
    adc->arg      = arg;
    adc->start    = apt_adc_start_intr;
    adc->stop     = apt_adc_stop_intr;
	
    csi_irq_attach((uint32_t)adc->dev.irq_num, &apt_adc_irqhandler, &adc->dev);
    csi_irq_enable((uint32_t)adc->dev.irq_num);
    return CSI_OK;

}
/** \brief  cancel adc callback function
 * 
 *  \param[in] gpio: ADC handle to operate
 *  \return none
 */ 
 void csi_adc_detach_callback(csi_adc_t *adc)
 {
	//CSI_PARAM_CHK_NORETVAL(adc);
	if(adc)
		return;
		
    adc->callback  = NULL;
    adc->arg       = NULL;
    adc->start     = NULL;
    adc->stop      = NULL;
 }
/** \brief start adc sample,asynchronism mode
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */ 
 csi_error_t csi_adc_start_async(csi_adc_t *adc)
{
    //CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

    csi_error_t ret = CSI_OK;
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	do{
		if ((adc->data == NULL) || (adc->num == 0U))		
		{
            ret = CSI_ERROR;
            break;
        }
      
        if (adc->state.readable == 0U || adc->state.writeable == 0U) 		// rx buffer not full or last conversion existed
		{
            ret = CSI_BUSY;
            break;
        }

        if (adc->start) 
		{
            adc->start(adc);

			if(csp_adc_get_status(adc_base, ADC12_CTCVS))	//continuous mode
			{
                adc->state.writeable = 0U;
                adc->state.readable  = 0U;
            }
        }
    }while(0);
	
	g_byAdcDepth = adc -> num;
	return ret;
}

/** \brief stop adc sample,asynchronism mode
 * 
 *  \param[in] adc: ADC handle to operate
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_adc_stop_async(csi_adc_t *adc)
{
    //CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
		
    csi_error_t ret = CSI_OK;

    if (adc->stop) 
	{
        adc->stop(adc);
        adc->state.readable  = 1U;
        adc->state.writeable = 1U;
        adc->data = NULL;
        //adc->num = 0U;
    }
	else 
	{
        ret = CSI_ERROR;
    }

    return ret;
}
/** \brief set adc vref
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] adc_vref: source of adc reference voltage
 *  \return error code \ref csi_error_t
 */  
csi_error_t csi_adc_set_vref(csi_adc_t *adc, csi_adc_vref_t src_vref)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;
	
    csi_error_t   ret = CSI_OK;
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csp_adc_set_vref(adc_base,(adc_vref_e)src_vref);
	return ret;
}
/** \brief set adc sync 
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] adc_trgin: sync(0~5) of adc input channels
 *  \param[in] trg_mode: adc sync  mode, continuous/once
 *  \param[in] trg_delay: adc input  delay, delay timer =  (trg_delay+1)*4 PCLK
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_adc_set_sync(csi_adc_t *adc, csi_adc_trgin_t adc_trgin, csi_adc_trgmode_t trg_mode, uint8_t trg_delay)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

    csi_error_t ret = CSI_OK;
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csp_adc_set_sync_delay(adc_base, adc_trgin, trg_delay);
	csp_adc_set_sync(adc_base, adc_trgin, trg_mode, true);
	
	return ret;
}
/** \brief rearm adc sync
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] adc_trgin: adc sync evtrg input channel(0~5)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_adc_rearm_sync(csi_adc_t *adc, csi_adc_trgin_t adc_trgin)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

    csi_error_t ret = CSI_OK;
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csp_adc_rearm_sync(adc_base, adc_trgin);
	
	return ret;
}

/** \brief set adc evtrg output
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] adc_trgsrc: adc evtrg source(0~23) 
 *  \param[in] trg_out: adc evtrg out select(0~1)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_adc_set_evtrg(csi_adc_t *adc, uint8_t trg_out, csi_adc_trgsrc_t adc_trgsrc)
{
	//CSI_PARAM_CHK(adc, CSI_ERROR);
	if(adc == NULL)
		return CSI_ERROR;

    csi_error_t ret = CSI_OK;
    csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	
	csp_adc_set_evtrg(adc_base, adc_trgsrc, trg_out);
	csp_adc_evtrg_en(adc_base, trg_out);
	
	return ret;
}

/** \brief clear adc INT status
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] wInt:  INT
 */
void csi_adc_clr_status(csi_adc_t *adc, uint32_t wInt)
{
	if(adc == NULL)
		return ;
		
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	 
	csp_adc_clr_status(adc_base, wInt);
}

/** \brief enable/disable adc INT status
 * 
 *  \param[in] adc: ADC handle to operate
 *  \param[in] wInt:  INT
 */
void csi_adc_int_enable(csi_adc_t *adc, uint32_t wInt, bool bEnable)
{
	if(adc == NULL)
		return;
	 csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	 
	 csp_adc_set_int(adc_base, wInt, bEnable);
}


/** \brief adc cmp0 config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp0 data
 *  \param[in] byCmpChnl: cmp0 chnl(adc seqx)
 *  \param	dir: direction
 *  \return none
 */
 void csi_adc_set_cmp0(csi_adc_t *adc, uint8_t byCmpChnl, csi_adc_cmp_dir_t dir,  uint32_t wCmpData)
 {
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	csp_adc_set_cmp_mode(adc_base, ADC12_CMP_ACROSS);
	csp_adc_set_cmp0(adc_base, wCmpData,byCmpChnl) ;
	if (dir == ADC_CMP_R)
		csp_adc_set_int(adc_base,ADC12_CMP0H , ENABLE);
	else
		csp_adc_set_int(adc_base,ADC12_CMP0L , ENABLE);
 }
 
 /** \brief adc cmp1 config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp1 data
 *  \param[in] byCmpChnl: cmp1 chnl(adc seqx)
 *  \param	dir: direction
 *  \return none
 */
 void csi_adc_set_cmp1(csi_adc_t *adc, uint8_t byCmpChnl, csi_adc_cmp_dir_t dir,  uint32_t wCmpData)
 {
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	csp_adc_set_cmp1(adc_base, wCmpData,byCmpChnl) ;
	if (dir == ADC_CMP_R)
		csp_adc_set_int(adc_base,ADC12_CMP1H , ENABLE);
	else
		csp_adc_set_int(adc_base,ADC12_CMP1L , ENABLE);
 }
 
 /** \brief fvr output config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] csi_adc_fvrsel_e: FVR output level select
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
 */
 void csi_adc_fvrout_enable(csi_adc_t *adc, csi_adc_fvrsel_e eLvl, bool bEnable)
 {
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	csp_adc_set_fvrout_lvl(adc_base, eLvl);
	csp_adc_fvrout_enable(adc_base, bEnable);
 }
 
  /** \brief buffer output(1V) config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return none
 */
 void csi_adc_bufout_enable(csi_adc_t *adc, bool bEnable)
 {
	csp_adc_t *adc_base = (csp_adc_t *)HANDLE_REG_BASE(adc);
	csp_adc_bufout_enable(adc_base, bEnable);
 }