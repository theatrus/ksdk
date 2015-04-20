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
* $FileName: usb_host.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains the USB Host API specific main functions.
*
*END************************************************************************/
#include "usb_host_config.h"
#if (USBCFG_HOST_KHCI) || (USBCFG_HOST_EHCI)
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host.h"
#include "usb_host_common.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#ifdef USBCFG_OTG
#include "usb_otg_main.h"
#include "usb_otg_private.h"
#endif

#define MAX_HOST_NUM 2
#define USB_HOST_HANDLE_OCCUPIED ((uint8_t)1)
#define USB_HOST_HANDLE_FREE     ((uint8_t)0)

#if USBCFG_HOST_KHCI
extern const struct usb_host_api_functions_struct _usb_khci_host_api_table;
#endif

#if USBCFG_HOST_EHCI
extern const struct usb_host_api_functions_struct _usb_ehci_host_api_table;
#endif

static usb_host_state_struct_t g_usb_host[MAX_HOST_NUM];

extern int32_t bsp_usb_host_init(uint8_t controller_id);
extern void* _usb_host_dev_get_instance(usb_host_handle, uint8_t, uint8_t, uint8_t);
#ifdef USBCFG_OTG
extern usb_otg_handle *  g_usb_otg_handle;
#endif

usb_status USB_log_error(char* file, uint32_t line, usb_status error)
{
    return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_handle
*  Returned Value :
*     The host state pointer
*
*  Comments       :
*     Get a valid host state pointer
*
*END*-----------------------------------------------------------------*/
static usb_host_state_struct_t* _usb_host_get_handle(void)
{
    uint8_t i = 0;
    OS_Lock();
    for (; i < MAX_HOST_NUM; i++)
    {
        if (g_usb_host[i].occupied != USB_HOST_HANDLE_OCCUPIED)
        {
            OS_Mem_zero(&g_usb_host[i], sizeof(usb_host_state_struct_t));
            g_usb_host[i].occupied = USB_HOST_HANDLE_OCCUPIED;
            OS_Unlock();
            return &g_usb_host[i];
        }
    }
    OS_Unlock();
    return NULL;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_release_handle
*  Returned Value :
*    
*
*  Comments       :
*     release a valid host state pointer
*
*END*-----------------------------------------------------------------*/
static void _usb_host_release_handle(usb_host_state_struct_t *usb_host)
{
    OS_Lock();
    usb_host->occupied = USB_HOST_HANDLE_FREE;
    OS_Unlock();
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_release_handle
*  Returned Value :
*    
*
*  Comments       :
*     Get the API table for the target host controller
*
*END*-----------------------------------------------------------------*/
static void _usb_host_get_api(uint8_t controller_id, usb_host_api_functions_struct_t** controller_api_ptr)
{
#if USBCFG_HOST_KHCI
    if(controller_id == USB_CONTROLLER_KHCI_0)
    {
        *controller_api_ptr = (usb_host_api_functions_struct_t*)&_usb_khci_host_api_table;
    }
#endif

#if USBCFG_HOST_EHCI
    if ((controller_id == USB_CONTROLLER_EHCI_0) || (controller_id == USB_CONTROLLER_EHCI_1))
    {
		*controller_api_ptr = (usb_host_api_functions_struct_t*)&_usb_ehci_host_api_table;
    }
#endif
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_link_tr
*  Returned Value :
*     USBERR_INVALID_PIPE_HANDLE if the pipe is invalid
*     USBERR_ALLOC if unable to get a new TR
*     USB_STATUS_TRANSFER_QUEUED upon success
*
*  Comments       :
*     Sets up a TR from init parameters.
*
*END*-----------------------------------------------------------------*/
usb_status  _usb_host_link_tr
   (
      /* [IN] the pipe handle */
      usb_pipe_handle           pipe_handle,

      /* [IN] the TR address on the pipe's TR list */
      tr_struct_t*              tr_ptr
   )
{
    pipe_struct_t* pipe_ptr;
    tr_struct_t*   tr_list;
    usb_host_handle usb_host_ptr;
 
    pipe_ptr = (pipe_struct_t*)pipe_handle;
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("_usb_host_link_tr");
    #endif
 
    /* Check if the pipe id is valid */
    if (!pipe_ptr->open)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_link_tr Invalid pipe handle");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
    }
 
    usb_host_ptr = usb_host_dev_mng_get_host(pipe_ptr->dev_instance);
    USB_Host_lock();
    tr_list = pipe_ptr->tr_list_ptr;
    if (tr_list == NULL)
    {  
        /* No list exists, start one */
        pipe_ptr->tr_list_ptr = tr_ptr;
    }
    else
    {  
        /* scan for unused TR's, and for index already in list */
        //temp = tr_list;
        while (tr_list->next != NULL)
        {
            tr_list = tr_list->next;
        }
 
        /* now tr_list is the last one in the list */
        tr_list->next = tr_ptr;
    }
    USB_Host_unlock();
    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_unlink_tr
*  Returned Value :
*     USBERR_INVALID_PIPE_HANDLE if the pipe is invalid
*     USBERR_ALLOC if unable to get a new TR
*     USB_STATUS_TRANSFER_QUEUED upon success
*
*  Comments       :
*     Sets up a TR from init parameters.
*
*END*-----------------------------------------------------------------*/
usb_status  _usb_host_unlink_tr
   (
      /* [IN] the pipe handle */
      usb_pipe_handle           pipe_handle,

      /* [IN] the TR address on the pipe's TR list */
      tr_struct_t*              tr_ptr
   )
{
    pipe_struct_t* pipe_ptr;
    tr_struct_t*   tr_list;
    tr_struct_t*   pre;
    usb_host_handle usb_host_ptr;

    pipe_ptr = (pipe_struct_t*)pipe_handle;
    #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_unlink_tr");
    #endif

    /* Check if the pipe id is valid */
    if (!pipe_ptr->open)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_unlink_tr Invalid pipe handle");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
    } /* Endif */

    usb_host_ptr = usb_host_dev_mng_get_host(pipe_ptr->dev_instance);
    USB_Host_lock();

    tr_list = pipe_ptr->tr_list_ptr;
    if (tr_list == NULL)
    {  
        USB_Host_unlock();
        return USBERR_NOT_FOUND;
    }
    else
    {  
        pre = tr_list;
        while ((tr_list != tr_ptr) && tr_list)
        {
            pre = tr_list;
            tr_list = tr_list->next;   
        }
        if (!tr_list)
        {
            USB_Host_unlock();
            return USBERR_NOT_FOUND;
        }

        if (tr_list == pipe_ptr->tr_list_ptr)
        {
            /* it is the first one in the tr_list */
            pipe_ptr->tr_list_ptr = tr_list->next;
        }
        else
        {
            /* now tr_list is the last one in the list */
            pre->next = tr_list->next;
        }
    }
    USB_Host_unlock();
    return USB_OK;
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_max_packet_size_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_max_packet_size_call_interface is used to notify device
*  drivers of the maximum packet size for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
usb_status _usb_host_update_max_packet_size_call_interface
(
    /* [IN] the USB Host state structure */
    usb_host_handle     handle,
    /* [OUT] Pipe descriptor */
    struct pipe_struct   * pipe_ptr
)
{
    usb_status error = USB_OK;
    usb_host_api_functions_struct_t* host_api;
    usb_host_state_struct_t* usb_host_ptr = (usb_host_state_struct_t*)handle;

    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_update_max_packet_size != NULL)
    {
        error = host_api->host_update_max_packet_size(usb_host_ptr->controller_handle, pipe_ptr);
    }

    return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_device_address_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_device_address_call_interface is used to notify device
*  drivers of the new device address for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
usb_status _usb_host_update_device_address_call_interface
(
   /* [IN] the USB Host state structure */
   usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_struct   * pipe_ptr
)
{
    usb_status error = USB_OK;
    usb_host_api_functions_struct_t* host_api;
    usb_host_state_struct_t* usb_host_ptr = (usb_host_state_struct_t*)handle;

    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_update_device_address != NULL)
    {
        error = host_api->host_update_device_address(usb_host_ptr->controller_handle, pipe_ptr);
    }

    return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered.
* 
*END*--------------------------------------------------------------------*/
usb_status _usb_host_call_service
   (
      /* [IN] Handle to the USB device */
      usb_host_handle handle,

      /* [IN] Type of service or endpoint */
      uint8_t           type,
      
      /* [IN] Number of bytes in transmission */
      uint32_t          length
   )
{
    usb_host_state_struct_t*           usb_host_ptr;
    usb_host_service_struct_t*         service_ptr;
    uint32_t                           i;
    
    usb_host_ptr = (usb_host_state_struct_t*)handle;
    
    USB_Host_lock();

    for (i = 0; i < MAX_HOST_SERVICE_NUMBER; i++)
    {
        service_ptr = &usb_host_ptr->services[i];
        if (service_ptr->type == type)
        {
            USB_Host_unlock();
            if (service_ptr->service)
            {
                service_ptr->service(handle, length);             
            }
            return USB_OK;
        }
    }

    USB_Host_unlock();
    return USBERR_CLOSED_SERVICE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initializes the USB hardware and installs the USB 
*  interrupt handler
*END*-----------------------------------------------------------------*/

usb_status usb_host_init
   (
      /* [IN] the USB device controller to initialize */
      uint8_t controller_id,

      /* [OUT] the USB host handle */
      usb_host_handle *  handle
   )
{
    usb_host_api_functions_struct_t* host_api     = NULL;
    usb_host_state_struct_t*         usb_host_ptr = NULL;
    usb_status                       error        = USB_OK;
    uint32_t                         i;
#ifdef USBCFG_OTG
    usb_otg_state_struct_t *    usb_otg_struct_ptr = (usb_otg_state_struct_t *)g_usb_otg_handle;
    usb_otg_status_t *          otg_status_ptr     = &usb_otg_struct_ptr->otg_status;
#endif
    usb_host_ptr = _usb_host_get_handle();

    if (usb_host_ptr == NULL)
    {
        return USBERR_HOST_BUSY;
    }
   
    _usb_host_get_api(controller_id, &host_api);

    if (host_api == NULL)
    {
        _usb_host_release_handle(usb_host_ptr);
        return USBERR_ERROR;
    }

    usb_host_ptr->host_controller_api = (void*)host_api;
   
    /* Initialize the USB interface. */
    if (host_api->host_preinit != NULL)
    {
        error = host_api->host_preinit(usb_host_ptr, (usb_host_handle *)(&(usb_host_ptr->controller_handle)));
    }

    if (usb_host_ptr->controller_handle == NULL)
    {
        _usb_host_release_handle(usb_host_ptr);
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_init preinit failure");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
    }
 
    for (i = 0; i < MAX_HOST_SERVICE_NUMBER; i++)
    {
        usb_host_ptr->services[i].type = (uint32_t)-1;
    }
 
    usb_host_ptr->device_list_ptr = NULL;
    usb_host_ptr->mutex = OS_Mutex_create();
    if (usb_host_ptr->mutex == NULL)
    {
        if (host_api->host_shutdown != NULL)
        {
            error = host_api->host_shutdown(usb_host_ptr->controller_handle);
        }
        _usb_host_release_handle(usb_host_ptr);
        USB_PRINTF("host create mutex failed\n");
        return USBERR_ALLOC;
    }

    usb_host_ptr->hub_mutex = OS_Mutex_create();
    if (usb_host_ptr->hub_mutex == NULL)
    {
        if (host_api->host_shutdown != NULL)
        {
            error = host_api->host_shutdown(usb_host_ptr->controller_handle);
        }
        _usb_host_release_handle(usb_host_ptr);
        USB_PRINTF("host create mutex failed\n");
        return USBERR_ALLOC;
    }

    usb_host_ptr->hub_sem = OS_Sem_create(0);
    if (usb_host_ptr->hub_sem == NULL)
    {
        if (host_api->host_shutdown != NULL)
        {
            error = host_api->host_shutdown(usb_host_ptr->controller_handle);
        }
        _usb_host_release_handle(usb_host_ptr);
        USB_PRINTF("host create mutex failed\n");
        return USBERR_ALLOC;
    }

    usb_host_ptr->hub_link = NULL;
    usb_host_ptr->hub_task = 0xFFFFFFFF;

#ifndef USBCFG_OTG
    error = bsp_usb_host_init(controller_id);
#endif
    if (error != USB_OK)
    {
        if (host_api->host_shutdown != NULL)
        {
            error = host_api->host_shutdown(usb_host_ptr->controller_handle);
        }
        _usb_host_release_handle(usb_host_ptr);
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_init: BSP-specific USB initialization failure");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_UNKNOWN_ERROR);
    }

    if (host_api->host_init != NULL)
    {
        error = host_api->host_init (controller_id, usb_host_ptr->controller_handle);
    }
   
    if (error != USB_OK)
    {
        if (host_api->host_shutdown != NULL)
        {
            error = host_api->host_shutdown(usb_host_ptr->controller_handle);
        }
        _usb_host_release_handle(usb_host_ptr);
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_init returning error status");
        #endif
        return USB_log_error(__FILE__,__LINE__,error);
   }
#ifdef USBCFG_OTG
    usb_host_ptr->otg_handle = g_usb_otg_handle;
    otg_status_ptr->active_stack = USB_ACTIVE_STACK_HOST;
#endif
    *handle = (usb_host_handle)usb_host_ptr;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_init SUCCESSFULL");
    #endif
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_device_deinit
*  Returned Value : USB_OK or error code
*  Comments       :
*  uninitializes the USB device specific data structures and calls 
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_deinit
   (
      /* [OUT] the USB_USB_dev_initialize state structure */
      usb_host_handle  handle
   )
{
    usb_status                       status;
    usb_host_state_struct_t*         usb_host_ptr = (usb_host_state_struct_t*)handle;
    usb_host_api_functions_struct_t* host_api;
#ifdef USBCFG_OTG
    usb_otg_state_struct_t *   usb_otg_struct_ptr = (usb_otg_state_struct_t *)g_usb_otg_handle;
    usb_otg_status_t *         otg_status_ptr     = &usb_otg_struct_ptr->otg_status;
#endif
    if (usb_host_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_shutdown");
        #endif
        return USBERR_INVALID_PARAM;
    }

    /* De-initialize and disconnect the host hardware from the bus */
    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_shutdown != NULL)
    {
        status = host_api->host_shutdown (usb_host_ptr->controller_handle);
    }

    if (status != USB_OK)
    {
        #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("_usb_host_shutdown FAILED");
        #endif
    }

    if (usb_host_ptr->mutex != NULL)
    {
        OS_Mutex_destroy(usb_host_ptr->mutex);
    }

    if (usb_host_ptr->hub_mutex != NULL)
    {
        OS_Mutex_destroy(usb_host_ptr->hub_mutex);
    }

    if (usb_host_ptr->hub_sem != NULL)
    {
        OS_Sem_destroy(usb_host_ptr->hub_sem);
    }

    if ( (0xFFFFFFFF != usb_host_ptr->hub_task) && ((uint32_t)OS_TASK_ERROR != usb_host_ptr->hub_task) )
    {
        OS_Task_delete(usb_host_ptr->hub_task);
    }

    /* Free the USB state structure */
    _usb_host_release_handle(usb_host_ptr);
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_shutdown SUCCESSFUL");
    #endif
#ifdef USBCFG_OTG
    otg_status_ptr->active_stack     = USB_ACTIVE_STACK_NONE;
    usb_otg_struct_ptr->dev_inst_ptr = NULL;
#endif
    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_open_pipe
*  Returned Value : USB_OK or error
*  Comments       :
*  usb_host_open_pipe routine is used to open a pipe for device
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_open_pipe
(
   /* [IN] the USB Host state structure */
   usb_host_handle     handle,
   /* [OUT] Pipe descriptor */
   usb_pipe_handle *  pipe_handle_ptr,
   pipe_init_struct_t*    pipe_init_ptr
)
{
    usb_status error = USB_OK;
    usb_host_api_functions_struct_t* host_api;
    usb_host_state_struct_t* usb_host_ptr = (usb_host_state_struct_t*)handle;

    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_open_pipe != NULL)
    {
        error = host_api->host_open_pipe(usb_host_ptr->controller_handle, pipe_handle_ptr, pipe_init_ptr);
    }

    return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_close_pipe
*  Returned Value : USB_OK or error
*  Comments       :
*  usb_host_close_pipe routine is used to close a opened pipe
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_close_pipe
(
   /* [IN] the USB Host state structure */
   usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   usb_pipe_handle     pipe_handle
)
{
    usb_status error = USB_OK;
    usb_host_api_functions_struct_t* host_api;
    usb_host_state_struct_t* usb_host_ptr = (usb_host_state_struct_t*)handle;

    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_close_pipe != NULL)
    {
        error = host_api->host_close_pipe(usb_host_ptr->controller_handle, pipe_handle);
    }

    return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Send Data routine is non-blocking routine that causes a block of data 
* to be made available for transmission to the USB host.
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_send_data
   (
      /* [IN] the USB Host state structure */
      usb_host_handle           handle, 
      /* [IN] the pipe handle */
      usb_pipe_handle           pipe_handle,
      /* [IN] transfer parameters */
      tr_struct_t*              tr_ptr
   )
{
    pipe_struct_t*                      pipe_ptr;
    usb_host_state_struct_t*            usb_host_ptr;
    usb_host_api_functions_struct_t*    host_api;
    usb_status                          status;  
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("_usb_host_send_data");
    #endif
 
    usb_host_ptr = (usb_host_state_struct_t*)handle;
    if (usb_host_ptr == NULL)
    {
        USB_PRINTF("invalid handle in usb_host_send_data\n");
        return USBERR_INVALID_PARAM;
    }
    pipe_ptr = (pipe_struct_t*)pipe_handle;
    if ((pipe_ptr == NULL) || pipe_ptr->open != (uint8_t)TRUE)
    {
        return USBERR_NOT_FOUND;
    }
 
    status = usb_hostdev_validate (pipe_ptr->dev_instance);
    if (status != USB_OK)
    {
        return USBERR_NOT_FOUND;
    }
    
    status = _usb_host_link_tr(pipe_handle, tr_ptr);
    if (status != USB_OK)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_send_data failed to queue transfer");
        #endif
  
        return USB_log_error(__FILE__,__LINE__,status);
    } /* Endif */

    /* We have obtained the current TR on the Pipe's TR list 
    ** from _usb_host_set_up_tr
    */

    /* Call the low-level send routine */      
    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;
    if (host_api->host_send != NULL)
    {
#if (USBCFG_BUFF_PROPERTY_CACHEABLE) 
        if (tr_ptr->tx_length > 0)
        {
            /* We do flush the buffer first before used by DMA */
            OS_dcache_flush_mlines((void *)tr_ptr->tx_buffer, tr_ptr->tx_length);
        }
#endif
        status = host_api->host_send(usb_host_ptr->controller_handle, pipe_ptr, tr_ptr);
    }
    if (status != USB_OK)
    {
        /* mark the transaction as unused */
        _usb_host_unlink_tr(pipe_handle, tr_ptr);
    }
   
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_send_data SUCCESSFUL");
    #endif

    if(status != USB_OK)
    {  
        return USB_log_error(__FILE__,__LINE__,status);
    }
    return status;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_send_setup
*  Returned Value : None
*  Comments       :
*  Sends a Setup packet. Internally, following the SendSetup call
*  this takes care of the whole transaction (including receiving or sending 
*  any data to or from the device.
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_send_setup
   (
      /* [IN] the USB Host state structure   */
      usb_host_handle           handle, 

      /* [IN] the pipe handle */
      usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      tr_struct_t*              tr_ptr
   )
{
    pipe_struct_t*            pipe_ptr;
    usb_host_state_struct_t*  usb_host_ptr;
    usb_host_api_functions_struct_t* host_api;
    usb_status                 status;   
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_host_send_setup");
    #endif
 
    usb_host_ptr = (usb_host_state_struct_t*)handle;
    pipe_ptr = (pipe_struct_t*)pipe_handle;
 
    if ((pipe_ptr == NULL) || pipe_ptr->open != (uint8_t)TRUE)
    {
        return USBERR_NOT_FOUND;
    }
 
    status = usb_hostdev_validate (pipe_ptr->dev_instance);
    if (status != USB_OK)
    {
        return USBERR_NOT_FOUND;
    }
 
    /* Initialize a TR and link it into the pipe's TR list */
    status = _usb_host_link_tr(pipe_handle, tr_ptr);
    if (status != USB_OK)
    {
        #ifdef _HOST_DEBUG_
           DEBUG_LOG_TRACE("usb_host_send_setup error status");
        #endif
  
        return status;
    }
 
    pipe_ptr->nextdata01 = 0;       /* reset DATA0/1 */
    
    /* true if this setup packet will have a send data phase */
    tr_ptr->send_phase = (bool)!(tr_ptr->setup_packet.bmrequesttype & USB_SETUP_DATA_XFER_DIRECTION);

   /* Call the low-level routine to send a setup packet */
   host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

   if (host_api->host_send_setup != NULL)
   { 
#if (USBCFG_BUFF_PROPERTY_CACHEABLE)     
       /***************************************************************
           For data caching it is important that we update the memory
           with the intended contents.
           ***************************************************************/
       OS_dcache_flush_mlines((void *)&tr_ptr->setup_packet, sizeof(usb_setup_t));
       if (tr_ptr->tx_length > 0)
       {
           /* We do flush the buffer first before used by DMA */
           OS_dcache_flush_mlines((void *)tr_ptr->tx_buffer, tr_ptr->tx_length);
       }
       else if (tr_ptr->rx_length > 0)
       {
           /* To ensure that the USB DMA transfer will work on a buffer that is not cached,
               ** we invalidate buffer cache lines.
               */
           OS_dcache_invalidate_mlines((void *)tr_ptr->rx_buffer, tr_ptr->rx_length);
       }
#endif 
       status = host_api->host_send_setup(usb_host_ptr->controller_handle, pipe_ptr, tr_ptr);
   }
   if (status == USB_OK)
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("usb_host_send_setup SUCCESSFUL");
       #endif
       return USB_OK;
   }
   else
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("usb_host_send_setup FAILED");
       #endif
 
       _usb_host_unlink_tr(pipe_handle, tr_ptr);
       return USB_log_error(__FILE__,__LINE__,status);
   }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Receive Data routine is non-blocking routine that causes a buffer 
* to be made available for data recieved from the 
* USB host. It takes the buffer and passes it down to lower
level hardware driver. 
*END*-----------------------------------------------------------------*/
usb_status usb_host_recv_data
   (
      /* [IN] the USB Host state structure */
      usb_host_handle           handle, 

      /* [IN] the pipe handle */
      usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      tr_struct_t*              tr_ptr
   )
{ /* Body */
    pipe_struct_t*           pipe_ptr;
    usb_host_state_struct_t* usb_host_ptr;
    usb_host_api_functions_struct_t* host_api;
    usb_status                status;  

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("_usb_host_recv_data");
#endif

    usb_host_ptr = (usb_host_state_struct_t*)handle;
    pipe_ptr = (pipe_struct_t*)pipe_handle;
    if ((pipe_ptr == NULL) || pipe_ptr->open != (uint8_t)TRUE)
    {
        return USBERR_NOT_FOUND;
    }
    
    status = usb_hostdev_validate (pipe_ptr->dev_instance);
    if (status != USB_OK)
    {
        return USBERR_NOT_FOUND;
    }
    
    status = _usb_host_link_tr(pipe_handle, tr_ptr);
    if (status != USB_OK)
    {
        #ifdef _HOST_DEBUG_
           DEBUG_LOG_TRACE("_usb_host_recv_data transfer queue failed");
        #endif
        return USB_log_error(__FILE__,__LINE__,status);
    }
 
    /* We have obtained the current TR on the Pipe's TR list 
    ** from _usb_host_set_up_tr
    */

    /* Call the low-level routine to receive data */
    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_recv != NULL)
    {
#if (USBCFG_BUFF_PROPERTY_CACHEABLE) 
        if (tr_ptr->rx_length > 0)
        {
            /* To ensure that the USB DMA transfer will work on a buffer that is not cached,
                ** we invalidate buffer cache lines.
                */
            OS_dcache_invalidate_mlines((void *)tr_ptr->rx_buffer, tr_ptr->rx_length);
        }
#endif
        status = host_api->host_recv(usb_host_ptr->controller_handle, pipe_ptr, tr_ptr);
    }
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_recv_data SUCCESSFUL");
    #endif
 
    if (status == USB_OK) 
    {
        return USB_OK;
    }
    else
    {
        _usb_host_unlink_tr(pipe_handle, tr_ptr);
        return USB_log_error(__FILE__,__LINE__,status);
    }  
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_cancel routine is used to cancel pending
*  transactions.  Device drivers may update/free internal data structures
* and/or modify registers to cancel the transaction.
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_cancel
(
   /* [IN] the USB Host state structure */
   usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   usb_pipe_handle     pipe_handle,
   
   /* [IN] Pipe transfer descriptor */
   tr_struct_t*        tr_ptr
)
{
    usb_status error = USB_OK;
    usb_host_api_functions_struct_t* host_api;
    usb_host_state_struct_t* usb_host_ptr = (usb_host_state_struct_t*)handle;

    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;

    if (host_api->host_cancel != NULL)
    {
        error = host_api->host_cancel(usb_host_ptr->controller_handle, pipe_handle, tr_ptr);
    }

    return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control
*  Returned Value : error or status of bus control
*  Comments       :
*  This function is used to control the bus, like suspend, resume
*END*-----------------------------------------------------------------*/
usb_status usb_host_bus_control
   (
      /* [IN] the USB Host state structure */
      usb_host_handle             handle,
      
      /* The operation to be performed on the bus */
      uint8_t                     bcontrol
   )
{
    usb_status status = USB_OK;
    usb_host_state_struct_t*         usb_host_ptr;
    usb_host_api_functions_struct_t* host_api;
    
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_bus_control");
    #endif
 
    usb_host_ptr = (usb_host_state_struct_t*)handle;
    if (usb_host_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_shutdown");
        #endif
        return USBERR_ERROR;
    }
    host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;
    if (host_api->host_bus_control != NULL)
    {
        status = host_api->host_bus_control(usb_host_ptr->controller_handle, bcontrol);
    }
    
    if (status != USB_OK)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_bus_control FAILED");
        #endif
        return USBERR_ERROR;
    }
 
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_bus_control SUCCESSFUL");
    #endif
    return USB_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_dev_remove
*  Returned Value : error or status of device remove
*  Comments       :
*  This function is used to remove device when error happened.
*END*-----------------------------------------------------------------*/
usb_status usb_host_dev_remove
   (
      /* [IN] the USB Host state structure */
      usb_host_handle                 handle,
      
      /* [IN] USB device */
      usb_device_instance_handle      dev_handle
   )
{
    usb_status                      status = USB_OK;
    usb_host_state_struct_t*        usb_host_ptr;
    dev_instance_t*                 dev_inst_ptr;
    uint8_t                         level;
    usb_interface_descriptor_handle intf_handle;
    class_handle                    class_handle;
    uint8_t                         interface_index;
    
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_remove");
    #endif
 
    if ((NULL == dev_handle) || (NULL == handle))
    {
        return USBERR_INVALID_PARAM;
    }
    dev_inst_ptr = (dev_instance_t*)dev_handle;
    if (handle != dev_inst_ptr->host)
    {
        return USBERR_INVALID_PARAM;
    }
    
    usb_host_ptr = (usb_host_state_struct_t*)dev_inst_ptr->host;
    if (usb_host_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_shutdown");
        #endif
        return USBERR_INVALID_PARAM;
    }
    level = dev_inst_ptr->level;
    if(level == 1)
    {
        //usb_host_dev_mng_detach(usb_host_ptr, dev_inst_ptr->hub_no, dev_inst_ptr->port_no);
        //usb_host_bus_control(usb_host_ptr,2);
        usb_host_bus_control(usb_host_ptr,1);
    }
    else
    {
#if USBCFG_HOST_HUB        
        extern void usb_host_hub_Port_Reset(hub_device_struct_t* hub_instance,uint8_t port);
        usb_host_hub_Port_Reset(dev_inst_ptr->hub_instance, dev_inst_ptr->port_no);
        //usb_host_dev_mng_detach(dev_inst_ptr->host, dev_inst_ptr->hub_no, dev_inst_ptr->port_no);
#endif
    }
    //usb_host_dev_mng_detach(dev_inst_ptr->host, dev_inst_ptr->hub_no, dev_inst_ptr->port_no);
    
    usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no, dev_inst_ptr->port_no);
    /* search device list for the one being detached */
    USB_Host_lock();
    dev_inst_ptr = (dev_instance_t*)_usb_host_dev_get_instance(dev_inst_ptr->host, dev_inst_ptr->hub_no, dev_inst_ptr->port_no, (uint8_t)1);

    if (dev_inst_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_dev_remove NULL device pointer");
        #endif
        USB_Host_unlock();
        return  USBERR_ERROR; /* No match, abandon ship! */
    }
    
    dev_inst_ptr->attached = (uint8_t)FALSE;
    USB_Host_unlock();
    
    for (interface_index = 0; interface_index < dev_inst_ptr->configuration.interface_count; ++interface_index)
    {
        intf_handle = (usb_interface_descriptor_handle)dev_inst_ptr->interface_info[interface_index].lphostintf;
        class_handle = (void*)dev_inst_ptr->interface_info[interface_index].lpClassHandle;
        if (dev_inst_ptr->interface_info[interface_index].open == (uint8_t)TRUE)
        {
            status = usb_host_close_dev_interface(dev_inst_ptr->host, dev_handle, intf_handle, class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_host_dev_remove close interface FAILED\r\n");
            }
        }
    }
    
    OS_Mem_free(dev_inst_ptr);

    if(level == 1)
    {
        usb_host_bus_control(usb_host_ptr,2);
        //usb_host_bus_control(usb_host_ptr,1);
    }
    
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_remove SUCCESSFUL");
    #endif
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_open_interface
* Returned Value : interface struct + USB_OK, or error status
* Comments       :
*     Function to tear down old interface, and set up
*     a new one with the same or different index/alternate.
*     For each interface, only one of its alternates
*     can be selected per USB spec.  Also per spec, an endpoint
*     can be in only one interface (though it may exist in
*     several alternates of that interface).
*
*END*--------------------------------------------------------------------*/
usb_status  usb_host_open_dev_interface
   (
      /* [IN] the USB Host state structure */
      usb_host_handle                 handle,
      
      /* [IN] USB device */
      usb_device_instance_handle      dev_handle,

      /* [IN] Interface to be selected */
      usb_interface_descriptor_handle intf_handle,

      /* [OUT] Initialized class-specific interface struct */
      class_handle *                   class_handle_ptr
   )
{ 
    dev_instance_t*                     dev;
    usb_host_driver_info_t*             info_ptr;
    usb_host_state_struct_t*            host_struct_ptr;
    interface_descriptor_t*             intf;
    class_map_t*                        class_map;
    usb_status                           error;
    usb_device_interface_info_struct_t* lpinterface_info = NULL;
    class_handle                         class_handle = NULL;

    error = usb_hostdev_validate (dev_handle);
    if (error != USB_OK)
    {
       goto BadExit;
    }
   
    dev = (dev_instance_t*)dev_handle;
    if (handle != dev->host)
    {
        goto BadExit;
    }
    intf = ((usb_device_interface_struct_t*)intf_handle)->lpinterfaceDesc;
 
    /* Get the host handle on which this device is on */
    host_struct_ptr = (usb_host_state_struct_t*)dev->host;
 
    /* Get the driver info for attach callback when a match occurs */
    info_ptr = (usb_host_driver_info_t*)host_struct_ptr->device_info_table;
 
    if ((intf->bDescriptorType != USB_DESC_TYPE_IF) ||
        (intf->bLength != USB_DESC_LEN_IF))
    {
        error = USBERR_NO_INTERFACE;
        goto BadExit;
    }
 
    lpinterface_info = usb_host_dev_mng_get_interface_info(dev_handle, intf);
    if (lpinterface_info == NULL)
    {
        error = USBERR_NO_INTERFACE;
        goto BadExit;
    }
    lpinterface_info->open = (uint8_t)TRUE;

    if (usb_host_driver_info_nonzero(info_ptr))
    {
        /* Map interface to class driver, get & initialize driver struct */
        error = USBERR_NO_DEVICE_CLASS;  /* Assume the worst */
  
        class_map = usb_host_get_class_map(intf);
        if (class_map == NULL)
        {
            goto BadExit;
        }
        lpinterface_info->lpClassDriverMap = class_map;
        error = class_map->class_init(dev_handle, intf_handle, &class_handle);
        if (error != USB_OK)
        {
            lpinterface_info->lpClassHandle = class_handle;
            goto BadExit;
        }
  
        lpinterface_info->lpClassHandle = class_handle;
  
        *class_handle_ptr = class_handle;
    }
    else
    {
        goto BadExit;
    }

    if (intf->bAlternateSetting != 0)
    {
        /* Set interface on the device */
        dev->state = DEVSTATE_SET_INTF;
        lpinterface_info->requesting_set_interface = (uint8_t)TRUE;
        error = _usb_host_ch9_set_interface((usb_device_instance_handle)dev, 
                                             intf->bAlternateSetting,
                                             intf->bInterfaceNumber);   
        if (error != USB_OK)
        {
            dev->state = DEVSTATE_INITIAL;
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("_usb_hostdev_open_interface FAILED");
            #endif
            goto BadExit;
        }
        else
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("_usb_hostdev_open_interface SUCCESSFUL");
            #endif
            return USB_OK;
        }
    }
    else
    {
        /* Set interface on the device */
        dev->state = DEVSTATE_ENUM_OK;
        lpinterface_info->requesting_set_interface = (uint8_t)TRUE;
        usb_host_dev_notify(dev, USB_INTF_OPENED_EVENT);
        return USB_OK;
    }
 
 BadExit:
    if (error == USB_OK)
    {
        error = USBERR_NO_DEVICE_CLASS;
    }
	if (lpinterface_info != NULL)
		{
		    if (lpinterface_info->open)
		    {
		        lpinterface_info->open = (uint8_t)FALSE;
		    }
		 
		    class_map = lpinterface_info->lpClassDriverMap;
		    if ((class_map != NULL) && (lpinterface_info->lpClassHandle != NULL))
		    {
		        class_map->class_deinit(lpinterface_info->lpClassHandle);
		    }
		 
		    lpinterface_info->lpClassDriverMap = NULL;
		    lpinterface_info->lpClassHandle    = NULL;
		 
		    #ifdef _HOST_DEBUG_
		        DEBUG_LOG_TRACE("_usb_hostdev_open_interface bad exit");
		    #endif
		}
 
    return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_close_interface
* Returned Value : interface struct + USB_OK, or error status
* Comments       :
*     Function to tear down old interface, and set up
*     a new one with the same or different index/alternate.
*     For each interface, only one of its alternates
*     can be selected per USB spec.  Also per spec, an endpoint
*     can be in only one interface (though it may exist in
*     several alternates of that interface).
*
*END*--------------------------------------------------------------------*/
usb_status  usb_host_close_dev_interface
   (
      /* [IN] the USB Host state structure */
      usb_host_handle                 handle,
      
      /* [IN] USB device */
      usb_device_instance_handle      dev_handle,

      /* [IN] Interface to be selected */
      usb_interface_descriptor_handle intf_handle,

      /* [IN] Initialized class-specific interface struct */
      class_handle                     class_handle
   )
{
    dev_instance_t*                      dev_instance_ptr;
    //usb_host_state_struct_t*            host_struct_ptr;
    interface_descriptor_t*              intf;
    class_map_t*                         class_map;
    usb_device_interface_info_struct_t*  lpinterface_info = NULL;
    uint8_t                              interface_index;
    uint8_t                              interface_open = (uint8_t)FALSE;
 
    if ((NULL == dev_handle) || (NULL == intf_handle) || (NULL == handle))
    {
        return USBERR_INVALID_PARAM;
    }
    dev_instance_ptr = (dev_instance_t*)dev_handle;
    if (handle != dev_instance_ptr->host)
    {
        return USBERR_INVALID_PARAM;
    }
    //host_struct_ptr = (usb_host_state_struct_t*)dev_instance_ptr->host;
    intf = ((usb_device_interface_struct_t*)intf_handle)->lpinterfaceDesc;
 
    lpinterface_info = usb_host_dev_mng_get_interface_info(dev_handle, intf);
    if ((lpinterface_info == NULL) || (lpinterface_info->lpClassHandle != class_handle))
    {
        return USBERR_NO_INTERFACE;
    }

    class_map = lpinterface_info->lpClassDriverMap;
    if ((class_map != NULL) && (lpinterface_info->lpClassHandle != NULL))
    {
        if (class_map->class_deinit != NULL)
        {
            class_map->class_deinit(lpinterface_info->lpClassHandle);
        }
    }
 
    lpinterface_info->lpClassDriverMap = NULL;
    lpinterface_info->lpClassHandle    = NULL;
    lpinterface_info->open             = (uint8_t)FALSE;
 
    if (dev_instance_ptr->attached == (uint8_t)TRUE)
    {
        return USB_OK;
    }
 
    for (interface_index = 0; interface_index < dev_instance_ptr->configuration.interface_count; interface_index++)
    {
        intf = dev_instance_ptr->configuration.interface[interface_index].lpinterfaceDesc;
        lpinterface_info = usb_host_dev_mng_get_interface_info(dev_instance_ptr, intf);
        if ((lpinterface_info != NULL) && (lpinterface_info->open == (uint8_t)TRUE))
        {
            interface_open = (uint8_t)TRUE;
            break;
        }
        else
        {
            //USB_PRINTF("ERROR in _usb_hostdev_close_interface, no valid lpinterface_info\n");
            continue;
        }
    }
 
    if (interface_open == (uint8_t)FALSE)
    {
    	if(dev_instance_ptr->control_pipe != NULL)
    	{
			usb_host_close_pipe(dev_instance_ptr->host, dev_instance_ptr->control_pipe);
			dev_instance_ptr->control_callback = NULL; /* no surprises */
			
			if (dev_instance_ptr->lpConfiguration != NULL)
			{
				OS_Mem_free(dev_instance_ptr->lpConfiguration);
			}
    	}
        OS_Mem_free(dev_instance_ptr);
    }
    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event.
* 
*END*--------------------------------------------------------------------*/
usb_status usb_host_register_service
   (
      /* [IN] Handle to the USB device */
      usb_host_handle         handle,
      
      /* [IN] type of event or endpoint number to service */
      uint8_t                 type,
      
      /* [IN] Pointer to the service's callback function */
      void(_CODE_PTR_ service)(void*, uint32_t)
   )
{
    usb_host_state_struct_t*           usb_host_ptr;
    usb_host_service_struct_t*         service_ptr;
    uint32_t                           i;
 
    if (handle == NULL)
    {
         return USBERR_ERROR;
    }
    usb_host_ptr = (usb_host_state_struct_t*)handle;
 
    USB_Host_lock();

    for (i = 0; i < MAX_HOST_SERVICE_NUMBER; i++)
    {
        service_ptr = &usb_host_ptr->services[i];
        if (service_ptr->type == type)
        {
            USB_Host_unlock();
            return USBERR_OPEN_SERVICE;
        }
    }

    for (i = 0; i < MAX_HOST_SERVICE_NUMBER; i++)
    {
        service_ptr = &usb_host_ptr->services[i];
        if (service_ptr->type == (uint32_t)-1)
        {
            service_ptr->type = type;
            service_ptr->service = service;
            USB_Host_unlock();
            return USB_OK;
        }
    }

    USB_Host_unlock();
    return USBERR_ALLOC;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
* 
*END*--------------------------------------------------------------------*/
usb_status usb_host_unregister_service
   (
      /* [IN] Handle to the USB device */
      usb_host_handle           handle,

      /* [IN] type of event or endpoint number to service */
      uint8_t                   type
   )
{
    usb_host_state_struct_t*           usb_host_ptr;
    usb_host_service_struct_t*         service_ptr;
    uint32_t                           i;

    if (handle == NULL)
    {
       return USBERR_ERROR;
    }

    usb_host_ptr = (usb_host_state_struct_t*)handle;

    USB_Host_lock();

    for (i = 0; i < MAX_HOST_SERVICE_NUMBER; i++)
    {
        service_ptr = &usb_host_ptr->services[i];
        if (service_ptr->type == type)
        {
            service_ptr->type    = (uint32_t)-1;
            service_ptr->service = NULL;
            USB_Host_unlock();
            return USB_OK;
        }
    }

    USB_Host_unlock();
    return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_register_ch9_callback
* Returned Value : USB_OK, or error status
* Comments       :
*     This function registers a callback function that will be called 
*  to notify the user of a ch9 command completion. This should be used 
*  only after enumeration is completed
* 
*END*--------------------------------------------------------------------*/
usb_status  usb_host_register_ch9_callback
   (
      /* [IN] Handle to the USB device */
      usb_host_handle              handle,
      
      /* usb device */
      usb_device_instance_handle   dev_handle,

      /* Callback upon completion */
      tr_callback                   callback,

      /* User provided callback param */
      void*                       callback_param
   )
{
    dev_instance_t*           dev_inst_ptr;
    usb_status                 error;
    usb_host_state_struct_t*  usb_host_ptr = (usb_host_state_struct_t*)handle;
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("_usb_host_register_ch9_callback");
    #endif
    if (usb_host_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    /* Verify that device handle is valid */

    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK)
    {
        #ifdef _HOST_DEBUG_
           DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, device not found");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
    }
    
    USB_Host_lock();
    dev_inst_ptr = (dev_instance_t*)dev_handle;
 
    /* This will be called if the device is already enumerated */
    dev_inst_ptr->control_callback = callback;
    dev_inst_ptr->control_callback_param = callback_param;
 
    USB_Host_unlock();
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, SUCCESSFUL");
    #endif
    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_unregister_ch9_callback
* Returned Value : USB_OK, or error status
* Comments       :
*     This function registers a callback function that will be called 
*  to notify the user of a ch9 command completion. This should be used 
*  only after enumeration is completed
* 
*END*--------------------------------------------------------------------*/
usb_status  usb_host_unregister_ch9_callback
   (
      /* [IN] Handle to the USB device */
      usb_host_handle              handle,
      /* usb device */
      usb_device_instance_handle   dev_handle
   )
{
    dev_instance_t*           dev_inst_ptr;
    usb_status                 error;
    usb_host_state_struct_t*  usb_host_ptr = (usb_host_state_struct_t*)handle;
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_register_ch9_callback");
    #endif
    if (usb_host_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    /* Verify that device handle is valid */
    USB_Host_lock();

    error = usb_hostdev_validate(dev_handle);
 
    if (error != USB_OK)
    {
        USB_Host_unlock();
        #ifdef _HOST_DEBUG_
           DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, device not found");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
    } /* Endif */
      
    dev_inst_ptr = (dev_instance_t*)dev_handle;
 
    /* This will be called if the device is already enumerated */
    dev_inst_ptr->control_callback = NULL;
    dev_inst_ptr->control_callback_param = NULL;
 
    USB_Host_unlock();
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, SUCCESSFUL");
    #endif
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_register_unsupported_device_notify
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to register user's desired-interfaces array of structs.
*
*END*--------------------------------------------------------------------*/
usb_status  usb_host_register_unsupported_device_notify
   (
      /* usb host */
      usb_host_handle     host_handle,

      /* Device info table */
      event_callback       unsupported_device_notify
   )
{
    usb_host_state_struct_t* usb_host_ptr;
    
    #ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("_usb_host_driver_info_register");
    #endif
 
    if (host_handle == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_driver_info_register, NULL pointers");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_UNKNOWN_ERROR);
    } /* Endif */
 
    usb_host_ptr = (usb_host_state_struct_t*)host_handle;
    USB_Host_lock();
    usb_host_ptr->unsupport_device_callback = unsupported_device_notify;
    USB_Host_unlock();
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_driver_info_register SUCCESSFUL");
    #endif
    return USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_register_driver_info
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to register user's desired-interfaces array of structs.
*
*END*--------------------------------------------------------------------*/
usb_status  usb_host_register_driver_info
   (
      /* usb host */
      usb_host_handle     host_handle,
      /* Device info table */
      void*              info_table_ptr
   )
{
    usb_host_state_struct_t* usb_host_ptr;
    
    #ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("_usb_host_driver_info_register");
    #endif
 
    if (host_handle == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_driver_info_register, NULL pointers");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_UNKNOWN_ERROR);
    } /* Endif */
 
    usb_host_ptr = (usb_host_state_struct_t*)host_handle;
    USB_Host_lock();
    usb_host_ptr->device_info_table = info_table_ptr;
    USB_Host_unlock();
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_driver_info_register SUCCESSFUL");
    #endif
    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_get_tr
*  Returned Value : Status
*  Comments       :
* usb_host_get_tr is used to get a valid tr. 
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_get_tr
   (
      /* [IN] the USB Host state structure */
      usb_host_handle              handle,
       /* [IN] callback to call after completion */
      tr_callback                callback,
      /* [IN] the param to pass back to the callback function */
      void*                    callback_param,
      /* [OUT] tr ptr */
      tr_struct_t* *               tr_ptr_ptr
   )
{ /* Body */
   usb_host_state_struct_t*             usb_host_ptr;
   uint8_t                              tr_index;
   //static uint32_t                     usage_number = 0;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_unregister_service");
   #endif

    usb_host_ptr = (usb_host_state_struct_t*)handle;

    USB_Host_lock();

    usb_host_ptr->tr_index++;
    if (usb_host_ptr->tr_index == 0)
    {
        usb_host_ptr->tr_index++;
    }

    for (tr_index = 0; tr_index < MAX_TR_NUMBER; tr_index++)
    {
        if (!usb_host_ptr->tr_list[tr_index].occupied)
        {
            usb_host_ptr->tr_list[tr_index].occupied = 1;  
            usb_host_ptr->tr_list[tr_index].next = NULL;
            usb_host_ptr->tr_list[tr_index].status = USB_STATUS_TRANSFER_QUEUED;
            usb_host_ptr->tr_list[tr_index].tr_index = usb_host_ptr->tr_index;
            usb_host_ptr->tr_list[tr_index].tx_buffer = NULL;
            usb_host_ptr->tr_list[tr_index].rx_buffer = NULL;
            usb_host_ptr->tr_list[tr_index].tx_length = 0;
            usb_host_ptr->tr_list[tr_index].rx_length = 0;
            usb_host_ptr->tr_list[tr_index].callback = callback;
            usb_host_ptr->tr_list[tr_index].callback_param = callback_param;
            usb_host_ptr->tr_list[tr_index].send_phase = FALSE;
            *tr_ptr_ptr = &usb_host_ptr->tr_list[tr_index];
            usb_host_ptr->tr_user++;
            //USB_PRINTF("get tr %d\n", usb_host_ptr->tr_user);
            USB_Host_unlock();
            return USB_OK;
        }
    }
    *tr_ptr_ptr = NULL;
    USB_Host_unlock();
    return USBERR_NOT_FOUND;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_release_tr
*  Returned Value : Status
*  Comments       :
* usb_host_release_tr is used to release a tr. 
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_release_tr
   (
      /* [IN] the USB Host state structure */
      usb_host_handle              handle,
      
      /* [OUT] tr ptr */
      tr_struct_t*                 tr_ptr
   )
{
    usb_host_state_struct_t*           usb_host_ptr;
    uint8_t                            tr_index;

    usb_host_ptr = (usb_host_state_struct_t*)handle;

    USB_Host_lock();

    if ((tr_ptr == NULL) || (tr_ptr->occupied == 0))
    {
        USB_Host_unlock();
        return USBERR_NOT_FOUND;
    }

    for (tr_index = 0; tr_index < MAX_TR_NUMBER; tr_index++)
    {
        if (&usb_host_ptr->tr_list[tr_index] == tr_ptr)
        {
            usb_host_ptr->tr_list[tr_index].occupied = 0;  
            usb_host_ptr->tr_list[tr_index].next = NULL;
            usb_host_ptr->tr_list[tr_index].status = USB_STATUS_IDLE;
            usb_host_ptr->tr_list[tr_index].tr_index = 0;
            usb_host_ptr->tr_list[tr_index].tx_buffer = NULL;
            usb_host_ptr->tr_list[tr_index].rx_buffer = NULL;
            usb_host_ptr->tr_list[tr_index].tx_length = 0;
            usb_host_ptr->tr_list[tr_index].rx_length = 0;
            usb_host_ptr->tr_list[tr_index].callback = NULL;
            usb_host_ptr->tr_list[tr_index].callback_param = NULL;
            usb_host_ptr->tr_list[tr_index].send_phase = FALSE;
            usb_host_ptr->tr_user--;
            //USB_PRINTF("release tr %d\n", usb_host_ptr->tr_user);
            //USB_PRINTF("release tr %d\n", release_number++);
            USB_Host_unlock();
            return USB_OK;
        }
    }
    USB_Host_unlock();
    return USBERR_NOT_FOUND;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_host_get_host_handle
*  Returned Value : Status
*  Comments       :
* usb_host_get_host is used to get the host handle of the device. 
*
*END*-----------------------------------------------------------------*/
usb_status usb_host_get_host_handle
   (
      /*[IN] usb device */
      usb_device_instance_handle   dev_handle,
      
      /*[OUT] host handle */
      usb_host_handle *            handle
   )
{
    dev_instance_t*           dev_inst_ptr;
    usb_status                error;

    dev_inst_ptr = (dev_instance_t*)dev_handle;

    if (dev_inst_ptr == NULL)
    {
        return USBERR_INVALID_PARAM;
    }

    error = usb_hostdev_validate(dev_handle);
 
    if (error != USB_OK)
    {
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
    } /* Endif */
    
    *handle = dev_inst_ptr->host;
    
    return USB_OK;
}

#endif

