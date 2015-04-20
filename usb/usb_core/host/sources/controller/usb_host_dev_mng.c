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
* $FileName: usb_host_dev_mng.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains the device-tree management for USB class drivers
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
#include "usb_otg_dev_api.h"
#endif
usb_status USB_log_error(char* file, uint32_t line, usb_status error);
bool  usb_host_driver_info_match(dev_instance_t*, interface_descriptor_t*, usb_host_driver_info_t*);
void* _usb_host_dev_get_instance(usb_host_handle, uint8_t, uint8_t, uint8_t);
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_address
* Returned Value : device address
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_address(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->address : 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_hubno
* Returned Value : device hub number
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_hubno(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->hub_no : 0xFF;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_portno
* Returned Value : device port number
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_portno(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->port_no : 0xFF;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_speed
* Returned Value : device speed
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_speed(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->speed : 0xFF;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_hub_speed
* Returned Value : device hub speed
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_hub_speed(usb_device_instance_handle dev_handle)
{
      if ((dev_instance_t*)dev_handle == NULL)
         return 0;
      else 
      {
          if (((dev_instance_t*)dev_handle)->hub_instance != NULL)
            return ((dev_instance_t*)dev_handle)->hub_instance->speed;
          else
            return 0;
      }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_attach_state
* Returned Value : device's state
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_attach_state(usb_device_instance_handle dev_handle)
{
     return dev_handle ? ((dev_instance_t*)dev_handle)->attached : 0x0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_host
* Returned Value : device's host pointer
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
usb_host_handle usb_host_dev_mng_get_host(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->host : NULL;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_control_pipe
* Returned Value : device's control pipe pointer
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
usb_pipe_handle usb_host_dev_mng_get_control_pipe(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->control_pipe : NULL;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_level
* Returned Value : device's control pipe pointer
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_level(usb_device_instance_handle dev_handle)
{
    return dev_handle ? ((dev_instance_t*)dev_handle)->level : 0xFF;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_hs_hub_no
* Returned Value : device's high speed hub port no
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_hs_hub_no(usb_device_instance_handle dev_handle)
{

      if ((dev_instance_t*)dev_handle == NULL)
         return 0;
      else 
      {
        if (((dev_instance_t*)dev_handle)->hub_instance != NULL)
          return ((dev_instance_t*)dev_handle)->hub_instance->hs_hub_no;
        else
          return 0;
      }
        
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_hs_port_no
* Returned Value : device's high speed hub port no
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
uint8_t usb_host_dev_mng_get_hs_port_no(usb_device_instance_handle dev_handle)
{
      if ((dev_instance_t*)dev_handle == NULL)
         return 0;
      else 
      {
        if (((dev_instance_t*)dev_handle)->hub_instance != NULL)
         return ((dev_instance_t*)dev_handle)->hub_instance->hs_port_no;
        else
         return 0;
      }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_pid
* Returned Value : device's control pipe pointer
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
uint16_t usb_host_dev_mng_get_pid(usb_device_instance_handle dev_handle)
{
    return (uint16_t)((uint16_t)(((dev_instance_t*)dev_handle)->dev_descriptor.idProduct[1] << 8)
    		| (uint16_t)(((dev_instance_t*)dev_handle)->dev_descriptor.idProduct[0]));
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_vid
* Returned Value : device's control pipe pointer
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
uint16_t usb_host_dev_mng_get_vid(usb_device_instance_handle dev_handle)
{
    return (uint16_t)((uint16_t)(((dev_instance_t*)dev_handle)->dev_descriptor.idVendor[1] << 8)
            | (uint16_t)(((dev_instance_t*)dev_handle)->dev_descriptor.idVendor[0]));
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_get_interface_info
* Returned Value : the matched interface info
* Comments       :
*     The function is used to get the matched interface info for the given inteferface 
*     descriptor
*
*END*--------------------------------------------------------------------*/
usb_device_interface_info_struct_t* usb_host_dev_mng_get_interface_info(usb_device_instance_handle dev_handle, interface_descriptor_t* intf)
{
    dev_instance_t*                     dev_ptr = (dev_instance_t*)dev_handle;
    usb_device_interface_info_struct_t* lpinterface_info = NULL;
    uint8_t                              intf_no;
    //usb_host_state_struct_t*            usb_host_ptr = (usb_host_state_struct_t*)dev_ptr->host;
    //USB_Host_lock();
    /* Check that there is an available interface pointer */
    for (intf_no = 0; intf_no < dev_ptr->num_of_interfaces;intf_no++)
    {
        lpinterface_info = &dev_ptr->interface_info[intf_no];
        if ((lpinterface_info->lphostintf != NULL) && (intf == lpinterface_info->lphostintf->lpinterfaceDesc))
        {
            //USB_Host_unlock();
            return lpinterface_info;
        }
    }
    //USB_Host_unlock();
    return NULL;
}


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_dev_mng_attach
* Returned Value : 
* Comments       :
*     This function will be called when attach interrupt happens, to
*       add onto the device list and do common initialization.     
* 
*END*--------------------------------------------------------------------*/
usb_status usb_host_dev_mng_attach
   (
      usb_host_handle  handle,
      hub_device_struct_t* hub_instance,
      uint8_t            speed,
      uint8_t            hub_no,
      uint8_t            port_no,
      uint8_t           level,
      usb_device_instance_handle* handle_ptr 
   )
{
    usb_status                  status;
    dev_instance_t*             new_instance_ptr;
    dev_instance_t*             dev_instance_ptr;
    dev_instance_t*             dev_instance_prev_ptr;
    usb_host_state_struct_t*    usb_host_ptr;
    //dev_instance_t*           device_root = NULL;
    pipe_init_struct_t          pipe_init;

    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_dev_list_attach_device attach device");
    #endif
    usb_host_ptr = (usb_host_state_struct_t*)handle;
    //USB_PRINTF("a %d %d %d\n", hub_no, port_no, level);
 
    /* Allocate new device instance */
    new_instance_ptr = (dev_instance_t*) OS_Mem_alloc_uncached_zero(sizeof(dev_instance_t));
    if (new_instance_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
           DEBUG_LOG_TRACE("usb_dev_list_attach_device failed to malloc device handle");
        #endif
        USB_PRINTF("memory allocation failed in usb_host_dev_mng_attach\n");
        return USB_log_error(__FILE__,__LINE__, USBERR_GET_MEMORY_FAILED);
    } /* EndIf */
 
    new_instance_ptr->host = handle;
    new_instance_ptr->speed = speed;
	new_instance_ptr->hub_instance = hub_instance;
    new_instance_ptr->hub_no = hub_no;
    new_instance_ptr->port_no = port_no;
    new_instance_ptr->level = level;
    new_instance_ptr->cfg_value = 0; /* We don't know yet what the device's default configuration is */
    new_instance_ptr->attached = (uint8_t)TRUE;
    new_instance_ptr->pre_detached = (uint8_t)FALSE;
    new_instance_ptr->to_be_detached = (uint8_t)FALSE;
    new_instance_ptr->stall_retries = USBCFG_HOST_CTRL_FAILED_RETRY;
    new_instance_ptr->ctrl_retries = USBCFG_HOST_CTRL_STALL_RETRY;

    //USB_PRINTF("l1\n");
    USB_Host_lock();
    //USB_PRINTF("l2\n");
    dev_instance_ptr = usb_host_ptr->device_list_ptr;
    while (dev_instance_ptr != NULL)
    {
        if ((dev_instance_ptr->hub_no == hub_no) &&
            (dev_instance_ptr->port_no == port_no))
        {
            USB_Host_unlock();
            OS_Mem_free((void*)new_instance_ptr);
            USB_PRINTF("invalidate attach\n");
            *handle_ptr = NULL;
            return USBERR_ERROR;
        }
        else
        {
            dev_instance_ptr = dev_instance_ptr->next;
        }
    }
    //USB_PRINTF("l3\n");
    /* Find unused address from 1 - 127 for this host */
    dev_instance_ptr = usb_host_ptr->device_list_ptr;
    if ((dev_instance_ptr == NULL) || (dev_instance_ptr->address != 1))
    {
        /* Insert at the beginning of list */
        new_instance_ptr->target_address = 1;
        new_instance_ptr->next = dev_instance_ptr;
        usb_host_ptr->device_list_ptr = new_instance_ptr;
    }
    else
    {
        dev_instance_prev_ptr = dev_instance_ptr;
        /* Searching for a 'hole' in devices instance adresses */
        while (dev_instance_ptr->target_address <= (dev_instance_prev_ptr->target_address + 1))
        {
            new_instance_ptr->target_address = dev_instance_ptr->target_address;
            dev_instance_prev_ptr = dev_instance_ptr;
            dev_instance_ptr = dev_instance_ptr->next;
            if (dev_instance_ptr == NULL)
                break;
        } /* EndWhile */
        if (new_instance_ptr->target_address >= 127)
        {
            /* If all 127 addresses used up, delete instance & bail out */
            USB_Host_unlock();
            OS_Mem_free((void*)new_instance_ptr);
            #ifdef _HOST_DEBUG_
               DEBUG_LOG_TRACE("usb_dev_list_attach_device out of addresses");
            #endif
            USB_PRINTF("no valid address for the device\n");
            *handle_ptr = NULL;
            return USB_log_error(__FILE__,__LINE__, USBERR_ADDRESS_ALLOC_FAILED);
        } /* EndIf */
        new_instance_ptr->target_address++;
        new_instance_ptr->next = dev_instance_ptr;
        dev_instance_prev_ptr->next = new_instance_ptr;
    };
    //USB_PRINTF("l4\n");   
    USB_Host_unlock();
 
    /*-----------------------------------------------------------**
        ** Open control pipe, get first 8 bytes of device descriptor **
        ** The host_ch9 routine internally sets the callback to      **
        **    usb_host_cntrl_transaction_done (in usb_host_ch9.c)    **
        **    where the action resumes on completion of the get.     **
        **-----------------------------------------------------------*/
    pipe_init.endpoint_number  = 0;
    pipe_init.direction        = 0;
    pipe_init.pipetype         = USB_CONTROL_PIPE;
    pipe_init.max_packet_size  = 64;
    pipe_init.interval         = 0;
    pipe_init.flags            = 0;
    pipe_init.dev_instance     = new_instance_ptr;
    pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
 
    if (USB_OK != usb_host_open_pipe(new_instance_ptr->host, &new_instance_ptr->control_pipe, &pipe_init))
    {
        OS_Mem_free((void*)new_instance_ptr);
        *handle_ptr = NULL;
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_dev_list_attach_device open pipe failed");
        #endif
        USB_PRINTF("can't open control pipe\n");
        return USB_log_error(__FILE__,__LINE__, USBERR_PIPE_OPENED_FAILED);
    } /* Endif */
 
    /* Set state to enter after transaction completion */
    new_instance_ptr->state = DEVSTATE_DEVDESC8;
 
    status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)new_instance_ptr,
                                          USB_DESC_TYPE_DEV << 8, 0, 8, 
                                          (uint8_t *)&new_instance_ptr->dev_descriptor);
       
    if (status != USB_OK)
    {
        new_instance_ptr->state = DEVSTATE_INITIAL;
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_dev_list_attach_device FAILED");
        #endif
        USB_PRINTF("get descriptor error\n");
        *handle_ptr = (usb_device_instance_handle)new_instance_ptr;
        return USB_log_error(__FILE__,__LINE__, USBERR_NO_DESCRIPTOR);
    }
    
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_dev_list_attach_device SUCCESSFUL");
    #endif
    *handle_ptr = (usb_device_instance_handle)new_instance_ptr;
    //USB_PRINTF("attach done\n");
    return USB_OK;
} /* EndBody */
 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_dev_mng_pre_detach
* Returned Value : 
* Comments       :
*     This function will be called when detach interrupt happens.
* 
*END*--------------------------------------------------------------------*/
usb_status  usb_host_dev_mng_pre_detach
(
    usb_host_handle    handle,
    uint8_t            hub_no,
    uint8_t            port_no
)
{ /* Body */
    usb_host_state_struct_t*            usb_host_ptr = (usb_host_state_struct_t*)handle;
    dev_instance_t*                     dev_instance_ptr;
    uint8_t                              interface_index;
    interface_descriptor_t*             intf = NULL;
    usb_device_interface_info_struct_t* lpinterface_info = NULL;
    class_map_t*                        class_map;

#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_mng_pre_detach");
#endif

    /* search device list for the one being detached */
    USB_Host_lock();
    dev_instance_ptr = (dev_instance_t*)_usb_host_dev_get_instance(handle, hub_no, port_no, (uint8_t)0);
    
    if (dev_instance_ptr == NULL)
    {
    #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_dev_mng_pre_detach NULL device pointer");
    #endif
        USB_Host_unlock();
        return  USBERR_ERROR; /* No match, abandon ship! */
    }
    dev_instance_ptr->to_be_detached = (uint8_t)TRUE;
    
    if (dev_instance_ptr->pre_detached == (uint8_t)TRUE)
    {
        USB_Host_unlock();
        return  USB_OK;
    }
    dev_instance_ptr->pre_detached = (uint8_t)TRUE;

    if (NULL != dev_instance_ptr->control_pipe)
    {
        USB_Host_unlock();
        /* Close control pipe */
        usb_host_cancel(handle, (pipe_struct_t*)dev_instance_ptr->control_pipe, (tr_struct_t*)NULL);
        USB_Host_lock();
    }

    for (interface_index = 0; interface_index < dev_instance_ptr->configuration.interface_count; interface_index++)
    {
        intf = dev_instance_ptr->configuration.interface[interface_index].lpinterfaceDesc;
        lpinterface_info = usb_host_dev_mng_get_interface_info(dev_instance_ptr, intf);
        if (lpinterface_info == NULL)
        {
            continue;
        }

        class_map = lpinterface_info->lpClassDriverMap;
        if (class_map != NULL)
        {
            USB_Host_unlock();
            class_map->class_pre_deinit(lpinterface_info->lpClassHandle);
            USB_Host_lock();
        }
    }

    USB_Host_unlock();
    if (dev_instance_ptr->state <= DEVSTATE_SET_INTF)
    {
        if (dev_instance_ptr->control_pipe != NULL)
        {
            usb_host_close_pipe(dev_instance_ptr->host, dev_instance_ptr->control_pipe);
            dev_instance_ptr->control_pipe = NULL;
        }
        if (dev_instance_ptr->lpConfiguration != NULL)
        {
            OS_Mem_free(dev_instance_ptr->lpConfiguration);
            dev_instance_ptr->lpConfiguration = NULL;
        }
    }
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_mng_pre_detach SUCCESSFUL");
#endif

    return USB_OK;
} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_dev_mng_detach
* Returned Value : 
* Comments       :
*     This function will be called when detach interrupt happens.
* 
*END*--------------------------------------------------------------------*/
usb_status  usb_host_dev_mng_detach
(
    usb_host_handle    handle,
    uint8_t            hub_no,
    uint8_t            port_no
)
{ /* Body */
    usb_host_state_struct_t*            usb_host_ptr = (usb_host_state_struct_t*)handle;
    dev_instance_t*                     dev_instance_ptr;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_mng_detach");
    #endif

    usb_host_dev_mng_pre_detach(handle, hub_no, port_no);
    /* search device list for the one being detached */
    USB_Host_lock();
    dev_instance_ptr = (dev_instance_t*)_usb_host_dev_get_instance(handle, hub_no, port_no, (uint8_t)1);

    if (dev_instance_ptr == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_dev_mng_detach NULL device pointer");
        #endif
        USB_Host_unlock();
        return  USBERR_ERROR; /* No match, abandon ship! */
    }
    
    dev_instance_ptr->attached = (uint8_t)FALSE;
    USB_Host_unlock();
    
    if (dev_instance_ptr->state > DEVSTATE_SET_CFG)
    {
        /* Notify the application of unavailable interfaces */
        usb_host_dev_notify(dev_instance_ptr,USB_DETACH_EVENT);     
#ifdef USBCFG_OTG
        usb_otg_host_on_detach_event(usb_host_ptr->otg_handle);
#endif 
    }
    else
    {
        OS_Mem_free(dev_instance_ptr);
    }
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_mng_detach SUCCESSFUL");
    #endif

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_check_configuration
* Returned Value : if the currect configuration can match the user registered info
* Comments       :
*     Scan device config for interfaces that may need callbacks
*     for matching classes etc. from each interface. This function is
*     called when there configuration change or attach or detach event
*
*END*--------------------------------------------------------------------*/
bool  usb_host_dev_mng_check_configuration
(
    /* [IN] USB device */
    usb_device_instance_handle dev_handle
)
{ 
    uint8_t                             interface_index;
    dev_instance_t*                     dev_ptr = (dev_instance_t*)dev_handle;
    usb_device_configuration_struct_t*  pConfiguration = &dev_ptr->configuration;
    usb_configuration_descriptor_t*     pConfigurationDesc = pConfiguration->lpconfigurationDesc;
    usb_device_interface_struct_t*      pInterface = NULL;
    usb_host_driver_info_t*             info_ptr;
    usb_host_state_struct_t*            usb_host_ptr;
    bool                                match = FALSE;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_hostdev_attach_detach");
    #endif
   
    /* Get the host handle on which this device is on */
    usb_host_ptr = (usb_host_state_struct_t*)dev_ptr->host;
  
    /* FIXME: it is presumed that memlist points to the config descriptor */
   
    if (pConfigurationDesc->bMaxPower > USBCFG_HOST_MAX_POWER)
    {
        USB_PRINTF("Error, MAX power exceed host can offer\n");
        return FALSE;
    }

    /* first, we need to check if there is a interface can match with the interface registered by APP */
    for (interface_index = 0; interface_index < pConfiguration->interface_count; interface_index++)
    {
        pInterface = &pConfiguration->interface[interface_index];
        USB_Host_lock();
        /* Get the driver info for attach callback when a match occurs */
        info_ptr = usb_host_ptr->device_info_table;
        while (usb_host_driver_info_nonzero(info_ptr))
        {
            if (usb_host_driver_info_match(dev_ptr, pInterface->lpinterfaceDesc, info_ptr))
            {
                dev_ptr->interface_info[interface_index].lphostintf = pInterface;
                dev_ptr->interface_info[interface_index].lpDriverInfo = info_ptr;
                //pInterface->lpDriverInfo = info_ptr;
                match = TRUE;
                break;
            }
            else
            {
                dev_ptr->interface_info[interface_index].lphostintf = NULL;
                dev_ptr->interface_info[interface_index].lpDriverInfo = NULL;
            }
            info_ptr++;
        }
        USB_Host_unlock();
    }

    if (!match)
    {
        /* no matched interface found */
        return FALSE;
    }

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_hostdev_attach_detach SUCCESSFUL");
    #endif
    return TRUE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_dev_mng_parse_configuration_descriptor
* Returned Value : USB_OK or error
* Comments       :
*     parse the configuration descriptor of device
*
*END*--------------------------------------------------------------------*/
usb_status usb_host_dev_mng_parse_configuration_descriptor(usb_device_instance_handle dev_handle)//, uint8_t desc_index)
{
    dev_instance_t*                 dev_ptr;
    descriptor_union_t                 ptr1, ptr2;
    //uint8_t                          interface_count = 0;
    //uint8_t                          ep_count = 0;
    usb_device_ep_struct_t *           pEndpoint = NULL;
    usb_device_interface_struct_t *    pInterface = NULL;
    usb_device_configuration_struct_t* pConfiguration = NULL;
    usb_status                       ret = USB_OK;

    /* Check that device handle points to a device */
    dev_ptr = (dev_instance_t*)dev_handle;

    if (dev_ptr->state < DEVSTATE_CFG_READ)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor device busy");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_BUSY);
    } /* Endif */

  
      /*--------------------------------------------------------**
      ** If descriptor type is Configuration, the pointer is    **
      **    found in ptr1.  Other types of descriptor need      **
      **    to be found by stepping through the config one      **
      **    descriptor at a time.                               **
      ** To prevent parsing past the config buffer, ptr2 is     **
      **    set to the starting address plus its total size.    **
      **--------------------------------------------------------*/

    ptr1.pntr = dev_ptr->lpConfiguration; /* offset for alignment */
    if (ptr1.cfig->bNumInterfaces > USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION)
    {
        return USBERR_ERROR;
    }

    /* for the configuration descriptor, the first descriptor must be configuration descriptor */
    if ((ptr1.cfig->bLength == USB_DESC_LEN_CFG) && (ptr1.cfig->bDescriptorType == USB_DESC_TYPE_CFG))
    {
        ptr2.word = ptr1.word + USB_SHORT_UNALIGNED_LE_TO_HOST(ptr1.cfig->wTotalLength);
        pConfiguration = &dev_ptr->configuration;//(usb_device_configuration_struct_t*)OS_Mem_alloc(sizeof(USB_HOST_CONFIGURATION_STRUCT));
        
        pConfiguration->lpconfigurationDesc = (usb_configuration_descriptor_t*)ptr1.cfig;
        pConfiguration->configurationExlength = 0;
        pConfiguration->configurationEx = NULL;
        pConfiguration->interface_count = 0;

        ptr1.word += ptr1.cfig->bLength;
        while (ptr1.word < ptr2.word)
        {
            if (ptr1.common->bDescriptorType != USB_DESC_TYPE_IF)
            {
                if (pConfiguration->configurationEx == NULL)
                {
                    pConfiguration->configurationEx = (uint8_t*)ptr1.bufr;
                }
                pConfiguration->configurationExlength += ptr1.common->bLength;
                ptr1.word += ptr1.common->bLength;
            }
            else
            {
                break;
            }
        }
        while (ptr1.word < ptr2.word)
        {
            /* the first interface descriptor found */
            if (ptr1.common->bDescriptorType == USB_DESC_TYPE_IF)
            {
                if (ptr1.intf->bNumEndpoints > USBCFG_HOST_MAX_EP_PER_INTERFACE)
                {
                    USB_PRINTF("too many endpoints in one interface, please increase the USBCFG_HOST_MAX_EP_PER_INTERFACE value\n");
                    ret = USBERR_ERROR;
                    break;
                }

                if (pConfiguration->interface_count >= USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION)
                {
                    USB_PRINTF("Unsupported Device attached\r\n too many interfaces in one configuration, please increase the USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION value\n");
                    ret = USBERR_ERROR;
                    break;
                }

                /* now ptr1 point to interface descriptor */
                pInterface = (usb_device_interface_struct_t*)&pConfiguration->interface[pConfiguration->interface_count];//OS_Mem_alloc_zero(sizeof(USB_HOST_INTF_STRUCT));
                pConfiguration->interface_count++;
                pInterface->lpinterfaceDesc = ptr1.intf;
                pInterface->interfaceExlength = 0;
                pInterface->interfaceEx = NULL;
                pInterface->ep_count = 0;

                ptr1.word += ptr1.common->bLength;

                while(ptr1.word < ptr2.word)
                {
                    if ((ptr1.common->bDescriptorType != USB_DESC_TYPE_IF) &&
                        (ptr1.common->bDescriptorType != USB_DESC_TYPE_EP))
                    {
                        if (pInterface->interfaceEx == NULL)
                        {
                            pInterface->interfaceEx = (uint8_t*)ptr1.bufr;
                        }
                        pInterface->interfaceExlength += ptr1.common->bLength;
                        ptr1.word += ptr1.common->bLength;
                    }
                    else
                    {
                        break;
                    }
                }

                /* now try to get the endpoint information */
                if (pInterface->lpinterfaceDesc->bNumEndpoints == 0)
                    continue;

                /* now the ptr1 should point to endpoint descriptor */
                if (ptr1.common->bDescriptorType != USB_DESC_TYPE_EP)
                {
                    USB_PRINTF("interface descriptor error\n");
                    ret = USBERR_ERROR;
                    break;
                }

                for (; pInterface->ep_count < pInterface->lpinterfaceDesc->bNumEndpoints; (pInterface->ep_count)++)
                {
                    if ((ptr1.ndpt->bDescriptorType != USB_DESC_TYPE_EP) ||
                        (ptr1.word >= ptr2.word))
                    {
                        USB_PRINTF("endpoint descriptor error\n");
                        ret = USBERR_ERROR;
                        break;
                    }
                    pEndpoint = (usb_device_ep_struct_t*)&pInterface->ep[pInterface->ep_count];
                    pEndpoint->lpEndpointDesc = ptr1.ndpt;
                    ptr1.word += ptr1.common->bLength;

                    while(ptr1.word < ptr2.word)
                    {
                        if ((ptr1.common->bDescriptorType != USB_DESC_TYPE_EP) &&
                            (ptr1.common->bDescriptorType != USB_DESC_TYPE_IF))
                        {
                            if (pEndpoint->endpointEx == NULL)
                            {
                                pEndpoint->endpointEx = (uint8_t*)ptr1.bufr;
                            }
                            pEndpoint->endpointExlength += ptr1.common->bLength;
                            ptr1.word += ptr1.common->bLength;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                if (ret != USB_OK)
                    break;
            }
            else
            {
                ret = USBERR_ERROR;
                break;
            }
        }
        
        dev_ptr->num_of_interfaces = pConfiguration->interface_count;
        return ret;
    } /* EndIf */
    else
    {
        return USBERR_INVALID_CFIG_NUM;
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_check_configuration
* Returned Value : none
* Comments       :
*     Scan device config for interfaces that may need callbacks
*     for matching classes etc. from each interface. This function is
*     called when there configuration change or attach or detach event
*
*END*--------------------------------------------------------------------*/
bool  usb_host_dev_notify
(
    /* [IN] USB device */
    dev_instance_t*           dev_ptr,
    /* [IN] event causing callback */
    uint32_t                   event_code
)
{ 
    uint8_t                                 interface_index;
    usb_device_configuration_struct_t*      pConfiguration = &dev_ptr->configuration;
    usb_device_interface_struct_t*          pInterface = NULL;
    usb_device_interface_info_struct_t*     lpInterfaceInfo = NULL;
    usb_host_state_struct_t*                host_struct_ptr;
 
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_notify");
    #endif
 
    /* Get the host handle on which this device is on */
    host_struct_ptr = (usb_host_state_struct_t*)dev_ptr->host;

    if (event_code == USB_ATTACH_DEVICE_NOT_SUPPORT)
    {
        if (host_struct_ptr->unsupport_device_callback != NULL)
        {
            host_struct_ptr->unsupport_device_callback(NULL, NULL, USB_ATTACH_DEVICE_NOT_SUPPORT);
        }
        return TRUE;
    }
 
    /* first, we need to check if there is a interface can match with the interface registered by APP */
    for (interface_index = 0; interface_index < pConfiguration->interface_count; interface_index++)
    {
        pInterface = &pConfiguration->interface[interface_index];
 
        if (dev_ptr->interface_info[interface_index].lphostintf == NULL)
        {
            //USB_PRINTF("error in usb_host_dev_notify event_code 0x%x\n", event_code);
            // means there is no interface registered by user can match this interface
            continue;
        }

        lpInterfaceInfo = &dev_ptr->interface_info[interface_index];
        if (lpInterfaceInfo == NULL)
        {
            USB_PRINTF("error to get lpInterfaceInfo \n");
            return FALSE;
        }

        if (event_code == USB_INTF_OPENED_EVENT)
        {
            if(lpInterfaceInfo->requesting_set_interface)
            {
                lpInterfaceInfo->requesting_set_interface = (uint8_t)FALSE;
                if (lpInterfaceInfo->lpDriverInfo != NULL)
                {
                    lpInterfaceInfo->lpDriverInfo->attach_call(dev_ptr, pInterface, event_code);
                }
                break;
            }
        }
        else
        {
            if (dev_ptr->interface_info[interface_index].lpDriverInfo != NULL)
            {
                dev_ptr->interface_info[interface_index].lpDriverInfo->attach_call(dev_ptr, pInterface, event_code);
            }
            else
            {
                if (host_struct_ptr->unsupport_device_callback != NULL && (event_code == USB_ATTACH_EVENT))
                    host_struct_ptr->unsupport_device_callback(dev_ptr, pInterface, USB_ATTACH_INTF_NOT_SUPPORT);
            }
        }
    }
    
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_dev_notify SUCCESSFUL");
    #endif
    return TRUE;
}

void* _usb_host_dev_get_instance
(
    usb_host_handle    handle,
    uint8_t            hub_no,
    uint8_t            port_no,
    uint8_t            remove
)
{
    usb_host_state_struct_t*            usb_host_ptr = (usb_host_state_struct_t*)handle;
    dev_instance_t*                     dev_instance_ptr;
    dev_instance_t*                     dev_instance_ptr_pre;
    dev_instance_t*                     device_root = (dev_instance_t*)usb_host_ptr->device_list_ptr;

    //USB_Host_lock();    
    dev_instance_ptr_pre = device_root;
    for (dev_instance_ptr = device_root; dev_instance_ptr != NULL; dev_instance_ptr = dev_instance_ptr->next)
    {
        if ((dev_instance_ptr->port_no != port_no) ||
            (dev_instance_ptr->hub_no != hub_no) ||
            (dev_instance_ptr->host != handle))
        {
            dev_instance_ptr_pre = dev_instance_ptr;
            continue;
        }
        /* we found it */
        if(remove)
        {
            if (dev_instance_ptr == device_root)
            {
                usb_host_ptr->device_list_ptr = dev_instance_ptr->next;
            }
            else
            {
                dev_instance_ptr_pre->next = dev_instance_ptr->next;
            }
        }
        break;
    }
    //USB_Host_unlock();

    return (void*)dev_instance_ptr;
}
#endif
