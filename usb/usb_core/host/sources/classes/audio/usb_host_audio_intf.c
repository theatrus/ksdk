/**HEADER********************************************************************
*
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: usb_host_audio_intf.c$
* $Version :
* $Date    :
*
* Comments:
*
*   This file Contains the implementation of class driver for Audio devices.
*
*END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"
#include "usb_host_audio.h"


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for audio control interface. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/

usb_status usb_class_audio_control_init
(
    /* [IN] the device handle related to the class driver */
    usb_device_instance_handle      dev_handle,
    /* [IN] the interface handle related to the class driver */
    usb_interface_descriptor_handle intf_handle,
    /* [OUT] printer call struct pointer */
    class_handle*       class_handle_ptr

)
{ 
    audio_control_struct_t*  audio_class = NULL;
    usb_device_interface_struct_t*  pDeviceIntf = NULL;

    audio_class = (audio_control_struct_t*)OS_Mem_alloc(sizeof(audio_control_struct_t));
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_control_init fail on memory allocation\n");
        return USBERR_ERROR;
    }

    audio_class->dev_handle = dev_handle;
    audio_class->intf_handle = intf_handle;
    pDeviceIntf = (usb_device_interface_struct_t*)intf_handle;
    audio_class->ifnum = pDeviceIntf->lpinterfaceDesc->bInterfaceNumber;
    audio_class->host_handle = (usb_host_handle)usb_host_dev_mng_get_host(audio_class->dev_handle);
    audio_class->interrupt_pipe = NULL;
    *class_handle_ptr = (class_handle)audio_class;
    audio_class->in_setup = FALSE;
    return USB_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_control_deinit
(
      /* [IN]  the class driver handle */
      class_handle    handle
)
{ /* Body */
    audio_control_struct_t*  audio_class = (audio_control_struct_t*)handle;
    usb_status                    status = USB_OK;
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_control_deinit fail\n");
        return USBERR_ERROR;
    }

    if (audio_class->interrupt_pipe != NULL)
    {
        status = usb_host_close_pipe(audio_class->host_handle, audio_class->interrupt_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_audio_control_deinit to close pipe\n");
        }
    }
    OS_Mem_free(handle);
    handle = NULL;
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_pre_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_control_pre_deinit
    (
        /* [IN]  the class driver handle */
        class_handle      handle
     )
{
    audio_control_struct_t*  audio_class = (audio_control_struct_t*)handle;
    usb_status                          status = USB_OK;
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_control_pre_deinit fail\n");
        return USBERR_ERROR;
    }

    if (audio_class->interrupt_pipe != NULL)
    {
        status = usb_host_cancel(audio_class->host_handle, audio_class->interrupt_pipe,NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for audio stream interface. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_stream_init
(
    /* [IN] the device handle related to the class driver */
    usb_device_instance_handle      dev_handle,
    /* [IN] the interface handle related to the class driver */
    usb_interface_descriptor_handle intf_handle,
    /* [OUT] printer call struct pointer */
    class_handle*       class_handle_ptr

)
{ /* Body */
    usb_device_interface_struct_t*  pDeviceIntf = NULL;
    //interface_descriptor_t*      intf = NULL;
    endpoint_descriptor_t*       ep_desc = NULL;
    audio_stream_struct_t*  audio_class = NULL;
    uint8_t                                  ep_num;
    usb_status                          status = USB_OK;
    pipe_init_struct_t              pipe_init;
     
    audio_class = (audio_stream_struct_t*)OS_Mem_alloc(sizeof(audio_stream_struct_t));
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_stream_init fail on memory allocation\n");
        return USBERR_ERROR;
    }

    audio_class->dev_handle = dev_handle;
    audio_class->intf_handle = intf_handle;
    audio_class->host_handle = (usb_host_handle)usb_host_dev_mng_get_host(audio_class->dev_handle);
      
    pDeviceIntf = (usb_device_interface_struct_t*)intf_handle;
    //intf = pDeviceIntf->lpinterfaceDesc;
    audio_class->iso_in_pipe = NULL;
    audio_class->iso_out_pipe = NULL;

    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        if (ep_desc->bEndpointAddress & IN_ENDPOINT )
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_RECV;
            pipe_init.pipetype         = USB_ISOCHRONOUS_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = audio_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(audio_class->host_handle, &audio_class->iso_in_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_hid_init fail to open in pipe\n");
                *class_handle_ptr = (class_handle)audio_class;
                return USBERR_ERROR;
            }
        } 
        else
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_SEND;
            pipe_init.pipetype         = USB_ISOCHRONOUS_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = audio_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(audio_class->host_handle, &audio_class->iso_out_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_hid_init fail to open in pipe\n");
                *class_handle_ptr = (class_handle)audio_class;
                return USBERR_ERROR;
            }
        }
            audio_class->iso_ep_num = pipe_init.endpoint_number;
    }
    if ((audio_class->iso_in_pipe == NULL) && (audio_class->iso_out_pipe == NULL)) {
        return USBERR_INIT_FAILED;
    }

    *class_handle_ptr = (class_handle)audio_class;
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_pre_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_stream_pre_deinit
    (
        /* [IN]  the class driver handle */
        class_handle      handle
     )
{
    audio_stream_struct_t*  audio_class = (audio_stream_struct_t*)handle;
    usb_status                          status = USB_OK;
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_stream_pre_deinit fail\n");
        return USBERR_ERROR;
    }

    if (audio_class->iso_in_pipe != NULL)
    {
        status = usb_host_close_pipe(audio_class->host_handle, audio_class->iso_in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_audio_stream_pre_deinit to close iso in pipe\n");
        }
    }
    if (audio_class->iso_out_pipe != NULL)
    {
        status = usb_host_close_pipe(audio_class->host_handle, audio_class->iso_out_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_audio_stream_pre_deinit to close iso out pipe\n");
        }
    }
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_stream_deinit
(
      /* [IN]  the class driver handle */
      class_handle    handle
)
{ /* Body */
    audio_stream_struct_t*  audio_class = (audio_stream_struct_t*)handle;
    usb_status                          status = USB_OK;
    if (audio_class == NULL)
    {
        USB_PRINTF("usb_class_audio_stream_deinit fail\n");
        return USBERR_ERROR;
    }

    if (audio_class->iso_in_pipe != NULL)
    {
        status = usb_host_cancel(audio_class->host_handle, audio_class->iso_in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close iso in pipe\n");
        }
    }
    if (audio_class->iso_out_pipe != NULL)
    {
        status = usb_host_cancel(audio_class->host_handle, audio_class->iso_out_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close iso out pipe\n");
        }
    }
    OS_Mem_free(handle);
    handle = NULL;
    return USB_OK;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_get_descriptors
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_control_get_descriptors
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle                  dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle             intf_handle,

    /* [OUT] pointer to header descriptor */
    usb_audio_ctrl_desc_header_t* *        header_desc,

    /* [OUT] pointer to input terminal descriptor */
    usb_audio_ctrl_desc_it_t* *         it_desc,

    /* [OUT] pointer to output terminal descriptor */
    usb_audio_ctrl_desc_ot_t**          ot_desc,

    /* [OUT] pointer to feature unit descriptor */
    usb_audio_ctrl_desc_fu_t* *             fu_desc
)
{
    usb_device_interface_struct_t*  pDeviceIntf  = (usb_device_interface_struct_t*)intf_handle;;
    uint8_t* pinterfaceEx = pDeviceIntf->interfaceEx;
    usb_status                 status = USB_OK;
    uint32_t i = 0, len = pDeviceIntf->interfaceExlength;
    usb_audio_ctrl_func_desc_t*      fd;

     while(i < len) {
       fd = (usb_audio_ctrl_func_desc_t*)pinterfaceEx;
        switch (fd->header.bdescriptorsubtype) {
        case USB_DESC_SUBTYPE_AUDIO_CS_HEADER:
         // if (USB_SHORT_UNALIGNED_LE_TO_HOST(((*header_desc)->bcdCDC)) > 0x0110)  
          //      status = USBERR_INIT_FAILED;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_IT:
            *it_desc = &fd->it;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_OT:
            *ot_desc = &fd->ot;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_FU:
            *fu_desc = &fd->fu;
            break;
        }
        i += fd->header.bfunctionlength;
       pinterfaceEx += fd->header.bfunctionlength;
    }
    return status;    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_get_descriptors
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

usb_status usb_class_audio_stream_get_descriptors
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle      dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [OUT] pointer to specific audio stream interface descriptor */
    usb_audio_stream_desc_spepific_as_if_t* *       as_itf_desc,

    /* [OUT] pointer to format type descriptor */
    usb_audio_stream_desc_format_type_t* *           frm_type_desc,

    /* [OUT] pointer to specific isochronous endpoint descriptor */
    usb_audio_stream_desc_specific_iso_endp_t* *        iso_endp_spec_desc
)
{
    usb_device_interface_struct_t*  pDeviceIntf  = (usb_device_interface_struct_t*)intf_handle;
    usb_device_ep_struct_t*        pep = pDeviceIntf->ep;
    uint8_t* pinterfaceEx = pDeviceIntf->interfaceEx;
    usb_status                      status = USB_OK;
    uint32_t i = 0, len = pDeviceIntf->interfaceExlength;
    usb_audio_stream_func_desc_t*      fd;
    
     while(i < len) {
        fd = (usb_audio_stream_func_desc_t*)pinterfaceEx;
         switch (fd->as_general.bdescriptorsubtype) {
            case USB_DESC_SUBTYPE_AS_CS_GENERAL:
                *as_itf_desc = &fd->as_general;
                break;
            case USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE:
                *frm_type_desc = &fd->frm_type;
                break;
        }
        i += fd->as_general.blength;
        pinterfaceEx += fd->as_general.blength;
     }
     if (pep->endpointExlength != 0)
        *iso_endp_spec_desc = (usb_audio_stream_desc_specific_iso_endp_t*)pep->endpointEx;
    return status;

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_get_sample_type
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

usb_status usb_class_audio_stream_get_sample_type
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle      dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [OUT] pointer to specific audio stream interface descriptor */
    uint8_t *       type
)
{
    usb_device_interface_struct_t*  pDeviceIntf  = (usb_device_interface_struct_t*)intf_handle;
    uint8_t* pinterfaceEx = pDeviceIntf->interfaceEx;

    if((pinterfaceEx != NULL) && (type !=NULL))
        *type = pinterfaceEx[14];

    return USB_OK;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_set_descriptors
* Returned Value : USB_OK if validation passed
* Comments       :
*     This function is used to set descriptors for control interface
*     Descriptors can be used afterwards by application or by driver
*END*--------------------------------------------------------------------*/

usb_status usb_class_audio_control_set_descriptors
(
    /* [IN] Class Interface structure pointer */
    class_handle            handle,

    /* [IN] header descriptor pointer */
    usb_audio_ctrl_desc_header_t*   header_desc,

    /* [IN] input terminal descriptor pointer */
    usb_audio_ctrl_desc_it_t*       it_desc,

    /* [IN] output terminal descriptor pointer */
    usb_audio_ctrl_desc_ot_t*       ot_desc,

    /* [IN] feature unit descriptor pointer */
    usb_audio_ctrl_desc_fu_t*       fu_desc
)
{ /* Body */
    audio_control_struct_t*  audio_class = (audio_control_struct_t*)handle;
    usb_status              status;

    if ((audio_class == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    status = usb_hostdev_validate(audio_class->dev_handle);
    if (status != USB_OK)
    {
        return USBERR_ERROR;
    }

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors");
#endif

    audio_class->header_desc = header_desc;
    audio_class->it_desc = it_desc;
    audio_class->ot_desc = ot_desc;
    audio_class->fu_desc = fu_desc;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors, SUCCESSFUL");
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_set_descriptors
* Returned Value : USB_OK if validation passed
* Comments       :
*     This function is used to set descriptors for stream interface
*     Descriptors can be used afterwards by application or by driver
*END*--------------------------------------------------------------------*/
usb_status usb_class_audio_stream_set_descriptors
(
    /* [IN] Class Interface structure pointer */
    class_handle                handle,

    /* [IN] audio stream interface descriptor pointer */
    usb_audio_stream_desc_spepific_as_if_t*    as_itf_desc,

    /* [IN] format type descriptor pointer */
    usb_audio_stream_desc_format_type_t*       frm_type_desc,

    /* [IN] isochonous endpoint specific descriptor pointer */
    usb_audio_stream_desc_specific_iso_endp_t* iso_endp_spec_desc
)
{ /* Body */

    audio_stream_struct_t*  audio_class = (audio_stream_struct_t*)handle;
    usb_status              status = USB_OK;

#ifdef _HOST_DEBUG_
     DEBUG_LOG_TRACE("usb_class_audio_stream_set_descriptors");
#endif

    if ((audio_class == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    status = usb_hostdev_validate(audio_class->dev_handle);
    if (status != USB_OK)
    {
        return USBERR_ERROR;
    }

    audio_class->as_itf_desc        = as_itf_desc;
    audio_class->frm_type_desc      = frm_type_desc;
    audio_class->iso_endp_spec_desc     = iso_endp_spec_desc;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors, SUCCESSFUL");
#endif

    return status;
} /* Endbody */
/* EOF */
