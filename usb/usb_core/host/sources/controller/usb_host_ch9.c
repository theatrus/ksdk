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
* $FileName: host_ch9.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains standard device requests (USB Chapter 9)
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
#include "usb_otg_host_api.h"
#endif
#if USBCFG_HOST_HUB
#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"
#include "usb_host_hub_sm.h"
extern void usb_host_hub_device_sm(void*, void *, uint8_t *, uint32_t, usb_status);
extern void usb_host_hub_Port_Reset(hub_device_struct_t* hub_instance,uint8_t port);
#endif


usb_status USB_log_error(char* file, uint32_t line, usb_status error);

/* Prototype, copy to stack frames for each use instance */
static const usb_setup_t req_prototype = 
{
    0,          /* request type */ 
    0,          /* request */
    {0,0},      /* wvalue  */
    {0,0},      /* windex  */
    {0,0}       /* wlength  */
};

/* Unpublished functions, not intended for application use */
static void  usb_host_cntrl_transaction_done(usb_pipe_handle, void*, uint8_t *, uint32_t, usb_status);
static usb_status  usb_host_ch9_dev_req(usb_device_instance_handle, usb_setup_t*, uint8_t *);


 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_cntrl_transaction_done
* Returned Value : none
* Comments       :
*     Callback function to process transaction-done events
*     State machine for enumeration/subsequent transactions
*     Between states, the 8-byte buffer in device_instance
*        is used to save the first part of the configuration.
*     Pointer desc in various flavors (uint8_t *, cfig_ptr)
*        is used to parse this buffer in various ways. 
* 
*END*--------------------------------------------------------------------*/
static void  usb_host_cntrl_transaction_done
(
    /* [IN] Unused */
    void*           tr_ptr,
    /* [IN] The user parameter */
    void*           user_parm,
    /* [IN] the buffer */
    uint8_t *         buffer,
    /* [IN] The length of the transfer */
    uint32_t           length,
    /* [IN] status of the transfer */
    usb_status        status
)
{
    usb_host_state_struct_t*    usb_host_ptr;
    dev_instance_t*             dev_inst_ptr = (dev_instance_t*)user_parm;
    pipe_struct_t*              pipe_ptr = dev_inst_ptr->control_pipe;
    descriptor_union_t          desc;
    int32_t                     config_size = 0;

    uint32_t                   aligned_config_size = 0;
	usb_host_api_functions_struct_t* host_api;
#ifdef USBCFG_OTG
    static bool                otg_hnp_support;
#endif
    usb_host_ptr = (usb_host_state_struct_t*)dev_inst_ptr->host;

	host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;


    if (usb_host_release_tr(usb_host_ptr, tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }

    switch (status)
    {
        case USB_OK:
            dev_inst_ptr->ctrl_retries = USBCFG_HOST_CTRL_FAILED_RETRY;
            dev_inst_ptr->stall_retries = USBCFG_HOST_CTRL_STALL_RETRY;
            break;
  
        case USBERR_TR_FAILED:
        case USBERR_ENDPOINT_STALLED:
        
            //USB_PRINTF("TR failed buffer : %x\n\r", buffer);
            dev_inst_ptr->ctrl_retries--;
            dev_inst_ptr->stall_retries--;
            if ((status == USBERR_ENDPOINT_STALLED)&&(dev_inst_ptr->stall_retries))
            {
               status = USB_OK;
               //USB_PRINTF("USBERR_ENDPOINT_STALLED\n");
               //status = _usb_host_ch9_clear_feature((usb_device_instance_handle)dev_inst_ptr, REQ_TYPE_ENDPOINT, 0, ENDPOINT_HALT);
            }
            else if ((dev_inst_ptr->stall_retries == 0) || ((dev_inst_ptr->ctrl_retries)&&(status == USBERR_TR_FAILED)))
            {
                if (dev_inst_ptr->state > DEVSTATE_SET_CFG)
                {
                    if ((dev_inst_ptr->control_callback != NULL))
                    {
                        dev_inst_ptr->control_callback(tr_ptr, dev_inst_ptr->control_callback_param, buffer, length, USBERR_TR_RETRY_FAILED);
                    }
                }
                else
                {
                    /* if hub level is 1 will do port reset */
                    if(dev_inst_ptr->level == 1)
                    {
                        host_api = (usb_host_api_functions_struct_t*)usb_host_ptr->host_controller_api;
                        if (host_api->host_bus_control != NULL)
                        {
                            host_api->host_bus_control(usb_host_ptr->controller_handle, 1);
                        }
                        status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)dev_inst_ptr,
                                                                  USB_DESC_TYPE_DEV << 8, 0, 8, 
                                                                  (uint8_t *)buffer);
                        dev_inst_ptr->state = DEVSTATE_DEVDESC8;
                    }
                    else  
                    {
    #if USBCFG_HOST_HUB                   
                        usb_host_hub_Port_Reset(dev_inst_ptr->hub_instance, dev_inst_ptr->port_no);
                        usb_host_dev_mng_detach(dev_inst_ptr->host, dev_inst_ptr->hub_no, dev_inst_ptr->port_no);
    #endif
                    }
                }
            }

            break;
  
        
        default:
            break;
    }

    if (status != USB_OK)
    {
        return;
    }

    /*----------------------------------------------------**
    ** Enumeration state machine -- cases are named after **
    ** the just-completed transactions.                   **
    **----------------------------------------------------*/

    switch (dev_inst_ptr->state)
    {
        case DEVSTATE_INITIAL:      /* initial device state = forever error state */
            break;   
        case DEVSTATE_DEVDESC8:     /* device descriptor [0..7]*/
            /* We must have received the first 8 bytes in 
                    * dev_inst_ptr->dev_descriptor which contains the 
                    * max packet size for this control endpoint 
                    */
            //USB_PRINTF("dev8\n");
            pipe_ptr->max_packet_size = dev_inst_ptr->dev_descriptor.bMaxPacketSize;
            //USB_PRINTF("DEVSTATE_DEVDESC8 %d\n",dev_inst_ptr->dev_descriptor.bMaxPacketSize);
              
            /* Notify device driver of MaxPacketSize0 for this device */
            status = _usb_host_update_max_packet_size_call_interface (usb_host_ptr, pipe_ptr);
           
            if (status != USB_OK)
            {
                USB_PRINTF("update max packet size error\n");
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
            else
            {
                dev_inst_ptr->state = DEVSTATE_ADDR_SET;
            }
  
            /* Now set the address that we assigned when we initialized 
                    * the device instance struct for this device 
                    */
            status = _usb_host_ch9_set_address((usb_device_instance_handle)dev_inst_ptr);
            
            if (status != USB_OK)
            {
                USB_PRINTF("set address error\n");
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
   
            break;
       case DEVSTATE_ADDR_SET:     /* address set */
            //pipe_ptr->DEVICE_ADDRESS = dev_inst_ptr->address;
         //   USB_PRINTF("add set %d\n", dev_inst_ptr->target_address);
            dev_inst_ptr->address = dev_inst_ptr->target_address;
            /* Notify device driver of USB device's new address */
            status = _usb_host_update_device_address_call_interface (usb_host_ptr, pipe_ptr);
            
            if (status != USB_OK)
            {
                USB_PRINTF("update device address error\n");
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
            dev_inst_ptr->state = DEVSTATE_DEV_DESC;
            //USB_PRINTF("descriptor address 0x%x\n", &dev_inst_ptr->dev_descriptor);
            /* Now get the full descriptor */
            status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)dev_inst_ptr,
                USB_DESC_TYPE_DEV << 8, 
                0, 
                USB_DESC_LEN_DEV,
                (uint8_t *)&dev_inst_ptr->dev_descriptor);
               
            if (status != USB_OK)
            {
                USB_PRINTF("get device descriptor error\n");
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
   
            break;   
       case DEVSTATE_DEV_DESC:     /* full device descriptor received */
            /* Now lets get the first 9 bytes of the configuration descriptor 
                    */
            desc.pntr = &dev_inst_ptr->buffer;
            //USB_PRINTF("dev %d cfg, c %d 0x%x\n", dev_inst_ptr->dev_descriptor.bNumConfigurations, dev_inst_ptr->cfg_value, desc.pntr);
            dev_inst_ptr->state = DEVSTATE_GET_CFG9;
            status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)dev_inst_ptr,
               USB_DESC_TYPE_CFG << 8 | dev_inst_ptr->cfg_value, 
               0, 
               sizeof(dev_inst_ptr->buffer),
               desc.bufr);
               
            if (status != USB_OK)
            {
                USB_PRINTF("get 9 byte configuration(%d) error\n", dev_inst_ptr->cfg_value);
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
   
            break;
      case DEVSTATE_GET_CFG9:     /* Read 9 bytes of config descriptor */
          
            /* Now select the configuration as specified in the 
                    * descriptor 
                    */
            desc.cfig = (usb_configuration_descriptor_t*)dev_inst_ptr->buffer;
            config_size = USB_SHORT_UNALIGNED_LE_TO_HOST(desc.cfig->wTotalLength);
            aligned_config_size = config_size;
            /* for KHCI, the start address and the length should be 4 byte align */
            if ((config_size && 0x3) != 0)
            {
                aligned_config_size = (config_size & 0xFFFFFFFC) + 4;
            }

            if (dev_inst_ptr->lpConfiguration != NULL)
            {
                OS_Mem_free(dev_inst_ptr->lpConfiguration);
                dev_inst_ptr->lpConfiguration = NULL;
            }

            dev_inst_ptr->lpConfiguration = (void*)OS_Mem_alloc_uncached(aligned_config_size);
       
            if (dev_inst_ptr->lpConfiguration == NULL)
            {
                USB_PRINTF("get memory for full configuration(%d) error\n", dev_inst_ptr->cfg_value);
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
           
            //USB_PRINTF("Get CFG9 %d %d %d\n",dev_inst_ptr->cfg_value, config_size,dev_inst_ptr->dev_descriptor.bNumConfigurations);
       
            /* We can only read one config descriptor at a time */
            status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)dev_inst_ptr,
               (USB_DESC_TYPE_CFG << 8) | dev_inst_ptr->cfg_value, 
               0, 
               (uint16_t)config_size,
               dev_inst_ptr->lpConfiguration);
               
            if (status != USB_OK)
            {
                USB_PRINTF("get full configuration(%d) error\n", dev_inst_ptr->cfg_value);
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }    
#ifdef USBCFG_OTG
            dev_inst_ptr->state = DEVSTATE_CHK_OTG;
#else
            /*dev_inst_ptr->state = DEVSTATE_SET_CFG;*/
            dev_inst_ptr->state = DEVSTATE_CFG_READ;
#endif
           break;  

#ifdef USBCFG_OTG
        case DEVSTATE_CHK_OTG:
            otg_hnp_support = FALSE;
            /* Point to the memory owned by this device */
            /* FIXME: it is presumed that memlist points to the config descriptor */
            desc.pntr = dev_inst_ptr->lpConfiguration;

            /* We will always start with config desc so update the search pointer */
            config_size = USB_SHORT_UNALIGNED_LE_TO_HOST(desc.cfig->wTotalLength);
            config_size -= desc.cfig->bLength;
            desc.word += desc.cfig->bLength;
       
            while (config_size)
            {
                if (desc.otg->bDescriptorType == USB_DESC_TYPE_OTG)
                {
                    /* Found the OTG descriptor */
                    break;
                }
                config_size -= desc.intf->bLength;
                desc.word += desc.intf->bLength;
            }

            /* Check for an OTG descriptor */
            dev_inst_ptr->state = DEVSTATE_CFG_READ;
       
            if (config_size && desc.otg->bDescriptorType == USB_DESC_TYPE_OTG &&
               (desc.otg->bmAttributes & OTG_HNP_SUPPORT))
            {
                otg_hnp_support = TRUE;
                if(usb_otg_host_get_otg_attribute(usb_host_ptr->otg_handle,desc.otg->bmAttributes) == USB_OK)
                {
                    if(usb_otg_host_set_feature_required(usb_host_ptr->otg_handle))
                    {
                        status = _usb_host_ch9_set_feature(dev_inst_ptr, 0, 0, OTG_A_HNP_SUPPORT);
                       
                        if (status != USB_OK)
                        {
                            USB_PRINTF("set feature for otg error\n");
                            usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                            break;
                        }
                        
                        else
                        {
                            dev_inst_ptr->state = DEVSTATE_SET_HNP;  
                            break;
                        }
                    }
                }
            }
        case DEVSTATE_SET_HNP:
            if (otg_hnp_support)
            {
                status = _usb_host_ch9_set_feature(dev_inst_ptr, 0, 0, OTG_B_HNP_ENABLE);
                if (status != USB_OK)
                {
                    dev_inst_ptr->state = DEVSTATE_INITIAL;
                }
                else
                {
                    dev_inst_ptr->state = DEVSTATE_SET_HNP_OK;
                }
                break; 
            }
        case  DEVSTATE_SET_HNP_OK:
            if(dev_inst_ptr->state == DEVSTATE_SET_HNP_OK)
            {
                usb_otg_host_a_set_b_hnp_en(usb_host_ptr->otg_handle, TRUE);
            }
#endif  
        case DEVSTATE_CFG_READ:
            dev_inst_ptr->cfg_value++;
            //USB_PRINTF("cfg %d, %d\n", dev_inst_ptr->cfg_value,dev_inst_ptr->dev_descriptor.bNumConfigurations);
            if (usb_host_dev_mng_parse_configuration_descriptor(dev_inst_ptr) == USB_OK)
            {
                //USB_PRINTF("parse cfg\n");
                if (usb_host_dev_mng_check_configuration(dev_inst_ptr))
                {
                    //USB_PRINTF("check cfg\n");
                    usb_host_dev_notify(dev_inst_ptr, USB_ATTACH_EVENT);
                    /* enumeration done */
                    dev_inst_ptr->state = DEVSTATE_SET_CFG;
                    /* send set_configuration */
                    status = _usb_host_ch9_set_configuration(dev_inst_ptr, dev_inst_ptr->configuration.lpconfigurationDesc->bConfigurationValue);
                    if (status != USB_OK)
                    {
                        USB_PRINTF("set configuration(%d) error\n", dev_inst_ptr->cfg_value);
                        usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                        break;
                    }
                }
                else
                {
                    //USB_PRINTF("cfg %d %d\n", dev_inst_ptr->cfg_value,dev_inst_ptr->dev_descriptor.bNumConfigurations);
                    /* get next configuration */
                    if (dev_inst_ptr->cfg_value < dev_inst_ptr->dev_descriptor.bNumConfigurations)
                    {
                        //USB_PRINTF("invalid cfg re\n");
                        desc.pntr = &dev_inst_ptr->buffer;
                        dev_inst_ptr->state = DEVSTATE_GET_CFG9;
                        status = _usb_host_ch9_get_descriptor((usb_device_instance_handle)dev_inst_ptr,
                            USB_DESC_TYPE_CFG << 8 | dev_inst_ptr->cfg_value, 
                            0, 
                            sizeof(dev_inst_ptr->buffer),
                            desc.bufr);
                        if (status != USB_OK)
                        {
                            USB_PRINTF("get 9 byte configuration(%d) error\n", dev_inst_ptr->cfg_value);
                            usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                            break;
                        }
                   }
                   else
                   {
                       #if !USBCFG_HOST_HUB
                       if (dev_inst_ptr->dev_descriptor.bDeviceClass == USB_CLASS_HUB )
                           USB_PRINTF("unsupported hub attached\n");
                       else
                       #endif
                           USB_PRINTF("unsupported device attached\n");
                       usb_host_dev_notify(dev_inst_ptr, USB_ATTACH_DEVICE_NOT_SUPPORT);
                       usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                       break;
                   }
               }
            }
            else
            {
                USB_PRINTF("parse configuration(%d) error\n", dev_inst_ptr->cfg_value);
                usb_host_dev_mng_pre_detach(dev_inst_ptr->host,dev_inst_ptr->hub_no,dev_inst_ptr->port_no);
                break;
            }
            break;
        case DEVSTATE_SET_CFG:     /* config descriptor [0..8] */
            dev_inst_ptr->state = DEVSTATE_SET_INTF;
            usb_host_dev_notify(dev_inst_ptr, USB_CONFIG_EVENT);
#ifdef USBCFG_OTG
            usb_otg_host_on_interface_event(usb_host_ptr->otg_handle, dev_inst_ptr);
#endif
            break;

        case DEVSTATE_SET_INTF:    /* Select interface done */
            dev_inst_ptr->state = DEVSTATE_ENUM_OK;
            usb_host_dev_notify(dev_inst_ptr, USB_INTF_OPENED_EVENT);
            break;
        case DEVSTATE_ENUM_OK:   /* enumeration complete */
            if ((dev_inst_ptr->control_callback != NULL))
            {
                dev_inst_ptr->control_callback(tr_ptr, dev_inst_ptr->control_callback_param, buffer, length, status);
            }
            break;
        default:
            break;
    } /* EndSwitch */
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_ch9_dev_req
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device requests in Chapter 9.
*        See Table 9-3 p. 250 of USB 2.0 specification.
*     This code does minimal error checking, on the assumption
*        that it is called only from wrappers in this file.
*     It is presumed that this function is called with USB interrupts disabled
* 
*END*--------------------------------------------------------------------*/
static usb_status  usb_host_ch9_dev_req
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* Device Request to send */
    usb_setup_t*                 devreq_ptr,
    /* buffer to send/receive */
    uint8_t *                    buff_ptr
)
{
    dev_instance_t*           dev_ptr;
    usb_pipe_handle           pipe_handle;
    tr_struct_t*              tr_ptr;
    usb_status                 error;

    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_host_ch9_dev_req");
    #endif
    
    /* Verify that device handle is valid */
    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK) 
    {
        #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("usb_host_ch9_dev_req device not found");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
    } /* Endif */

    dev_ptr = (dev_instance_t*)dev_handle;
    pipe_handle = dev_ptr->control_pipe;

    if (usb_host_get_tr(dev_ptr->host, dev_ptr->control_callback, dev_ptr->control_callback_param, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr ch9\n");
        return USBERR_ERROR;
    }

    /* Set buffer length if required */
    switch (devreq_ptr->brequest)
    {
        case REQ_SET_DESCRIPTOR:
            tr_ptr->tx_buffer = buff_ptr;
            tr_ptr->tx_length = USB_SHORT_UNALIGNED_LE_TO_HOST(devreq_ptr->wlength);
            break;
        case REQ_GET_CONFIGURATION:
        case REQ_GET_DESCRIPTOR:
        case REQ_GET_INTERFACE:
        case REQ_GET_STATUS:
        case REQ_SYNCH_FRAME:
            tr_ptr->rx_buffer = buff_ptr;
            tr_ptr->rx_length = USB_SHORT_UNALIGNED_LE_TO_HOST(devreq_ptr->wlength);
            break;
    } /* EndSwitch */

    tr_ptr->setup_packet = *devreq_ptr;

    if ((dev_ptr->state < DEVSTATE_ENUM_OK) ||
        (tr_ptr->callback == NULL))
    {
        tr_ptr->callback = usb_host_cntrl_transaction_done;
        tr_ptr->callback_param = dev_ptr;
    } /* Endif */

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_ch9_dev_req SUCCESSFUL");
    #endif
    error = usb_host_send_setup(dev_ptr->host, pipe_handle, tr_ptr);

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_clear_feature
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_clear_feature
(
    /* usb device */
    usb_device_instance_handle     dev_handle,
    /* request type device/interface/endpoint */
    uint8_t                        req_type,
    /* device = 0, or interface/endpoint */
    uint8_t                        intf_endpt,
    /* feature selection */
    uint16_t                       feature
)
{ /* Body */
   usb_setup_t  request = req_prototype;
   usb_status   error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature");
   #endif

    switch (req_type) 
    {
        case REQ_TYPE_DEVICE:
            break;
        case REQ_TYPE_INTERFACE:
        case REQ_TYPE_ENDPOINT:
            *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(intf_endpt);
            break;
        default:
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature, invalid request");
            #endif
            return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
    } /* EndSwitch */

    request.bmrequesttype = (uint8_t)(req_type | REQ_TYPE_OUT);
    request.brequest = REQ_CLEAR_FEATURE;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(feature);

    error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature, SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_configuration
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_get_configuration
(
    /* [IN] usb device */
    usb_device_instance_handle   dev_handle,
    /* [OUT] configuration value */
    uint8_t *                     buffer
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_configuration");
    #endif

    request.bmrequesttype = REQ_TYPE_DEVICE | REQ_TYPE_IN;
    request.brequest = REQ_GET_CONFIGURATION;
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT_CONST(1);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_configuration, SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_descriptor
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_get_descriptor
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* descriptor type & index */
    uint16_t                       type_index,
    /* Language ID or 0 */
    uint16_t                       lang_id,
    /* buffer length */
    uint16_t                       buflen,
    /* descriptor buffer */
    uint8_t *                     buffer
)
{ /* Body */
    usb_setup_t  request;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_descriptor");
    #endif

    request.bmrequesttype = REQ_TYPE_DEVICE | REQ_TYPE_IN;
    request.brequest = REQ_GET_DESCRIPTOR;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(type_index);
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(lang_id);
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT(buflen);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_descriptor, SUCCESSFUL");
    #endif

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_interface
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_get_interface
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* interface index */
    uint8_t            interface,
    /* alternate setting buffer */
    uint8_t *         buffer
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_interface");
    #endif

    request.bmrequesttype = REQ_TYPE_INTERFACE | REQ_TYPE_IN;
    request.brequest = REQ_GET_INTERFACE;
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(interface);
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT_CONST(1);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_interface, SUCCESSFUL");
    #endif

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_status
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_get_status
(
/* usb device */
    usb_device_instance_handle  dev_handle,
    /* request type device/interface/endpoint */
    uint8_t                     req_type,
    /* device = 0, or interface/endpoint */
    uint16_t                     intf_endpt,
    /* returned status */
    uint8_t *                   buffer
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_status");
    #endif

    switch (req_type) 
    {
        case REQ_TYPE_DEVICE:
        case REQ_TYPE_INTERFACE:
        case REQ_TYPE_ENDPOINT:
            break;
        default:
            #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_ch9_get_status, invalid parameter");
            #endif
            return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
    } /* EndSwitch */

    request.bmrequesttype = (uint8_t)(req_type | REQ_TYPE_IN);
    request.brequest = REQ_GET_STATUS;
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(intf_endpt);
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT_CONST(2);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_get_status, SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_address
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_set_address
(
    /* usb device */
    usb_device_instance_handle   dev_handle
)
{ /* Body */
    dev_instance_t*  dev_ptr = (dev_instance_t*)dev_handle;
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_address");
    #endif

    request.bmrequesttype = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
    request.brequest = REQ_SET_ADDRESS;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(dev_ptr->target_address);

    error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_address,SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_configuration
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_set_configuration
(
    /* usb device */
    usb_device_instance_handle     dev_handle,
    /* configuration value */
    uint16_t                       config
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status   error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_configuration");
    #endif

    request.bmrequesttype = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
    request.brequest = REQ_SET_CONFIGURATION;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(config);

    error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_configuration SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_descriptor
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_set_descriptor
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* descriptor type & index */
    uint16_t           type_index,
    /* Language ID or 0 */
    uint16_t           lang_id,
    /* buffer length */
    uint16_t           buflen,
    /* descriptor buffer */
    uint8_t *         buffer
)
{ /* Body */
    usb_setup_t  request;
    usb_status error = USB_OK;
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_descriptor");
    #endif

    request.bmrequesttype = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
    request.brequest = REQ_SET_DESCRIPTOR;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(type_index);
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(lang_id);
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT(buflen);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_descriptor SUCCESSFUL");
    #endif

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_feature
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_set_feature
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* request type device/interface/endpoint */
    uint8_t            req_type,
    /* device = 0, or interface/endpoint */
    uint8_t            intf_endpt,
    /* feature selection */
    uint16_t           feature
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_feature");
    #endif

    switch (req_type) {
        case REQ_TYPE_DEVICE:
            break;
        case REQ_TYPE_INTERFACE:
        case REQ_TYPE_ENDPOINT:
            *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(intf_endpt);
            break;
        default:
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("_usb_host_ch9_set_feature, invalid param");
            #endif
            return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
    } /* EndSwitch */

    request.bmrequesttype = (uint8_t)(req_type | REQ_TYPE_OUT);
    request.brequest = REQ_SET_FEATURE;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(feature);

    error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_feature, SUCCESSFUL");
    #endif
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_interface
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_set_interface
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* alternate setting */
    uint8_t            alternate,
    /* interface */
    uint8_t            intf
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_interface");
    #endif

    request.bmrequesttype = REQ_TYPE_INTERFACE | REQ_TYPE_OUT;
    request.brequest = REQ_SET_INTERFACE;
    *(uint16_t*)request.wvalue = USB_HOST_TO_LE_SHORT(alternate);
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(intf);

    error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_set_interface, SUCCESSFUL");
    #endif

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_synch_frame
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
usb_status  _usb_host_ch9_synch_frame
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* interface index */
    uint8_t                      interface,
    /* configuration buffer */
    uint8_t *                    buffer
)
{ /* Body */
    usb_setup_t  request = req_prototype;
    usb_status error = USB_OK;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_synch_frame");
    #endif

    request.bmrequesttype = REQ_TYPE_ENDPOINT | REQ_TYPE_IN;
    request.brequest = REQ_SYNCH_FRAME;
    *(uint16_t*)request.windex = USB_HOST_TO_LE_SHORT(interface);
    *(uint16_t*)request.wlength = USB_HOST_TO_LE_SHORT_CONST(2);

    error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("_usb_host_ch9_synch_frame, SUCCESSFUL");
    #endif

    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

#endif
