/**HEADER********************************************************************
 *
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: usb_host_cdc_intf.c$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 *   This file Contains the implementation of class driver for CDC devices.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_cdc.h"
#include "usb_host_dev_mng.h"
#include "usb_host_common.h"

static cdc_class_call_struct_t * ctrl_anchor = NULL;
static cdc_class_call_struct_t * data_anchor = NULL;

//static void *                         acm_anchor_abstract; /* used for lower abstract layer */
//static void *                         data_anchor_abstract; /* used for lower abstract layer */

usb_status usb_class_cdc_acm_deinit(usb_class_intf_handle handle);
usb_status usb_class_cdc_data_deinit(usb_class_intf_handle handle);
static void usb_class_cdc_bind_data_interface(cdc_class_call_struct_t* control_class_ptr, void* data_descriptor_ptr);
static void usb_class_cdc_bind_ctrl_interface(cdc_class_call_struct_t* data_class_ptr, void* control_descriptor_ptr);
static void usb_class_cdc_register_interface(cdc_class_call_struct_t* class_ptr, cdc_class_call_struct_t** anchor_ptr);
static void usb_class_cdc_unregister_interface(cdc_class_call_struct_t* class_ptr, cdc_class_call_struct_t** anchor_ptr);
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_intf_validate
 * Returned Value : None
 * Comments       :
 *     This function is called to determine whether class interface is validate.
 *
 *END*--------------------------------------------------------------------*/
uint32_t usb_class_cdc_intf_validate(void * param)
{
    return (NULL == param) ? 0 : 1;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_acm_init
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver
 *     for AbstractClassControl. It is called in response to a select
 *     interface call by application.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_acm_init
(
    /* [IN]  the device handle related to the class driver */
    usb_device_instance_handle dev_handle,
    /* [IN]  the interface handle related to the class driver */
    usb_interface_descriptor_handle intf_handle,
    /* [OUT] printer call struct pointer */
    usb_class_handle* class_handle_ptr
    )
{
    /* Body */
    usb_acm_class_intf_struct_t * acm_class_intf = NULL;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    interface_descriptor_t* intf = NULL;
    endpoint_descriptor_t* ep_desc = NULL;
    uint8_t ep_num;
    usb_status status = USB_OK;
    pipe_init_struct_t pipe_init;

    acm_class_intf = (usb_acm_class_intf_struct_t *) OS_Mem_alloc_uncached_zero(sizeof(usb_acm_class_intf_struct_t));
    if (acm_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_acm_init fail on memory allocation\n");
        return USBERR_ERROR;
    }

    acm_class_intf->dev_handle = dev_handle;
    acm_class_intf->host_handle = (usb_host_handle) usb_host_dev_mng_get_host(acm_class_intf->dev_handle);

    pDeviceIntf = (usb_device_interface_struct_t*) intf_handle;
    intf = pDeviceIntf->lpinterfaceDesc;
    acm_class_intf->intf_handle = intf;
    acm_class_intf->intf_num = intf->bInterfaceNumber;
    acm_class_intf->mutex = OS_Mutex_create();
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_acm_init");
#endif

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
            pipe_init.dev_instance = acm_class_intf->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(acm_class_intf->host_handle, &acm_class_intf->interrupt_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_cdc_acm_init fail to open in pipe\n");
                *class_handle_ptr = (usb_class_handle) acm_class_intf;
                return USBERR_ERROR;
            }
        }
    }
    /* Make sure the device is still attached */
#ifdef _HOST_DEBUG_
    if (status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_acm_init, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_acm_init, FAILED");
    }
#endif
    *class_handle_ptr = (usb_class_handle) acm_class_intf;
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_acm_use_lwevent
 * Returned Value : None
 * Comments       :
 *     This function is injector of events that are used in the class but
 *     the destruction are allowed only in task context.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_acm_use_lwevent
(
    /* [IN] acm call struct pointer */
    cdc_class_call_struct_t * ccs_ptr,

    /* [IN] acm event */
    os_event_handle acm_event
    )
{
    usb_acm_class_intf_struct_t * if_ptr = (usb_acm_class_intf_struct_t *) ccs_ptr->class_intf_handle;

    if (acm_event == NULL)
    {
        return USBERR_INIT_DATA;
    }

    if_ptr->acm_event = acm_event;

    /* prepare events to be auto or manual */
    OS_Event_clear(if_ptr->acm_event, 0xFFFFFF);
    OS_Event_set(if_ptr->acm_event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_INT_PIPE_FREE);
    return USB_OK;
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_acm_pre_deinit
 * Returned Value : None
 * Comments       :
 *     This function is destructor for device instance called after detach
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_acm_pre_deinit
(
    /* [IN]  the class driver handle */
    usb_class_handle handle
    )
{
    usb_acm_class_intf_struct_t * acm_class_intf = (usb_acm_class_intf_struct_t *) handle;
    usb_status status = USB_OK;
    if (acm_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_acm_pre_deinit fail\n");
        return USBERR_ERROR;
    }

    if (acm_class_intf->interrupt_pipe != NULL)
    {
        status = usb_host_cancel(acm_class_intf->host_handle, acm_class_intf->interrupt_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }

    return USB_OK;
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_acm_deinit
 * Returned Value : None
 * Comments       :
 *     This function is destructor for device instance called after detach
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_acm_deinit
(
    /* [IN] acm call struct pointer */
    usb_class_handle handle
    )
{
    usb_acm_class_intf_struct_t * acm_class_intf = (usb_acm_class_intf_struct_t *) handle;
    usb_status status = USB_OK;

    if (acm_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_acm_deinit fail\n");
        return USBERR_ERROR;
    }

    if (acm_class_intf->interrupt_pipe != NULL)
    {
        status = usb_host_close_pipe(acm_class_intf->host_handle, acm_class_intf->interrupt_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_cdc_acm_deinit to close pipe\n");
        }
    }

    if (acm_class_intf->acm_event != NULL)
    {
        OS_Event_set(acm_class_intf->acm_event, USB_ACM_DETACH); /* mark we are not using input pipe */
    }

    OS_Mutex_destroy(acm_class_intf->mutex);
    OS_Mem_free(handle);
    return USB_OK;
    /* destroying lwevent is up to application */
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_data_init
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver
 *     for AbstractClassControl. It is called in response to a select
 *     interface call by application.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_data_init
(
    /* [IN]  the device handle related to the class driver */
    usb_device_instance_handle dev_handle,
    /* [IN]  the interface handle related to the class driver */
    usb_interface_descriptor_handle intf_handle,
    /* [OUT] printer call struct pointer */
    usb_class_handle* class_handle_ptr
    )
{
    /* Body */
    usb_data_class_intf_struct_t * data_class_intf = NULL;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    interface_descriptor_t* intf = NULL;
    endpoint_descriptor_t* ep_desc = NULL;
    uint8_t ep_num;
    usb_status status = USB_OK;
    pipe_init_struct_t pipe_init;

    data_class_intf = (usb_data_class_intf_struct_t *) OS_Mem_alloc_zero(sizeof(usb_data_class_intf_struct_t));
    if (data_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_data_init fail on memory allocation\n");
        return USBERR_ERROR;
    }

    data_class_intf->dev_handle = dev_handle;
    data_class_intf->host_handle = (usb_host_handle) usb_host_dev_mng_get_host(data_class_intf->dev_handle);

    pDeviceIntf = (usb_device_interface_struct_t*) intf_handle;
    intf = pDeviceIntf->lpinterfaceDesc;
    data_class_intf->intf_handle = intf;
    data_class_intf->intf_num = intf->bInterfaceNumber;
    data_class_intf->mutex = OS_Mutex_create();
    data_class_intf->is_rx_xferring = FALSE;
    data_class_intf->is_tx_xferring = FALSE;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_data_init");
#endif

    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        if ((ep_desc->bEndpointAddress & IN_ENDPOINT) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction = USB_RECV;
            pipe_init.pipetype = USB_BULK_PIPE;
            pipe_init.max_packet_size = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval = ep_desc->iInterval;
            pipe_init.flags = 0;
            pipe_init.dev_instance = data_class_intf->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(data_class_intf->host_handle, (usb_pipe_handle *) &data_class_intf->in_pipe, &pipe_init);
        }
        else if ((!(ep_desc->bEndpointAddress & IN_ENDPOINT)) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction = USB_SEND;
            pipe_init.pipetype = USB_BULK_PIPE;
            pipe_init.max_packet_size = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval = ep_desc->iInterval;
            pipe_init.flags = 0;
            pipe_init.dev_instance = data_class_intf->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(data_class_intf->host_handle, (usb_pipe_handle *) &data_class_intf->out_pipe, &pipe_init);
        }
        if (status != USB_OK)
        {
            USB_PRINTF("usb_class_cdc_data_init fail to open in pipe\n");
            *class_handle_ptr = (usb_class_handle) data_class_intf;
            return USBERR_ERROR;
        }
    }

    /* Make sure the device is still attached */

    if ((data_class_intf->in_pipe == NULL) && (data_class_intf->out_pipe == NULL))
    {
        status = USBERR_OPEN_PIPE_FAILED;
    }

    if (data_class_intf->out_pipe)
    {
        /* Don't use host - predefined constant for nak_count...
         ** NOTE!!!
         ** This hack is not very clean. We need to maximize number of retries to minimize the time of
         ** transaction (minimize task's time while waiting for 1 transaction to be done (with or without data))
         ** The time depends on user expectation of the read() latency, on the delay between 2 NAKs and on number
         ** of NAKs to be performed.
         ** The workaround is to limit amount of retries for the pipe maximally to 3.
         ** Number 3 is hard-coded here for now.
         */
        //    if (((pipe_struct_t*)(data_class_intf->in_pipe))->nak_count > 3)
        //        ((pipe_struct_t*)data_class_intf->in_pipe)->nak_count = 3; /* don't use host - predefined constant */
    }
    if (data_class_intf->in_pipe)
    {
        /* The same as for OUT pipe applies here */
        //    if (((pipe_struct_t*)data_class_intf->out_pipe)->nak_count > 3)
        //        ((pipe_struct_t*)data_class_intf->out_pipe)->nak_count = 3; /* don't use host - predefined constant */
        /* initialize buffer */
        /* size of buffer equals to the size of endpoint data size */
        data_class_intf->RX_BUFFER_SIZE = ((pipe_struct_t*) data_class_intf->in_pipe)->max_packet_size;
        if (NULL == (data_class_intf->rx_buffer = (uint8_t *) OS_Mem_alloc_zero(data_class_intf->RX_BUFFER_SIZE)))
        {
            status = USBERR_ALLOC;
        }
        else
        {
            /* initialize members */
            data_class_intf->RX_BUFFER_APP = data_class_intf->RX_BUFFER_DRV = data_class_intf->rx_buffer;
        }
    }

#ifdef _HOST_DEBUG_
    if (status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_data_init, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_data_init, FAILED");
    }
#endif
    *class_handle_ptr = (usb_class_handle) data_class_intf;
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_data_use_lwevent
 * Returned Value : None
 * Comments       :
 *     This function is injector of events that are used in the class but
 *     the destruction are allowed only in task context.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_data_use_lwevent
(
    /* [IN] acm call struct pointer */
    cdc_class_call_struct_t * ccs_ptr,

    /* [IN] acm event */
    os_event_handle data_event
    )
{
    usb_data_class_intf_struct_t * if_ptr = (usb_data_class_intf_struct_t *) ccs_ptr->class_intf_handle;

    if (data_event == NULL)
    {
        return USBERR_INIT_DATA;
    }

    if_ptr->data_event = data_event;

    /* prepare events to be auto or manual */
    /* pre-set events */
    OS_Event_clear(if_ptr->data_event, 0xFFFFFF);
    OS_Event_set(if_ptr->data_event, USB_DATA_READ_PIPE_FREE | USB_DATA_SEND_PIPE_FREE);

    return USB_OK;
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_data_pre_deinit
 * Returned Value : None
 * Comments       :
 *     This function is destructor for device instance called after detach
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_data_pre_deinit
(
    /* [IN]  the class driver handle */
    usb_class_handle handle
    )
{
    usb_data_class_intf_struct_t * data_class_intf = (usb_data_class_intf_struct_t *) handle;
    usb_status status = USB_OK;
    if (data_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_data_pre_deinit fail\n");
        return USBERR_ERROR;
    }

    if (data_class_intf->in_pipe != NULL)
    {
        status = usb_host_cancel(data_class_intf->host_handle, data_class_intf->in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }
    if (data_class_intf->out_pipe != NULL)
    {
        status = usb_host_cancel(data_class_intf->host_handle, data_class_intf->out_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_data_deinit
 * Returned Value : None
 * Comments       :
 *     This function is destructor for device instance called after detach
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_data_deinit
(
    /* [IN] data call struct pointer */
    usb_class_handle handle
    )
{
    usb_data_class_intf_struct_t * data_class_intf = (usb_data_class_intf_struct_t *) handle;
    usb_status status = USB_OK;
    if (data_class_intf == NULL)
    {
        USB_PRINTF("usb_class_cdc_data_deinit fail\n");
        return USBERR_ERROR;
    }

    if (data_class_intf->in_pipe != NULL)
    {
        status = usb_host_close_pipe(data_class_intf->host_handle, data_class_intf->in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_cdc_data_deinit to close pipe\n");
        }
    }
    if (data_class_intf->out_pipe != NULL)
    {
        status = usb_host_close_pipe(data_class_intf->host_handle, data_class_intf->out_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_cdc_data_deinit to close pipe\n");
        }
    }

    if ((data_class_intf->in_pipe != NULL) && (data_class_intf->data_event != NULL))
    {
        OS_Event_set(data_class_intf->data_event, USB_DATA_DETACH); /* mark we are not using input pipe */
    }

    if (data_class_intf->rx_buffer != NULL)
    {
        OS_Mem_free(data_class_intf->rx_buffer);
    }
    OS_Mutex_destroy(data_class_intf->mutex);
    OS_Mem_free(handle);
    return status;

    /* destroying lwevent is up to application */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_bind_data_interface
 * Returned Value :
 * Comments       :
 *     This function is used to bind data interface with control interface.
 *     It must be run with interrupts disabled to have interfaces validated.
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_bind_data_interface
(
    /* [IN] pointer to control interface call struct */
    cdc_class_call_struct_t * control_class_ptr,

    /* [IN] pointer to (data) interface descriptor to be controlled */
    void * data_descriptor_ptr
    )
{
    cdc_class_call_struct_t * ccs_data_interface = data_anchor;
    usb_data_class_intf_struct_t * data_interface;

    /* Search for all registered data interfaces */
    while (ccs_data_interface != NULL)
    {
        if (usb_class_cdc_intf_validate(ccs_data_interface))
        {
            data_interface = (usb_data_class_intf_struct_t *) ccs_data_interface->class_intf_handle;
            /* test if interface descriptor matches */
            if (data_interface->intf_handle == data_descriptor_ptr)
            {
                data_interface->BOUND_CONTROL_INTERFACE = control_class_ptr;
                break;
            }
        }
        ccs_data_interface = (cdc_class_call_struct_t *) ccs_data_interface->next;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_bind_ctrl_interface
 * Returned Value :
 * Comments       :
 *     This function is used to bind control interface with data interface.
 *     It must be run with interrupts disabled to have interfaces validated.
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_bind_ctrl_interface
(
    /* [IN] pointer to data interface */
    cdc_class_call_struct_t * data_class_ptr,

    /* [IN] pointer to (control) interface descriptor to be used to control */
    void * control_descriptor_ptr
    )
{
    cdc_class_call_struct_t * ccs_control_interface = ctrl_anchor;
    usb_acm_class_intf_struct_t * control_interface;

    /* Search for all registered control interfaces */
    while (ccs_control_interface != NULL)
    {
        if (usb_class_cdc_intf_validate(ccs_control_interface))
        {
            control_interface = (usb_acm_class_intf_struct_t *) ccs_control_interface->class_intf_handle;
            /* test if interface descriptor matches */
            if (control_interface->intf_handle == control_descriptor_ptr)
            {
                ((usb_data_class_intf_struct_t *) data_class_ptr->class_intf_handle)->BOUND_CONTROL_INTERFACE = ccs_control_interface;
                break;
            }
        }
        ccs_control_interface = (cdc_class_call_struct_t *) ccs_control_interface->next;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_register_interface
 * Returned Value :
 * Comments       :
 *     This function is used to register interface in the chain.
 *     It must be run with interrupts disabled to have interfaces validated.
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_register_interface
(
    /* [IN] pointer to registered interface */
    cdc_class_call_struct_t * class_ptr,

    cdc_class_call_struct_t * * anchor_ptr
    )
{
    cdc_class_call_struct_t * interface_parser;

    if (*anchor_ptr == NULL)
    {
        *anchor_ptr = class_ptr;
        (*anchor_ptr)->next = NULL;
    }
    else
    {
        interface_parser = *anchor_ptr;
        /* add new class to the list */
        while (interface_parser->next != NULL)
        {
            interface_parser = (cdc_class_call_struct_t *) interface_parser->next;
        }
        interface_parser->next = class_ptr;
        class_ptr->next = NULL;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_unregister_interface
 * Returned Value :
 * Comments       :
 *     This function is used to unregister interface from the chain.
 *     It must be run with interrupts disabled to have interfaces validated.
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_unregister_interface
(
    /* [IN] pointer to control interface */
    cdc_class_call_struct_t * class_ptr,

    cdc_class_call_struct_t * * anchor_ptr
    )
{
    cdc_class_call_struct_t * interface_parser;

    if (*anchor_ptr != NULL)
    {
        if (*anchor_ptr == class_ptr)
        {
            *anchor_ptr = (cdc_class_call_struct_t *) class_ptr->next;
        }
        else
        {
            /* remove control class from the list */
            for (interface_parser = *anchor_ptr; interface_parser->next != NULL; interface_parser = (cdc_class_call_struct_t *) interface_parser->next)
            {
                if (interface_parser->next == class_ptr)
                {
                    interface_parser->next = class_ptr->next;
                    break;
                }
            }
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_get_ctrl_interface
 * Returned Value : control interface instance
 * Comments       :
 *     This function is used to find registered control interface in the chain.
 *END*--------------------------------------------------------------------*/

cdc_class_call_struct_t * usb_class_cdc_get_ctrl_interface
(
    /* [IN] pointer to interface handle */
    void * intf_handle
    )
{
    cdc_class_call_struct_t * control_parser;
    usb_device_interface_struct_t* intf_ptr =
    (usb_device_interface_struct_t*) intf_handle;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_interface");
#endif

    /* find this acm interface in the list with interface descriptor */
    for (control_parser = ctrl_anchor; control_parser != NULL; control_parser = (cdc_class_call_struct_t *) control_parser->next)
    {
        if (usb_class_cdc_intf_validate(control_parser))
        {
            if (((usb_acm_class_intf_struct_t *) control_parser->class_intf_handle)->intf_handle ==
            intf_ptr->lpinterfaceDesc)
            {
                break;
            }
        }
    }

#ifdef _HOST_DEBUG_
    if (control_parser != NULL)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_interface, SUCCESSFULL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_interface, FAILED");
    }
#endif

    return control_parser;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_get_data_interface
 * Returned Value : data interface instance
 * Comments       :
 *     This function is used to find registered data interface in the chain.
 *END*--------------------------------------------------------------------*/

cdc_class_call_struct_t * usb_class_cdc_get_data_interface
(
    /* [IN] pointer to interface handle */
    void * intf_handle
    )
{
    cdc_class_call_struct_t * data_parser;
    usb_device_interface_struct_t* intf_ptr =
    (usb_device_interface_struct_t*) intf_handle;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_get_data_interface");
#endif

    /* find this acm interface in the list with interface descriptor */
    for (data_parser = data_anchor; data_parser != NULL; data_parser = (cdc_class_call_struct_t *) data_parser->next)
    {
        if (usb_class_cdc_intf_validate(data_parser))
        {
            if (((usb_data_class_intf_struct_t *) data_parser->class_intf_handle)->intf_handle == intf_ptr->lpinterfaceDesc)
            {
                break;
            }
        }
    }

#ifdef _HOST_DEBUG_
    if (data_parser != NULL)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_interface, SUCCESSFULL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_interface, FAILED");
    }
#endif

    return data_parser;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_get_acm_descriptors
 * Returned Value : USB_OK
 * Comments       :
 *     This function is hunting for descriptors in the device configuration
 *     and fills back fields if the descriptor was found.
 *     Must be run in locked state and validated USB device.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_get_acm_descriptors
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    usb_cdc_desc_acm_t * * acm_desc,
    usb_cdc_desc_cm_t * * cm_desc,
    usb_cdc_desc_header_t * * header_desc,
    usb_cdc_desc_union_t * * union_desc
    )
{
    dev_instance_t* dev_ptr;
    descriptor_union_t ptr1, ptr2;
    usb_device_interface_struct_t* intf_ptr =
    (usb_device_interface_struct_t*) intf_handle;
    usb_status status;
    //int32_t i;
    usb_cdc_func_desc_t * fd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_get_acm_descriptors");
#endif

    status = USB_OK;

    dev_ptr = (dev_instance_t*) dev_handle;
    ptr1.pntr = dev_ptr->lpConfiguration; /* offset for alignment */
    ptr2.word = ptr1.word + USB_SHORT_UNALIGNED_LE_TO_HOST(ptr1.cfig->wTotalLength);

    while (ptr1.word < ptr2.word)
    {
        if (USB_DESC_TYPE_CS_INTERFACE == (ptr1.common->bDescriptorType))
        {
            fd = (usb_cdc_func_desc_t *) ptr1.pntr;

            switch(fd->header.bDescriptorSubtype)
            {
            case USB_DESC_SUBTYPE_CS_HEADER:
                *header_desc = &fd->header;
                if ((((uint32_t)((*header_desc)->bcdCDC[1]) << 8) + (*header_desc)->bcdCDC[0]) > 0x0110)
                {
                    status = USBERR_INIT_FAILED;
                }
                break;
            case USB_DESC_SUBTYPE_CS_UNION:
                /* Check if this union descriptor describes master for this interface */
                if (fd->uni.bMasterInterface == intf_ptr->lpinterfaceDesc->bInterfaceNumber)
                {
                    /* Check if another union descriptor has not been already assigned */
                    if (*union_desc == NULL)
                    {
                        *union_desc = &fd->uni;
                    }
                    else
                    {
                        status = USBERR_INIT_FAILED;
                    }
                }
                break;
            case USB_DESC_SUBTYPE_CS_ACM:
                *acm_desc = &fd->acm;
                break;
            case USB_DESC_SUBTYPE_CS_CM:
                *cm_desc = &fd->cm;
                break;
            default:
                break;
            }
        }
        if (status != USB_OK)
        {
            break;
        }
        ptr1.word += ptr1.common->bLength;
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_acm_descriptors, SUCCESSFULL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_acm_descriptors, FAILED");
    }
#endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_set_acm_descriptors
 * Returned Value : USB_OK if validation passed
 * Comments       :
 *     This function is used to set descriptors for ACM interface
 *     Descriptors can be used afterwards by application or by driver
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_set_acm_descriptors
(
    /* [IN] acm call struct pointer */
    cdc_class_call_struct_t * ccs_ptr,

    /* [IN] acm functional descriptor pointer */
    usb_cdc_desc_acm_t * acm_desc,

    /* [IN] acm functional descriptor pointer */
    usb_cdc_desc_cm_t * cm_desc,

    /* [IN] header functional descriptor pointer */
    usb_cdc_desc_header_t * header_desc,

    /* [IN] union functional descriptor pointer */
    usb_cdc_desc_union_t * union_desc
    )
{
    /* Body */
    usb_acm_class_intf_struct_t * if_acm_ptr;
    usb_status status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_set_acm_descriptors");
#endif

    /* Make sure the device is still attached */

    /* Validity checking */
    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        if_acm_ptr = (usb_acm_class_intf_struct_t *) ccs_ptr->class_intf_handle;
        USB_CDC_ACM_lock();
        if_acm_ptr->acm_desc = acm_desc;
        if_acm_ptr->header_desc = header_desc;
        if_acm_ptr->union_desc = union_desc;
        USB_CDC_ACM_unlock();
    } /* Endif */

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_set_acm_descriptors, SUCCESSFUL");
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_get_ctrl_descriptor
 * Returned Value : USB_OK if found
 * Comments       :
 *     This function is hunting for descriptor of control interface, which
 *     controls data interface identified by descriptor (intf_handle)
 *     The found control interface descriptor is written to if_desc_ptr
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_get_ctrl_descriptor
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    interface_descriptor_t* * if_desc_ptr
)
{
    dev_instance_t* dev_ptr;
    descriptor_union_t ptr1, ptr2;
    usb_cdc_func_desc_t * fd_desc;
    interface_descriptor_t* if_desc;
    interface_descriptor_t* intf_ptr =
    ((usb_device_interface_struct_t*) intf_handle)->lpinterfaceDesc;
    usb_status status;
    uint8_t* bSlaveInterface;
    int32_t i, j;
    int32_t ctrl_intf = -1; /* assume not found */

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_descriptor");
#endif

    status = USBERR_INIT_FAILED;
    *if_desc_ptr = NULL; /* presume not found */

    dev_ptr = (dev_instance_t*) dev_handle;
    ptr1.pntr = dev_ptr->lpConfiguration; /* offset for alignment */
    ptr2.word = ptr1.word + USB_SHORT_UNALIGNED_LE_TO_HOST(ptr1.cfig->wTotalLength);

    while (ptr1.word < ptr2.word)
    {
        if (USB_DESC_TYPE_CS_INTERFACE == (ptr1.common->bDescriptorType))
        {
            fd_desc = (usb_cdc_func_desc_t *) ptr1.pntr;
            if (USB_DESC_SUBTYPE_CS_UNION == fd_desc->header.bDescriptorSubtype)
            {
                /* Here we get only when union descriptor found */
                /* Find if there is this data interface in the found union descriptor */
                bSlaveInterface = &(fd_desc->uni.bMasterInterface);
                bSlaveInterface++;
                for (j = 0; j < fd_desc->uni.bFunctionLength - 3; j++)
                {
                    if (intf_ptr->bInterfaceNumber == bSlaveInterface[j])
                    {
                        /* Union descriptor containing our data interface found */
                        ctrl_intf = fd_desc->uni.bMasterInterface; /* copy control interface number */
                        break;
                    }
                }
                if (ctrl_intf != -1) /* when control interface was already found */
                {
                    break; /* finish any next search for union descriptors */
                }
            }
        }
        ptr1.word += ptr1.common->bLength;
    }

    if (ctrl_intf != -1)
    {
        for (i = 0; i < dev_ptr->configuration.interface_count; i++)
        {
            if_desc = dev_ptr->configuration.interface[i].lpinterfaceDesc;
            if (if_desc->bInterfaceNumber == ctrl_intf)
            {
                /* Interface descriptor of control interface found */
                *if_desc_ptr = if_desc;
                status = USB_OK;
                break;
            }
        }

    }
#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_descriptor, SUCCESSFULL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_ctrl_descriptor, FAILED");
    }
#endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_bind_data_interfaces
 * Returned Value : USB_OK if found
 * Comments       :
 *     All data interfaces belonging to ACM control instance
 *     (specified by ccs_ptr) will be bound to this interface.
 *     Union functional descriptor describes which data interfaces
 *     should be bound.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_bind_data_interfaces
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    cdc_class_call_struct_t * ccs_ptr
)
{
    dev_instance_t* dev_ptr;
    usb_cdc_desc_union_t * union_desc;
    interface_descriptor_t* if_desc;
    int32_t i, j;
    uint8_t* bSlaveInterface;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_bind_data_interfaces");
#endif
    dev_ptr = (dev_instance_t*) dev_handle;

    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        union_desc = ((usb_acm_class_intf_struct_t *) ccs_ptr->class_intf_handle)->union_desc;

        /* Now link all already initialized interfaces
         that should be controlled by this ACM */
        for (i = 0; i < dev_ptr->configuration.interface_count; i++)
        {
            if_desc = dev_ptr->configuration.interface[i].lpinterfaceDesc;

            bSlaveInterface = &(union_desc->bMasterInterface);
            bSlaveInterface++;
            for (j = 0; j < union_desc->bFunctionLength - 3; j++)
            {
                if (if_desc->bInterfaceNumber == bSlaveInterface[j])
                {
                    /* the found (data) interface if_desc should be controlled by this ACM */
                    usb_class_cdc_bind_data_interface((cdc_class_call_struct_t *) ccs_ptr, if_desc);
                    break;
                }
            }
        }
        /* Store control interface for data interfaces included later */
        usb_class_cdc_register_interface(ccs_ptr, &ctrl_anchor);
    }

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_bind_data_interfaces, SUCCESSFULL");
#endif

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_unbind_data_interfaces
 * Returned Value : USB_OK if found
 * Comments       :
 *     All data interfaces bound to ACM control instance
 *     will be unbound from this interface.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_unbind_data_interfaces
(
    cdc_class_call_struct_t * ccs_ptr
)
{
    cdc_class_call_struct_t * data_parser;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_unbind_data_interfaces");
#endif

    for (data_parser = data_anchor; data_parser != NULL; data_parser = (cdc_class_call_struct_t *) data_parser->next)
    {
        if (usb_class_cdc_intf_validate(data_parser))
        {
            if (((usb_data_class_intf_struct_t *) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE == ccs_ptr)
            {
                ((usb_data_class_intf_struct_t *) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE = NULL;
            }
        }
    }
    /* Delete control interface for data interfaces included later */
    usb_class_cdc_unregister_interface(ccs_ptr, &ctrl_anchor);

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_unbind_data_interfaces, SUCCESSFULL");
#endif

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_bind_acm_interface
 * Returned Value : USB_OK
 * Comments       :
 *     Data interface (specified by ccs_ptr) will be bound to appropriate
 *     control interface.
 *     Must be run in locked state and validated USB device.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_bind_acm_interface
(
    cdc_class_call_struct_t * ccs_ptr,

    interface_descriptor_t* if_desc
)
{
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_bind_acm_interface");
#endif

    usb_class_cdc_bind_ctrl_interface(ccs_ptr, if_desc);
    usb_class_cdc_register_interface(ccs_ptr, &data_anchor);

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_bind_acm_interface, SUCCESSFULL");
#endif

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_unbind_acm_interface
 * Returned Value : USB_OK
 * Comments       :
 *     Data interface (specified by ccs_ptr) will be unbound from
 *     appropriate control interface.
 *     Must be run in locked state and validated USB device.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_unbind_acm_interface
(
    cdc_class_call_struct_t * ccs_ptr
)
{
    usb_data_class_intf_struct_t * if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_unbind_acm_interface");
#endif

    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        if_ptr = (usb_data_class_intf_struct_t *) ccs_ptr->class_intf_handle;
        usb_class_cdc_unregister_interface(ccs_ptr, &data_anchor);
        if_ptr->BOUND_CONTROL_INTERFACE = NULL;
    }

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_unbind_acm_interface, SUCCESSFULL");
#endif

    return USB_OK;
}

/* EOF */
