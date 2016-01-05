/**HEADER********************************************************************
* 
* Copyright (c) 2004-2010, 2014 Freescale Semiconductor;
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
* $FileName: virtual_camera.c$
* $Version : 
* $Date    : 
*
* Comments:
*        The file contains Macro's and functions required for Virtual Camera
*        Loopback Application
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"

#include "usb_descriptor.h"
#include "virtual_camera.h"   /* Virtual camera Application Header File */
#include "usb_request.h"


#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"

#include <stdio.h>
#include <stdlib.h>

#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 #define MAIN_TASK       10
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void APP_init(void);
void USB_Prepare_Data(void);
extern void Main_Task(uint32_t param);

/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern const unsigned char video_data[];
extern const uint32_t video_size;

extern usb_desc_request_notify_struct_t  desc_callback;
virtual_camera_struct_t                  virtual_camera;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 /******************************************************************************
 *
 *   @name        USB_Prepare_Data
 *
 *   @brief       This function prepares data to send
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************
 * This function prepare data before sending
 *****************************************************************************/
void USB_Prepare_Data(void)
{
    uint16_t k;
    uint8_t j;
    uint8_t format_index = 0;
    uint32_t  frame_interval = 0;
    video_mjpeg_payload_header_struct_t* video_mjpeg_header_ptr = (video_mjpeg_payload_header_struct_t*)virtual_camera.image_buffer;

    video_mjpeg_header_ptr->bHeaderLength = HEADER_PACKET_SIZE;
    for (j = 0;j < sizeof(video_mjpeg_header_ptr->bSourceClockReference);j++)
    {
        video_mjpeg_header_ptr->bSourceClockReference[j] = 0;
    }
    video_mjpeg_header_ptr->dwPresentationTime = 0;
    video_mjpeg_header_ptr->HeaderInfo.bitMap.frame_id = virtual_camera.frame_id;
    video_mjpeg_header_ptr->HeaderInfo.bitMap.end_of_frame = 0;
    video_mjpeg_header_ptr->HeaderInfo.bitMap.still_image = virtual_camera.still_image_transmit;
    
    if (virtual_camera.still_image_transmit > 0)
    {
        format_index = virtual_camera.still_commit_struct.bFormatIndex;
        frame_interval = virtual_camera.commit_struct.dwFrameInterval;
    }
    else
    {
        format_index = virtual_camera.commit_struct.bFormatIndex;
        frame_interval = virtual_camera.commit_struct.dwFrameInterval;
    }
    
    virtual_camera.frame_progress += 10000;
    if (virtual_camera.frame_sent > 0)
    {
        virtual_camera.frame_send_length = HEADER_PACKET_SIZE;
        if (virtual_camera.frame_progress < frame_interval)
        {
            return;
        }
        else
        {
            video_mjpeg_header_ptr->HeaderInfo.bitMap.end_of_frame = 1;
            virtual_camera.frame_id = 1 - virtual_camera.frame_id;
            virtual_camera.frame_sent = 0;
            virtual_camera.frame_progress = 0;
            
            if(virtual_camera.still_image_transmit > 0)
            {
                virtual_camera.still_image_transmit = 0;
                virtual_camera.transmit_type = 0;
            }
            
            if ((virtual_camera.transmit_type > 0) && (virtual_camera.transmit_type < 3))
            {
                virtual_camera.still_image_transmit = 1;
            }
            return;
        }
    }

    k=HEADER_PACKET_SIZE;
    
    if (VS_FORMAT_MJPEG_DESC_INDEX == format_index)
    {
        for (;k<virtual_camera.commit_struct.dwMaxPayloadTransferSize;k++,virtual_camera.image_position[format_index-1]++)
        {
            if ((0xff==video_data[virtual_camera.image_position[format_index-1]-1])&&(0xd9==video_data[virtual_camera.image_position[format_index-1]]))
            {
                if (virtual_camera.frame_progress < frame_interval)
                {
                    virtual_camera.frame_sent = 1;
                }
                else
                {
                    video_mjpeg_header_ptr->HeaderInfo.bitMap.end_of_frame = 1;
                    virtual_camera.frame_id = 1 - virtual_camera.frame_id;
                    virtual_camera.frame_sent = 0;
                    virtual_camera.frame_progress = 0;
                    
                    if(virtual_camera.still_image_transmit > 0)
                    {
                        virtual_camera.still_image_transmit = 0;
                        virtual_camera.transmit_type = 0;
                    }

                    if ((virtual_camera.transmit_type > 0) && (virtual_camera.transmit_type < 3))
                    {
                        virtual_camera.still_image_transmit = 1;
                    }
                }
                
                virtual_camera.image_buffer[k] = video_data[virtual_camera.image_position[format_index-1]];
                k++;
                
                virtual_camera.image_position[format_index-1]++;
                if(virtual_camera.image_position[format_index-1] >= video_size)
                {
                    virtual_camera.image_position[format_index-1] = 0;
                }
                break;
            }
            else
            {
                virtual_camera.image_buffer[k] = video_data[virtual_camera.image_position[format_index-1]];
            }
        }
    }
    virtual_camera.frame_send_length = k;
}

/******************************************************************************
 * 
 *    @name        USB_App_Param_Callback
 *
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
 uint8_t USB_App_Param_Callback
 (
    uint8_t request, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    uint8_t error = USB_OK;

    if ((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_VIDEO_ISO_REQ_VAL_INVALID))
    {
        virtual_camera.is_sending = (uint8_t)FALSE;
        
        if (NULL != arg)
        {
            if (((uint8_t)TRUE == virtual_camera.attached) && ((uint8_t)TRUE == virtual_camera.start_send))
            {
                if ((uint8_t)FALSE == virtual_camera.is_sending)
                {
                    virtual_camera.is_sending = (uint8_t)TRUE;
                    USB_Prepare_Data();
                    (void)USB_Class_Video_Send_Data(virtual_camera.video_handle,VIDEO_ISO_ENDPOINT,virtual_camera.image_buffer,virtual_camera.frame_send_length);
                }
            }
        }
        return error;
    }

    error = USB_Class_Get_feature(virtual_camera.video_handle, value, data, size);
    if (error == USBERR_INVALID_REQ_TYPE)
    {
        error = USB_Class_Set_feature(virtual_camera.video_handle, value, data, size);
    }

    return error; 
}   

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{
    uint16_t interface_setting;
    uint8_t interface_alternate;
    uint8_t interface_num;

    if ((event_type == USB_DEV_EVENT_BUS_RESET) || (event_type == USB_DEV_EVENT_CONFIG_CHANGED))
    {
        virtual_camera.attached=FALSE;
        if (USB_OK == USB_Class_Video_Get_Speed(virtual_camera.video_handle, &virtual_camera.app_speed))
        {
            USB_Desc_Set_Speed(virtual_camera.video_handle, virtual_camera.app_speed);
        }
    }
    else if (event_type == USB_DEV_EVENT_ENUM_COMPLETE) 
    {
        virtual_camera.attached=TRUE; 
        USB_PRINTF("Virtual camera is working ... \r\n");
    }
    else if (event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    else if (event_type == USB_DEV_EVENT_INTERFACE_CHANGED)
    {
        interface_setting = *((uint16_t*)val);
        interface_alternate = (uint8_t)(interface_setting&0x00FF);
        interface_num = (uint8_t)((interface_setting>>8)&0x00FF);
        
        if (VIDEO_STREAM_IF_INDEX == interface_num)
        {
            if (0 != interface_alternate)
            {
                virtual_camera.start_send = TRUE;
                if (((uint8_t)TRUE == virtual_camera.attached) && ((uint8_t)TRUE == virtual_camera.start_send))
                {
                    if ((uint8_t)FALSE == virtual_camera.is_sending)
                    {
                        virtual_camera.is_sending = (uint8_t)TRUE;
                        virtual_camera.frame_sent = 0;
                        virtual_camera.frame_progress = 0;
                        USB_Prepare_Data();
                        (void)USB_Class_Video_Send_Data(virtual_camera.video_handle,VIDEO_ISO_ENDPOINT,virtual_camera.image_buffer,virtual_camera.frame_send_length);
                    }
                }
            }
            else
            {
                virtual_camera.transmit_type = 0;
                virtual_camera.start_send = (uint8_t)FALSE;
                if ((uint8_t)TRUE == virtual_camera.is_sending)
                {
                    virtual_camera.is_sending = (uint8_t)FALSE;
                    (void)USB_Class_Video_Cancel(virtual_camera.video_handle,VIDEO_ISO_ENDPOINT,USB_SEND);
                }
            }
        }
    }
  
}



 /******************************************************************************
 *
 *   @name        APP_init
 *
 *   @brief       This function is the entry for the Virtual camera application
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************
 * This function starts the virtual camera application
 *****************************************************************************/

void APP_init(void)
{
    video_config_struct_t video_config;
    video_config.video_application_callback.callback = USB_App_Callback;
    video_config.video_application_callback.arg = &virtual_camera.video_handle;
    video_config.class_specific_callback.callback = USB_App_Param_Callback;
    video_config.class_specific_callback.arg = &virtual_camera.video_handle;
    video_config.board_init_callback.callback = NULL;
    video_config.board_init_callback.arg = NULL;
    video_config.desc_callback_ptr = &desc_callback;

    OS_Mem_zero((uint8_t*)&virtual_camera.probe_struct, sizeof(virtual_camera.probe_struct));
    OS_Mem_zero((uint8_t*)&virtual_camera.commit_struct, sizeof(virtual_camera.commit_struct));
    OS_Mem_zero((uint8_t*)&virtual_camera.still_probe_struct, sizeof(virtual_camera.still_probe_struct));
    OS_Mem_zero((uint8_t*)&virtual_camera.still_commit_struct, sizeof(virtual_camera.still_commit_struct));
    
    virtual_camera.probe_struct.bFormatIndex = VS_FORMAT_MJPEG_DESC_INDEX;
    virtual_camera.probe_struct.bFrameIndex = 1;
    virtual_camera.probe_struct.dwFrameInterval = 0x0A2C2A;
    virtual_camera.probe_struct.dwMaxPayloadTransferSize = VIDEO_ISO_ENDPOINT_PACKET_SIZE;
    virtual_camera.probe_struct.dwMaxVideoFrameSize = 0xC600;
    
    virtual_camera.commit_struct.bFormatIndex = VS_FORMAT_MJPEG_DESC_INDEX;
    virtual_camera.commit_struct.bFrameIndex = 1;
    virtual_camera.commit_struct.dwFrameInterval = 0x0A2C2A;
    virtual_camera.commit_struct.dwMaxPayloadTransferSize = VIDEO_ISO_ENDPOINT_PACKET_SIZE;
    virtual_camera.commit_struct.dwMaxVideoFrameSize = 0xC600;
    
    virtual_camera.probe_length = 26;
    virtual_camera.commit_length = 26;
    virtual_camera.probe_info = 0x03;
    virtual_camera.commit_info = 0x03;
    virtual_camera.frame_progress = 0;
    
    virtual_camera.still_probe_struct.bFormatIndex = VS_FORMAT_MJPEG_DESC_INDEX;
    virtual_camera.still_probe_struct.bFrameIndex = 1;
    virtual_camera.still_probe_struct.bCompressionIndex = 1;
    virtual_camera.still_probe_struct.dwMaxPayloadTransferSize = VIDEO_ISO_ENDPOINT_PACKET_SIZE;
    virtual_camera.still_probe_struct.dwMaxVideoFrameSize = 0xC600;
    
    virtual_camera.still_commit_struct.bFormatIndex = VS_FORMAT_MJPEG_DESC_INDEX;
    virtual_camera.still_commit_struct.bFrameIndex = 1;
    virtual_camera.still_commit_struct.bCompressionIndex = 1;
    virtual_camera.still_commit_struct.dwMaxPayloadTransferSize = VIDEO_ISO_ENDPOINT_PACKET_SIZE;
    virtual_camera.still_commit_struct.dwMaxVideoFrameSize = 0xC600;

    virtual_camera.still_probe_length = 26;
    virtual_camera.still_commit_length = 26;
    virtual_camera.still_probe_info = 0x03;
    virtual_camera.still_commit_info = 0x03;
    virtual_camera.transmit_type = 0;

    /* Initialize the USB interface */
    USB_Class_Video_Init(CONTROLLER_ID, &video_config, &virtual_camera.video_handle);
}

/******************************************************************************
 *
 *    @name        APP_task
 *
 *    @brief       This function use to send data
 *
 *    @param       None
 *
 *    @return      None
 *
 *****************************************************************************
 *
 *****************************************************************************/

void APP_task(void)
{

}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      2500L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
    #endif
    
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
void main(void)
#endif
{
    OSA_Init();
    hardware_init();
    dbg_uart_init();

    APP_init();

    OSA_Start();
}
#endif

/* EOF */
