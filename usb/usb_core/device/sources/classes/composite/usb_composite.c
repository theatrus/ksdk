/**HEADER********************************************************************
 * 
 * Copyright (c) 2004 -2010, 2013 - 2015 Freescale Semiconductor;
 * All Rights Reserved
 *
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
 * @file usb_composite.c
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file contains USB composite layer implementation.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"

#include "usb_device_stack_interface.h"
#include "usb_class_composite.h"
#include "usb_composite.h"
#include "usb_class_internal.h"


#if USBCFG_DEV_HID
#include "usb_hid_config.h"
#include "usb_class_hid.h"
#include "usb_hid.h"
#endif

#if USBCFG_DEV_MSC
#include "usb_msc_config.h"
#include "usb_class_msc.h"
#include "usb_msc.h"
#endif

#if ((defined USBCFG_DEV_AUDIO) && USBCFG_DEV_AUDIO)
#include "usb_audio_config.h"
#include "usb_class_audio.h"
#include "usb_audio.h"
#endif

#if ((defined USBCFG_DEV_VIDEO) && (USBCFG_DEV_VIDEO))
#include "usb_video_config.h"
#include "usb_class_video.h"
#include "usb_video.h"
#endif

#if USBCFG_DEV_PHDC
#include "usb_phdc_config.h"
#include "usb_class_phdc.h"
#include "usb_phdc.h"
#endif

#if USBCFG_DEV_CDC
#include "usb_cdc_config.h"
#include "usb_class_cdc.h"
#include "usb_cdc.h"
#endif

#if ((defined USBCFG_DEV_DFU) && USBCFG_DEV_DFU)
#include "usb_dfu.h"
#endif

/*****************************************************************************
 * Local variables
 *****************************************************************************/ 
const device_class_map_t device_class_interface_map[] = 
{
#if USBCFG_DEV_HID
    {
        (device_class_init_call)((uint32_t)USB_Class_HID_Init),
        USB_Class_HID_Deinit,
        USB_Class_Hid_Event,
        USB_HID_Requests,
        USB_CLASS_HID
    },
#endif

#if USBCFG_DEV_MSC
    {
        (device_class_init_call)((uint32_t)USB_Class_MSC_Init),
        USB_Class_MSC_Deinit,
        USB_Class_MSC_Event,
        USB_MSC_Requests,
        USB_CLASS_MASS_STORAGE
    },
#endif

#if ((defined USBCFG_DEV_AUDIO) && USBCFG_DEV_AUDIO)
    {
        (device_class_init_call)((uint32_t)USB_Class_Audio_Init),
        USB_Class_Audio_Deinit,
        USB_Class_Audio_Event,
        USB_Audio_Requests,
        USB_CLASS_AUDIO
    },
#endif

#if ((defined USBCFG_DEV_VIDEO) && (USBCFG_DEV_VIDEO))
    {
        (device_class_init_call)((uint32_t)USB_Class_Video_Init),
        USB_Class_Video_Deinit,
        USB_Class_Video_Event,
        USB_Video_Requests,
        USB_CLASS_VIDEO
    },
#endif

#if USBCFG_DEV_PHDC
    {
        (device_class_init_call)((uint32_t)USB_Class_PHDC_Init),
        USB_Class_PHDC_Deinit,
        USB_Class_PHDC_Event,
        USB_PHDC_Requests,
        USB_CLASS_PHDC
    },
#endif

#if USBCFG_DEV_CDC
    {
        (device_class_init_call)((uint32_t)USB_Class_CDC_Init),
        USB_Class_CDC_Deinit,
        USB_Class_CDC_Event,
        USB_CDC_Other_Requests,
        USB_CLASS_COMMUNICATION
    },
#endif
};



static composite_device_struct_t s_composite_device_struct[USBCFG_DEV_MAX_CLASS_OBJECT];

/*****************************************************************************
 * Local functions prototypes
 *****************************************************************************/
void USB_Composite_Event (uint8_t event, void* val,void * arg);

usb_status USB_Composite_Requests (
usb_setup_struct_t * setup_packet, /* [IN] Setup packet received */
uint8_t * *data,
uint32_t *size,
void* arg );


/*************************************************************************//*!
 *
 * @name  USB_Composite_Allocate_Handle
 *
 * @brief The function reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static usb_status USB_Composite_Allocate_Handle(composite_device_struct_t** pclassobj)
{
    int32_t cnt = 0;
    for (;cnt< USBCFG_DEV_MAX_CLASS_OBJECT;cnt++)
    {
        if (s_composite_device_struct[cnt].handle == NULL)
        {
            *pclassobj = &s_composite_device_struct[cnt];
            return USB_OK;
        }
    }
    return USBERR_DEVICE_BUSY;
}

/*************************************************************************//*!
 *
 * @name  USB_Class_Free_Handle
 *
 * @brief The function releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/
static usb_status USB_Composite_Free_Handle(composite_device_struct_t* handle)
{
    int32_t cnt = 0;
    for (;cnt< USBCFG_DEV_MAX_CLASS_OBJECT;cnt++)
    {
        if (&s_composite_device_struct[cnt] == handle)
        {
            s_composite_device_struct[cnt].handle = NULL;
            s_composite_device_struct[cnt].class_handle = 0;
            s_composite_device_struct[cnt].user_handle = 0;
            s_composite_device_struct[cnt].cl_count = 0;
            return USB_OK;
        }
    }

    return USBERR_INVALID_PARAM;
}

/*****************************************************************************
 * Global functions
 *****************************************************************************/

/**************************************************************************//*!
 *
 * @name  USB_Composite_Init
 *
 * @brief   The function initializes the Device and Controller layer
 *
 * @param   controller_id               : Controller ID
 * @param   composite_callback_ptr      : Pointer to app callback  
 *
 * @return status:
 *                        USB_OK        : When Successfully
 *                        Others        : When Error
 *
 ******************************************************************************
 *This function initializes the Composite layer
 *****************************************************************************/
usb_status USB_Composite_Init
(
    uint8_t controller_id, /* [IN] Controller ID */
    composite_config_struct_t *composite_callback_ptr, /* [IN] Pointer to class info */
    composite_handle_t * compositeHandle
)
{
    uint32_t                     intf_map_count;
    usb_status                   status = USB_OK;
    composite_device_struct_t*   devicePtr;
    usb_composite_info_struct_t* usb_composite_info;
    uint8_t                      count;

    if (NULL == composite_callback_ptr)
    {
        return USBERR_ERROR;
    }
    
    if (USB_OK != USB_Composite_Allocate_Handle(&devicePtr))
    {
#if _DEBUG
        USB_PRINTF("USB_Class_COMPOSITE_Init: Memalloc devicePtr failed\n");
#endif  
        return USBERR_ALLOC;
    }
    devicePtr->cl_count = composite_callback_ptr->count;
    if (CONFIG_MAX < devicePtr->cl_count)
    {
#if _DEBUG
        USB_PRINTF("USB_Class_COMPOSITE_Init: Too many class\n");
#endif  
        return USBERR_ERROR;
    }


    OS_Mem_copy(composite_callback_ptr->class_app_callback,
    devicePtr->class_app_callback,devicePtr->cl_count*sizeof(class_config_struct_t));

    status = usb_device_init(controller_id, (void* )&composite_callback_ptr->class_app_callback[0].board_init_callback, &devicePtr->handle);
    devicePtr->class_app_callback->desc_callback_ptr->get_desc_entity((uint32_t)devicePtr->handle,
    USB_COMPOSITE_INFO,
    (uint32_t *)&usb_composite_info);
    OS_Mem_copy(usb_composite_info,
    &devicePtr->class_composite_info , sizeof(usb_composite_info_struct_t));
    if(status == USB_OK)
    {
        /* Initialize the generic class functions */
        devicePtr->class_handle = USB_Class_Init(devicePtr->handle,USB_Composite_Event, USB_Composite_Requests,(void *)devicePtr,
        composite_callback_ptr->class_app_callback[0].desc_callback_ptr);
        
        if ((class_handle_t)NULL == devicePtr->class_handle)
        {
            USB_Composite_Free_Handle(devicePtr);
            usb_device_deinit(devicePtr->handle);
            return USBERR_INIT_FAILED;
        }
        
        for(count = 0; count < USBCFG_DEV_MAX_CLASS_OBJECT; count++)
        {
            if (devicePtr->class_handle == g_class_handle[count].class_handle)
            {
                USB_Composite_Free_Handle(devicePtr);
                usb_device_deinit(devicePtr->handle);
                return USBERR_DEVICE_BUSY;
            }
            if ((class_handle_t)NULL == g_class_handle[count].class_handle)
            {
                g_class_handle[count].class_handle = devicePtr->class_handle;
                g_class_handle[count].controller_id = controller_id;
                break;
            }
        }

        for(count = 0; count < devicePtr->cl_count; count++)
        {
            /* Initializes sub_classes */
            for (intf_map_count = 0; intf_map_count < (sizeof(device_class_interface_map)/sizeof(device_class_map_t));intf_map_count++)
            {
                if (device_class_interface_map[intf_map_count].type == devicePtr->class_app_callback[count].type)
                {
                    (void)device_class_interface_map[intf_map_count].class_init(controller_id,&devicePtr->class_app_callback[count],
                                                                 &devicePtr->class_app_callback[count].class_handle);
                    composite_callback_ptr->class_app_callback[count].class_handle = devicePtr->class_app_callback[count].class_handle;
                    break;
                }
            }
        }
    }

    /* Initialize the device layer*/
    *compositeHandle = (composite_handle_t)devicePtr;

    status = usb_device_postinit(controller_id,devicePtr->handle);
    return status;
}

/**************************************************************************//*!
 *
 * @name  USB_Composite_DeInit
 *
 * @brief   The function De-initializes the Device and Controller layer
 *
 * @param   controller_id               : Controller ID
 * @param   composite_callback_ptr      : Pointer to app callback  
 *
 * @return status:
 *                        USB_OK        : When Successfully
 *                        Others        : When Error
 *
 ******************************************************************************
 *This function De-initializes the Composite layer
 *****************************************************************************/
usb_status USB_Composite_DeInit
(
    composite_handle_t handle /* [IN] Controller ID */
)
{
    composite_device_struct_t *devicePtr;
    uint32_t                   intf_map_count;
    uint8_t                    count;
    usb_status status = USB_OK;
    if (handle == 0)
    {
        return USBERR_ERROR;
    }

    devicePtr = (composite_device_struct_t *)handle;

    if (NULL == devicePtr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }

    for(count = 0; count < devicePtr->cl_count; count++)
    {
        /* Deinitializes sub_classes */
        for (intf_map_count = 0; intf_map_count < (sizeof(device_class_interface_map)/sizeof(device_class_map_t));intf_map_count++)
        {
            if (device_class_interface_map[intf_map_count].type == devicePtr->class_app_callback[count].type)
            {
                (void)device_class_interface_map[intf_map_count].class_deinit(devicePtr->class_app_callback[count].class_handle);
                break;
            }
        }
    }

    for(count = 0; count < USBCFG_DEV_MAX_CLASS_OBJECT; count++)
    {
        if (devicePtr->class_handle == g_class_handle[count].class_handle)
        {
            g_class_handle[count].class_handle = 0;
            g_class_handle[count].controller_id = 0;
            break;
        }
    }

    if(status == USB_OK)
    {
        /* De-initialize the generic class functions */
        status = USB_Class_Deinit(devicePtr->handle,devicePtr->class_handle);
    }
    if(status == USB_OK)
    {
        /* De-initialize the device layer*/
        status = usb_device_deinit(devicePtr->handle);
    }
    
    USB_Composite_Free_Handle(devicePtr);

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Composite_Event
 *
 * @brief The function initializes composite endpoint
 *
 * @param controller_id     : Controller ID
 * @param event             : Event Type
 * @param val               : Pointer to configuration Value
 *
 * @return None
 *
 ******************************************************************************
 * 
 *****************************************************************************/
void USB_Composite_Event (uint8_t event, void* val,void * arg)
{
    composite_device_struct_t *devicePtr;
    uint32_t                   intf_map_count;
    uint8_t                    count;

    devicePtr = (composite_device_struct_t *)arg;

    if (NULL == devicePtr)
    {
        return;
    }

    for(count = 0; count < devicePtr->cl_count; count++)
    {
        /* Event callback */
        for (intf_map_count = 0; intf_map_count < (sizeof(device_class_interface_map)/sizeof(device_class_map_t));intf_map_count++)
        {
            if (device_class_interface_map[intf_map_count].type == devicePtr->class_app_callback[count].type)
            {
                (void)device_class_interface_map[intf_map_count].class_event_callback(event,val,(void *)devicePtr->class_app_callback[count].class_handle);
                break;
            }
        }
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Composite_Other_Requests
 *
 * @brief   The function provides flexibility to add class and vendor specific
 *              requests
 *
 * @param controller_id     : Controller ID
 * @param setup_packet      : Setup packet received
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 *
 * @return status:
 *                        USB_OK        : When Successfully
 *                        Others        : When Error
 *
 ******************************************************************************
 * Handles Class requests and forwards vendor specific request to the
 * application
 *****************************************************************************/
usb_status USB_Composite_Requests (
usb_setup_struct_t * setup_packet, /* [IN] Setup packet received */
uint8_t * *data,
uint32_t *size,
void* arg)
{
    composite_device_struct_t *devicePtr;
    uint32_t                   intf_map_count;
    usb_status                 status = USB_OK;
    uint8_t                    count;
    uint8_t                    itf_num = 0xFF;
    uint8_t                    type_sel;

    devicePtr = (composite_device_struct_t *)arg;

    if (NULL == devicePtr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }

    if(setup_packet->request_type & 0x01)
    {
        itf_num = (setup_packet->index);
    }

    for (type_sel = 0;type_sel < devicePtr->class_composite_info.count;type_sel++)
    {
        if (itf_num == devicePtr->class_composite_info.class_handle[type_sel].interfaces.interface->index)
        {
            /* Request callback */
            for (intf_map_count = 0; intf_map_count < (sizeof(device_class_interface_map)/sizeof(device_class_map_t));intf_map_count++)
            {
                if (device_class_interface_map[intf_map_count].type == devicePtr->class_composite_info.class_handle[type_sel].type)
                {
                    for(count = 0; count < devicePtr->cl_count; count++)
                    {
                        if (device_class_interface_map[intf_map_count].type == devicePtr->class_app_callback[count].type)
                        {
                            status = device_class_interface_map[intf_map_count].class_request_callback(setup_packet,data,size,(void *)devicePtr->class_app_callback[count].class_handle);
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return status;
}

/**************************************************************************//*!
 *
 * @name  USB_Composite_Get_Speed
 *
 * @brief This functions get speed from Host.
 *
 * @param handle          :   handle returned by USB_Composite_Init
 * @param speed           :   speed
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
usb_status USB_Composite_Get_Speed
(
composite_handle_t handle,
uint16_t * speed/* [OUT] the requested error */
)
{
    composite_device_struct_t * composite_obj_ptr;
    usb_status error = USB_OK;

    composite_obj_ptr = (composite_device_struct_t *)handle;
    if (NULL == composite_obj_ptr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }
    error = USB_Class_Get_Status(composite_obj_ptr->class_handle, USB_STATUS_SPEED, speed);

    return error;
}

/* EOF */
