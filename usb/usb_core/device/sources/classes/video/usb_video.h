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
* $FileName: usb_video.h$
* $Version : 
* $Date    : 
*
* Comments:
*        The file contains USB stack Video class layer API header function.
*
*****************************************************************************/

#ifndef _USB_VIDEO_H
#define _USB_VIDEO_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_stack_interface.h"    
#include "usb_class.h"
#include "usb_class_video.h"
#include "usb_video_config.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Code of bmRequest Type */
#define SET_REQUEST_ITF                      (0x21)
#define SET_REQUEST_EP                       (0x22)
#define GET_REQUEST_ITF                      (0xA1)
#define GET_REQUEST_EP                       (0xA2)

/******************************************************************************
 * Types
 *****************************************************************************/


 /* structure to hold a request in the endpoint queue */
#if VIDEO_IMPLEMENT_QUEUING
typedef struct _video_queue_struct
{
    uint8_t handle;   /* Controller ID*/
    uint8_t channel;         /* Endpoint number */
    uint8_t* app_buff;    /* Buffer to send */
    uint32_t size;
}video_queue_struct_t;
#endif

/* USB class video endpoint data */
typedef struct _video_endpoint_struct
{
    uint8_t endpoint;        /* Endpoint number */
    uint8_t type;            /* Type of endpoint (interrupt,
                               bulk or isochronous) */
#if VIDEO_IMPLEMENT_QUEUING
    uint8_t bin_consumer;    /* Num of queued elements */
    uint8_t bin_producer;    /* Num of de-queued elements */
    uint8_t queue_num;       /* Num of queue */
    video_queue_struct_t queue[MAX_VIDEO_QUEUE_ELEMS]; /* Queue data */
#endif
}video_endpoint_struct_t;

/* contains the endpoint data for non control endpoints */
typedef struct _video_endpoint_data_struct
{
    /* Num of non control endpoints */
    uint8_t count;
    /* contains the endpoint info */
    video_endpoint_struct_t ep[MAX_VIDEO_CLASS_CTL_EP_NUM + MAX_VIDEO_CLASS_STREAM_EP_NUM];
}video_endpoint_data_struct_t;


typedef struct _video_ut_data_struct
{
    video_ut_struct_t                   ut[MAX_VIDEO_CLASS_UT_NUM];
}video_ut_data_struct_t;


typedef struct _video_variable_struct
{
    usb_device_handle                           handle;
    uint32_t                                    user_handle;
    video_handle_t                              video_handle;
    class_handle_t                              class_handle;
    usb_endpoints_t*                            usb_stream_ep_data;
    usb_endpoints_t*                            usb_control_ep_data;
    video_units_struct_t*                       video_units_data;
    video_ut_data_struct_t                      video_ut_data;
    video_endpoint_data_struct_t                video_endpoint_data;
    usb_application_callback_struct_t           video_application_callback;
    usb_vendor_req_callback_struct_t            vendor_req_callback;            
    usb_class_specific_callback_struct_t        class_specific_callback;
    usb_desc_request_notify_struct_t            desc_callback;
    uint32_t                                    control_interface_index;
    uint32_t                                    stream_interface_index;
    uint8_t                                     control_interface_alternate;
    uint8_t                                     stream_interface_alternate;
    uint8_t                                     controller_id;
}video_device_struct_t; 

/******************************************************************************
 * Global Functions
 *****************************************************************************/
void       USB_Class_Video_Event(uint8_t event, void* val,void* arg);
usb_status USB_Video_Requests(usb_setup_struct_t * setup_packet, uint8_t * *data, uint32_t *size,void* arg);



#endif
