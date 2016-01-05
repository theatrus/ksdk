/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: usb_host_hub.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains the implementation of class driver for hub devices.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_HUB
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"
#include "usb_host_dev_mng.h"

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_init
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_init
(
/* [IN]  the device handle related to the class driver */
usb_device_instance_handle dev_handle,
/* [IN]  the interface handle related to the class driver */
usb_interface_descriptor_handle intf_handle,
/* [OUT] printer call struct pointer */
usb_class_handle* class_handle_ptr
)
{ /* Body */
    usb_hub_class_struct_t* hub_class = NULL;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    //interface_descriptor_t*       intf = NULL;
    endpoint_descriptor_t* ep_desc = NULL;
    uint8_t ep_num;
    usb_status status = USB_OK;
    pipe_init_struct_t pipe_init;
    uint8_t level = 0xFF;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_init");
#endif

    level = usb_host_dev_mng_get_level(dev_handle);

    if (level > 5)
    {
        USB_PRINTF("Too many level of hub attached\n");
        *class_handle_ptr = NULL;
        return USBERR_ERROR;
    }

    hub_class = (usb_hub_class_struct_t*)OS_Mem_alloc_zero(sizeof(usb_hub_class_struct_t));
    if (hub_class == NULL)
    {
        USB_PRINTF("usb_class_hub_init fail on memory allocation\n");
        *class_handle_ptr = NULL;
        return USBERR_ERROR;
    }

    hub_class->dev_handle = dev_handle;
    hub_class->intf_handle = intf_handle;
    hub_class->host_handle = usb_host_dev_mng_get_host(hub_class->dev_handle);
    hub_class->level = usb_host_dev_mng_get_level(hub_class->dev_handle);

    pDeviceIntf = (usb_device_interface_struct_t*)intf_handle;
    //intf = pDeviceIntf->lpinterfaceDesc;

    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        if ((ep_desc->bEndpointAddress & IN_ENDPOINT) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == IRRPT_ENDPOINT))
        {
            pipe_init.endpoint_number = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction = USB_RECV;
            pipe_init.pipetype = USB_INTERRUPT_PIPE;
            pipe_init.max_packet_size = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval = ep_desc->iInterval;
            pipe_init.flags = 0;
            pipe_init.dev_instance = hub_class->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(hub_class->host_handle, &hub_class->interrupt_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_hid_init fail to open in pipe\n");
                *class_handle_ptr = (usb_class_handle)hub_class;
                return USBERR_ERROR;
            }
        }
    }
    hub_class->control_pipe = usb_host_dev_mng_get_control_pipe(hub_class->dev_handle);
    *class_handle_ptr = hub_class;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_init, SUCCESSFUL");
#endif
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_deinit
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to deinitialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hub_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_hub_class_struct_t* hub_class = (usb_hub_class_struct_t*)handle;
    usb_status status = USB_OK;
    if (hub_class == NULL)
    {
        USB_PRINTF("usb_class_hid_deinit fail\n");
        return USBERR_ERROR;
    }

    if (hub_class->interrupt_pipe != NULL)
    {
        status = usb_host_close_pipe(hub_class->host_handle, hub_class->interrupt_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_hid_deinit to close pipe\n");
        }
    }

    OS_Mem_free(handle);
    //USB_PRINTF("HID class driver de-initialized\n");
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_pre_deinit
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hub_pre_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_hub_class_struct_t* hub_class = (usb_hub_class_struct_t*)handle;
    usb_status status = USB_OK;
    if (hub_class == NULL)
    {
        USB_PRINTF("_usb_host_cancel_call_interface fail\n");
        return USBERR_ERROR;
    }

    if (hub_class->interrupt_pipe != NULL)
    {
        status = usb_host_cancel(hub_class->host_handle, hub_class->interrupt_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }

    //USB_PRINTF("HID class driver pre_deinit\n");
    return USB_OK;
} /* Endbody */

usb_status usb_class_hub_get_level
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] buffer length (how many bytes to read) */
uint8_t* level
)
{
    usb_hub_class_struct_t* hub_class;
    //usb_status                     status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_recv_bitmap");
#endif

    if ((com_ptr == NULL) || (com_ptr->class_ptr == NULL))
    {
        return USBERR_ERROR;
    }

    hub_class = (usb_hub_class_struct_t*)com_ptr->class_ptr;

    *level = hub_class->level;
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_cntrl_callback
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This is the callback used when hub information is sent or received
 *
 *END*--------------------------------------------------------------------*/

void usb_class_hub_cntrl_callback
(
/* [IN] Unused */
void* tr_ptr,
/* [IN] Pointer to the class interface instance */
void* param,
/* [IN] Data buffer */
uint8_t * buffer,
/* [IN] Length of buffer */
uint32_t len,
/* [IN] Error code (if any) */
usb_status status
)
{
    usb_hub_class_struct_t* hub_class = (usb_hub_class_struct_t*)param;

    //USB_PRINTF("ctrl c\n");
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_cntrl_callback");
#endif

    if (usb_host_release_tr(hub_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }

    hub_class->in_setup = FALSE;

    if (hub_class->ctrl_callback)
    {
        hub_class->ctrl_callback(NULL, hub_class->ctrl_param, buffer, len, status);
    }
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_cntrl_callback, SUCCESSFUL");
#endif 
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_cntrl_common
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is used to send a control request
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_cntrl_common
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] Bitmask of the request type */
uint8_t bmrequesttype,
/* [IN] Request code */
uint8_t brequest,
/* [IN] Value to copy into wvalue field of the REQUEST */
uint16_t wvalue,
/* [IN] Length of the data associated with REQUEST */
uint16_t windex,
/* [IN] Index field of CTRL packet */
uint16_t wlength,
/* [IN] the buffer to be transfered */
uint8_t * data
)
{
    usb_hub_class_struct_t* hub_class = NULL;
    //usb_setup_t                        req;
    usb_status status = USB_OK;
    tr_struct_t* tr_ptr;

    if ((com_ptr == NULL) || (com_ptr->class_ptr == NULL))
    {
        return USBERR_ERROR;
    }

    hub_class = (usb_hub_class_struct_t*)com_ptr->class_ptr;
    if (hub_class->in_setup)
    {
        return USBERR_TRANSFER_IN_PROGRESS;
    }

    if (hub_class->dev_handle == NULL)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_hub_cntrl_common, invalid device handle");
#endif
        return USBERR_DEVICE_NOT_FOUND;
    }

    hub_class->ctrl_callback = com_ptr->callback_fn;
    hub_class->ctrl_param = com_ptr->callback_param;

    if (usb_host_get_tr(hub_class->host_handle, usb_class_hub_cntrl_callback, hub_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr hub\n");
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
    tr_ptr->setup_packet.brequest = brequest;
    *(uint16_t*)&tr_ptr->setup_packet.wvalue[0] = USB_HOST_TO_LE_SHORT(wvalue);
    *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(windex);
    *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT(wlength);

    status = usb_host_send_setup(hub_class->host_handle, hub_class->control_pipe, tr_ptr);
    if (status != USB_OK)
    {
        usb_host_release_tr(hub_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
    hub_class->in_setup = TRUE;
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_get_descriptor
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is called by the application to read the descriptor
 *     of hub device
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_get_descriptor
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] Buffer to receive descriptor */
uint8_t* buf,
/* [IN] buffer length (how many bytes to read) */
uint8_t len
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("_usb_class_hub_get_descriptor");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_DEVICE | REQ_TYPE_IN | REQ_TYPE_CLASS,
    REQ_GET_DESCRIPTOR,
    0,
    0,
    len,
    buf);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_set_port_feature
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Sets feature of specified hub port
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_set_port_feature
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] port number */
uint8_t port_nr,
/* [IN] feature ID */
uint8_t feature
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_set_port_feature, SUCCESSFUL");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
    REQ_SET_FEATURE,
    feature,
    port_nr,
    0,
    NULL);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_clear_feature
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Clears hub feature
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_clear_feature
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] feature ID */
uint8_t feature
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_clear_port_feature, SUCCESSFUL");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS,
    REQ_CLEAR_FEATURE,
    feature,
    0,
    0,
    NULL);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_clear_port_feature
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Clears feature of selected hub port
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_clear_port_feature
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] port number */
uint8_t port_nr,
/* [IN] feature ID */
uint8_t feature
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_clear_port_feature, SUCCESSFUL");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
    REQ_CLEAR_FEATURE,
    feature,
    port_nr,
    0,
    NULL);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_get_status
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Gets hub status
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_get_status
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] Buffer to receive descriptor */
uint8_t* buf,
/* [IN] buffer length (how many bytes to read) */
uint8_t len
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_get_port_status, SUCCESSFUL");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_IN | REQ_TYPE_CLASS,
    REQ_GET_STATUS,
    0,
    0,
    len,
    buf);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_get_port_status
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Gets the status of specified port
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_hub_get_port_status
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] port number */
uint8_t port_nr,
/* [IN] Buffer to receive descriptor */
uint8_t* buf,
/* [IN] buffer length (how many bytes to read) */
uint8_t len
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_get_port_status, SUCCESSFUL");
#endif

    return usb_class_hub_cntrl_common(
    com_ptr,
    REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
    REQ_GET_STATUS,
    0,
    port_nr,
    len,
    buf);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_recv_bitmap
 * Returned Value : None
 * Comments       :
 *     Starts interrupt endpoint to poll for interrupt on specified hub
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hub_recv_bitmap
(
/* [IN] Class Interface structure pointer */
hub_command_t* com_ptr,
/* [IN] The buffer address */
uint8_t * buffer,
/* size of buffer to be used */
uint8_t length
)
{
    usb_hub_class_struct_t* hub_class;
    tr_struct_t* tr_ptr;
    usb_status status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_hub_recv_bitmap");
#endif
    //USB_PRINTF("r \n");

    if ((com_ptr == NULL) || (com_ptr->class_ptr == NULL))
    {
        return USBERR_ERROR;
    }

    hub_class = (usb_hub_class_struct_t*)com_ptr->class_ptr;

    if ((hub_class == NULL) || (buffer == NULL))
    {
        USB_PRINTF("input parameter error\n");
        return USBERR_ERROR;
    }

    hub_class->interrupt_callback = com_ptr->callback_fn;
    hub_class->interrupt_param = com_ptr->callback_param;

    if (hub_class->dev_handle == NULL)
    {
        return USBERR_ERROR;
    }

    if (hub_class->in_interrupt)
    {
        return USBERR_ERROR;
    }

    if (usb_host_get_tr(hub_class->host_handle, usb_class_hub_int_callback, hub_class, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }

    tr_ptr->rx_buffer = buffer;
    tr_ptr->rx_length = length;
    status = usb_host_recv_data(hub_class->host_handle, hub_class->interrupt_pipe, tr_ptr);
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in usb_class_hub_recv_bitmap: %x", (unsigned int)status);
        usb_host_release_tr(hub_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
    hub_class->in_interrupt = TRUE;
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hub_cntrl_callback
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This is the callback used when hub information is sent or received
 *
 *END*--------------------------------------------------------------------*/
void usb_class_hub_int_callback
(
/* [IN] Unused */
void* tr_ptr,
/* [IN] Pointer to the class interface instance */
void* param,
/* [IN] Data buffer */
uint8_t * buffer,
/* [IN] Length of buffer */
uint32_t len,
/* [IN] Error code (if any) */
usb_status status
)
{
    usb_hub_class_struct_t* hub_class = (usb_hub_class_struct_t*)param;

    //USB_PRINTF("hub int\n");
    if (usb_host_release_tr(hub_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }
    hub_class->in_interrupt = FALSE;

    if (hub_class->interrupt_callback)
    {
        hub_class->interrupt_callback(NULL, hub_class->interrupt_param, buffer, len, status);
    }
}
#endif
