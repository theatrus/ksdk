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
 * $FileName: usb_host_cdc.c$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 *   This file Contains the implementation of class driver for CDC devices.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_CDC
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_cdc.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "poll.h"
#endif
#include "usb_host_dev_mng.h"
static void usb_class_cdc_cntrl_callback(void* tr_ptr, void* param, uint8_t* buffer, uint32_t len, usb_status status);
static usb_status usb_class_cdc_cntrl_common
(
/* [IN] The communication device common command structure */
cdc_command_t * com_ptr,
/* [IN] The communication device control interface */
usb_acm_class_intf_struct_t * if_ctrl_ptr,
/* [IN] Bitmask of the request type */
uint8_t bmrequesttype,
/* [IN] Request code */
uint8_t brequest,
/* [IN] Value to copy into wvalue field of the REQUEST */
uint16_t wvalue,
/* [IN] Length of the data associated with REQUEST */
uint16_t wlength,
/* [IN] Pointer to data buffer used to send/recv */
uint8_t * data
);
static void usb_class_cdc_int_acm_callback(void* tr_ptr, void* param, uint8_t* buffer, uint32_t len, uint32_t status);
static void usb_class_cdc_ctrl_acm_callback(void* tr_ptr, void* param, uint8_t* buffer, uint32_t len, uint32_t status);

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_os_event_wait
 * Returned Value : OS_EVENT_OK if get event
 * Comments       :
 *     This serves particularly for event waiting on BM when timeout equals 0, which means infinite wait.
 *
 *END*--------------------------------------------------------------------*/
uint32_t usb_class_cdc_os_event_wait(os_event_handle handle, uint32_t bitmask, uint32_t flag, uint32_t timeout)
{
    uint32_t ret;

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)            /* USB stack running on MQX */
    ret = OS_Event_wait(handle, bitmask, flag, timeout);
#else
    while(OS_EVENT_OK != (ret = OS_Event_wait(handle, bitmask, flag, timeout)))
    {
        /* Actually we want to run _usb_khci_task() */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (!(USE_RTOS))
        OSA_PollAllOtherTasks();
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#endif
    }
#endif
    return ret;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_cntrl_callback
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This is the callback used when ACM information is sent or received.
 *
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_cntrl_callback
(
/* [IN] Unused */
void * tr_ptr,
/* [IN] pointer to the class interface instance */
void * param,
/* [IN] Data buffer */
uint8_t * buffer,
/* [IN] Length of buffer */
uint32_t len,
/* [IN] Error code (if any) */
usb_status status
)
{ /* Body */
    usb_data_class_intf_struct_t * if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_cntrl_callback");
#endif

    /*
     ** There is no need for USB_lock in the callback function, and there is also no
     ** need to check if the device is still attached (otherwise this callback
     ** would never have been called!
     */

    /* Get class interface handle, reset IN_SETUP and call callback */
    if_ptr = (usb_data_class_intf_struct_t *)param;

    if (usb_host_release_tr(if_ptr->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("usb_class_cdc_cntrl_callback: _usb_host_release_tr failed\n");
    }

    if (if_ptr->ctrl_callback)
    {
        if_ptr->ctrl_callback(tr_ptr, if_ptr->ctrl_callback_param, buffer, len, status);
    } /* Endif */

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_cntrl_callback, SUCCESSFUL");
#endif
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_cntrl_common
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *     This function is used to send a control request.
 *     It must be run with USB locked.
 *
 *END*--------------------------------------------------------------------*/

static usb_status usb_class_cdc_cntrl_common
(
/* [IN] The communication device common command structure */
cdc_command_t * com_ptr,
/* [IN] The communication device control interface */
usb_acm_class_intf_struct_t * if_ctrl_ptr,
/* [IN] Bitmask of the request type */
uint8_t bmrequesttype,
/* [IN] Request code */
uint8_t brequest,
/* [IN] Value to copy into wvalue field of the REQUEST */
uint16_t wvalue,
/* [IN] Length of the data associated with REQUEST */
uint16_t wlength,
/* [IN] Pointer to data buffer used to send/recv */
uint8_t * data
)
{ /* Body */
    usb_data_class_intf_struct_t * if_ptr;
    //usb_setup_t                        req;
    usb_status status = USBERR_NO_INTERFACE;
    usb_pipe_handle pipe_handle;
    tr_struct_t* tr_ptr;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_cntrl_common");
#endif

    if_ptr = (usb_data_class_intf_struct_t *) com_ptr->CLASS_PTR->class_intf_handle;

    /* Save the higher level callback and ID */
    if_ptr->ctrl_callback = com_ptr->CALLBACK_FN;
    if_ptr->ctrl_callback_param = com_ptr->CALLBACK_PARAM;

    pipe_handle = (usb_pipe_handle)usb_host_dev_mng_get_control_pipe(if_ptr->dev_handle);
    if (usb_host_get_tr(if_ptr->host_handle, usb_class_cdc_cntrl_callback, if_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("usb_class_cdc_cntrl_common: error to get tr cdc\n");
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
    *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(if_ctrl_ptr->intf_num);
    *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT(wlength);
    /* Since this function is general, we must distinguish here interface class to perform
     ** appropriate action.
     ** Only ACM devices are supported now.
     */
    switch (((interface_descriptor_t*) if_ctrl_ptr->intf_handle)->bInterfaceSubClass)
    {
        case USB_SUBCLASS_COM_DIRECT:
        break;
        case USB_SUBCLASS_COM_ABSTRACT:
        if (USB_OK == (status =
        usb_host_send_setup(if_ptr->host_handle, pipe_handle, tr_ptr)))
        {
            status = USB_OK;
        }
        else
        {
            USB_PRINTF("\nError in usb_class_hid_cntrl_common: %x", (unsigned int)status);
            usb_host_release_tr(if_ptr->host_handle, tr_ptr);
            status = USBERR_ERROR;
        }
        break;
        case USB_SUBCLASS_COM_TELEPHONE:
        break;
        case USB_SUBCLASS_COM_MULTICHAN:
        break;
        case USB_SUBCLASS_COM_CAPI:
        break;
        case USB_SUBCLASS_COM_ETHERNET:
        break;
        case USB_SUBCLASS_COM_ATM_NET:
        break;
        default:
        break;
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_cntrl_common, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_cntrl_common, FAILED");
    }
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_get_acm_line_coding
 * Returned Value : Success as USB_OK
 * Comments       :
 *     This function is used to get parameters of current line (baudrate,
 *     HW control...)
 *     This function cannot be run in ISR
 * NOTE!!!
 *     DATA instance communication structure is passed here as parameter,
 *     not control interface.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_get_acm_line_coding
(
/* [IN] The communication device data instance structure */
cdc_class_call_struct_t * ccs_ptr,
/* [IN] Where to store coding */
usb_cdc_uart_coding_t * uart_coding_ptr
)
{ /* Body */
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_acm_class_intf_struct_t * if_acm_ptr;
    cdc_class_call_struct_t * acm_instance;
    usb_status status = USBERR_NO_INTERFACE;
    os_event_handle event = NULL;
    cdc_command_t cmd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_get_acm_line_coding");
#endif

    /* Validity checking: for data interface */
    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        if_data_ptr = (usb_data_class_intf_struct_t *) ccs_ptr->class_intf_handle;
        acm_instance = if_data_ptr->BOUND_CONTROL_INTERFACE;

        if (usb_class_cdc_intf_validate(if_data_ptr->BOUND_CONTROL_INTERFACE))
        {
            if_acm_ptr = (usb_acm_class_intf_struct_t *) if_data_ptr->BOUND_CONTROL_INTERFACE->class_intf_handle;

            USB_CDC_ACM_lock();
            if (if_acm_ptr->acm_desc->bmCapabilities & USB_ACM_CAP_LINE_CODING)
            {
                event = if_acm_ptr->acm_event;

                if (event != NULL)
                {
                    usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
                    if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
                    {
                        OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
                    }
                    if (OS_Event_check_bit(event, USB_ACM_DETACH))
                    {
                        OS_Event_clear(event, USB_ACM_DETACH);
                    }
                }

                if (usb_class_cdc_intf_validate(acm_instance))
                {
                    cmd.CLASS_PTR = ccs_ptr;
                    cmd.CALLBACK_FN = (tr_callback)usb_class_cdc_ctrl_acm_callback;
                    cmd.CALLBACK_PARAM = acm_instance;
                    status = usb_class_cdc_cntrl_common(&cmd, if_acm_ptr,
                    REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
                    USB_CDC_GET_LINE_CODING, 0, 7 /* sizeof(usb_cdc_uart_coding_t)*/, (uint8_t *) uart_coding_ptr);
                }
            }
            else
            {
                status = USBERR_INVALID_BMREQ_TYPE;
            }
            USB_CDC_ACM_unlock();
        } /* Endif */
    } /* Endif */

    /* This piece of code cannot be executed if it is running in USB interrupt context */
    if ((!status) && (event != NULL))
    {
        /* wait for command completion */
        usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
        if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
        {
            OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
        }
        if (OS_Event_check_bit(event, USB_ACM_DETACH))
        {
            OS_Event_clear(event, USB_ACM_DETACH);
        }
        /* and unlock control pipe as it is automatic event */
        OS_Event_set(event, USB_ACM_CTRL_PIPE_FREE);
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_acm_line_coding, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_get_acm_line_coding, FAILED");
    }
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_set_acm_line_coding
 * Returned Value : Success as USB_OK
 * Comments       :
 *     This function is used to set parameters of current line (baudrate,
 *     HW control...)
 *     This function cannot be run in ISR
 * NOTE!!!
 *     DATA instance communication structure is passed here as parameter,
 *     not control interface.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_set_acm_line_coding
(
/* [IN] The communication device data instance structure */
cdc_class_call_struct_t * ccs_ptr,
/* [IN] Coding to set */
usb_cdc_uart_coding_t * uart_coding_ptr
)
{ /* Body */
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_acm_class_intf_struct_t * if_acm_ptr;
    cdc_class_call_struct_t * acm_instance;
    usb_status status = USBERR_NO_INTERFACE;
    os_event_handle event = NULL;
    cdc_command_t cmd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_set_acm_line_coding");
#endif

    /* Validity checking: for data interface */
    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        if_data_ptr = (usb_data_class_intf_struct_t *)ccs_ptr->class_intf_handle;
        acm_instance = if_data_ptr->BOUND_CONTROL_INTERFACE;

        if (usb_class_cdc_intf_validate(acm_instance))
        {
            if_acm_ptr = (usb_acm_class_intf_struct_t *) if_data_ptr->BOUND_CONTROL_INTERFACE->class_intf_handle;
            USB_CDC_ACM_lock();
            if (if_acm_ptr->acm_desc->bmCapabilities & USB_ACM_CAP_LINE_CODING)
            {
                event = if_acm_ptr->acm_event;

                if (event != NULL)
                {
                    usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
                    if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
                    {
                        OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
                    }
                    if (OS_Event_check_bit(event, USB_ACM_DETACH))
                    {
                        OS_Event_clear(event, USB_ACM_DETACH);
                    }
                }

                if (usb_class_cdc_intf_validate(acm_instance))
                {
                    cmd.CLASS_PTR = ccs_ptr;
                    cmd.CALLBACK_FN = (tr_callback) usb_class_cdc_ctrl_acm_callback;
                    cmd.CALLBACK_PARAM = acm_instance;
                    status = usb_class_cdc_cntrl_common(&cmd, if_acm_ptr,
                    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
                    USB_CDC_SET_LINE_CODING, 0, 7 /* sizeof(usb_cdc_uart_coding_t)*/, (uint8_t *) uart_coding_ptr);
                }
            }
            else
            {
                status = USBERR_INVALID_BMREQ_TYPE;
            }
            USB_CDC_ACM_unlock();
        } /* Endif */
    } /* Endif */

    /* This piece of code cannot be executed if it is running in USB interrupt context */
    if ((!status) && (event != NULL))
    {
        /* wait for command completion */
        usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
        if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
        {
            OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
        }
        if (OS_Event_check_bit(event, USB_ACM_DETACH))
        {
            OS_Event_clear(event, USB_ACM_DETACH);
        }
        /* and unlock control pipe as it is automatic event */
        OS_Event_set(event, USB_ACM_CTRL_PIPE_FREE);
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_set_acm_line_coding, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_set_acm_line_coding, FAILED");
    }
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_set_acm_ctrl_state
 * Returned Value : Success as USB_OK
 * Comments       :
 *     This function is used to set parameters of current line (baudrate,
 *     HW control...)
 *     This function cannot be run in ISR
 * NOTE!!!
 *     DATA instance communication structure is passed here as parameter,
 *     not control interface.
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_set_acm_ctrl_state
(
/* [IN] The communication device data instance structure */
cdc_class_call_struct_t * ccs_ptr,
/* [IN] DTR state to set */
uint8_t dtr,
/* [IN] RTS state to set */
uint8_t rts
)
{ /* Body */
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_acm_class_intf_struct_t * if_acm_ptr;
    cdc_class_call_struct_t * acm_instance;
    usb_status status = USBERR_NO_INTERFACE;
    os_event_handle event = NULL;
    cdc_command_t cmd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_set_acm_ctrl_state");
#endif

    /* Validity checking: for data interface */
    if (usb_class_cdc_intf_validate(ccs_ptr))
    {
        if_data_ptr = (usb_data_class_intf_struct_t *)ccs_ptr->class_intf_handle;
        acm_instance = if_data_ptr->BOUND_CONTROL_INTERFACE;

        if (usb_class_cdc_intf_validate(acm_instance))
        {
            if_acm_ptr = (usb_acm_class_intf_struct_t *) if_data_ptr->BOUND_CONTROL_INTERFACE->class_intf_handle;
            USB_CDC_ACM_lock();
            if (if_acm_ptr->acm_desc->bmCapabilities & USB_ACM_CAP_LINE_CODING)
            {
                event = if_acm_ptr->acm_event;

                if (event != NULL)
                {
                    usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
                    if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
                    {
                        OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
                    }
                    if (OS_Event_check_bit(event, USB_ACM_DETACH))
                    {
                        OS_Event_clear(event, USB_ACM_DETACH);
                    }
                }

                if (usb_class_cdc_intf_validate(acm_instance))
                {
                    cmd.CLASS_PTR = ccs_ptr;
                    cmd.CALLBACK_FN = (tr_callback) usb_class_cdc_ctrl_acm_callback;
                    cmd.CALLBACK_PARAM = acm_instance;

                    if_acm_ptr->ctrl_state.state = dtr ? USB_ACM_LINE_STATE_DTR : 0u;
                    if_acm_ptr->ctrl_state.state |= rts ? USB_ACM_LINE_STATE_RTS : 0u;
                    status = usb_class_cdc_cntrl_common(&cmd, if_acm_ptr,
                    REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
                    USB_CDC_SET_CTRL_LINE_STATE, if_acm_ptr->ctrl_state.state, 0, NULL);
                }
            }
            else
            {
                status = USBERR_INVALID_BMREQ_TYPE;
            }
            USB_CDC_ACM_unlock();
        } /* Endif */
    } /* Endif */

    /* This piece of code cannot be executed if it is running in USB interrupt context */
    if ((!status) && (event != NULL))
    {
        /* wait for command completion */
        usb_class_cdc_os_event_wait(event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_DETACH, FALSE, 0);
        if (OS_Event_check_bit(event, USB_ACM_CTRL_PIPE_FREE))
        {
            OS_Event_clear(event, USB_ACM_CTRL_PIPE_FREE);
        }
        if (OS_Event_check_bit(event, USB_ACM_DETACH))
        {
            OS_Event_clear(event, USB_ACM_DETACH);
        }
        /* and unlock control pipe as it is automatic event */
        OS_Event_set(event, USB_ACM_CTRL_PIPE_FREE);
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_set_acm_ctrl_state, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_set_acm_ctrl_state, FAILED");
    }
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_acm_int_callback
 * Returned Value : None
 * Comments       :
 *     Called on interrupt endpoint data reception.
 *
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_int_acm_callback
(
/* [IN] pointer to pipe */
void * tr_ptr,

/* [IN] user-defined parameter */
void * param,

/* [IN] buffer address */
uint8_t * buffer,

/* [IN] length of data transferred */
uint32_t len,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{ /* Body */
    //cdc_class_call_struct_t * acm_parser;
    tr_struct_t* tr_int_ptr;
    usb_acm_class_intf_struct_t * if_acm_ptr = (usb_acm_class_intf_struct_t *)param;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_int_acm_callback");
#endif
    if (usb_host_release_tr(if_acm_ptr->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("usb_class_cdc_int_acm_callback: _usb_host_release_tr failed\n");
    }
    if(USB_OK == status)
    {
        /* To Do... */
    }
    else
    {
        /* In case of error, e.g. KHCI_ATOM_TR_TO, need to re-issue tr for interrupt pipe */
        USB_CDC_ACM_lock();
        if (if_acm_ptr->interrupt_pipe != NULL)
        {

            if (usb_host_get_tr(if_acm_ptr->host_handle, usb_class_cdc_int_acm_callback, if_acm_ptr, &tr_int_ptr) != USB_OK)
            {
                USB_PRINTF("usb_class_cdc_int_acm_callback: error to get tr\n");
                USB_CDC_ACM_unlock();
                return;
            }
            tr_int_ptr->rx_buffer = (uint8_t *) &if_acm_ptr->interrupt_buffer;
            tr_int_ptr->rx_length = sizeof(if_acm_ptr->interrupt_buffer);
            status = usb_host_recv_data(if_acm_ptr->host_handle, if_acm_ptr->interrupt_pipe, tr_int_ptr);
            if (status != USB_OK)
            {
                USB_PRINTF("\nError in usb_class_cdc_int_acm_callback: %x", (unsigned int)status);
                usb_host_release_tr(if_acm_ptr->host_handle, tr_int_ptr);
            }
        }
        else
        {
            status = USBERR_OPEN_PIPE_FAILED;
        }
        USB_CDC_ACM_unlock();
    }
    /* we do not use USB_ACM_INT_PIPE_FREE in this version at all */
//    _lwevent_set(if_ptr->acm_event, USB_ACM_INT_PIPE_FREE); 
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_int_acm_callback, SUCCESSFUL");
#endif

} /*EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_acm_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called on ctrl endpoint data reception / send
 *
 *END*--------------------------------------------------------------------*/

static void usb_class_cdc_ctrl_acm_callback
(
/* [IN] pointer to pipe */
void * tr_ptr,

/* [IN] user-defined parameter */
void * param,

/* [IN] buffer address */
uint8_t * buffer,

/* [IN] length of data transferred */
uint32_t len,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{ /* Body */
    cdc_class_call_struct_t * acm_instance = (cdc_class_call_struct_t *) param;
    usb_acm_class_intf_struct_t * if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_ctrl_acm_callback");
#endif

    if_ptr = (usb_acm_class_intf_struct_t *) acm_instance->class_intf_handle;

    if (if_ptr->acm_event != NULL)
    {
        OS_Event_set(if_ptr->acm_event, USB_ACM_CTRL_PIPE_FREE); /* mark we are not using control pipe */
    }

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_ctrl_acm_callback, SUCCESSFUL");
#endif

} /*EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_data_in_callback
 * Returned Value : Success as USB_OK
 * Comments       :
 *     Called on interrupt endpoint data reception
 *END*--------------------------------------------------------------------*/

void usb_class_cdc_in_data_callback
(
/* [IN] pointer to pipe */
void * tr_ptr,

/* [IN] user-defined parameter */
void * param,

/* [IN] buffer address */
uint8_t * buffer,

/* [IN] length of data transferred */
uint32_t len,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{ /* Body */
    file_cdc_t * fd_ptr = (file_cdc_t *) param;
    cdc_class_call_struct_t * data_instance = (cdc_class_call_struct_t *) fd_ptr->dev_ptr->driver_init_ptr;
    usb_data_class_intf_struct_t * if_ptr;
    cdc_class_call_struct_t * acm_instance;
    usb_acm_class_intf_struct_t * acm_if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_in_data_callback");
#endif

    if_ptr = (usb_data_class_intf_struct_t *) data_instance->class_intf_handle;
    acm_instance = if_ptr->BOUND_CONTROL_INTERFACE;

    if ((acm_instance == NULL) || (!usb_class_cdc_intf_validate(acm_instance)))
    {
        len = 0;
    }
    else if (((cdc_serial_init_t *) fd_ptr->flags)->flags & USB_UART_HW_FLOW)
    {
        acm_if_ptr = (usb_acm_class_intf_struct_t *) acm_instance->class_intf_handle;
        /* check the state of DCD signal for HW flow control files */
        if (!(acm_if_ptr->interrupt_buffer.bmStates & USB_ACM_STATE_RX_CARRIER))
        {
            len = 0; /* ignore all received bytes if DCD is not set */
        }
        /* check the state of DTR signal */
        if (!(acm_if_ptr->ctrl_state.state & USB_ACM_LINE_STATE_DTR))
        {
            len = 0; /* ignore all sent bytes if DTR is not set */
        }
    }

    /* in the case we have less data than expected, status is not USB_OK, but buffer is not NULL */
    if ((status != USB_OK) && (buffer == NULL)) /* if no data received */
    {
        len = 0;
    }
    if (if_ptr->RX_BUFFER_DRV != NULL)
    {
        if_ptr->RX_BUFFER_DRV += len;
    }
    if_ptr->RX_READ = len;

    if (if_ptr->data_event != NULL)
    {
        OS_Event_set(if_ptr->data_event, USB_DATA_READ_COMPLETE); /* signal that we have completed transfer on input pipe */
    }

    if(if_ptr->data_rx_cb)
    {
        if_ptr->data_rx_cb(NULL);
        //if_ptr->data_rx_cb = NULL;
    }
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_in_data_callback, SUCCESSFUL");
#endif

} /*EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_data_out_callback
 * Returned Value : Success as USB_OK
 * Comments       :
 *     Called on interrupt endpoint data reception
 *END*--------------------------------------------------------------------*/

void usb_class_cdc_out_data_callback
(
/* [IN] pointer to pipe */
void * tr_ptr,

/* [IN] user-defined parameter */
void * param,

/* [IN] buffer address */
uint8_t * buffer,

/* [IN] length of data transferred */
uint32_t len,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{ /* Body */
    file_cdc_t * fd_ptr = (file_cdc_t *) param;
    cdc_class_call_struct_t * data_instance = (cdc_class_call_struct_t *) fd_ptr->dev_ptr->driver_init_ptr;
    usb_data_class_intf_struct_t * if_ptr;
    cdc_class_call_struct_t * acm_instance;
    usb_acm_class_intf_struct_t * acm_if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_in_data_callback");
#endif

    if_ptr = (usb_data_class_intf_struct_t *) data_instance->class_intf_handle;
    acm_instance = if_ptr->BOUND_CONTROL_INTERFACE;

    /* Prepare ACM control interface for future use */
    if ((acm_instance == NULL) || (!usb_class_cdc_intf_validate(acm_instance)))
    {
        len = 0;
    }
    else if (((cdc_serial_init_t *) fd_ptr->flags)->flags & USB_UART_HW_FLOW)
    {
        acm_if_ptr = (usb_acm_class_intf_struct_t *) acm_instance->class_intf_handle;
        /* check the state of DSR signal for HW flow control files */
        if (!(acm_if_ptr->interrupt_buffer.bmStates & USB_ACM_STATE_TX_CARRIER))
        {
            len = 0;
        }
        /* check the state of RTS signal */
        if (!(acm_if_ptr->ctrl_state.state & USB_ACM_LINE_STATE_RTS))
        {
            len = 0; /* ignore all sent bytes if RTS is not set */
        }
    }

    /* in the case we have less data than expected, status is not USB_OK, but buffer is not NULL */
    if ((status != USB_OK) && (buffer == NULL)) /* if no data received */
    {
        len = 0;
    }
    if_ptr->TX_SENT = len;

    if (if_ptr->data_event != NULL)
    {
        OS_Event_set(if_ptr->data_event, USB_DATA_SEND_COMPLETE); /* signal that we have completed transfer on output pipe */
    }

    if(if_ptr->data_tx_cb)
    {
        if_ptr->data_tx_cb(NULL);
        //if_ptr->data_tx_cb = NULL;
    }
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_in_data_callback, SUCCESSFUL");
#endif

} /*EndBody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_init_ipipe
 * Returned Value : Success as USB_OK, no interrupt pipe (not error!) as 
 * Comments       :
 *     Starts interrupt endpoint to poll for interrupt on specified device.
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_init_ipipe
(
/* ACM interface instance */
cdc_class_call_struct_t * acm_instance
)
{ /* Body */
    usb_status status = USBERR_NO_INTERFACE;
    usb_acm_class_intf_struct_t * if_acm_ptr;
    tr_struct_t* tr_ptr;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_init_ipipe");
#endif

    /* Validity checking, always needed when passing data to lower API */
    if (usb_class_cdc_intf_validate(acm_instance))
    {
        if_acm_ptr = (usb_acm_class_intf_struct_t *) acm_instance->class_intf_handle;
        USB_CDC_ACM_lock();
        if (if_acm_ptr->interrupt_pipe != NULL)
        {

            if (usb_host_get_tr(if_acm_ptr->host_handle, usb_class_cdc_int_acm_callback, if_acm_ptr, &tr_ptr) != USB_OK)
            {
                USB_PRINTF("usb_class_cdc_init_ipipe: error to get tr\n");
                return USBERR_ERROR;
            }

            tr_ptr->rx_buffer = (uint8_t *) &if_acm_ptr->interrupt_buffer;
            tr_ptr->rx_length = sizeof(if_acm_ptr->interrupt_buffer);
            status = usb_host_recv_data(if_acm_ptr->host_handle, if_acm_ptr->interrupt_pipe, tr_ptr);
            if (status != USB_OK)
            {
                USB_PRINTF("\nError in usb_class_cdc_init_ipipe: %x", (unsigned int)status);
                usb_host_release_tr(if_acm_ptr->host_handle, tr_ptr);
                return USBERR_ERROR;
            }
        }
        else
        {
            status = USBERR_OPEN_PIPE_FAILED;
        }
        USB_CDC_ACM_unlock();
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_init_ipipe, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_init_ipipe, FAILED");
    }
#endif

    return status;
}

#endif
