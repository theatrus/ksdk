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
* $FileName: virtual_camera.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*****************************************************************************/

#ifndef _VIRTUAL_CAMERA_H
#define _VIRTUAL_CAMERA_H

#include "usb_video_config.h"
#include "usb_class_video.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/
typedef struct _virtual_camera_struct
{
    video_handle_t                             video_handle;
    video_probe_and_commit_controls_struct_t   probe_struct;
    video_probe_and_commit_controls_struct_t   commit_struct;
    video_still_probe_and_commit_controls_struct_t still_probe_struct;
    video_still_probe_and_commit_controls_struct_t still_commit_struct;
    uint32_t                                   frame_progress;
    uint32_t                                   frame_send_length;
    uint32_t                                   image_position[2];
    uint16_t                                   app_speed;
    uint8_t                                    image_buffer[VIDEO_ISO_ENDPOINT_PACKET_SIZE];
    uint8_t                                    frame_id;
    uint8_t                                    probe_length;
    uint8_t                                    probe_info;
    uint8_t                                    commit_length;  
    uint8_t                                    commit_info;
    uint8_t                                    still_probe_length;
    uint8_t                                    still_probe_info;
    uint8_t                                    still_commit_length;  
    uint8_t                                    still_commit_info;
    uint8_t                                    interface_alternate;
    uint8_t                                    attached;
    uint8_t                                    start_send;
    uint8_t                                    is_sending;
    uint8_t                                    frame_sent;
    uint8_t                                    transmit_type;
    uint8_t                                    still_image_transmit;
}virtual_camera_struct_t;

/******************************************************************************
 * Macro's
 *****************************************************************************/
#ifndef HIGH_SPEED
#define  HIGH_SPEED                         (0)
#endif

#if HIGH_SPEED
#define CONTROLLER_ID                       USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                       USB_CONTROLLER_KHCI_0
#endif

/*****************************************************************************
 * Global variables
 *****************************************************************************/
extern virtual_camera_struct_t                  virtual_camera;

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);

#define HEADER_PACKET_SIZE 12

#endif 
