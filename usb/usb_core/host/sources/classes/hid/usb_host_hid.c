/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: usb_host_hid.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file Contains the implementation of class driver for HID devices.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_HID
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "usb_host_common.h"
#include "usb_host_ch9.h"
#include "usb_host_dev_mng.h"

extern volatile uint32_t sleep_test;

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_init
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_init
(
/* [IN]  the device handle related to the class driver */
usb_device_instance_handle dev_handle,
/* [IN]  the interface handle related to the class driver */
usb_interface_descriptor_handle intf_handle,
/* [OUT] printer call struct pointer */
usb_class_handle* class_handle_ptr
)
{ /* Body */
    usb_hid_class_struct_t* hid_class = NULL;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    //interface_descriptor_t*      intf = NULL;
    endpoint_descriptor_t* ep_desc = NULL;
    uint8_t ep_num;
    usb_status status = USB_OK;
    pipe_init_struct_t pipe_init;

    hid_class = (usb_hid_class_struct_t*)OS_Mem_alloc_zero(sizeof(usb_hid_class_struct_t));
    if (hid_class == NULL)
    {
#if _DEBUG
        USB_PRINTF("usb_class_hid_init fail on memory allocation\n");
#endif
        return USBERR_ERROR;
    }

    hid_class->dev_handle = dev_handle;
    hid_class->intf_handle = intf_handle;
    hid_class->host_handle = usb_host_dev_mng_get_host(hid_class->dev_handle);

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
            pipe_init.dev_instance = hid_class->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(hid_class->host_handle, &hid_class->in_pipe, &pipe_init);
            if (status != USB_OK)
            {
#if _DEBUG
                USB_PRINTF("usb_class_hid_init fail to open in pipe\n");
#endif
                *class_handle_ptr = (usb_class_handle)hid_class;
                return USBERR_ERROR;
            }
        }
    }

    hid_class->in_setup = FALSE;
    hid_class->ctrl_callback = NULL;
    hid_class->ctrl_param = NULL;
    hid_class->recv_callback = NULL;
    hid_class->recv_param = NULL;

    *class_handle_ptr = (usb_class_handle)hid_class;

    //USB_PRINTF("HID class driver initialized\n");

    return USB_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_deinit
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_hid_class_struct_t* hid_class = (usb_hid_class_struct_t*)handle;
    usb_status status = USB_OK;
    if (hid_class == NULL)
    {
#if _DEBUG
        USB_PRINTF("usb_class_hid_deinit fail\n");
#endif
        return USBERR_ERROR;
    }

    if (hid_class->in_pipe != NULL)
    {
        status = usb_host_close_pipe(hid_class->host_handle, hid_class->in_pipe);
        if (status != USB_OK)
        {
#if _DEBUG
            USB_PRINTF("error in usb_class_hid_deinit to close pipe\n");
#endif
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
usb_status usb_class_hid_pre_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_hid_class_struct_t* hid_class = (usb_hid_class_struct_t*)handle;
    usb_status status = USB_OK;

    if (hid_class == NULL)
    {
#if _DEBUG    
        USB_PRINTF("_usb_host_cancel_call_interface fail\n");
#endif
        return USBERR_ERROR;
    }

    if (hid_class->in_pipe != NULL)
    {
        status = usb_host_cancel(hid_class->host_handle, hid_class->in_pipe, NULL);
        if (status != USB_OK)
        {
#if _DEBUG
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
#endif
        }
    }

    //USB_PRINTF("HID class driver pre_deinit\n");
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_cntrl_callback
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This is the callback used when HID information is sent or received
 *
 *END*--------------------------------------------------------------------*/
static void usb_class_hid_cntrl_callback
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
{ /* Body */
    usb_hid_class_struct_t* hid_class = (usb_hid_class_struct_t*)param;

    if (usb_host_release_tr(hid_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("_usb_host_release_tr failed\n");
#endif
    }

    hid_class->in_setup = FALSE;
    if (hid_class->ctrl_callback)
    {
        hid_class->ctrl_callback(NULL, hid_class->ctrl_param, buffer, len, status);
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_recv_callback
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *    
 *
 *END*--------------------------------------------------------------------*/
static void usb_class_hid_recv_callback
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
{ /* Body */
    usb_hid_class_struct_t* hid_class = (usb_hid_class_struct_t*)param;

    if (usb_host_release_tr(hid_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("_usb_host_release_tr failed\n");
#endif
    }

    if (hid_class->recv_callback)
    {
        hid_class->recv_callback(NULL, hid_class->recv_param, buffer, len, status);
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_cntrl_common
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is used to send a control request
 *
 *END*--------------------------------------------------------------------*/
static usb_status usb_class_hid_cntrl_common
(
/* [IN] The communication device common command structure */
hid_command_t* com_ptr,
/* [IN] Bitmask of the request type */
uint8_t bmrequesttype,
/* [IN] Request code */
uint8_t brequest,
/* [IN] Value to copy into wvalue field of the REQUEST */
uint16_t wvalue,
/* [IN] Length of the data associated with REQUEST */
uint16_t wlength,
/* [IN] Pointer to data buffer used to send/recv */
uint8_t* data
)
{ /* Body */
    usb_hid_class_struct_t* hid_class = NULL;
    //usb_setup_t                        req;
    usb_status status = USB_OK;
    usb_pipe_handle pipe_handle;
    tr_struct_t* tr_ptr;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    interface_descriptor_t* intf = NULL;
    uint8_t interfaceIndex;

    if ((com_ptr == NULL) || (com_ptr->class_ptr == NULL))
    {
        return USBERR_ERROR;
    }

    hid_class = (usb_hid_class_struct_t*)com_ptr->class_ptr;
    if (hid_class->in_setup)
    {
        return USBERR_TRANSFER_IN_PROGRESS;
    }

    if (hid_class->dev_handle == NULL)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, invalid device handle");
#endif
        return USBERR_DEVICE_NOT_FOUND;
    }

    pDeviceIntf = (usb_device_interface_struct_t*)hid_class->intf_handle;
    intf = pDeviceIntf->lpinterfaceDesc;
    interfaceIndex = intf->bInterfaceNumber;

    hid_class->ctrl_callback = com_ptr->callback_fn;
    hid_class->ctrl_param = com_ptr->callback_param;

    pipe_handle = usb_host_dev_mng_get_control_pipe(hid_class->dev_handle);

    if (usb_host_get_tr(hid_class->host_handle, usb_class_hid_cntrl_callback, hid_class, &tr_ptr) != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("error to get tr hid\n");
#endif
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
    *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(interfaceIndex);
    *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT(wlength);

    hid_class->in_setup = TRUE;
    status = usb_host_send_setup(hid_class->host_handle, pipe_handle, tr_ptr);
    if (status != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("\nError in usb_class_hid_cntrl_common: %x", status);
#endif
        hid_class->in_setup = FALSE;
        usb_host_release_tr(hid_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_recv_data
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is used to recv interrupt data
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_recv_data
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,
/* [IN] The buffer address */
uint8_t * buffer,
/* [IN] The buffer address */
uint16_t length
)
{
    usb_hid_class_struct_t* hid_class;
    tr_struct_t* tr_ptr;
    usb_status status;

    if ((com_ptr == NULL) || (com_ptr->class_ptr == NULL))
    {
        return USBERR_ERROR;
    }

    hid_class = (usb_hid_class_struct_t*)com_ptr->class_ptr;

    if ((hid_class == NULL) || (buffer == NULL))
    {
#if _DEBUG
        USB_PRINTF("input parameter error\n");
#endif
        return USBERR_ERROR;
    }

    hid_class->recv_callback = com_ptr->callback_fn;
    hid_class->recv_param = com_ptr->callback_param;

    if (hid_class->dev_handle == NULL)
    {
        return USBERR_ERROR;
    }
#if 0
    if (sleep_test)
    {
        USB_PRINTF("begin to sleep 3000ms \n");
        OS_Time_delay(3000);
    }
#endif
    if (usb_host_get_tr(hid_class->host_handle, usb_class_hid_recv_callback, hid_class, &tr_ptr) != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("error to get tr\n");
#endif
        return USBERR_ERROR;
    }

    tr_ptr->rx_buffer = buffer;
    tr_ptr->rx_length = length;
    status = usb_host_recv_data(hid_class->host_handle, hid_class->in_pipe, tr_ptr);
    if (status != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("\nError in usb_class_hid_recv_data: %x", status);
#endif
        usb_host_release_tr(hid_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_get_descriptor
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is called by the application to read the descriptor
 *     of hub device
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_get_descriptor
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,
/* [IN] Class descriptor type */
uint8_t type,
/* [IN] Buffer to receive descriptor */
uint8_t* buf,
/* [IN] buffer length (how many bytes to read) */
uint16_t len
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("_usb_class_hub_get_descriptor");
#endif

    return usb_class_hid_cntrl_common(
    com_ptr,
    REQ_TYPE_INTERFACE | REQ_TYPE_IN,
    REQ_GET_DESCRIPTOR,
    (uint16_t)((uint16_t)type << 8),
    len,
    buf);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_get_report
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is called by the application to get a report from the HID
 *     device
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_get_report
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,
/* [IN] Report ID (see HID specification) */
uint8_t rid,
/* [IN] Report type (see HID specification) */
uint8_t rtype,
/* [IN] Buffer to receive report data */
void* buf,
/* [IN] length of the Buffer */
uint16_t blen
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    GET_REPORT, (uint16_t)((uint16_t)((uint16_t)rtype << 8) | rid), blen, (uint8_t *)buf);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_set_report
 * Returned Value : USB_OK if command has been passed on USB
 * Comments       :
 *     This function is called by the application to send a report to the HID
 *     device
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_set_report
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,
/* [IN] Report ID (see HID specification) */
uint8_t rid,
/* [IN] Report type (see HID specification) */
uint8_t rtype,
/* [IN] Buffer to receive report data */
void* buf,
/* [IN] length of the Buffer */
uint16_t blen
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    SET_REPORT, (uint16_t)((uint16_t)((uint16_t)rtype << 8) | rid), blen, (uint8_t *)buf);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_get_idle
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is called by the application to read the idle rate of a
 *     particular HID device report
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_get_idle
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,

/* [IN] Report ID (see HID specification) */
uint8_t rid,

/* [OUT] idle rate of this report */
uint8_t * idle_rate
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    GET_IDLE, rid, 1, idle_rate);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_set_idle
 * Returned Value : USB_OK if command has been passed on USB
 * Comments       :
 *     This function is called by the application to silence a particular report
 *     on interrupt In pipe until a new event occurs or specified  time elapses
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_set_idle
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,

/* [IN] Report ID (see HID specification) */
uint8_t rid,

/*[IN] Idle rate */
uint8_t duration
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    SET_IDLE, (uint16_t)((uint16_t)((uint16_t)duration << 8) | rid), 0, NULL);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_get_protocol
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     Reads the active protocol (boot protocol or report protocol)
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_get_protocol
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,
/* [IN] Protocol (1 byte, 0 = Boot Protocol or 1 = Report Protocol */
uint8_t * protocol
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    GET_PROTOCOL, 0, 1, protocol);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_hid_set_protocol
 * Returned Value : USB_OK if command has been passed on USB
 * Comments       :
 *     Switches between the boot protocol and the report protocol (or vice versa)
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_hid_set_protocol
(
/* [IN] Class Interface structure pointer */
hid_command_t* com_ptr,

/* [IN] The protocol (0 = Boot, 1 = Report) */
uint8_t protocol
)
{
    return usb_class_hid_cntrl_common(com_ptr,
    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
    SET_PROTOCOL, protocol, 0, NULL);
}
#endif
