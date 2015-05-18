/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: usb_otg_main.c$
 * $Version : 
 * $Date    : 
 *
 * Comments : This file contains the implementation of the OTG driver for USB FS
 *
 *         
 *****************************************************************************/
#include "usb.h"
#include "usb_device_config.h"
#include "usb_otg_main.h"
#include "usb_otg_host_api.h"
#include "usb_otg_dev_api.h"
#include "usb_otg_sm.h"
#include "usb_otg_private.h"
#include "usb_device_stack_interface.h"
#include "usb_otg_khci.h"

#include "usb_otg.h"

/* OTG task parameters */
#define USB_OTG_TASK_TEMPLATE_INDEX       0
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& (USE_RTOS))  
/* USB stack running on MQX */
#define USB_OTG_TASK_ADDRESS              _usb_otg_task_stun
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& !(USE_RTOS)) 
/* USB stack running on BM  */
#define USB_OTG_TASK_ADDRESS              _usb_otg_task
#endif

#define USB_OTG_TASK_STACKSIZE            2000
#define USBCFG_OTG_TASK_PRIORITY          9
#define USB_OTG_TASK_NAME                 "OTG Task"
#define USB_OTG_TASK_ATTRIBUTES           0
#define USB_OTG_TASK_DEFAULT_TIME_SLICE   0
#define USB_OTG_ISR_EVENT_MASK            (~(uint32_t)0)

extern const usb_otg_api_functions_struct_t g_usb_otg_callback_table;

/* Type Definitions*********************************************************/

/* Private functions prototypes ***********************************************/
static usb_status _usb_otg_task_create(usb_otg_handle otg_handle);
static void _usb_otg_task(void* otg_handle);
extern usb_status bsp_usb_otg_init(uint8_t controller_id);

#ifdef __cplusplus
extern "C"
{
#endif
    extern usb_status USB_log_error(char* file, uint32_t line, usb_status error);
#ifdef __cplusplus
}
#endif
/* Private memory definitions ***********************************************/
/* store the otg handle */
usb_otg_state_struct_t * g_usb_otg_handle;
int32_t g_otg_task_id;
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_task_stun
 *  Returned Value : none
 *  Comments       :
 *  
 *END*-----------------------------------------------------------------*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& (USE_RTOS)))  
static void _usb_otg_task_stun(void* dev_inst_ptr)
{
    while (1)
    {
        _usb_otg_task(dev_inst_ptr);
    }
}
#endif
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_task_create
 *  Returned Value : error or USB_OK
 *  Comments       :
 *  
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_task_create(usb_otg_handle otg_handle)
{
    int32_t task_id;
    task_id = OS_Task_create(USB_OTG_TASK_ADDRESS,
        (void*) otg_handle,
        (uint32_t) USBCFG_OTG_TASK_PRIORITY,
        USB_OTG_TASK_STACKSIZE,
        USB_OTG_TASK_NAME,
        NULL);

    if (task_id == OS_TASK_ERROR)
    {
        return USBERR_UNKNOWN_ERROR;
    }
    g_otg_task_id = task_id;
    return USB_OK;
}
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_otg_get_state
 *  Returned Value : error or USB_OK
 *  Comments       :
 *  
 *END*-----------------------------------------------------------------*/
uint8_t usb_otg_get_state(usb_otg_handle otg_handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    return usb_otg_struct_ptr->device_state;
}
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_task_delete
 *  Returned Value : error or USB_OK
 *  Comments       :
 *  
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_task_delete(usb_otg_handle otg_handle)
{
    int32_t task_id;
    task_id = g_otg_task_id;
    if (task_id == OS_TASK_ERROR)
    {
        return USBERR_UNKNOWN_ERROR;
    }
    OS_Task_delete(task_id);
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_get_api
 *  Returned Value :
 *    
 *
 *  Comments       :
 *  
 *
 *END*-----------------------------------------------------------------*/
static void _usb_otg_get_api(uint8_t controller_id, const usb_otg_api_functions_struct_t * * controller_api_ptr)
{
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        *controller_api_ptr = (const usb_otg_api_functions_struct_t *) &g_usb_otg_callback_table;
    }
    else
    {
        *controller_api_ptr = NULL;
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_init
 * Returned Value   : initialization message
 * Comments         : Initializes OTG stack 
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_init
(
    /* [IN] the USB device controller to initialize */
    uint8_t controller_id,
    /*[IN] address of the OTG initialization  structure */
    otg_int_struct_t * init_struct_ptr,
    /* [OUT] the USB host handle */
    usb_otg_handle * handle
    )
{
    usb_status error = USB_OK;
    const usb_otg_api_functions_struct_t * otg_api = NULL;
    usb_otg_state_struct_t * usb_otg_ptr = NULL;
    if ((init_struct_ptr == NULL) || (handle == NULL))
    {
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
        DEBUG_LOG_TRACE ("usb_otg_init invalid parameters");
#endif

        return USBERR_ERROR;
    }
    _usb_otg_get_api(controller_id, &otg_api);
    if (otg_api == NULL)
    {
        return USBERR_ERROR;
    }
    /* Initialize the USB interface. */
    if (otg_api->otg_preinit != NULL)
    {
        error = otg_api->otg_preinit((usb_otg_handle *) (&(usb_otg_ptr)));
        if (error || (usb_otg_ptr == NULL))
        {
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
            DEBUG_LOG_TRACE ("usb_OTG_init preinit failure");
#endif
            return USBERR_ALLOC;
        }
    }
    g_usb_otg_handle = usb_otg_ptr;
    usb_otg_ptr->otg_controller_api = otg_api;

    error = bsp_usb_otg_init(controller_id);

    if (error != USB_OK)
    {
        if (otg_api->otg_shutdown != NULL)
        {
            error = otg_api->otg_shutdown(usb_otg_ptr);
        }
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
        DEBUG_LOG_TRACE ("_usb_host_init: BSP-specific USB initialization failure");
#endif
        return USB_log_error(__FILE__, __LINE__, USBERR_UNKNOWN_ERROR);
    }

    usb_otg_ptr->init_struct = init_struct_ptr;
    /* initialize the event used for application signaling and for ISR*/
    usb_otg_ptr->otg_app_event = OS_Event_create(0);
    usb_otg_ptr->otg_isr_event = OS_Event_create(0);
    usb_otg_ptr->device_state = USB_OTG_DEVSTATE_UNDEFINED;
    usb_otg_ptr->sub_state = USB_OTG_SM_UNDEFINED;
    usb_otg_ptr->power_up = TRUE;

    if (otg_api->otg_init != NULL)
    {
        error = otg_api->otg_init(controller_id, usb_otg_ptr);
    }

    if (error != USB_OK)
    {
        if (otg_api->otg_shutdown != NULL)
        {
            error = otg_api->otg_shutdown(usb_otg_ptr);
        }
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
        DEBUG_LOG_TRACE ("_usb_host_init: BSP-specific USB initialization failure");
#endif
        return USB_log_error(__FILE__, __LINE__, USBERR_UNKNOWN_ERROR);
    }

    error = _usb_otg_task_create(usb_otg_ptr);

    if (error != USB_OK)
    {
        if (otg_api->otg_shutdown != NULL)
        {
            otg_api->otg_shutdown(usb_otg_ptr);
        }
        (void) OS_Event_destroy(usb_otg_ptr->otg_app_event);
        (void) OS_Event_destroy(usb_otg_ptr->otg_isr_event);
        OS_Mem_free(usb_otg_ptr);
        usb_otg_ptr = NULL;

        OS_Unlock();
        return error;
    }
    *handle = (usb_otg_handle) usb_otg_ptr;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_session_request
 * Returned Value   : session request message
 * Comments         : B-device requests a new session to be started by the A device
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_session_request(usb_otg_handle handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;

    if (handle == NULL)
    {
        return USBERR_ERROR;
    }

    if ((usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_B)
    || (usb_otg_struct_ptr->sub_state >= USB_OTG_SM_B_PERI_BUS_SUSP_WAIT))
    {
        return USBERR_INVALID_REQ_TYPE;
    }

    usb_otg_struct_ptr->srp_request = TRUE;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_bus_request
 * Returned Value   : bus request message
 * Comments         : B-device requests to become Host
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_bus_request(usb_otg_handle handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;
    uint16_t dev_otg_status;

    if (handle == NULL)
    {
        return USBERR_ERROR;
    }

    if ((usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_B)
    || (usb_otg_struct_ptr->sub_state != USB_OTG_SM_B_PERI_BUS_SUSP_WAIT))
    {
        return USBERR_INVALID_REQ_TYPE;
    }

    usb_otg_struct_ptr->bus_request = TRUE;

    usb_device_get_status(usb_otg_struct_ptr->dev_inst_ptr, USB_STATUS_OTG, &dev_otg_status);

    dev_otg_status |= USB_OTG_HOST_REQUEST_FLAG;

    usb_device_set_status(usb_otg_struct_ptr->dev_inst_ptr, USB_STATUS_OTG, dev_otg_status);

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_bus_release
 * Returned Value   : bus release message
 * Comments         : B-device hands over the bus back to the A device
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_bus_release(usb_otg_handle handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;

    if (handle == NULL)
    {
        return USBERR_ERROR;
    }

    if ((usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_B) ||
    (usb_otg_struct_ptr->sub_state != USB_OTG_SM_B_HOST))
    {
        return USBERR_INVALID_REQ_TYPE;
    }

    usb_otg_struct_ptr->bus_release = TRUE;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_device_hnp_enable
 * Returned Value   : HNP enable status
 * Comments         : This function is intended to be called from the Peripheral USB stack 
 *                  : in response to SET/CLEAR Feature requests from the Host for HNP_Enable 
 *    
 *
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_device_hnp_enable
(
    usb_otg_handle handle,
    uint8_t enable
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;

    if (usb_otg_struct_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    usb_otg_struct_ptr->hnp_enabled = enable;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_task
 *  Returned Value :
 *  Comments       : OTG task
 *        
 *END*------------------------------------------------------------------*/

static void _usb_otg_task(void* otg_handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& (USE_RTOS))  
    OS_Event_wait(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT | USB_OTG_MAX3353_ISR_EVENT, FALSE, 10);
    _usb_otg_callback_get_status((usb_otg_handle) otg_handle);
#else
    if(OS_Event_check_bit(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT|USB_OTG_MAX3353_ISR_EVENT))
    {
        _usb_otg_callback_get_status((usb_otg_handle)otg_handle);
    }
#endif
    _usb_otg_sm(usb_otg_struct_ptr);

    /* Application events */
    if (OS_Event_check_bit(usb_otg_struct_ptr->otg_app_event, OTG_STATE_EVENT_MASK))
    {
        /* App indications */
        if (usb_otg_struct_ptr->init_struct->app_otg_callback)
        {
            usb_otg_struct_ptr->init_struct->app_otg_callback((usb_otg_handle) usb_otg_struct_ptr, usb_otg_struct_ptr->otg_app_event);
        }
        OS_Event_clear((usb_otg_struct_ptr->otg_app_event), OTG_STATE_EVENT_MASK);
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_set_a_bus_req
 * Returned Value   : set A bus request status
 * Comments         : This function is called from the application to set/clear the 
 *                    a_bus_req parameter. This is one of the parameters that determines 
 *                    A state machine behavior.If the A device is in peripheral state 
 *                    the otg status changes to USB_OTG_HOST_REQUEST_FLAG.  
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_set_a_bus_req(usb_otg_handle otg_handle, bool a_bus_req)
{
    uint16_t dev_otg_status;
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;
    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_A)
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    otg_status->a_bus_req = a_bus_req;
    if (usb_otg_struct_ptr->sub_state == USB_OTG_SM_A_PERIPHERAL)
    {
        usb_device_get_status(usb_otg_struct_ptr->dev_inst_ptr, USB_STATUS_OTG, &dev_otg_status);
        dev_otg_status |= USB_OTG_HOST_REQUEST_FLAG;
        usb_device_set_status(usb_otg_struct_ptr->dev_inst_ptr, USB_STATUS_OTG, dev_otg_status);
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_get_a_bus_req
 * Returned Value   : get A bus request status
 * Comments         : This function is called from the application to get 
 *                    a_bus_req parameter.  
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_get_a_bus_req(usb_otg_handle otg_handle, bool* a_bus_req)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;
    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_A)
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    *a_bus_req = otg_status->a_bus_req;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_set_a_bus_drop
 * Returned Value   : set A bus drop status
 * Comments         : This function is called from the application to set/clear the 
 *                    a_bus_drop parameter. This is one of the parameters that determines 
 *                    A state machine behavior.
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_set_a_bus_drop(usb_otg_handle otg_handle, bool a_bus_drop)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_A)
    {
        return USBERR_INVALID_REQ_TYPE;
    }

    if (a_bus_drop)
    {
        otg_status->a_bus_drop = TRUE;
    }
    else
    {
        otg_status->a_bus_drop = FALSE;
    }

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_get_a_bus_drop
 * Returned Value   : set A bus drop status
 * Comments         : This function is called from the application to get the 
 *                    a_bus_drop parameter. 
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_get_a_bus_drop(usb_otg_handle otg_handle, bool* a_bus_drop)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_A)
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    *a_bus_drop = otg_status->a_bus_drop;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_set_a_clear_err
 * Returned Value   : set A clear error status
 * Comments         : This function is called from the application to set the a_clr_err
 *                    parameter which is one way to escape from the a_vbus_err state.
 *                    The other two are id = FALSE and a_bus_drop = TRUE.
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_set_a_clear_err(usb_otg_handle otg_handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    if (usb_otg_struct_ptr->device_state != USB_OTG_DEVSTATE_A)
    {
        return USBERR_INVALID_REQ_TYPE;
    }

    otg_status->a_clr_err = TRUE;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_host_a_set_b_hnp_en
 * Returned Value   : set B HNP status
 * Comments         : This function is called from usb host stack at device enumeration.
 *                    More specific it is called from usb_host_cntrl_transaction_done function
 *                    (in host_ch9.c) when b_hnp_enable feature was successfully set in the B OTG device.
 *                    The function simply set the a_set_b_hnp_en status parameter. 
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_host_a_set_b_hnp_en(usb_otg_handle otg_handle, bool b_hnp_en)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t * otg_status_ptr;

    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    otg_status_ptr = &usb_otg_struct_ptr->otg_status;
    otg_status_ptr->a_set_b_hnp_en = b_hnp_en;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_host_get_otg_attribute
 * Returned Value   : get OTG attribute status
 * Comments         : This function is called from usb host stack at device enumeration.
 *                    More specific it is called from usb_host_cntrl_transaction_done function
 *                    (in host_ch9.c) when configuration descriptor was read and an OTG descriptor     .
 *                    was identified in it.
 *                    The function simply set srp_support hnp_support status parameters according with
 *                    their corresponding values in the OTG descriptor bmAttributes.
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_host_get_otg_attribute(usb_otg_handle otg_handle, uint8_t bm_attributes)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t * otg_status;
    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }
    otg_status = &usb_otg_struct_ptr->otg_status;
    otg_status->srp_support = (bm_attributes & OTG_ATTR_SRP_SUPPORT) ? (TRUE) : (FALSE);
    otg_status->hnp_support = (bm_attributes & OTG_ATTR_HNP_SUPPORT) ? (TRUE) : (FALSE);

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_host_set_feature_required
 * Returned Value   : set feature status
 * Comments         : This function is called from usb host stack at device enumeration.
 *                    More specific it is called from usb_host_cntrl_transaction_done function
 *                    (in host_ch9.c) when configuration descriptor  was read ,  an OTG descriptor     .
 *                    was identified in it and _usb_otg_get_otg_attribute was already called.
 *                    The function decides if a_hnp_support and b_hnp_enable features have to be set
 *                    in the attached peripheral.
 *                    The function returns TRUE if the following two conditions are met: the peripheral  
 *                    supports hnp and the host is the A device.
 *END*----------------------------------------------------------------------*/
uint8_t usb_otg_host_set_feature_required(usb_otg_handle otg_handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t * otg_status_ptr;
    if (otg_handle == NULL)
    {
        return FALSE;
    }

    otg_status_ptr = &usb_otg_struct_ptr->otg_status;

    if ((otg_status_ptr->hnp_support) && (otg_status_ptr->id == (uint8_t) FALSE))
    {
        return TRUE;
    }

    return FALSE;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_host_on_interface_event
 * Returned Value   : on interface event status
 * Comments         : This function is called from the host application at interface event.
 *                    The function simply set the dev_inst_ptr pointer in the status struct  
 *                    to the (DEV_INSTANCE_PTR)dev_handle value after dev_handle value was 
 *                    checked and found to be valid.
 *                    The dev_inst_ptr value will be used in the OTG state machine to poll 
 *                    the peripheral for hnp request.
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_host_on_interface_event(usb_otg_handle otg_handle, usb_device_handle dev_handle)
{

    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t * otg_status = &usb_otg_struct_ptr->otg_status;
    if ((dev_handle == NULL) || (otg_handle == NULL))
    {
        return USBERR_ERROR;
    }

    usb_otg_struct_ptr->dev_inst_ptr = dev_handle;
    otg_status->host_req_poll_timer = 0;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_host_on_detach_event
 * Returned Value   : on detach status
 * Comments         : This function is called from the host event function in the host application 
 *                    at detach event.
 *                    The function resets all peripheral related parameters in the OTG state struct
 *                    if the host event function was called due to a detach event.
 *                    The function doesn't take any actions if the host event function was called
 *                    due to a host stack unload.
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_host_on_detach_event(usb_otg_handle otg_handle)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr;
    usb_otg_status_t * otg_status_ptr;

    if (otg_handle == NULL)
    {
        return USBERR_ERROR;
    }

    usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;

    if (usb_otg_struct_ptr->dev_inst_ptr != NULL)
    {
        usb_otg_struct_ptr->dev_inst_ptr = NULL;
        otg_status_ptr = &usb_otg_struct_ptr->otg_status;
        otg_status_ptr->a_conn = FALSE;
        otg_status_ptr->b_conn = 0;
        otg_status_ptr->hnp_req = 0;
        otg_status_ptr->srp_support = 0;
        otg_status_ptr->hnp_support = 0;

    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_device_on_class_init
 * Returned Value   : 
 * Comments         : 
 *
 *
 *END*----------------------------------------------------------------------*/
uint32_t usb_otg_device_on_class_init(usb_otg_handle otg_handle, usb_device_handle dev_handle, uint8_t bm_attributes)
{

    usb_otg_state_struct_t * usb_otg_struct_ptr;
    usb_otg_status_t * otg_status_ptr;
    if ((dev_handle == NULL) || (otg_handle == NULL))
    {
        return USBERR_ERROR;
    }
    usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    otg_status_ptr = &usb_otg_struct_ptr->otg_status;
    usb_otg_struct_ptr->dev_inst_ptr = dev_handle;
    otg_status_ptr->srp_support = (bm_attributes & OTG_ATTR_SRP_SUPPORT) ? (TRUE) : (FALSE);
    otg_status_ptr->hnp_support = (bm_attributes & OTG_ATTR_HNP_SUPPORT) ? (TRUE) : (FALSE);
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_shut_down
 * Returned Value   : de-initialization status
 * Comments         : De-Initializes OTG stack 
 *        
 *
 *END*----------------------------------------------------------------------*/
usb_status usb_otg_shut_down
(
    /*[IN] address of the OTG interface structure */
    usb_otg_handle otg_handle
    )
{
    usb_status error = USB_OK;
    usb_otg_state_struct_t * usb_otg_ptr = g_usb_otg_handle;
    const usb_otg_api_functions_struct_t * otg_api = NULL;
    if ((otg_handle == NULL) || (usb_otg_ptr == NULL))
    {
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
        DEBUG_LOG_TRACE ("_usb_otg_init invalid parameters");
#endif

        return USBERR_ERROR;
    }
    otg_api = usb_otg_ptr->otg_controller_api;
    if (otg_api == NULL)
    {
        return USBERR_ERROR;
    }
    error = _usb_otg_task_delete(usb_otg_ptr);
    if (error != USB_OK)
    {
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
        DEBUG_LOG_TRACE ("usb_otg_shut_down _usb_otg_task_delete failure");
#endif
        return USBERR_ALLOC;
    }
    /* Initialize the USB interface. */
    if (otg_api->otg_shutdown != NULL)
    {
        error = otg_api->otg_shutdown(usb_otg_ptr);
        if (error)
        {
#if ((defined _OTG_DEBUG_) && (_OTG_DEBUG))
            DEBUG_LOG_TRACE ("usb_otg_shut_down otg_shutdown failure");
#endif
            return USBERR_ALLOC;
        }
    }
    usb_otg_ptr->init_struct = NULL;
    OS_Event_destroy(usb_otg_ptr->otg_app_event);
    OS_Event_destroy(usb_otg_ptr->otg_isr_event);
    usb_otg_ptr->device_state = USB_OTG_DEVSTATE_UNDEFINED;
    usb_otg_ptr->sub_state = USB_OTG_SM_UNDEFINED;
    usb_otg_ptr->power_up = FALSE;
    OS_Mem_free(usb_otg_ptr);
    usb_otg_ptr = NULL;
    return USB_OK;
}
