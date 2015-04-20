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
* $FileName: usb_host_common.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains common functions for USB class drivers
*   Implementation Notes:
*   An interface is supported by two data structures on the
*   owning device's list of allocated memory:-
*   (1) a pipe bundle struct with pointers to open pipes for
*   all endpoints on the interface
*   (2) a class-interface struct with queues for TR logging,
*   pipe handles, and other class-specific status etc.
*   Each <class_name>.h must define its interface struct, which must
*   begin with the fields in GENERAL_CLASS (see common.h),
*   if common routines are going to be used.
*   It is strongly recommended that all class init functions
*   call usb_host_class_intf_init (pointer to pipe bundle,
*   pointer to class-intf, (pointer)&class_anchor ).  This
*   will verify correct pointers, zero the interface struct,
*   and set up the header pointers + key correctly.
*   All class usage of the pointers etc. in the struct
*   must be protected against detach (which occurs without
*   advance notice):-
*   OS_Lock();
*   if (usb_host_class_intf_validate(class-call-struct pointer)) {
*       Issue commands / Use pipes etc.
*   }
*   OS_Unlock();
*
*END************************************************************************/
#include "usb_host_config.h"
#if (USBCFG_HOST_KHCI) || (USBCFG_HOST_EHCI)
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"

extern class_map_t class_interface_map[];

//static uint32_t tr_index = 0;

/* Unpublished functions, not intended for application use */
static uint32_t usb_hostdev_mask_class_etc(uint8_t * , uint8_t *);
bool  usb_host_driver_info_match(dev_instance_t*, interface_descriptor_t*, usb_host_driver_info_t*);
bool usb_host_driver_info_nonzero(usb_host_driver_info_t*);
class_map_t*  usb_host_get_class_map(interface_descriptor_t*);
usb_status USB_log_error(char* file, uint32_t line, usb_status error);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_validate
* Returned Value : USB_OK or USBERR_DEVICE_NOT_FOUND
* Comments       :
*     Function to verify pointer is address of an instance on device list.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
usb_status  usb_hostdev_validate
(
    usb_device_instance_handle  device_handle
)
{ /* Body */
    dev_instance_t*           dev_ptr = (dev_instance_t*)device_handle;
    usb_host_state_struct_t*  usb_host_ptr;
    dev_instance_t*           test_ptr;
    usb_status                error;
    //uint32_t                    i;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_hostdev_validate");
    #endif

    if (device_handle == NULL)
    {
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_hostdev_validate FAILED, invalid device number");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_DEVICE_NUM);
    }
    usb_host_ptr = (usb_host_state_struct_t*)dev_ptr->host;

    USB_Host_lock();
    /* usb_host_ptr is valid host state structure, check for list of attached device instances */
    test_ptr = (dev_instance_t*)usb_host_ptr->device_list_ptr;
    while ((test_ptr != dev_ptr) && (test_ptr != NULL))
    {
        test_ptr = test_ptr->next;
    }
    USB_Host_unlock();

    #ifdef _HOST_DEBUG_
    if (test_ptr == NULL) 
    {
        DEBUG_LOG_TRACE("usb_hostdev_validate null device handle");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_hostdev_validate SUCCESSFUL");
    }
    #endif

    error = (test_ptr == NULL) ? USBERR_DEVICE_NOT_FOUND : USB_OK;

    if((error == USB_OK) && (dev_ptr->to_be_detached == (uint8_t)TRUE))
    {
        error = USBERR_DEVICE_DETACH;
    }
    return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_match
* Returned Value : TRUE for driver match, else FALSE
* Comments       :
*     Match device driver info with class etc. in device/interface
*     following the ordering in "USB Common Class Specification"
*     Rev. 1.0, August 18, 1998, p. 7
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
bool  usb_host_driver_info_match
(
    /* [IN] USB device */
    dev_instance_t*           dev_ptr,
    /* [IN] Configuration number */
    interface_descriptor_t*   intf_ptr,
    /* [IN] TRUE=attach, FALSE=detach */
    usb_host_driver_info_t*   info_ptr
)
{ /* Body */
    uint16_t  info_Vendor, info_Product, dev_Vendor, dev_Product;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_driver_info_match");
    #endif

    info_Vendor = USB_SHORT_UNALIGNED_LE_TO_HOST(info_ptr->idVendor);
    info_Product = USB_SHORT_UNALIGNED_LE_TO_HOST(info_ptr->idProduct);

    /* If vendor and product are listed in table (non-zero) */
    if ((info_Vendor | info_Product) != 0)
    {
        dev_Vendor = USB_SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idVendor);
        dev_Product = USB_SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idProduct);
 
        if ((info_Vendor == dev_Vendor) &&
            (info_Product == dev_Product))
        {
            #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match PID, VID match");
            #endif
            return TRUE;
        } /* Endif */
    }

   /* note: zero value class in the device descriptor means
   ** that the class is defined in interface descriptor
   */
    if ((info_ptr->bDeviceClass == 0xFF) || /* if the device class is any OR */
      ((info_ptr->bDeviceClass ==             /* or the device class matches */
      dev_ptr->dev_descriptor.bDeviceClass) && 
      (dev_ptr->dev_descriptor.bDeviceClass != 0x00))) /* but it is not zero-value match */
    {
        if ((info_ptr->bDeviceSubClass ==
             dev_ptr->dev_descriptor.bDeviceSubClass) &&
             (info_ptr->bDeviceProtocol ==
             dev_ptr->dev_descriptor.bDeviceProtocol))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match class, match subclass, match protocol");
            #endif
            return TRUE;
        } /* Endif */

        if ((info_ptr->bDeviceSubClass ==
             dev_ptr->dev_descriptor.bDeviceSubClass) &&
             (info_ptr->bDeviceProtocol == 0xFF))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match class, match subclass");
            #endif
            return TRUE;
        }

        if ((info_ptr->bDeviceSubClass == 0xFF) && 
            (info_ptr->bDeviceProtocol ==
            dev_ptr->dev_descriptor.bDeviceProtocol))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match class, match protocol");
            #endif
            return TRUE;
        }

        if ((info_ptr->bDeviceSubClass == 0xFF) && 
            (info_ptr->bDeviceProtocol == 0xFF))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match class");
            #endif
            return TRUE;
        }
    }

    /* No Device match, try Interface */
    if ((info_ptr->bDeviceClass == 0xFF) || /* if the device class is any OR */
        ((info_ptr->bDeviceClass == /* or the class matches interface class */
        intf_ptr->bInterfaceClass)))
    {
        if ((info_ptr->bDeviceSubClass ==
             intf_ptr->bInterfaceSubClass) &&
            (info_ptr->bDeviceProtocol ==
             intf_ptr->bInterfaceProtocol))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface subclass, match interface protocol");
            #endif
            return TRUE;
        } /* Endif */

        if ((info_ptr->bDeviceSubClass ==
             intf_ptr->bInterfaceSubClass) &&
            (info_ptr->bDeviceProtocol == 0xFF))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface subclass, match interface protocol");
            #endif
            return TRUE;
        }

        if ((info_ptr->bDeviceSubClass == 0xFF) && 
            (info_ptr->bDeviceProtocol ==
            intf_ptr->bInterfaceProtocol))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface protocol");
            #endif
            return TRUE;
        }

        if ((info_ptr->bDeviceSubClass == 0xFF) && 
            (info_ptr->bDeviceProtocol == 0xFF))
        {
            #ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_host_driver_info_match interface class");
            #endif
            return TRUE;
        }
   }

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_host_driver_info_match interface No Match");
    #endif
    return FALSE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_nonzero
* Returned Value : none
* Comments       :
*     Function to detect the all-zero end-of-list item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
bool  usb_host_driver_info_nonzero
(
    /* [IN] USB device */
    usb_host_driver_info_t*           info_ptr
)
{
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_host_driver_info_nonzero");
    #endif
 
    if (info_ptr->bDeviceClass != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->bDeviceProtocol != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->bDeviceSubClass != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->idVendor[0] != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->idVendor[1] != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->idProduct[0] != 0)
    {
       return TRUE;
    }
 
    if (info_ptr->idProduct[1] != 0)
    {
       return TRUE;
    }
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_host_driver_info_nonzero FAILURE");
    #endif
 
    return FALSE;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_get_class_map
* Returned Value : pointer to matching class map item, or NULL
* Comments       :
*           Map interface class etc. to a class driver table item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
class_map_t*  usb_host_get_class_map
(
    /* interface's device/descriptor/pipe bundle */
    interface_descriptor_t*  interface_desc_ptr
)
{
    class_map_t*              map_ptr;
    uint32_t                    intf_class, map_class;
 
    #ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_host_get_class_map");
    #endif
 
    for (map_ptr = &class_interface_map[0]; ; map_ptr++)
    {
        if (map_ptr->class_init == NULL)
        {
            map_ptr = NULL;
            break;
        }
        intf_class = usb_hostdev_mask_class_etc(&interface_desc_ptr->bInterfaceClass,
                                                &map_ptr->class_code_mask);
        map_class = usb_hostdev_mask_class_etc(&map_ptr->class_code,
                                               &map_ptr->class_code_mask);
  
        if (intf_class != 0)
        {
            if (intf_class == map_class)
            {
               break;
            }
        }
    } /* EndFor */
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_host_get_class_map SUCCESSFUL");
    #endif
    return map_ptr;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_mask_class_etc
* Returned Value : masked class/sub-class/protocol
* Comments       :
*     It is assumed that class, sub-class, and protocol are
*     successive uint-8 values (as in a USB descriptor),
*     and likewise the masks to be ANDed with them.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static uint32_t  usb_hostdev_mask_class_etc
(
    /* [IN] pointer to class etc. */
    uint8_t *      csp_ptr,
    /* [IN] pointer to masks */
    uint8_t *      msk_ptr
)
{
    uint32_t  res;
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_hostdev_mask_class_etc");
    #endif
 
    /* MSB of result = class */
    res = (uint32_t)((*csp_ptr++) & (*msk_ptr++));
 
    /* 2nd byte of result = sub-class */
    res = (res << 8);
    res |= (uint32_t) ((*csp_ptr++) & (*msk_ptr++));
 
    /* LSB of result = sub-class */
    res = (res << 8);
    res |= (uint32_t) ((*csp_ptr) & (*msk_ptr));
 
    #ifdef _HOST_DEBUG_
       DEBUG_LOG_TRACE("usb_hostdev_mask_class_etc SUCCESSFUL");
    #endif
    
    return res;
}

#endif
