/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
************************************************************************** *
 *
 * @file usb_descriptor.h
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file is a header file for USB Descriptors required for Mouse
 *        Application
 *****************************************************************************/

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_video_config.h"
#include "usb_class_video.h"
#include "usb_device_stack_interface.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/

#define BCD_USB_VERSION                   (0x0200)
 
/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR             (1)
#define USB_CONFIG_DESCRIPTOR             (2)
#define USB_STRING_DESCRIPTOR             (3)
#define USB_IFACE_DESCRIPTOR              (4)
#define USB_ENDPOINT_DESCRIPTOR           (5)
#define USB_DEVQUAL_DESCRIPTOR            (6)
#define USB_OTHER_SPEED_DESCRIPTOR        (7)

#define VS_FRAME_MJPEG                    (0x07)

#define USB_IFACE_ASSOCIATION_DESCRIPTOR  (0x0B)

/* string descriptors sizes */
#define USB_STR_DESC_SIZE                 (2)
#define USB_STR_0_SIZE                    (2)
#define USB_STR_1_SIZE                    (56)
#define USB_STR_2_SIZE                    (28)
#define USB_STR_n_SIZE                    (32)


/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (0x12) //18

#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)

   
#if USBCFG_VIDEO_CLASS_1_1
#define CONFIG_DESC_SIZE                  (0xC2) //Video
#else
#define CONFIG_DESC_SIZE                  (0xC1) //Video
#endif

#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)

#define VC_HEADER_ONLY_DESC_SIZE          (0x0D)
#define INPUT_TERMINAL_ONLY_DESC_SIZE     (0x11)
#define OUTPUT_TERMINAL_ONLY_DESC_SIZE    (0x09)

#if USBCFG_VIDEO_CLASS_1_1
#define PROCESSING_UNIT_ONLY_DESC_SIZE    (0x0C)
#else
#define PROCESSING_UNIT_ONLY_DESC_SIZE    (0x0B)
#endif

#define INTERRUPT_ENDPOINT_DESC_SIZE      (0x07)
#define VIDEO_STREAMING_ENDP_DESC_SIZE    (0x07)

#define VS_FRAME_MJPEG_DESC_SIZE          (0x32)
#define VS_FORMAT_MJPEG_DESC_INDEX        (0x01)

#define IAD_DESC_SIZE                     (0x08)

#define VS_STILL_IMAGE_FRAME_ONLY_DESC_SIZE   (0x0A)

#define VS_STILL_IMAGE_CAPTURE_METHOD         (0x02)

/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS               (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS    (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED              (1)

#define DEVICE_OTHER_DESC_NUM_CONFIG_SUPPORTED (0x01)
/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS            (3)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED       (1)

#define VIDEO_UNIT_COUNT                  (3)
#define VIDEO_DESC_ISO_ENDPOINT_COUNT     (1)
#define VIDEO_ISO_ENDPOINT                (2)
#define VIDEO_ISO_ENDPOINT_PACKET_SIZE    (512)
#define VIDEO_STREAM_IF_INDEX             (1)
#define HS_VIDEO_STREAM_ENDP_INTERVAL     (0x04) /* 2^(4-1) = 1ms */
#define FS_VIDEO_STREAM_ENDP_INTERVAL     (0x01)

#define VIDEO_DESC_CTL_ENDPOINT_COUNT     (1)
#define VIDEO_CTL_ENDPOINT                (1)
#define VIDEO_CTL_ENDPOINT_PACKET_SIZE    (8)
#define VIDEO_CTL_IF_INDEX                (0)
#define HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE (8)
#define FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE (8)
#define HS_VIDEO_CTL_ENDP_INTERVAL        (0x09) /* 2^(9-1) = 32ms */
#define FS_VIDEO_CTL_ENDP_INTERVAL        (0x20)

#define USB_AUDIO_DESCRIPTOR              (0x25)
#define USB_REPORT_DESCRIPTOR             (0x22)

#define USB_MAX_SUPPORTED_INTERFACES      (2)

/******************************************************************************
 * Types
 *****************************************************************************/


/******************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

uint8_t USB_Desc_Set_Speed
(
    uint32_t handle,
    uint16_t speed
);

#ifdef __cplusplus
}
#endif

#endif
