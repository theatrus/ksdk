/**HEADER********************************************************************
*
* Copyright (c) 2011,2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: usb_host_audio.c$
* $Version : 3.8.5.0$
* $Date    : Aug-31-2012$
*
* Comments:
*
*   This file Contains the implementation of class driver for Audio devices.
*
*END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_AUDIO
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"
#include "usb_host_audio.h"
#include "usb_host.h"


static void usb_class_audio_cntrl_callback(void* , void* , uint8_t *, uint32_t, usb_status);
static usb_status check_valid_fu(usb_audio_ctrl_desc_fu_t*, uint8_t, uint8_t);
static usb_status check_valid_ep(usb_audio_stream_desc_specific_iso_endp_t*, uint8_t);
extern usb_pipe_handle usb_host_dev_mng_get_control_pipe(usb_device_instance_handle dev_handle);

os_event_handle USB_Event;
#define USB_EVENT_CTRL 0x01
#define USB_EVENT_DATA 0x02

/* usb audio feature command declaration */
static usb_audio_command_t usb_audio_feature_commands[NUMBER_OF_FEATURE_COMMANDS] = 
{
    /* Mute commands */
    {FU_MUTE_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_MUTE, 1}, /* USB_AUDIO_GET_CUR_MUTE */
    {FU_MUTE_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_MUTE, 1}, /* USB_AUDIO_SET_CUR_MUTE */
    /* Volume commands */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_CUR_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_CUR_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_MIN_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_MIN_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_MAX_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_MAX_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_RES_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_RES_VOLUME */
    /* Bass commands */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_CUR_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_CUR_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_MIN_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_MIN_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_MAX_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_MAX_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_RES_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_RES_BASS */
    /* Mid commands */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_CUR_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_CUR_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_MIN_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_MIN_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_MAX_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_MAX_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_RES_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_RES_MID */
    /* Treble commands */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_CUR_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_CUR_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_MIN_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_MIN_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_MAX_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_MAX_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_RES_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_RES_TREBLE */
    /* AGC commands */
    {FU_AGC_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_AGC, 1}, /* USB_AUDIO_GET_CUR_AGC */
    {FU_AGC_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_AGC, 1}, /* USB_AUDIO_SET_CUR_AGC */
    /* Delay commands */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_CUR_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_CUR_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_MIN_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_MIN_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_MAX_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_MAX_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_RES_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_RES_DELAY */
    /* Bass boost commands */
    {FU_BASS_BOOST_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_BASS_BOOST, 1}, /* USB_AUDIO_GET_CUR_BASS_BOOST */
    {FU_BASS_BOOST_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_BASS_BOOST, 1}, /* USB_AUDIO_SET_CUR_BASS_BOOST */
};
/* usb audio endpoint command declaration */
static usb_audio_command_t usb_audio_endpoint_commands[NUMBER_OF_ENDPOINT_COMMANDS] = 
{
    /* Pitch commands */
    {PITCH_MASK, GET_REQUEST_EP , GET_CUR, PITCH_CONTROL, 1}, /* USB_AUDIO_GET_CUR_PITCH */
    {PITCH_MASK, SET_REQUEST_EP, SET_CUR, PITCH_CONTROL, 1}, /* USB_AUDIO_SET_CUR_PITCH */
    /* Sampling frequency commands */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_CUR, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_CUR_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_CUR, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_CUR_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_MIN, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_MIN_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_MIN, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_MIN_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_MAX, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_MAX_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_MAX, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_MAX_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_RES, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_RES_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_RES, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_RES_SAMPLING_FREQ */
};
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when audio control information is sent or received.
*
*END*--------------------------------------------------------------------*/
static void usb_class_audio_cntrl_callback
(
    /* [IN] Unused */
    void*      tr_ptr,
    /* [IN] void*  to the class interface instance */
    void*      param,
    /* [IN] Data buffer */
    uint8_t *   buffer,
    /* [IN] Length of buffer */
    uint32_t     len,
    /* [IN] Error code (if any) */
    usb_status  status
)
{ /* Body */
    usb_status usbstatus;

    audio_control_struct_t* audio_class = (audio_control_struct_t*)param;

#ifdef _HOST_DEBUG_
     DEBUG_LOG_TRACE("usb_class_audio_cntrl_callback");
#endif
    usbstatus = usb_host_release_tr(audio_class->host_handle, tr_ptr) ;
    if (usbstatus != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed status:%d \r\n", (unsigned int)usbstatus);
    }

    audio_class->in_setup = FALSE;
    if (audio_class->ctrl_callback)
    {
        audio_class->ctrl_callback(NULL, audio_class->ctrl_param, buffer, len, status);
    }  

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_cntrl_callback, SUCCESSFUL");
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request.
*     It must be run with USB locked.
*
*END*--------------------------------------------------------------------*/
static usb_status usb_class_audio_cntrl_common
(
    /* [IN] The communication device common command structure */
    audio_command_t*           com_ptr,

    /* [IN] The audio control interface */
    //audio_control_struct_t* if_ctrl_ptr,

    /* [IN] Bitmask of the request type */
    uint8_t                    bmrequesttype,

    /* [IN] Request code */
    uint8_t                    brequest,

    /* [IN] Value to copy into wvalue field of the REQUEST */
    uint16_t                   wvalue,

    /* [IN] Value to copy into windex field of the REQUEST */
    uint16_t                   windex,

    /* [IN] Length of the data associated with REQUEST */
    uint16_t                   wlength,

    /* [IN] Pointer to data buffer used to send/recv */
    uint8_t *                 data
)
{ /* Body */
    audio_control_struct_t*  audio_class = NULL;
    //usb_setup_t                       req;
    usb_status                          status = USB_OK;
    usb_pipe_handle                     pipe_handle;
    tr_struct_t*                       tr_ptr;

    if ((com_ptr == NULL) || (com_ptr->class_control_handle == NULL))
    {
        return USBERR_ERROR;
    }
    audio_class = (audio_control_struct_t*)com_ptr->class_control_handle;

    if (audio_class->in_setup)
    {
        return USBERR_TRANSFER_IN_PROGRESS;
    }
    if (audio_class->dev_handle == NULL)
    {
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
    }

    audio_class->ctrl_callback = com_ptr->callback_fn;
    audio_class->ctrl_param = com_ptr->callback_param;
    pipe_handle = usb_host_dev_mng_get_control_pipe(audio_class->dev_handle);

    if (usb_host_get_tr(audio_class->host_handle, usb_class_audio_cntrl_callback, audio_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr audio \r\n");
        return USBERR_ERROR;
    }

    /* Set TR buffer length as required */
    if ((REQ_TYPE_IN & bmrequesttype) != 0)
    {
        tr_ptr->rx_buffer = data;
        tr_ptr->rx_length = wlength;
    }
    else
    {
        tr_ptr->tx_buffer = data;
        tr_ptr->tx_length = wlength;
    }

    tr_ptr->setup_packet.bmrequesttype = bmrequesttype;
    tr_ptr->setup_packet.brequest     = brequest;
    *(uint16_t*)tr_ptr->setup_packet.wvalue = USB_HOST_TO_LE_SHORT(wvalue);
    *(uint16_t*)tr_ptr->setup_packet.windex =  USB_HOST_TO_LE_SHORT(windex);//USB_HOST_TO_LE_SHORT(((interface_descriptor_t*)(audio_class->intf_handle))->bInterfaceNumber);
    *(uint16_t*)tr_ptr->setup_packet.wlength = USB_HOST_TO_LE_SHORT(wlength);

    audio_class->in_setup = TRUE;
    status = usb_host_send_setup(audio_class->host_handle, pipe_handle, tr_ptr);  
    if (status != USB_OK)
    {
        audio_class->in_setup = FALSE;
        USB_PRINTF("\nError in usb_class_audio_cntrl_common: %x", (unsigned int)status);
        usb_host_release_tr(audio_class->host_handle, tr_ptr);
    }
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_valid_fu
* Returned Value : None
* Comments       :
*     Check whether attribute of feature valid or not.
*
*END*--------------------------------------------------------------------*/
static usb_status check_valid_fu
(
    /* [IN] pointer to feature unit */
    usb_audio_ctrl_desc_fu_t* fu_ptr,

    /* [IN] Control mask */
    uint8_t control_mask,

    uint8_t atribute_index
)
{
    usb_status status = USBERR_ERROR;

    if (NULL!=fu_ptr)
    {
        if(atribute_index<(fu_ptr->blength - 7))
        /* Call num_logical_chan is number of logical channels.
         * Compare atribute_index with (fu_ptr->bcontrolsize*(num_logical_chan + 1))= (fu_ptr->blength -7)
         * to make sure atribute_index is valid element in bmacontrols() array */
        {
            if (fu_ptr->bmacontrols[atribute_index] & control_mask)
            {
                status = USB_OK;
            }
        }
    }
    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_valid_ep
* Returned Value : None
* Comments       :
*     Check whether attribute of endpoint valid or not.
*
*END*--------------------------------------------------------------------*/
static usb_status check_valid_ep
(
    /* [IN] pointer to isochonounous endpoint */
    usb_audio_stream_desc_specific_iso_endp_t* ep_ptr,

    /* [IN] control selector */
    uint8_t control_selector
)
{
    usb_status status = USBERR_ERROR;

    if (NULL!=ep_ptr)
    {
        if (ep_ptr->bmattributes && control_selector)
            status = USB_OK;
        else
            status = USBERR_INVALID_REQ_TYPE;
    }

    return status;
}

static void usb_class_audio_control_recv_callback
   (
      /* [IN] Unused */
      void*      tr_ptr,
      /* [IN] void*  to the class interface instance */
      void*      param,
      /* [IN] Data buffer */
      uint8_t *   buffer,
      /* [IN] Length of buffer */
      uint32_t     len,
      /* [IN] Error code (if any) */
      usb_status  status
   )
{ /* Body */
    audio_control_struct_t*    audio_class = (audio_control_struct_t*)param;

    if (usb_host_release_tr(audio_class->host_handle, tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }

    if (audio_class->interrupt_callback)
    {
        audio_class->interrupt_callback(NULL, audio_class->interrupt_callback_param, buffer, len, status);
    }

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_audio_recv_data
* Returned Value : USB status
* Comments       :
*     Receive data on isochronous IN pipe.
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_control_recv_data
(
    /* [IN] audio control class interface void*   */
    audio_command_t*   audio_ptr,

    /* [IN] buffer pointer */
    uint8_t *               buffer,
        /* [IN] data length */
    uint32_t                 buf_size
    
)
{ /* Body */
    audio_control_struct_t*    audio_class;
    usb_status status = USBERR_ERROR;
    //uint16_t request_value;
    tr_struct_t*             tr_ptr;
  
    if ((audio_ptr == NULL) || (audio_ptr->class_control_handle == NULL))
    {
        return USBERR_ERROR;
    }

    audio_class = (audio_control_struct_t*)audio_ptr->class_control_handle;
    if ((audio_ptr == NULL) || (buffer == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    audio_class->interrupt_callback = audio_ptr->callback_fn;
    audio_class->interrupt_callback_param = audio_ptr->callback_param;
    if (audio_class->dev_handle == NULL)
    {
        return USBERR_ERROR;
    }
     if (usb_host_get_tr(audio_class->host_handle, usb_class_audio_control_recv_callback, audio_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }
    
    tr_ptr->rx_buffer = buffer;
    tr_ptr->rx_length = buf_size;                                            
    status = usb_host_recv_data(audio_class->host_handle, audio_class->interrupt_pipe, tr_ptr);
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in usb_class_hid_recv_data: %x", (unsigned int)status);
        usb_host_release_tr(audio_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
     
    return USB_OK;
} /* Endbody */

static void usb_class_audio_recv_callback
   (
      /* [IN] Unused */
      void*      tr_ptr,
      /* [IN] void*  to the class interface instance */
      void*      param,
      /* [IN] Data buffer */
      uint8_t *   buffer,
      /* [IN] Length of buffer */
      uint32_t     len,
      /* [IN] Error code (if any) */
      usb_status  status
   )
{ /* Body */
    audio_stream_struct_t*    audio_class = (audio_stream_struct_t*)param;

    if (usb_host_release_tr(audio_class->host_handle, tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }
    
    if (audio_class->recv_callback)
    {
        audio_class->recv_callback(NULL, audio_class->recv_param, buffer, len, status);
    }

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_audio_recv_data
* Returned Value : USB status
* Comments       :
*     Receive data on isochronous IN pipe.
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_recv_data
(
    /* [IN] audio control class interface pointer  */
    audio_command_t*   audio_ptr,

    /* [IN] buffer pointer */
    uint8_t *               buffer,
        /* [IN] data length */
    uint32_t                 buf_size
)
{ /* Body */
    audio_stream_struct_t*    audio_class;
    usb_status status = USBERR_ERROR;
    //uint16_t request_value;
    tr_struct_t*             tr_ptr;
    //usb_audio_command_t* p_endpoint_command;
    
    if ((audio_ptr == NULL) || (audio_ptr->class_stream_handle == NULL))
    {
        return USBERR_ERROR;
    }

    audio_class = (audio_stream_struct_t*)audio_ptr->class_stream_handle;
    if ((audio_class == NULL) || (buffer == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    audio_class->recv_callback = audio_ptr->callback_fn;
    audio_class->recv_param = audio_ptr->callback_param;

    if (audio_class->dev_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_host_get_tr(audio_class->host_handle, usb_class_audio_recv_callback, audio_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }
    
    tr_ptr->rx_buffer = buffer;
    tr_ptr->rx_length = buf_size;                                            
    status = usb_host_recv_data(audio_class->host_handle, audio_class->iso_in_pipe, tr_ptr);
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in _usb_host_recv_data: %x", (unsigned int)status);
        usb_host_release_tr(audio_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
     
    return USB_OK;
} /* Endbody */


static void usb_class_audio_send_callback
   (
      /* [IN] Unused */
      void*      tr_ptr,
      /* [IN] Pointer to the class interface instance */
      void*      param,
      /* [IN] Data buffer */
      uint8_t *   buffer,
      /* [IN] Length of buffer */
      uint32_t     len,
      /* [IN] Error code (if any) */
      usb_status  status
   )
{ /* Body */
    audio_stream_struct_t*    audio_class = (audio_stream_struct_t*)param;
    usb_status usbstatus;

    usbstatus = usb_host_release_tr(audio_class->host_handle, tr_ptr);
    if (usbstatus != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed:%x\n",(unsigned int)usbstatus);
    }
    
    if (audio_class->send_callback)
    {
        audio_class->send_callback(NULL, audio_class->send_param, buffer, len, status);
    }

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_audio_send_data
* Returned Value : USB status
* Comments       :
*     Send audio data to device through a isochronous OUT pipe.
*
*END*--------------------------------------------------------------------*/

usb_status usb_class_audio_send_data
(
    /* [IN] audio control class interface pointer  */
    audio_command_t*   audio_ptr,

    /* [IN] buffer pointer */
    uint8_t *               buffer,
    
    /* [IN] data length */
    uint32_t                        buf_size
)
{ /* Body */

    audio_stream_struct_t*    audio_class;
    usb_status status = USBERR_ERROR;
    //uint16_t request_value;
    tr_struct_t*             tr_ptr;
    //usb_audio_command_t* p_endpoint_command;

    if ((audio_ptr == NULL) || (audio_ptr->class_stream_handle == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    audio_class = (audio_stream_struct_t*)audio_ptr->class_stream_handle;
    if ((audio_class == NULL) || (buffer == NULL))
    {
        USB_PRINTF("get audio class parameter error\n");
        return USBERR_ERROR;
    }

    audio_class->send_callback = audio_ptr->callback_fn;
    audio_class->send_param = audio_ptr->callback_param;

    if (audio_class->dev_handle == NULL)
    {
        USB_PRINTF("get audio class dev handle error\n");
        return USBERR_ERROR;
    }
    if (usb_host_get_tr(audio_class->host_handle, usb_class_audio_send_callback, audio_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }

    tr_ptr->tx_buffer = buffer;
    tr_ptr->tx_length = buf_size;                                            
    status = usb_host_send_data(audio_class->host_handle, audio_class->iso_out_pipe, tr_ptr);
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in _usb_host_send_data: %x", (unsigned int)status);
        usb_host_release_tr(audio_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
     
    return USB_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_copy_protect
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get copy protect value
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_copy_protect
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] Buffer to receive data */
    void*                     buf
)
{ /* Body */

    audio_control_struct_t* if_ptr;
    uint16_t windex;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->it_desc->bterminalid)<<8)|(if_ptr->ifnum));

    /* check valid of input terminal descriptor */
    if (if_ptr->it_desc==NULL)
    {
        return USBERR_NO_DESCRIPTOR;
    }
    else
    {
        return usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_CUR,
                                        (uint16_t)((COPY_PROTECT_CONTROL << 8)),windex,1,buf);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_copy_protect
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send copy protect value
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_copy_protect
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] Buffer to send data */
    void*                    buf
)
{ /* Body */

    audio_control_struct_t*     if_ptr;
    uint16_t windex;
    usb_audio_ctrl_desc_ot_t* ot_desc;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    ot_desc = (usb_audio_ctrl_desc_ot_t*)if_ptr->ot_desc;

    windex = (uint16_t)(((ot_desc->bterminalid)<<8)|(if_ptr->ifnum));

    /* check valid of input terminal descriptor */
    if (ot_desc==NULL)
    {
        return USBERR_NO_DESCRIPTOR;
    }
    else
    {
        return usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_CUR,
                                    (uint16_t)((COPY_PROTECT_CONTROL << 8)),windex,1,buf);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_feature_command
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get or set feature units 
*     of audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_feature_command
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to send data */
    void*                       buf,
    /* [IN] The command to perform */
    uint32_t                      cmd
)
{ /* Body */
    uint16_t windex;
    uint16_t request_value;
    audio_control_struct_t*     if_ptr;
    usb_audio_command_t* p_feature_command;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    /* Validity checking: for audio interface */
    status = usb_hostdev_validate(if_ptr->dev_handle);
    if (USB_OK == status)
    {
        /* pointer to command */
        p_feature_command = &(usb_audio_feature_commands[cmd]);
        /* get request value */
        request_value = (uint16_t)((p_feature_command->request_value << 8) | channel_no);
        windex = (uint16_t)(((if_ptr->fu_desc->bunitid) << 8)|(if_ptr->ifnum));

        /* Check whether this attribute valid or not */
        status = check_valid_fu(if_ptr->fu_desc,p_feature_command->control_mask,if_ptr->fu_desc->bcontrolsize*channel_no);

        if (USB_OK == status)
        {
            status = usb_class_audio_cntrl_common(com_ptr,
                                  p_feature_command->request_type,
                                  p_feature_command->request_code,
                                  request_value,
                                  windex,
                                  p_feature_command->length,
                                  buf);
        }
    }
    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_endpoint_command
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to performs services
*      for endpoint
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_endpoint_command
(
    /* [IN] Class Interface structure void*  */
    audio_command_t*            com_ptr,

    /* [IN] Buffer to receive data */
    void*                       buf,
    /* [IN] The command to perform */
    uint32_t                      cmd
)
{
    uint8_t Endp_num;
    audio_stream_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;
    uint16_t request_value;
    usb_audio_command_t* p_endpoint_command;


    if_ptr = (audio_stream_struct_t*)com_ptr->class_stream_handle;
    
    /* pointer to command */
    p_endpoint_command = &(usb_audio_endpoint_commands[cmd]);
    /* get request value */
    request_value     = (uint16_t)((p_endpoint_command->request_value << 8));
    
    /* Check whether this attribute valid or not */
    status = check_valid_ep(if_ptr->iso_endp_spec_desc,p_endpoint_command->control_mask);

    if (USB_OK == status)
    {
        /* Any isochronous pipe is supported? */
        if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
        {
            return USBERR_INVALID_NUM_OF_ENDPOINTS;
        }
        else if (NULL!=if_ptr->iso_in_pipe)
        {

            Endp_num = (if_ptr->iso_ep_num | 0x80);
        }
        else
        {
            Endp_num = if_ptr->iso_ep_num; 
        }/* Endif */

        status = usb_class_audio_cntrl_common(com_ptr,
                                              p_endpoint_command->request_type,
                                              p_endpoint_command->request_code,
                                              request_value,
                                              (uint16_t)Endp_num,
                                              p_endpoint_command->length,
                                              buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_cur_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get current graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_cur_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to receive data */
    void*                       buf
)
{ /* Body */

    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_CUR,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_cur_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send current graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_cur_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to send data */
    void*                      buf
)
{ /* Body */
    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_CUR,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_min_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get min graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_min_graphic_eq
(
    /* [IN] Class Interface structure void*  */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to receive data */
    void*                       buf
)
{ /* Body */

    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_MIN,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_min_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send min graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_min_graphic_eq
(
    /* [IN] Class Interface structure void*  */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to send data */
    void*                       buf
)
{ /* Body */
    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_MIN,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_max_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get max graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_max_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to receive data */
    void*                     buf
)
{ /* Body */

    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_MAX,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_max_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send max graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_max_graphic_eq
(
    /* [IN] Class Interface structure void*  */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to send data */
    void*                       buf
)
{ /* Body */
    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_MAX,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_res_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get resolution graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_res_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to receive data */
    void*                       buf
)
{ /* Body */
    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_RES,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_res_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send resolution graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_res_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*            com_ptr,

    /* [IN] length of the Buffer */
    uint16_t                      blen,

    /* [IN] channel number */
    uint8_t                       channel_no,

    /* [IN] Buffer to send data */
    void*                       buf
)
{ /* Body */
    uint16_t windex;
    audio_control_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_control_struct_t*)com_ptr->class_control_handle;
    windex = (uint16_t)(((if_ptr->fu_desc->bunitid)<<8)|(if_ptr->ifnum));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bcontrolsize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_RES,
                                            (uint16_t)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),windex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_mem_endpoint
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get memory of the endpoint
*     from the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_get_mem_endpoint
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*          com_ptr,

    /* [IN] zero-based offset value */
    uint16_t                    offset,

    /* [IN] length of the Buffer */
    uint16_t                    blen,

    /* [IN] Buffer to receive data */
    void*                     buf
)
{ /* Body */

    uint8_t Endp_num;
    audio_stream_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;
//  pipe_struct_t* pipe_ptr; 

    if_ptr = (audio_stream_struct_t*)com_ptr->class_stream_handle;

    /* Any isochronous pipe is supported? */
    if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
    {
        return USBERR_INVALID_NUM_OF_ENDPOINTS;
    }
    else if (if_ptr->iso_in_pipe!=NULL)
    {
         Endp_num = if_ptr->iso_ep_num ;
    }
    else
    {
        Endp_num = if_ptr->iso_ep_num ;
    }/* Endif */

    status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_EP,GET_MEM,
                                       offset,(uint16_t)Endp_num,blen,buf);

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_mem_endpoint
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send memory of the endpoint
*     to the audio device
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_set_mem_endpoint
(
    /* [IN] Class Interface structure pointer */
    audio_command_t*           com_ptr,

    /* [IN] zero-based offset value */
    uint16_t                     offset,

    /* [IN] length of the Buffer */
    uint16_t                     blen,

    /* [IN] Buffer to send data */
    void*                      buf
)
{ /* Body */

    uint8_t Endp_num;
    audio_stream_struct_t*     if_ptr;
    usb_status status = USBERR_ERROR;

    if_ptr = (audio_stream_struct_t*)com_ptr->class_stream_handle;

    /* Any isochronous pipe is supported? */
    if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
    {
        return USBERR_INVALID_NUM_OF_ENDPOINTS;
    }
    else if (if_ptr->iso_in_pipe!=NULL)
    {
         Endp_num = if_ptr->iso_ep_num ;
    }
    else
    {
         Endp_num = if_ptr->iso_ep_num ;
    }/* Endif */

    status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_EP,SET_MEM,
                                         offset,(uint16_t)Endp_num,blen,buf);
    return status;
}

#endif
