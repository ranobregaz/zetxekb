#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "drv/ringbuffer.h"


/* Private macro------------------------------------------------------*/
#define min(a, b)  (((a) < (b)) ? (a) : (b))

/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/


/** \brief ringbuffer in  receive a bunch of data; dynamic scan
 * 
 *  \param[in] [in] ptFifo: The fifo to be used.
 *  \return none
 */ 
void ringbuffer_in_dynamic_scan(ringbuffer_t *ptFifo)
{
	if(ptFifo->hwDataLen > 0)
	{
		if(ptFifo->hwPreDataLen  == ptFifo->hwDataLen)			//receive a bunch of data complete
		{
			ptFifo->hwPreDataLen = 0;
			ptFifo->byRecvFlg = 1;								//receive a bunch of data complete, set status flag
		}
		else
			ptFifo->hwPreDataLen = ptFifo->hwDataLen;			//receive go on
	}
}
/** \brief  Is the FIFO full?
  * 
  * \param  [in] ptFifo: The fifo to be used.
  * \retval true:      Yes.
  * \retval false:     No.
  */
bool ringbuffer_in_dynamic_status(ringbuffer_t *ptFifo)
{
    return (ptFifo->byRecvFlg  > 0);
}

/** \brief  Removes the entire FIFO contents.
  * 
  * \param  [in] ptFifo: The fifo to be emptied.
  * \return None.
  */
void ringbuffer_reset(ringbuffer_t *ptFifo)
{
    ptFifo->hwWrite = 0U;
    ptFifo->hwRead  = 0U;
    ptFifo->hwDataLen = 0U;
	ptFifo->byRecvFlg = 0U;
}

/** \brief  Puts onebyte data into the FIFO.
  * 
  * \param  [in] fifo: The fifo to be used.
  * \param  [in] in:   The data to be added.
  * \note   This function copies at most @len bytes from the @in into
  *         the FIFO depending on the free space, and returns the number
  *         of bytes copied.
  */
void ringbuffer_byte_in(ringbuffer_t *ptFifo, uint8_t byDataIn)
{
	if(ptFifo->hwDataLen < ptFifo->hwSize)
	{
		ptFifo->pbyBuf[ptFifo->hwWrite] = byDataIn;
		ptFifo->hwWrite = (ptFifo->hwWrite + 1) % ptFifo->hwSize;
		ptFifo->hwDataLen ++;
	}
}
/** \brief  Puts some data into the FIFO.
  * 
  * \param  [in] ptFifo: The fifo to be used.
  * \param  [in] pDataIn: The data to be added.
  * \param  [in] hwLen: The length of the data to be added.
  * \return The number of bytes copied.
  * \note   This function copies at most @len bytes from the @in into
  *         the FIFO depending on the free space, and returns the number
  *         of bytes copied.
  */
uint32_t ringbuffer_in(ringbuffer_t *ptFifo, const void *pDataIn, uint16_t hwLen)
{
    uint32_t writelen = 0U, tmplen = 0U;

    if (ringbuffer_is_full(ptFifo)) {
        writelen = 0U;
    } else {
        tmplen = ptFifo->hwSize - ptFifo->hwDataLen;
        writelen = (tmplen > hwLen) ? hwLen : tmplen;

        if (ptFifo->hwWrite < ptFifo->hwRead) {
            memcpy((void *)&ptFifo->pbyBuf[ptFifo->hwWrite], (void *)pDataIn, writelen);
        } else {
            tmplen = ptFifo->hwSize - ptFifo->hwWrite;

            if (writelen <= tmplen) {
                memcpy((void *)&ptFifo->pbyBuf[ptFifo->hwWrite], (void *)pDataIn, writelen);
            } else {
                memcpy((void *)&ptFifo->pbyBuf[ptFifo->hwWrite], (void *)pDataIn, tmplen);
                memcpy((void *)ptFifo->pbyBuf, (uint8_t *)pDataIn + tmplen, writelen - tmplen);
            }
        }
        ptFifo->hwWrite = (ptFifo->hwWrite + writelen) % ptFifo->hwSize;
        ptFifo->hwDataLen += writelen;
    }

    return writelen;
}
/** \brief  Gets some data from the FIFO.
  * 
  * \param  [in] ptFifo: The fifo to be used.
  * \param  [in] pOutBuf: Where the data must be copied.
  * \param  [in] hwLen: The size of the destination buffer.
  * \return The number of copied bytes.
  * \note   This function copies at most @len bytes from the FIFO into
  *         the @out and returns the number of copied bytes.
  */
uint32_t ringbuffer_out(ringbuffer_t *ptFifo, void *pOutBuf, uint16_t hwLen)
{
	uint32_t readlen = 0U, tmplen = 0U;
	
	if(ringbuffer_is_empty(ptFifo)) 
        readlen = 0U;
    else
	{
		readlen = (hwLen > ptFifo->hwDataLen) ? ptFifo->hwDataLen : hwLen;
        tmplen = ptFifo->hwSize - ptFifo->hwRead;
		
		if(NULL != pOutBuf)
		{
			if(readlen < tmplen)
				memcpy((void *)pOutBuf, (void *)&ptFifo->pbyBuf[ptFifo->hwRead], readlen);
			else
			{
				memcpy((void *)pOutBuf, (void *)&ptFifo->pbyBuf[ptFifo->hwRead], tmplen);
				memcpy((uint8_t *)pOutBuf + tmplen, (void *)ptFifo->pbyBuf, readlen - tmplen);
			}
		}
	}
	
	ptFifo->hwRead = (ptFifo->hwRead + readlen) % ptFifo->hwSize;
	ptFifo->hwDataLen -= readlen;
	
	return readlen;
	
//    uint32_t readlen = 0U, tmplen = 0U;
//
//    if (ringbuffer_is_empty(fifo)) {
//        readlen = 0U;
//    } 
//	else {
//        readlen = (len > fifo->data_len) ? fifo->data_len : len;
//        tmplen = fifo->size - fifo->read;
//
//        if (NULL != outbuf) {
//            if (readlen <= tmplen) {
//                memcpy((void *)outbuf, (void *)&fifo->buffer[fifo->read], readlen);
//            } else {
//                memcpy((void *)outbuf, (void *)&fifo->buffer[fifo->read], tmplen);
//                memcpy((uint8_t *)outbuf + tmplen, (void *)fifo->buffer, readlen - tmplen);
//            }
//        }
//
//        fifo->read = (fifo->read + readlen) % fifo->size;
//        fifo->data_len -= readlen;
//    }
//
//    return readlen;
}

