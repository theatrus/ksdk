/**HEADER********************************************************************
* 
* Copyright (c) 2004-2010, 2015 Freescale Semiconductor;
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
* $FileName: usb_video.c$
* $Version : 
* $Date    : 
*
* Comments:
*        The file contains USB stack Video layer implementation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"

 
#if USBCFG_DEV_VIDEO
    #include "usb_class_internal.h"
    #include "usb_class_video.h"
    #include "usb_video.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
video_device_struct_t g_video_class[MAX_VIDEO_DEVICE];

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
    
/*************************************************************************//*!
 *
 * @name  USB_Video_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static usb_status USB_Video_Allocate_Handle(video_device_struct_t** handle)
{
    uint32_t cnt = 0;
    for (;cnt< MAX_VIDEO_DEVICE;cnt++)
    {
        if (g_video_class[cnt].handle == NULL)
        {
            *handle = (video_device_struct_t*)&g_video_class[cnt];
            return USB_OK;
        }
    }
    return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Video_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/

static usb_status USB_Video_Free_Handle(video_device_struct_t* handle)
{
    int32_t cnt = 0;
    for (;cnt< MAX_VIDEO_DEVICE;cnt++)
    {
        if ((&g_video_class[cnt]) == handle)
        {
            OS_Mem_zero((void*)handle, sizeof(video_device_struct_t));
            return USB_OK;
        }
    }
    return USBERR_INVALID_PARAM;
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Video_Interrupt_IN
 *
 * @brief The funtion is callback function of Video Status Interrupt endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
 void USB_Service_Video_Interrupt_IN(usb_event_struct_t* event,void* arg)
{
    video_device_struct_t*  video_ptr;

    video_ptr = (video_device_struct_t*)arg;

    if (NULL == video_ptr)
    {
    #if _DEBUG
            USB_PRINTF("USB_Service_Video_Interrupt_IN: video_ptr is NULL\n");
    #endif  
        return;
    }

    /* Callback to application */
    if (video_ptr->class_specific_callback.callback != NULL)
    {
        /* notify the app of the send complete */
        video_ptr->class_specific_callback.callback(USB_DEV_EVENT_SEND_COMPLETE, USB_REQ_VAL_INVALID, 
                        &event->buffer_ptr, &event->len, video_ptr->class_specific_callback.arg);
    }

}

/**************************************************************************//*!
 *
 * @name  USB_Service_Video_Isochronous_IN
 *
 * @brief The funtion ic callback function of Video Isochronous IN endpoint 
 *
 * @param event
 *
 * @return None       
 *
 ****************************************************************************
 * The function is called in response to Isochronous IN Service  
 ******************************************************************************/
void USB_Service_Video_Isochronous_IN(usb_event_struct_t* event, void* arg)
{
    video_device_struct_t *video_ptr = (video_device_struct_t*)arg;

    if (NULL == video_ptr)
    {
        #if _DEBUG
            USB_PRINTF("USB_Service_Video_Isochronous_IN: video_ptr is NULL\n");
        #endif  
        return;
    }

    if (video_ptr->class_specific_callback.callback != NULL)
    {
        video_ptr->class_specific_callback.callback(USB_DEV_EVENT_SEND_COMPLETE, USB_VIDEO_ISO_REQ_VAL_INVALID, 
                         &event->buffer_ptr, &event->len, video_ptr->class_specific_callback.arg);
    }/*EndIf*/
}/*EndBody*/

/**************************************************************************//*!
 *
 * @name  USB_Service_Video_Isochronous_OUT
 *
 * @brief The funtion ic callback function of Video Isochronous OUT endpoint 
 *
 * @param event
 *
 * @return None       
 *
 ****************************************************************************
 * The function is called in response to Isochronous OUT Service  
 ******************************************************************************/
void USB_Service_Video_Isochronous_OUT(usb_event_struct_t* event, void* arg)
{
    video_device_struct_t *video_ptr = (video_device_struct_t*)arg;

    if (NULL == video_ptr)
    {
        #if _DEBUG
            USB_PRINTF("USB_Service_Video_Isochronous_OUT: video_ptr is NULL\n");
        #endif  
        return;
    }

    if (video_ptr->class_specific_callback.callback != NULL)
    {
        video_ptr->class_specific_callback.callback(USB_DEV_EVENT_DATA_RECEIVED, USB_VIDEO_ISO_REQ_VAL_INVALID,
                         &event->buffer_ptr, &event->len, video_ptr->class_specific_callback.arg);
    }/*EndIf*/
}/*EndBody*/


/**************************************************************************//*!
 *
 * @name  USB_Service_Video_Bulk_IN
 *
 * @brief The funtion ic callback function of Video Bulk IN endpoint 
 *
 * @param event
 *
 * @return None       
 *
 ****************************************************************************
 * The function is called in response to Isochronous IN Service  
 ******************************************************************************/
void USB_Service_Video_Bulk_IN(usb_event_struct_t* event, void* arg)
{
    video_device_struct_t *video_ptr = (video_device_struct_t*)arg;

    if (NULL == video_ptr)
    {
        #if _DEBUG
            USB_PRINTF("USB_Service_Video_Bulk_IN: video_ptr is NULL\n");
        #endif  
        return;
    }

    if (video_ptr->class_specific_callback.callback != NULL)
    {
        video_ptr->class_specific_callback.callback(USB_DEV_EVENT_SEND_COMPLETE, USB_VIDEO_BULK_REQ_VAL_INVALID,
                         &event->buffer_ptr, &event->len, video_ptr->class_specific_callback.arg);
    }/*EndIf*/
}/*EndBody*/

/**************************************************************************//*!
 *
 * @name  USB_Service_Video_Bulk_OUT
 *
 * @brief The funtion ic callback function of Video Bulk OUT endpoint 
 *
 * @param event
 *
 * @return None       
 *
 ****************************************************************************
 * The function is called in response to Isochronous OUT Service  
 ******************************************************************************/
void USB_Service_Video_Bulk_OUT(usb_event_struct_t* event, void* arg)
{
    video_device_struct_t *video_ptr = (video_device_struct_t*)arg;

    if (NULL == video_ptr)
    {
        #if _DEBUG
            USB_PRINTF("USB_Service_Video_Bulk_OUT: video_ptr is NULL\n");
        #endif  
        return;
    }

    if (video_ptr->class_specific_callback.callback != NULL)
    {
        video_ptr->class_specific_callback.callback(USB_DEV_EVENT_DATA_RECEIVED, USB_VIDEO_BULK_REQ_VAL_INVALID,
                         &event->buffer_ptr, &event->len, video_ptr->class_specific_callback.arg);
    }/*EndIf*/
}/*EndBody*/


void USB_Video_Init_Control_Interface(video_device_struct_t* video_ptr)
{
    uint8_t          count;
    usb_ep_struct_t* ep_struct_ptr = NULL;
    /* intialize all non control endpoints */

    if (NULL != video_ptr->usb_control_ep_data)
    {
        count = 0;
        while (count < video_ptr->usb_control_ep_data->count) 
        {
            ep_struct_ptr = (usb_ep_struct_t*)&video_ptr->usb_control_ep_data->ep[count];

            (void)usb_device_init_endpoint(video_ptr->handle,
             ep_struct_ptr,TRUE);

            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                case USB_INTERRUPT_PIPE :
                    (void)usb_device_register_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))),
                        USB_Service_Video_Interrupt_IN,(void *)video_ptr);
                    break;
                default : break;
            }
            count++;                                                    
        }/* EndWhile */
    }
}

void USB_Video_Init_Stream_Interface(video_device_struct_t* video_ptr)
{
    uint8_t          count;
    usb_ep_struct_t* ep_struct_ptr = NULL;
    
    /* intialize all non control endpoints */
    if (NULL != video_ptr->usb_stream_ep_data)
    {
        count = 0;
        while (count < video_ptr->usb_stream_ep_data->count) 
        {
            ep_struct_ptr = (usb_ep_struct_t*)&video_ptr->usb_stream_ep_data->ep[count];

            (void)usb_device_init_endpoint(video_ptr->handle,
             ep_struct_ptr,TRUE);

            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                  case USB_ISOCHRONOUS_PIPE :
                    if (ep_struct_ptr->direction == USB_RECV) 
                    {
                    (void)usb_device_register_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))),
                        USB_Service_Video_Isochronous_OUT,(void *)video_ptr);
                    } 
                    else
                    {
                    (void)usb_device_register_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))),
                        USB_Service_Video_Isochronous_IN,(void *)video_ptr);
                    }
                    break;
                case USB_BULK_PIPE:
                    if (ep_struct_ptr->direction == USB_RECV) 
                    {
                    (void)usb_device_register_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))),
                        USB_Service_Video_Bulk_OUT,(void *)video_ptr);
                    } 
                    else
                    {
                    (void)usb_device_register_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))),
                        USB_Service_Video_Bulk_IN,(void *)video_ptr);
                    }
                    break;
                  default :
                    break;        
            }
            count++;                                                    
        }/* EndWhile */
    }
}

void USB_Video_Deinit_Control_Interface(video_device_struct_t* video_ptr)
{
    uint8_t          count;
    usb_ep_struct_t* ep_struct_ptr = NULL;

    if (NULL != video_ptr->usb_control_ep_data)
    {
        count = 0;
        while (count < video_ptr->usb_control_ep_data->count) 
        {
            ep_struct_ptr = (usb_ep_struct_t*)&video_ptr->usb_control_ep_data->ep[count];

#if USBCFG_DEV_ADVANCED_CANCEL_ENABLE            
            (void)USB_Class_Video_Cancel((video_handle_t)video_ptr,
             ep_struct_ptr->ep_num, ep_struct_ptr->direction);
#endif
            (void)usb_device_deinit_endpoint(video_ptr->handle,
             ep_struct_ptr->ep_num, ep_struct_ptr->direction);
  
            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                case USB_INTERRUPT_PIPE :
                    (void)usb_device_unregister_service(video_ptr->handle,
                        (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))));
                    break;
                default : break;   
            }
            count++;
        }/* EndWhile */
    }
}

void USB_Video_Deinit_Stream_Interface(video_device_struct_t* video_ptr)
{
    uint8_t          count;
    usb_ep_struct_t* ep_struct_ptr = NULL;

    if (NULL != video_ptr->usb_stream_ep_data)
    {
        count = 0;
        while (count < video_ptr->usb_stream_ep_data->count) 
        {
            ep_struct_ptr = (usb_ep_struct_t*)&video_ptr->usb_stream_ep_data->ep[count];

#if USBCFG_DEV_ADVANCED_CANCEL_ENABLE            
            (void)USB_Class_Video_Cancel((video_handle_t)video_ptr,
             ep_struct_ptr->ep_num, ep_struct_ptr->direction);
#endif
            (void)usb_device_deinit_endpoint(video_ptr->handle,
             ep_struct_ptr->ep_num, ep_struct_ptr->direction);
  
            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                  case USB_ISOCHRONOUS_PIPE :
                      if (ep_struct_ptr->direction == USB_RECV) 
                      {
                          (void)usb_device_unregister_service(video_ptr->handle,
                            (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))));
                      } 
                      else
                      {
                          (void)usb_device_unregister_service(video_ptr->handle,
                            (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))));
                      }
                      break;
                case USB_BULK_PIPE:
                      if (ep_struct_ptr->direction == USB_RECV) 
                      {
                          (void)usb_device_unregister_service(video_ptr->handle,
                            (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))));
                      } 
                      else
                      {
                          (void)usb_device_unregister_service(video_ptr->handle,
                            (uint8_t)((uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num) | ((uint8_t)(ep_struct_ptr->direction << 7))));
                      }
                      break;
                  default : break;   
            }
            count++;                                                    
        }/* EndWhile */
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Event
 *
 * @brief The funtion initializes Video endpoint
 *
 * @param controller_ID     : Controller ID
 * @param event             : Event Type
 * @param val               : Pointer to configuration Value
 *
 * @return None
 *
 ******************************************************************************
 * The funtion initializes the Video endpoints when event of comleted enumeration is
 * received
 *****************************************************************************/
void USB_Class_Video_Event(uint8_t event, void* val,void* arg)
{
    video_device_struct_t *      video_ptr;
    usb_if_struct_t*             if_struct_ptr;
    usb_class_struct_t*          usbclass;
    uint8_t                      index = 0;
    uint8_t                      if_index = 0;
    uint8_t                      control_interface_ep = 0;
    
    video_ptr = (video_device_struct_t *)arg;
    
    if (NULL == video_ptr)
    {
        #if _DEBUG
            USB_PRINTF("USB_Class_Video_Event: video_ptr is NULL\n");
        #endif  
        return;
    }
    
    switch (event)
    {
    case USB_DEV_EVENT_CONFIG_CHANGED:
        {
            USB_Video_Deinit_Control_Interface(video_ptr);
            USB_Video_Deinit_Stream_Interface(video_ptr);

            video_ptr->control_interface_alternate = 0;
            video_ptr->stream_interface_alternate = 0;
            video_ptr->control_interface_index = 0xFF;
            video_ptr->stream_interface_index = 0xFF;
            if (USB_UNINITIALIZED_VAL_32 != USB_Class_Get_Class_Handle(video_ptr->controller_id))
            {
                uint8_t                      type_sel;
                usb_composite_info_struct_t* usb_composite_info;
                uint32_t                     interface_index = 0xFF;
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_COMPOSITE_INFO, (uint32_t *)&usb_composite_info);
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_CLASS_INTERFACE_INDEX_INFO, (uint32_t *)&interface_index);

                if (interface_index == 0xFF)
                {
                    USB_PRINTF("not find interface index\n");
                    return;
                }
                
                for(type_sel = 0;type_sel < usb_composite_info->count;type_sel++)
                {
                    if ((usb_composite_info->class_handle[type_sel].type == USB_CLASS_VIDEO) && (type_sel == interface_index))
                    {
                        break;
                    }
                }
                if (type_sel >= usb_composite_info->count)
                {
                    USB_PRINTF("not find video interface\n");
                    return;
                }
                usbclass = &usb_composite_info->class_handle[type_sel];
            }
            else
            {
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_CLASS_INFO, (uint32_t *)&usbclass);
            }

            video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                USB_VIDEO_CTL_IF_INDEX_INFO, (uint32_t *)&video_ptr->control_interface_index);
            video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                USB_VIDEO_STREAM_IF_INDEX_INFO, (uint32_t *)&video_ptr->stream_interface_index);

            for (if_index = 0; if_index < usbclass->interfaces.count; if_index++)
            {
                if_struct_ptr = &usbclass->interfaces.interface[if_index];
                if ((if_index == video_ptr->control_interface_index) && (if_struct_ptr->alternate_setting == video_ptr->control_interface_alternate))
                {
                    video_ptr->usb_control_ep_data= (usb_endpoints_t *) &if_struct_ptr->endpoints;
                }
                else if ((if_index == video_ptr->stream_interface_index) && (if_struct_ptr->alternate_setting == video_ptr->stream_interface_alternate))
                {
                    video_ptr->usb_stream_ep_data= (usb_endpoints_t *) &if_struct_ptr->endpoints;
                }
            }

            index = 0;
            if (NULL != video_ptr->usb_control_ep_data)
            {
                for(; index < video_ptr->usb_control_ep_data->count; index++)
                {
                    video_ptr->video_endpoint_data.ep[index].endpoint = video_ptr->usb_control_ep_data->ep[index].ep_num;
                    video_ptr->video_endpoint_data.ep[index].type = video_ptr->usb_control_ep_data->ep[index].type;
        #if VIDEO_IMPLEMENT_QUEUING
                    video_ptr->video_endpoint_data.ep[index].bin_consumer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].bin_producer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].queue_num = 0x00;
        #endif
                }
                if (video_ptr->usb_control_ep_data->count > MAX_VIDEO_CLASS_CTL_EP_NUM)
                {
                    USB_PRINTF("too many control endpoint for the video class driver\n");
                    return;
                }
            }

            if (NULL != video_ptr->usb_stream_ep_data)
            {
                control_interface_ep = index;
                for(;index < (video_ptr->usb_stream_ep_data->count + control_interface_ep); index++)
                {
                    video_ptr->video_endpoint_data.ep[index].endpoint = video_ptr->usb_stream_ep_data->ep[index - control_interface_ep].ep_num;
                    video_ptr->video_endpoint_data.ep[index].type = video_ptr->usb_stream_ep_data->ep[index - control_interface_ep].type;
        #if VIDEO_IMPLEMENT_QUEUING
                    video_ptr->video_endpoint_data.ep[index].bin_consumer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].bin_producer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].queue_num = 0x00;
        #endif
                }
                if (video_ptr->usb_stream_ep_data->count > MAX_VIDEO_CLASS_STREAM_EP_NUM)
                {
                    USB_PRINTF("too many video endpoint for the video class driver\n");
                    return;
                }
            }

            video_ptr->video_endpoint_data.count = index;

            USB_Video_Init_Control_Interface(video_ptr);
            USB_Video_Init_Stream_Interface(video_ptr);
        }
        break;
    case USB_DEV_EVENT_ENUM_COMPLETE:
        {
        
        }
        break;
    case USB_DEV_EVENT_BUS_RESET:
        {
    #if VIDEO_IMPLEMENT_QUEUING
            /* clear producer and consumer on reset */
            for(index = 0; index < video_ptr->video_endpoint_data.count; index++)
            {
                video_ptr->video_endpoint_data.ep[index].bin_consumer = 0x00;
                video_ptr->video_endpoint_data.ep[index].bin_producer = 0x00;
                video_ptr->video_endpoint_data.ep[index].queue_num    = 0x00;
            }
    #endif
        }
        break;
    case USB_DEV_EVENT_INTERFACE_CHANGED:
        {
            uint16_t interface_setting = *((uint16_t*)val);
            uint8_t interface_alternate = (uint8_t)(interface_setting&0x00FF);
            uint8_t interface_num = (uint8_t)((uint16_t)(interface_setting>>8)&0x00FF);

            video_ptr->control_interface_index = 0xFF;
            video_ptr->stream_interface_index = 0xFF;
            
            video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                USB_VIDEO_CTL_IF_INDEX_INFO, (uint32_t *)&video_ptr->control_interface_index);
            video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                USB_VIDEO_STREAM_IF_INDEX_INFO, (uint32_t *)&video_ptr->stream_interface_index);
            
            if (interface_num  == video_ptr->control_interface_index)
            {
                if (interface_alternate != video_ptr->control_interface_alternate)
                {
                    USB_Video_Deinit_Control_Interface(video_ptr);
                    video_ptr->control_interface_alternate = interface_alternate;
                }
                else
                {
                    break;
                }
            }
            else if (interface_num  == video_ptr->stream_interface_index)
            {
                if (interface_alternate != video_ptr->stream_interface_alternate)
                {
                    USB_Video_Deinit_Stream_Interface(video_ptr);
                    video_ptr->stream_interface_alternate = interface_alternate;
                }
                else
                {
                    break;
                }
            }
            else
            {
                return;
            }

            if (USB_UNINITIALIZED_VAL_32 != USB_Class_Get_Class_Handle(video_ptr->controller_id))
            {
                uint8_t                      type_sel;
                usb_composite_info_struct_t* usb_composite_info;
                uint32_t                     interface_index = 0xFF;
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_COMPOSITE_INFO, (uint32_t *)&usb_composite_info);
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_CLASS_INTERFACE_INDEX_INFO, (uint32_t *)&interface_index);
              
                if (interface_index == 0xFF)
                {
                    USB_PRINTF("not find interface index\n");
                    return;
                }
                
                for(type_sel = 0;type_sel < usb_composite_info->count;type_sel++)
                {
                    if ((usb_composite_info->class_handle[type_sel].type == USB_CLASS_VIDEO) && (type_sel == interface_index))
                    {
                        break;
                    }
                }
                if (type_sel >= usb_composite_info->count)
                {
                    USB_PRINTF("not find video interface\n");
                    return;
                }
                usbclass = &usb_composite_info->class_handle[type_sel];
            }
            else
            {
                video_ptr->desc_callback.get_desc_entity((uint32_t)video_ptr,
                    USB_CLASS_INFO, (uint32_t *)&usbclass);
            }

            for (if_index = 0; if_index < usbclass->interfaces.count; if_index++)
            {
                if_struct_ptr = &usbclass->interfaces.interface[if_index];
                if ((interface_num == video_ptr->control_interface_index) && (if_struct_ptr->alternate_setting == video_ptr->control_interface_alternate))
                {
                    video_ptr->usb_control_ep_data= (usb_endpoints_t *) &if_struct_ptr->endpoints;
                }
                else if ((interface_num == video_ptr->stream_interface_index) && (if_struct_ptr->alternate_setting == video_ptr->stream_interface_alternate))
                {
                    video_ptr->usb_stream_ep_data= (usb_endpoints_t *) &if_struct_ptr->endpoints;
                }
            }

            if ((interface_num == video_ptr->control_interface_index) && (NULL != video_ptr->usb_control_ep_data))
            {
                if (video_ptr->usb_control_ep_data->count > MAX_VIDEO_CLASS_CTL_EP_NUM)
                {
                    USB_PRINTF("too many control endpoint for the video class driver\n");
                    return;
                }
            }

            if ((interface_num == video_ptr->stream_interface_index) && (NULL != video_ptr->usb_stream_ep_data))
            {
                if (video_ptr->usb_stream_ep_data->count > MAX_VIDEO_CLASS_STREAM_EP_NUM)
                {
                    USB_PRINTF("too many video endpoint for the video class driver\n");
                    return;
                }
            }

            index = 0;
            if (NULL != video_ptr->usb_control_ep_data)
            {
                for(; index < video_ptr->usb_control_ep_data->count; index++)
                {
                    video_ptr->video_endpoint_data.ep[index].endpoint = video_ptr->usb_control_ep_data->ep[index].ep_num;
                    video_ptr->video_endpoint_data.ep[index].type = video_ptr->usb_control_ep_data->ep[index].type;
        #if VIDEO_IMPLEMENT_QUEUING
                    video_ptr->video_endpoint_data.ep[index].bin_consumer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].bin_producer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].queue_num = 0x00;
        #endif
                }
            }
            
            if (NULL != video_ptr->usb_stream_ep_data)
            {
                control_interface_ep = index;
                for(;index < (video_ptr->usb_stream_ep_data->count + control_interface_ep); index++)
                {
                    video_ptr->video_endpoint_data.ep[index].endpoint = video_ptr->usb_stream_ep_data->ep[index - control_interface_ep].ep_num;
                    video_ptr->video_endpoint_data.ep[index].type = video_ptr->usb_stream_ep_data->ep[index - control_interface_ep].type;
        #if VIDEO_IMPLEMENT_QUEUING
                    video_ptr->video_endpoint_data.ep[index].bin_consumer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].bin_producer = 0x00;
                    video_ptr->video_endpoint_data.ep[index].queue_num = 0x00;
        #endif
                }
            }
            video_ptr->video_endpoint_data.count = index;

            if (interface_num  == video_ptr->control_interface_index)
            {
                USB_Video_Init_Control_Interface(video_ptr);
            }
            else if (interface_num  == video_ptr->stream_interface_index)
            {
                USB_Video_Init_Stream_Interface(video_ptr);
            }
        }
        break;
    default:
        break;
    }
    
    if (video_ptr->video_application_callback.callback != NULL) 
    {
        video_ptr->video_application_callback.callback(event,
            val,video_ptr->video_application_callback.arg);
    } 
}

/**************************************************************************//*!
 *
 * @name: USB_Video_Get_Power_Mode_Requests 
 *
 * @brief This function is called in response to Get Power Mode Requests 
 *
 * @param video_ptr         : Controller Handle
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 *****************************************************************************/
static uint8_t USB_Video_Get_Power_Mode_Requests(
    video_device_struct_t* video_ptr,               /* [IN] Controller Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    switch(setup_packet->request)
    {
        case GET_CUR:
            cmd = GET_CUR_VC_POWER_MODE_CONTROL;
            break;
    
        case GET_INFO:
            cmd = GET_INFO_VC_POWER_MODE_CONTROL;
            break;
    
        default:
            break;
    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }
    return status;
}/*EndBody*/


/**************************************************************************//*!
 *
 * @name: USB_Desc_Get_Error_Code_Requests 
 *
 * @brief This function is called in response to Get Error Code Requests 
 *
 * @param video_ptr         : Controller Handle
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 *****************************************************************************/
static uint8_t USB_Desc_Get_Error_Code_Requests(
    video_device_struct_t* video_ptr,               /* [IN] Controller Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    switch(setup_packet->request)
    {
        case GET_CUR:
            cmd = GET_CUR_VC_ERROR_CODE_CONTROL;
            break;
    
        case GET_INFO:
            cmd = GET_INFO_VC_ERROR_CODE_CONTROL;
            break;
    
        default:
            break;
    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }
    return status;
}/*EndBody*/


/**************************************************************************//*!
 *
 * @name: USB_Video_Get_Control_Interface_Requests 
 *
 * @brief This function is called in response to Get Interface Requests 
 *
 * @param controller_ID     : Controller ID
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 ******************************************************************************
 * The funtion Get Feature unit and Terminal requests. It is called in
 * USB_Other_Request function
 *****************************************************************************/
static uint8_t USB_Video_Get_Control_Interface_Requests(
    video_device_struct_t* video_ptr,               /* [IN] Controller Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */

)
{
    uint8_t status = USBERR_INVALID_REQ_TYPE;
    uint8_t control_selector=(uint8_t)(setup_packet->value>>8);

    switch(control_selector)
    {
        case VC_VIDEO_POWER_MODE_CONTROL:
            status=USB_Video_Get_Power_Mode_Requests(video_ptr, setup_packet, data, size);
            break;
    
        case VC_REQUEST_ERROR_CODE_CONTROL:
            status=USB_Desc_Get_Error_Code_Requests(video_ptr, setup_packet, data, size); 
            break;
    
        default:
            break;
    }/*EndSwitch*/        

    return status;
}/*EndBody*/


/**************************************************************************//*!
 *
 * @name: USB_Video_Set_Power_Mode_Requests 
 *
 * @brief This function is called in response to Set Power Mode Requests 
 *
 * @param video_ptr         : Controller Handle
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 *****************************************************************************/
static uint8_t USB_Video_Set_Power_Mode_Requests(
    video_device_struct_t* video_ptr,               /* [IN] Controller Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    switch(setup_packet->request)
    {
        case SET_CUR:
            cmd = SET_CUR_VC_POWER_MODE_CONTROL;
            break;
    
        default:
            break;
    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }
    return status;
}/*EndBody*/

/**************************************************************************//*!
 *
 * @name: USB_Video_Set_Control_Interface_Requests 
 *
 * @brief This function is called in response to Get Interface Requests 
 *
 * @param controller_ID     : Controller ID
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 ******************************************************************************
 * The funtion Get Feature unit and Terminal requests. It is called in
 * USB_Other_Request function
 *****************************************************************************/
static uint8_t USB_Video_Set_Control_Interface_Requests(
    video_device_struct_t* video_ptr,               /* [IN] Controller Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status = USBERR_INVALID_REQ_TYPE;
    uint8_t control_selector=(uint8_t)(setup_packet->value>>8);

    /*Body*/
    switch(control_selector)
    {
    case VC_VIDEO_POWER_MODE_CONTROL:
        status=USB_Video_Set_Power_Mode_Requests(video_ptr, setup_packet, data, size);
        break;

    default:
        break;

    }/*EndSwitch*/
    
    return status;

}/*EndBody*/

static uint8_t USB_Video_Set_Cur_Processing_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = SET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = SET_CUR_PU_BRIGHTNESS_CONTROL;
        break;
        
    case PU_CONTRACT_CONTROL:
        cmd = SET_CUR_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = SET_CUR_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        cmd = SET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = SET_CUR_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = SET_CUR_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = SET_CUR_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = SET_CUR_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        cmd = SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = SET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        cmd = SET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = SET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = SET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        cmd = SET_CUR_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        //cmd = SET_CUR_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        //cmd = SET_CUR_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case PU_CONTRAST_AUTO_CONTROL:
        cmd =SET_CUR_PU_CONTRAST_AUTO_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Cur_Processing_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_CUR_PU_BRIGHTNESS_CONTROL;
        break;
        
    case PU_CONTRACT_CONTROL:
        cmd = GET_CUR_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_CUR_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        cmd = GET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_CUR_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_CUR_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_CUR_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_CUR_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        cmd = GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        cmd = GET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        cmd = GET_CUR_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        cmd = GET_CUR_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        cmd = GET_CUR_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case PU_CONTRAST_AUTO_CONTROL:
        cmd = GET_CUR_PU_CONTRAST_AUTO_CONTROL;
        break;
        
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Min_Processing_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_MIN_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_MIN_PU_BRIGHTNESS_CONTROL;
        break;
        
    case PU_CONTRACT_CONTROL:
        cmd = GET_MIN_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_MIN_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        //cmd = GET_MIN_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_MIN_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_MIN_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_MIN_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_MIN_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_MIN_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        //cmd = GET_MIN_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_MIN_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        //cmd = GET_MIN_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_MIN_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_MIN_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        //cmd = GET_MIN_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        //cmd = GET_MIN_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        //cmd = GET_MIN_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Max_Processing_Unit(
   video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_MAX_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_MAX_PU_BRIGHTNESS_CONTROL;
        break;
        
    case PU_CONTRACT_CONTROL:
        cmd = GET_MAX_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_MAX_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        //cmd = GET_MAX_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_MAX_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_MAX_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_MAX_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_MAX_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_MAX_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        //cmd = GET_MAX_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_MAX_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        //cmd = GET_MAX_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_MAX_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_MAX_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        //cmd = GET_MAX_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        //cmd = GET_MAX_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        //cmd = GET_MAX_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Res_Processing_Unit(
   video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_RES_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_RES_PU_BRIGHTNESS_CONTROL;
        break;
        
    case PU_CONTRACT_CONTROL:
        cmd = GET_RES_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_RES_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        //cmd = GET_RES_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_RES_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_RES_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_RES_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_RES_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_RES_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        //cmd = GET_RES_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_RES_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        //cmd = GET_RES_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_RES_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_RES_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        //cmd = GET_RES_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        //cmd = GET_RES_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        //cmd = GET_RES_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;

}/*EndBody*/

static uint8_t USB_Video_Get_Info_Processing_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_INFO_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_INFO_PU_BRIGHTNESS_CONTROL;
        break;

    case PU_CONTRACT_CONTROL:
        cmd = GET_INFO_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_INFO_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        cmd = GET_INFO_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_INFO_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_INFO_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_INFO_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_INFO_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        cmd = GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_INFO_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        cmd = GET_INFO_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_INFO_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_INFO_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        cmd = GET_INFO_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        cmd = GET_INFO_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        cmd = GET_INFO_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case PU_CONTRAST_AUTO_CONTROL:
        cmd = GET_INFO_PU_CONTRAST_AUTO_CONTROL;
        break;
        
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Def_Processing_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case PU_BACKLIGHT_COMPENSATION_CONTROL:
        cmd = GET_DEF_PU_BACKLIGHT_COMPENSATION_CONTROL;
        break;

    case PU_BRIGHTNESS_CONTROL:
        cmd = GET_DEF_PU_BRIGHTNESS_CONTROL;
        break;

    case PU_CONTRACT_CONTROL:
        cmd = GET_DEF_PU_CONTRACT_CONTROL;
        break;
        
    case PU_GAIN_CONTROL:
        cmd = GET_DEF_PU_GAIN_CONTROL;
        break;
        
    case PU_POWER_LINE_FREQUENCY_CONTROL:
        cmd = GET_DEF_PU_POWER_LINE_FREQUENCY_CONTROL;
        break;
        
    case PU_HUE_CONTROL:
        cmd = GET_DEF_PU_HUE_CONTROL;
        break;
        
    case PU_SATURATION_CONTROL:
        cmd = GET_DEF_PU_SATURATION_CONTROL;
        break;
        
    case PU_SHARRNESS_CONTROL:
        cmd = GET_DEF_PU_SHARRNESS_CONTROL;
        break;
        
    case PU_GAMMA_CONTROL:
        cmd = GET_DEF_PU_GAMMA_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        cmd = GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        cmd = GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_CONTROL:
        cmd = GET_DEF_PU_WHITE_BALANCE_COMPONENT_CONTROL;
        break;
        
    case PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        cmd = GET_DEF_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_CONTROL:
        cmd = GET_DEF_PU_DIGITAL_MULTIPLIER_CONTROL;
        break;
        
    case PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        cmd = GET_DEF_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL;
        break;
        
    case PU_HUE_AUTO_CONTROL:
        cmd = GET_DEF_PU_HUE_AUTO_CONTROL;
        break;
        
    case PU_ANALOG_VIDEO_STANDARD_CONTROL:
        //cmd = GET_DEF_PU_ANALOG_VIDEO_STANDARD_CONTROL;
        break;
        
    case PU_ANALOG_LOCK_STATUS_CONTROL:
        //cmd = GET_DEF_PU_ANALOG_LOCK_STATUS_CONTROL;
        break;
        
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case PU_CONTRAST_AUTO_CONTROL:
        cmd = GET_DEF_PU_CONTRAST_AUTO_CONTROL;
        break;
        
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Set_Processing_Unit
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
){

    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*EndBody*/
    switch(setup_packet->request)
    {
    case SET_CUR:
        status=USB_Video_Set_Cur_Processing_Unit(video_ptr,setup_packet,data,size);
        break;

    default:
        break;

    }/*EndSwitch*/
    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Processing_Unit
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{

    uint8_t status = USBERR_INVALID_REQ_TYPE;
    
    /*Body*/
    switch(setup_packet->request)
    {
    case GET_CUR:
        status=USB_Video_Get_Cur_Processing_Unit(video_ptr,setup_packet,data,size);

        break;

    case GET_MIN:
        status=USB_Video_Get_Min_Processing_Unit(video_ptr,setup_packet,data,size);
        break;    

    case GET_MAX:
        status=USB_Video_Get_Max_Processing_Unit(video_ptr,setup_packet,data,size);
        break; 

    case GET_RES:
        status=USB_Video_Get_Res_Processing_Unit(video_ptr,setup_packet,data,size);

        break;

    case GET_LEN:

        break;      

    case GET_INFO:
        status=USB_Video_Get_Info_Processing_Unit(video_ptr,setup_packet,data,size);
        break;

    case GET_DEF:
        status=USB_Video_Get_Def_Processing_Unit(video_ptr,setup_packet,data,size);
        break;    

    default:
        break;

    }/*EndSwitch*/
    return status;

}/*EndBody*/


static uint8_t USB_Video_Set_Cur_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        cmd = SET_CUR_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        cmd = SET_CUR_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        cmd = SET_CUR_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        cmd = SET_CUR_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = SET_CUR_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = SET_CUR_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        cmd = SET_CUR_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        cmd = SET_CUR_CT_FOCUS_SIMPLE_CONTROL;
        break;
    case CT_DIGITAL_WINDOW_CONTROL:
        cmd = SET_CUR_CT_DIGITAL_WINDOW_CONTROL;
        break;
    case CT_REGION_OF_INTEREST_CONTROL:
        cmd = SET_CUR_CT_REGION_OF_INTEREST_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Cur_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        cmd = GET_CUR_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        cmd = GET_CUR_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        cmd = GET_CUR_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        cmd = GET_CUR_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_CUR_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_CUR_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        cmd = GET_CUR_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        cmd = GET_CUR_CT_FOCUS_SIMPLE_CONTROL;
        break;
    case CT_DIGITAL_WINDOW_CONTROL:
        cmd = GET_CUR_CT_DIGITAL_WINDOW_CONTROL;
        break;
    case CT_REGION_OF_INTEREST_CONTROL:
        cmd = GET_CUR_CT_REGION_OF_INTEREST_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Min_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        //cmd = GET_MIN_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        //cmd = GET_MIN_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        //cmd = GET_MIN_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        //cmd = GET_MIN_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_MIN_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        //cmd = GET_MIN_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        //cmd = GET_MIN_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_MIN_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_MIN_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_MIN_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_MIN_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        //cmd = GET_MIN_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        //cmd = GET_MIN_CT_FOCUS_SIMPLE_CONTROL;
        break;
    case CT_DIGITAL_WINDOW_CONTROL:
        cmd = GET_MIN_CT_DIGITAL_WINDOW_CONTROL;
        break;
    case CT_REGION_OF_INTEREST_CONTROL:
        cmd = GET_MIN_CT_REGION_OF_INTEREST_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/



static uint8_t USB_Video_Get_Max_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        //cmd = GET_MAX_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        //cmd = GET_MAX_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        //cmd = GET_MAX_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        //cmd = GET_MAX_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_MAX_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        //cmd = GET_MAX_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        //cmd = GET_MAX_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_MAX_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_MAX_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_MAX_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_MAX_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        //cmd = GET_MAX_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        //cmd = GET_MAX_CT_FOCUS_SIMPLE_CONTROL;
        break;
    case CT_DIGITAL_WINDOW_CONTROL:
        cmd = GET_MAX_CT_DIGITAL_WINDOW_CONTROL;
        break;
    case CT_REGION_OF_INTEREST_CONTROL:
        cmd = GET_MAX_CT_REGION_OF_INTEREST_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Res_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        //cmd = GET_RES_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        cmd = GET_RES_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        //cmd = GET_RES_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        //cmd = GET_RES_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_RES_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        //cmd = GET_RES_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        //cmd = GET_RES_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_RES_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_RES_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_RES_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_RES_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        //cmd = GET_RES_CT_PRIVACY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Info_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        cmd = GET_INFO_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        cmd = GET_INFO_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        cmd = GET_INFO_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        cmd = GET_INFO_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_INFO_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_INFO_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        cmd = GET_INFO_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        cmd = GET_INFO_CT_FOCUS_SIMPLE_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Def_Camera_Terminal(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case CT_SCANNING_MODE_CONTROL:
        //cmd = GET_DEF_CT_SCANNING_MODE_CONTROL;
        break;
    case CT_AE_MODE_CONTROL:
        cmd = GET_DEF_CT_AE_MODE_CONTROL;
        break;
    case CT_AE_PRIORITY_CONTROL:
        //cmd = GET_DEF_CT_AE_PRIORITY_CONTROL;
        break;
    case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
        break;
    case CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        //cmd = GET_DEF_CT_EXPOSURE_TIME_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_FOCUS_ABSOLUTE_CONTROL;
        break;
    case CT_FOCUS_RELATIVE_CONTROL:
        cmd = GET_DEF_CT_FOCUS_RELATIVE_CONTROL;
        break;
    case CT_FOCUS_AUTO_CONTROL:
        cmd = GET_DEF_CT_FOCUS_AUTO_CONTROL;
        break;
    case CT_IRIS_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_IRIS_ABSOLUTE_CONTROL;
        break;
    case CT_IRIS_RELATIVE_CONTROL:
        //cmd = GET_DEF_CT_IRIS_RELATIVE_CONTROL;
        break;
    case CT_ZOOM_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_ZOOM_ABSOLUTE_CONTROL;
        break;
    case CT_ZOOM_RELATIVE_CONTROL:
        cmd = GET_DEF_CT_ZOOM_RELATIVE_CONTROL;
        break;
    case CT_PANTILT_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_PANTILT_ABSOLUTE_CONTROL;
        break;
    case CT_PANTILT_RELATIVE_CONTROL:
        cmd = GET_DEF_CT_PANTILT_RELATIVE_CONTROL;
        break;
    case CT_ROLL_ABSOLUTE_CONTROL:
        cmd = GET_DEF_CT_ROLL_ABSOLUTE_CONTROL;
        break;
    case CT_ROLL_RELATIVE_CONTROL:
        cmd = GET_DEF_CT_ROLL_RELATIVE_CONTROL;
        break;
    case CT_PRIVACY_CONTROL:
        //cmd = GET_DEF_CT_PRIVACY_CONTROL;
        break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    case CT_FOCUS_SIMPLE_CONTROL:
        cmd = GET_DEF_CT_FOCUS_SIMPLE_CONTROL;
        break;
    case CT_DIGITAL_WINDOW_CONTROL:
        cmd = GET_DEF_CT_DIGITAL_WINDOW_CONTROL;
        break;
    case CT_REGION_OF_INTEREST_CONTROL:
        cmd = GET_DEF_CT_REGION_OF_INTEREST_CONTROL;
        break;
#endif
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Set_Camera_Terminal
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{

    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*EndBody*/
    switch(setup_packet->request)
    {
    case SET_CUR:
        status=USB_Video_Set_Cur_Camera_Terminal(video_ptr,setup_packet,data,size);
        break;

    default:
        break;

    }/*EndSwitch*/
    return status;

}/*EndBody*/


static uint8_t USB_Video_Get_Camera_Terminal
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{

    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*Body*/
    switch(setup_packet->request)
    {
    case GET_CUR:
        status=USB_Video_Get_Cur_Camera_Terminal(video_ptr,setup_packet,data,size);

        break;

    case GET_MIN:
        status=USB_Video_Get_Min_Camera_Terminal(video_ptr,setup_packet,data,size);
        break;    

    case GET_MAX:
        status=USB_Video_Get_Max_Camera_Terminal(video_ptr,setup_packet,data,size);
        break; 

    case GET_RES:
        status=USB_Video_Get_Res_Camera_Terminal(video_ptr,setup_packet,data,size);

        break;

    case GET_LEN:

        break;      

    case GET_INFO:
        status=USB_Video_Get_Info_Camera_Terminal(video_ptr,setup_packet,data,size);
        break;

    case GET_DEF:
        status=USB_Video_Get_Def_Camera_Terminal(video_ptr,setup_packet,data,size);
        break;    

    default:
        break;

    }/*EndSwitch*/
    return status;

}/*EndBody*/

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
static uint8_t USB_Video_Set_Cur_Encoding_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        cmd = SET_CUR_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        cmd = SET_CUR_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = SET_CUR_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = SET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = SET_CUR_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        cmd = SET_CUR_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = SET_CUR_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = SET_CUR_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = SET_CUR_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = SET_CUR_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = SET_CUR_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = SET_CUR_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        cmd = SET_CUR_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        cmd = SET_CUR_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        cmd = SET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = SET_CUR_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = SET_CUR_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        cmd = SET_CUR_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        cmd = SET_CUR_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        cmd = SET_CUR_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Cur_Encoding_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        cmd = GET_CUR_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        cmd = GET_CUR_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_CUR_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_CUR_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        cmd = GET_CUR_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_CUR_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_CUR_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_CUR_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_CUR_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = GET_CUR_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = GET_CUR_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        cmd = GET_CUR_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        cmd = GET_CUR_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        cmd = GET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = GET_CUR_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_CUR_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        cmd = GET_CUR_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        cmd = GET_CUR_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        cmd = GET_CUR_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Min_Encoding_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        //cmd = GET_MIN_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        //cmd = GET_MIN_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_MIN_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_MIN_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_MIN_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        //cmd = GET_MIN_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_MIN_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_MIN_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_MIN_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_MIN_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = GET_MIN_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        //cmd = GET_MIN_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        //cmd = GET_MIN_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        //cmd = GET_MIN_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        cmd = GET_MIN_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = GET_MIN_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_MIN_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        //cmd = GET_MIN_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        //cmd = GET_MIN_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        //cmd = GET_MIN_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Max_Encoding_Unit(
   video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        //cmd = GET_MAX_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        //cmd = GET_MAX_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_MAX_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_MAX_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_MAX_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        //cmd = GET_MAX_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_MAX_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_MAX_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_MAX_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_MAX_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = GET_MAX_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = GET_MAX_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        //cmd = GET_MAX_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        //cmd = GET_MAX_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        cmd = GET_MAX_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = GET_MAX_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_MAX_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        //cmd = GET_MAX_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        //cmd = GET_MAX_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        //cmd = GET_MAX_EU_ERROR_RESILIENCY_CONTROL;
        break;        
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Res_Encoding_Unit(
   video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        //cmd = GET_RES_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        //cmd = GET_RES_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        //cmd = GET_RES_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        //cmd = GET_RES_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        //cmd = GET_RES_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        //cmd = GET_RES_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_RES_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_RES_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_RES_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_RES_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        //cmd = GET_RES_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        //cmd = GET_RES_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        //cmd = GET_RES_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        //cmd = GET_RES_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        //cmd = GET_RES_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        //cmd = GET_RES_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        //cmd = GET_RES_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        //cmd = GET_RES_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        //cmd = GET_RES_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        cmd = GET_RES_EU_ERROR_RESILIENCY_CONTROL;
        break;        
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;

}/*EndBody*/

static uint8_t USB_Video_Get_Len_Encoding_Unit(
   video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        cmd = GET_LEN_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        cmd = GET_LEN_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_LEN_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_LEN_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_LEN_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        cmd = GET_LEN_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_LEN_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_LEN_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_LEN_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_LEN_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = GET_LEN_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = GET_LEN_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        cmd = GET_LEN_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        cmd = GET_LEN_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        //cmd = GET_LEN_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        //cmd = GET_LEN_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_LEN_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        cmd = GET_LEN_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        cmd = GET_LEN_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        //cmd = GET_LEN_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;

}/*EndBody*/


static uint8_t USB_Video_Get_Info_Encoding_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        cmd = GET_INFO_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        cmd = GET_INFO_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_INFO_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_INFO_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_INFO_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        cmd = GET_INFO_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_INFO_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_INFO_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_INFO_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_INFO_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        cmd = GET_INFO_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = GET_INFO_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        cmd = GET_INFO_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        cmd = GET_INFO_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        //cmd = GET_INFO_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = GET_INFO_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_INFO_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        cmd = GET_INFO_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        cmd = GET_INFO_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        //cmd = GET_INFO_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Def_Encoding_Unit(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case EU_SELECT_LAYER_CONTROL:
        //cmd = GET_DEF_EU_SELECT_LAYER_CONTROL;
        break;
    case EU_PROFILE_TOOLSET_CONTROL:
        cmd = GET_DEF_EU_PROFILE_TOOLSET_CONTROL;
        break;
    case EU_VIDEO_RESOLUTION_CONTROL:
        cmd = GET_DEF_EU_VIDEO_RESOLUTION_CONTROL;
        break;
    case EU_MIN_FRAME_INTERVAL_CONTROL:
        cmd = GET_DEF_EU_MIN_FRAME_INTERVAL_CONTROL;
        break;
    case EU_SLICE_MODE_CONTROL:
        cmd = GET_DEF_EU_SLICE_MODE_CONTROL;
        break;
    case EU_RATE_CONTROL_MODE_CONTROL:
        cmd = GET_DEF_EU_RATE_CONTROL_MODE_CONTROL;
        break;
    case EU_AVERAGE_BITRATE_CONTROL:
        cmd = GET_DEF_EU_AVERAGE_BITRATE_CONTROL;
        break;
    case EU_CPB_SIZE_CONTROL:
        cmd = GET_DEF_EU_CPB_SIZE_CONTROL;
        break;
    case EU_PEAK_BIT_RATE_CONTROL:
        cmd = GET_DEF_EU_PEAK_BIT_RATE_CONTROL;
        break;
    case EU_QUANTIZATION_PARAMS_CONTROL:
        cmd = GET_DEF_EU_QUANTIZATION_PARAMS_CONTROL;
        break;
    case EU_SYNC_REF_FRAME_CONTROL:
        //cmd = GET_DEF_EU_SYNC_REF_FRAME_CONTROL;
        break;
    case EU_LTR_BUFFER_CONTROL:
        cmd = GET_DEF_EU_LTR_BUFFER_CONTROL;
        break;
    case EU_LTR_PICTURE_CONTROL:
        cmd = GET_DEF_EU_LTR_PICTURE_CONTROL;
        break;
    case EU_LTR_VALIDATION_CONTROL:
        cmd = GET_DEF_EU_LTR_VALIDATION_CONTROL;
        break;
    case EU_LEVEL_IDC_LIMIT_CONTROL:
        cmd = GET_DEF_EU_LEVEL_IDC_LIMIT_CONTROL;
        break;
    case EU_SEI_PAYLOADTYPE_CONTROL:
        cmd = GET_DEF_EU_SEI_PAYLOADTYPE_CONTROL;
        break;
    case EU_QP_RANGE_CONTROL:
        cmd = GET_DEF_EU_QP_RANGE_CONTROL;
        break;
    case EU_PRIORITY_CONTROL:
        //cmd = GET_DEF_EU_PRIORITY_CONTROL;
        break;
    case EU_START_OR_STOP_LAYER_CONTROL:
        //cmd = GET_DEF_EU_START_OR_STOP_LAYER_CONTROL;
        break;
    case EU_ERROR_RESILIENCY_CONTROL:
        cmd = GET_DEF_EU_ERROR_RESILIENCY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/



static uint8_t USB_Video_Set_Encoding_Unit
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
){

    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*EndBody*/
    switch(setup_packet->request)
    {
    case SET_CUR:
        status=USB_Video_Set_Cur_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;

    default:
        break;

    }/*EndSwitch*/
    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Encoding_Unit
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{

    uint8_t status = USBERR_INVALID_REQ_TYPE;
    
    /*Body*/
    switch(setup_packet->request)
    {
    case GET_CUR:
        status=USB_Video_Get_Cur_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;

    case GET_MIN:
        status=USB_Video_Get_Min_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;    

    case GET_MAX:
        status=USB_Video_Get_Max_Encoding_Unit(video_ptr,setup_packet,data,size);
        break; 

    case GET_RES:
        status=USB_Video_Get_Res_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;

    case GET_LEN:
        status=USB_Video_Get_Len_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;      

    case GET_INFO:
        status=USB_Video_Get_Info_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;

    case GET_DEF:
        status=USB_Video_Get_Def_Encoding_Unit(video_ptr,setup_packet,data,size);
        break;    

    default:
        break;

    }/*EndSwitch*/
    return status;

}/*EndBody*/
#endif

/**************************************************************************//*!
 *
 * @name: USB_Video_Get_Control_Interface_Requests 
 *
 * @brief This function is called in response to Get Interface Requests 
 *
 * @param controller_ID     : Controller ID
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 ******************************************************************************
 * The funtion Get Feature unit and Terminal requests. It is called in
 * USB_Other_Request function
 *****************************************************************************/
static uint8_t USB_Video_Get_Unit_Terminal_Requests(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t i;
    uint8_t status = USBERR_INVALID_REQ_TYPE;
    video_units_struct_t *entity_desc_data;
    video_ut_struct_t* ut_struct_ptr;

    /* get the I/O Interface and Feature  from the descriptor module */
    entity_desc_data = video_ptr->video_units_data;

    for (i=0;i<entity_desc_data->count; i++)
    {
        ut_struct_ptr = &video_ptr->video_ut_data.ut[i];
        if ((setup_packet->index >>8) == ut_struct_ptr->unit_id)
        {
            switch(ut_struct_ptr->type)
            {
            case VC_INPUT_TERMINAL:
                if (ut_struct_ptr->unit_type == ITT_CAMERA)
                {
                    status = USB_Video_Get_Camera_Terminal(video_ptr, setup_packet, data, size);
                }
                break;
            case VC_PROCESSING_UNIT:
                /* Select GET request Control Processing Unit Module */
                status = USB_Video_Get_Processing_Unit(video_ptr, setup_packet, data, size);
                break;
            case VC_OUTPUT_TERMINAL:
                break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
            case VC_ENCODING_UNIT:
                /* Select GET request Control Encoding Unit Module */
                status = USB_Video_Get_Encoding_Unit(video_ptr, setup_packet, data, size);
                break;
#endif
            default:
                break;
            }
        }
    }
    return status;
}

/**************************************************************************//*!
 *
 * @name: USB_Video_Set_Control_Interface_Requests 
 *
 * @brief This function is called in response to Get Interface Requests 
 *
 * @param controller_ID     : Controller ID
 * @param setup_packet      : Pointer to setup packet
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 * 
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 *
 ******************************************************************************
 * The funtion Get Feature unit and Terminal requests. It is called in
 * USB_Other_Request function
 *****************************************************************************/
static uint8_t USB_Video_Set_Unit_Terminal_Requests(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t i;
    uint8_t status = USBERR_INVALID_REQ_TYPE;
    video_units_struct_t *entity_desc_data;
    video_ut_struct_t* ut_struct_ptr;

    /* get the I/O Interface and Feature  from the descriptor module */
    entity_desc_data = video_ptr->video_units_data;

    for (i=0;i<entity_desc_data->count; i++)
    {
        ut_struct_ptr = &video_ptr->video_ut_data.ut[i];
        if ((setup_packet->index >>8) == ut_struct_ptr->unit_id)
        {
            switch(ut_struct_ptr->type)
            {
            case VC_INPUT_TERMINAL:
                if (ut_struct_ptr->unit_type == ITT_CAMERA)
                {
                    status = USB_Video_Set_Camera_Terminal(video_ptr, setup_packet, data, size);
                }
                break;
            case VC_PROCESSING_UNIT:
                /* Select SET request Control Processing Unit Module */
                status = USB_Video_Set_Processing_Unit(video_ptr, setup_packet, data, size);
                break;
            case VC_OUTPUT_TERMINAL:
                break;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
            case VC_ENCODING_UNIT:
                /* Select SET request Control Encoding Unit Module */
                status = USB_Video_Set_Encoding_Unit(video_ptr, setup_packet, data, size);
                break;
#endif
            default:
                break;
            }
        }
    }
    return status;
}

static uint8_t USB_Video_Set_Cur_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = SET_CUR_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        cmd = SET_CUR_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = SET_CUR_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        cmd = SET_CUR_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        cmd = SET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        cmd = SET_CUR_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        cmd = SET_CUR_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = SET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = SET_CUR_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Cur_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_CUR_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        cmd = GET_CUR_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_CUR_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        cmd = GET_CUR_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        cmd = GET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        cmd = GET_CUR_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        cmd = GET_CUR_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_CUR_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Get_Min_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_MIN_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        //cmd = GET_MIN_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_MIN_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        //cmd = GET_MIN_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        //cmd = GET_MIN_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        //cmd = GET_MIN_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        //cmd = GET_MIN_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_MIN_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_MIN_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Max_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_MAX_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        //cmd = GET_MAX_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_MAX_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        //cmd = GET_MAX_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        //cmd = GET_MAX_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        //cmd = GET_MAX_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        //cmd = GET_MAX_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_MAX_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_MAX_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Res_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_RES_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        //cmd = GET_RES_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_RES_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        //cmd = GET_RES_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        //cmd = GET_RES_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        //cmd = GET_RES_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        //cmd = GET_RES_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_RES_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_RES_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Info_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_INFO_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        cmd = GET_INFO_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_INFO_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        cmd = GET_INFO_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        cmd = GET_INFO_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        cmd = GET_INFO_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        cmd = GET_INFO_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_INFO_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_INFO_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Def_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_DEF_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        //cmd = GET_DEF_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_DEF_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        //cmd = GET_DEF_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        //cmd = GET_DEF_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        //cmd = GET_DEF_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        //cmd = GET_DEF_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        cmd = GET_DEF_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        cmd = GET_DEF_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Len_Stream_Interface(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    int32_t    cmd = -1;
    usb_status status = USBERR_INVALID_REQ_TYPE;

    uint8_t control_selector = (uint8_t)(setup_packet->value >> 8);

    /*Body*/
    switch(control_selector)
    {
    case VS_PROBE_CONTROL:
        cmd = GET_LEN_VS_PROBE_CONTROL;
        break;
    case VS_COMMIT_CONTROL:
        cmd = GET_LEN_VS_COMMIT_CONTROL;
        break;
    case VS_STILL_PROBE_CONTROL:
        cmd = GET_LEN_VS_STILL_PROBE_CONTROL;
        break;
    case VS_STILL_COMMIT_CONTROL:
        cmd = GET_LEN_VS_STILL_COMMIT_CONTROL;
        break;
    case VS_STILL_IMAGE_TRIGGER_CONTROL:
        //cmd = GET_LEN_VS_STILL_IMAGE_TRIGGER_CONTROL;
        break;
    case VS_STREAM_ERROR_CODE_CONTROL:
        //cmd = GET_LEN_VS_STREAM_ERROR_CODE_CONTROL;
        break;
    case VS_GENERATE_KEY_FRAME_CONTROL:
        //cmd = GET_LEN_VS_GENERATE_KEY_FRAME_CONTROL;
        break;
    case VS_UPDATE_FRAME_SEGMENT_CONTROL:
        //cmd = GET_LEN_VS_UPDATE_FRAME_SEGMENT_CONTROL;
        break;
    case VS_SYNCH_DELAY_CONTROL:
        //cmd = GET_LEN_VS_SYNCH_DELAY_CONTROL;
        break;
    default:
        break;

    }/*EndSwitch*/

    if ((cmd > 0) && (video_ptr->class_specific_callback.callback != NULL))
    {
         status = video_ptr->class_specific_callback.callback(video_ptr->stream_interface_alternate,cmd,
                                             data,size,video_ptr->class_specific_callback.arg);
    }

    return status;
}/*EndBody*/


static uint8_t USB_Video_Set_Stream_Interface_Requests
(
    video_device_struct_t* video_ptr,               /* [IN] class Handle */
    usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*Body*/
    switch(setup_packet->request)
    {
    case SET_CUR:
        status=USB_Video_Set_Cur_Stream_Interface(video_ptr,setup_packet,data,size);
        break;

    default:
        break;

    }/*EndSwitch*/
    return status;
}/*EndBody*/

static uint8_t USB_Video_Get_Stream_Interface_Requests(
        video_device_struct_t* video_ptr,               /* [IN] class Handle */
        usb_setup_struct_t * setup_packet,    /* [IN] Pointer to setup packet */
        uint8_t* *data,                   /* [OUT] Pointer to Data */
        uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t status = USBERR_INVALID_REQ_TYPE;

    /*Body*/
    switch(setup_packet->request)
    {
    case GET_CUR:
        status=USB_Video_Get_Cur_Stream_Interface(video_ptr, setup_packet, data, size);
        break;
    case GET_MIN:
        status=USB_Video_Get_Min_Stream_Interface(video_ptr, setup_packet, data, size);
        break;
    case GET_MAX:
        status=USB_Video_Get_Max_Stream_Interface(video_ptr, setup_packet, data, size);
        break;
    case GET_RES:
        status=USB_Video_Get_Res_Stream_Interface(video_ptr, setup_packet, data, size);
        break;
    case GET_DEF:
        status=USB_Video_Get_Def_Stream_Interface(video_ptr, setup_packet, data, size);
        break;
    case GET_LEN:
        status=USB_Video_Get_Len_Stream_Interface(video_ptr, setup_packet, data, size);
        break;      
    case GET_INFO:
        status=USB_Video_Get_Info_Stream_Interface(video_ptr, setup_packet, data, size);
        break;  
    default:
        break;

    }/*EndSwitch*/
    return status;
}/*EndBody*/


/**************************************************************************//*!
 *
 * @name  USB_Video_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param handle
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
usb_status USB_Video_Requests 
(
    usb_setup_struct_t * setup_packet, 
    uint8_t * *data, 
    uint32_t* size,
    void* arg
)
{
    usb_status             error = USBERR_INVALID_REQ_TYPE;
    video_device_struct_t* video_ptr = (video_device_struct_t*)arg;
    uint8_t                itf_num;
    uint8_t                unitID;

    /* point to the data which comes after the setup packet */
    *data = ((uint8_t*)setup_packet)+USB_SETUP_PKT_SIZE;

    if (size == NULL)
    {
        return USBERR_GET_MEMORY_FAILED;
    }/*EndIf*/


    if (video_ptr == NULL)
    {
        #ifdef _DEV_DEBUG
            USB_PRINTF("USB_Video_Requests:video_ptr is NULL\n");
        #endif
        return error;
    }
    
    if ((setup_packet->request_type & USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_POS) == 
                                              USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_CLASS)
    {
        itf_num = (setup_packet->index);
        unitID=(uint8_t)(setup_packet->index>>8);

        switch(setup_packet->request_type)
        {
        case GET_REQUEST_ITF: 
            if (video_ptr->control_interface_index == itf_num)
            {
                /* Video Control Interface is Interface 0 */
                if (0 == unitID)
                {
                    /* Interface Control Requests */
                    error=USB_Video_Get_Control_Interface_Requests(video_ptr,
                            setup_packet,data,size);
                }
                else
                {
                    /* Unit and Terminal Requests */
                    error=USB_Video_Get_Unit_Terminal_Requests(video_ptr,
                            setup_packet,data,size);
                } /*EndIf*/
            }
            else if (video_ptr->stream_interface_index == itf_num)
            {
                /* Video Stream Inteface */
                error=USB_Video_Get_Stream_Interface_Requests(video_ptr,
                        setup_packet,data,size);
            } /*EndIf*/

            break; 

        case SET_REQUEST_ITF:
            if (video_ptr->control_interface_index == itf_num)
            {
                *size = 0;
                /* Video Control Interface is Interface 0 */
                if (0 == unitID)
                {
                    /* Interface Control Requests */
                    error=USB_Video_Set_Control_Interface_Requests(video_ptr, setup_packet, data, size);
                }
                else
                {
                    /* Unit and Terminal Requests */
                    error=USB_Video_Set_Unit_Terminal_Requests(video_ptr, setup_packet, data, size);
                } /*EndIf*/
            }
            else if (video_ptr->stream_interface_index == itf_num)
            {
                /* Video Stream Inteface */
                *size=0;
                error=USB_Video_Set_Stream_Interface_Requests(video_ptr, setup_packet, data, size);
            }/*EndIf*/
            break;

        default:
            break;
        } /*EndSwitch*/
    }
    else if ((setup_packet->request_type & USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_POS) == 
                                                 USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_VENDOR)
    {   /* vendor specific request  */    
        if (video_ptr->vendor_req_callback.callback != NULL) 
        {
            error = video_ptr->vendor_req_callback.callback(setup_packet,
            data,size,video_ptr->vendor_req_callback.arg);
        }
    }
    
    return error;
}/*EndBody*/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Init
 *
 * @brief   The funtion initializes the Device and Controller layer
 *
 * @param   controller_ID               : Controller ID
 * @param   video_class_callback        : Video Class Callback
 * @param   vendor_req_callback         : Vendor Request Callback
 * @param   param_callback              : Class requests Callback
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 *This function initializes the Video Class layer and layers it is dependent on
 *****************************************************************************/
usb_status USB_Class_Video_Init
(
    uint8_t                controller_id,
    video_config_struct_t* video_config_ptr,
    video_handle_t*        video_handle
)
{
    video_device_struct_t*  devicePtr = NULL;
    video_units_struct_t *  video_units_data_ptr = NULL;
    usb_status              error = USB_OK;
    uint8_t                 index;

    if (NULL == video_config_ptr)
    {
        return USBERR_ERROR;
    }
    
    error = USB_Video_Allocate_Handle(&devicePtr);

    if (USB_OK != error)
    {
        return error;
    }
    devicePtr->usb_control_ep_data = NULL;
    devicePtr->usb_stream_ep_data = NULL;
    devicePtr->control_interface_alternate = 0;
    devicePtr->stream_interface_alternate = 0;
    devicePtr->control_interface_index = 0xFF;
    devicePtr->stream_interface_index = 0xFF;
    devicePtr->controller_id = controller_id;

    if (USB_UNINITIALIZED_VAL_32 != USB_Class_Get_Class_Handle(devicePtr->controller_id))
    {
        devicePtr->class_handle = (class_handle_t)USB_Class_Get_Class_Handle(devicePtr->controller_id);
        devicePtr->handle = (usb_device_handle)USB_Class_Get_Ctrler_Handle(devicePtr->class_handle);
        if (NULL == devicePtr->handle)
        {
            USB_Video_Free_Handle(devicePtr);
            return USBERR_INIT_FAILED;
        }
    }
    else
    {
        /* Initialize the device layer*/
        error = usb_device_init(controller_id, (void *)&video_config_ptr->board_init_callback, &devicePtr->handle);
        if (error != USB_OK)
        {
            USB_Video_Free_Handle(devicePtr);
            devicePtr = NULL;
            return USBERR_INIT_FAILED;
        }
        
        devicePtr->class_handle = USB_Class_Init(devicePtr->handle,
            USB_Class_Video_Event,USB_Video_Requests,(void *)devicePtr, video_config_ptr->desc_callback_ptr);
    }

    /* save the callback pointer */
    OS_Mem_copy(&video_config_ptr->video_application_callback,
    &devicePtr->video_application_callback,sizeof(usb_application_callback_struct_t));           

    /* save the callback pointer */
    OS_Mem_copy(&video_config_ptr->vendor_req_callback,
    &devicePtr->vendor_req_callback,sizeof(usb_vendor_req_callback_struct_t));        

    /* Save the callback to ask application for class specific params*/
    OS_Mem_copy(&video_config_ptr->class_specific_callback,
    &devicePtr->class_specific_callback ,sizeof(usb_class_specific_callback_struct_t));                    

    /* Save the dec  callback to ask application for class specific params*/
    OS_Mem_copy(video_config_ptr->desc_callback_ptr,
        &devicePtr->desc_callback ,sizeof(usb_desc_request_notify_struct_t)); 
  
    devicePtr->desc_callback.get_desc_entity((uint32_t)devicePtr->handle,
        USB_VIDEO_UNITS, (uint32_t *)&video_units_data_ptr);

    if (MAX_VIDEO_CLASS_UT_NUM < video_units_data_ptr->count)
    {
        USB_PRINTF("too many UT in video device, please increase the MAX_VIDEO_CLASS_UT_NUM value\n ");
        USB_Class_Video_Deinit((video_handle_t)devicePtr);
        devicePtr = NULL;
        return USBERR_INIT_FAILED;
    }
    for(index = 0; index < video_units_data_ptr->count; index++) 
    {
        devicePtr->video_ut_data.ut[index].unit_id = video_units_data_ptr->put[index].unit_id;
        devicePtr->video_ut_data.ut[index].type = video_units_data_ptr->put[index].type;
    }
    devicePtr->video_units_data = video_units_data_ptr;
 
    *video_handle = (uint32_t)devicePtr;
    devicePtr->user_handle = *video_handle;
 
    usb_device_postinit(controller_id,devicePtr->handle);
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Deinit
 *
 * @brief   The funtion de-initializes the Device and Controller layer
 *
 * @param   video_handle_t handle               : class handle
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 *This function de-initializes the Video Class layer
 *****************************************************************************/
usb_status USB_Class_Video_Deinit 
(
    video_handle_t handle
) 
{
    usb_status error = USB_OK;

    video_device_struct_t*  devicePtr;

    devicePtr = (video_device_struct_t*)handle;
        
    if (NULL == devicePtr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }

    if (USB_UNINITIALIZED_VAL_32 == USB_Class_Get_Class_Handle(devicePtr->controller_id))
    {
        if (error == USB_OK)
        {
            /* Deinitialize the generic class functions */
            error = USB_Class_Deinit(devicePtr->handle,devicePtr->class_handle);
            if (error == USB_OK)
            {
                /* Deinitialize the device layer*/
                error = usb_device_deinit(devicePtr->handle);
            }
        }
    }

    USB_Video_Free_Handle(devicePtr);
    return error;
} 


#if USBCFG_DEV_ADVANCED_CANCEL_ENABLE
/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Cancel
 *
 * @brief 
 *
 * @param handle          :   handle returned by USB_Class_Video_Init
 * @param ep_num          :   endpoint num 
 * @param direction        :   direction of the endpoint 
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/

usb_status USB_Class_Video_Cancel
(
    video_handle_t handle,/*[IN]*/
    uint8_t ep_num,/*[IN]*/
    uint8_t direction
)
{
    usb_status error = USB_OK;
    video_device_struct_t*  devicePtr;
    
    devicePtr = (video_device_struct_t*)handle;    

    if (NULL == devicePtr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }
    error = usb_device_cancel_transfer(devicePtr->handle,
            ep_num,direction);
    
    return error;
    
}
#endif


/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Send_Data
 *
 * @brief This fucntion is used by Application to send data through Video class
 *
 * @param controller_ID     : Controller ID
 * @param ep_num            : Endpoint number
 * @param app_buff          : Buffer to send
 * @param size              : Length of the transfer
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 * This fucntion is used by Application to send data through Video class
 *****************************************************************************/
uint8_t USB_Class_Video_Send_Data (
        video_handle_t handle,       /* [IN] class handle */
        uint8_t ep_num,              /* [IN] Endpoint Number */
        uint8_t* app_buff,        /* [IN] Buffer to Send */
        uint32_t size        /* [IN] Length of the Transfer */
)
{
    usb_status error = USB_OK;
    video_device_struct_t*  devicePtr = (video_device_struct_t*)handle;


#if VIDEO_IMPLEMENT_QUEUING
    uint8_t index;
    uint8_t producer, queue_num;
    video_queue_struct_t queue_tmp;
#endif

    if (NULL == devicePtr)
    {
        return USBERR_DEVICE_NOT_FOUND;
    }

#if VIDEO_IMPLEMENT_QUEUING
    usb_endpoints_t *usb_ep_data = devicePtr->usb_ep_data;

    /*Body*/                                    
    /* map the endpoint num to the index of the endpoint structure */

    for(index = 0; index < usb_ep_data->count; index++) 
    {
        if (usb_ep_data->ep[index].ep_num == ep_num)
            break;
    }/*EndFor*/

    producer = devicePtr->video_endpoint_data.ep[index].bin_producer;
    queue_num = devicePtr->video_endpoint_data.ep[index].queue_num;

    if (MAX_AUDIO_CLASS_UT_NUM != queue_num)
    {
        /* the bin is not full*/
        /* put all send request parameters in the endpoint data structure */
        queue_tmp = &(devicePtr->video_endpoint_data.ep[index].queue[producer]);
        queue_tmp->handle         = handle;
        queue_tmp->channel        = ep_num;
        queue_tmp->app_buff       = app_buff;
        queue_tmp->size           = size;

        /* increment producer bin by 1*/
        if (producer == (MAX_AUDIO_CLASS_UT_NUM - 1))
        {
            devicePtr->video_endpoint_data.ep[index].bin_producer = 0;
        } /*Else*/
        else 
        {
            devicePtr->video_endpoint_data.ep[index].bin_producer++;
        }/*EndElse*/
        devicePtr->video_endpoint_data.ep[index].queue_num++;

        if (devicePtr->video_endpoint_data.ep[index].queue_num == 1)
        {
#endif
            error = USB_Class_Send_Data(devicePtr->class_handle, ep_num, app_buff,size);
#if IMPLEMENT_QUEUING
        }/*EndIf*/
    }/*Else*/
    else /* bin is full */
    {
        error = USBERR_DEVICE_BUSY;
    }/*EndIf*/
#endif
    return error;
}/*EndBody*/

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Recv_Data
 *
 * @brief This fucntion is used by Application to send data through Video class
 *
 * @param handle            : Class handle
 * @param ep_num            : Endpoint number
 * @param app_buff          : Buffer to send
 * @param size              : Length of the transfer
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 * This fucntion is used by Application to send data through Video class
 *****************************************************************************/
uint8_t USB_Class_Video_Recv_Data (
        video_handle_t handle,       /* [IN] class handle */
        uint8_t ep_num,              /* [IN] Endpoint Number */
        uint8_t* app_buff,        /* [IN] Buffer to Send */
        uint32_t size        /* [IN] Length of the Transfer */
)
{
    usb_status error = USB_OK;
    video_device_struct_t*  devicePtr = (video_device_struct_t*)handle;

    if (NULL == devicePtr)
    {
        return USBERR_DEVICE_NOT_FOUND;
    }
    
    error = usb_device_recv_data(devicePtr->handle, ep_num, app_buff,size);
    return error;
}/*EndBody*/

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Get_Speed
 *
 * @brief This functions get speed from Host.
 *
 * @param handle          :   handle returned by USB_Class_Video_Init
 * @param speed           :   speed
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
usb_status USB_Class_Video_Get_Speed
(
    video_handle_t        handle,
    uint16_t *            speed/* [OUT] the requested error */
)
{
    video_device_struct_t * video_obj_ptr;
    usb_status error = USB_OK;

    video_obj_ptr = (video_device_struct_t *)handle;
    if (NULL == video_obj_ptr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }
    error = USB_Class_Get_Status(video_obj_ptr->class_handle, USB_STATUS_SPEED, speed);
    
    return error;
}

#endif
