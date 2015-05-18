/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 Freescale Semiconductor;
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
 * $FileName: usb_classes.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains  the USB class driver mapping table
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"

#if USBCFG_HOST_MSC
#include "usb_host_msd_bo.h"
#endif

#if USBCFG_HOST_PRINTER
#include "usb_host_printer.h"
#endif

#if USBCFG_HOST_HID
#include "usb_host_hid.h"
#endif

#if USBCFG_HOST_CDC
#include "usb_host_cdc.h"
#endif

#if USBCFG_HOST_AUDIO
#include "usb_host_audio.h"
#endif

#if USBCFG_HOST_PHDC
#include "usb_host_phdc.h"
#endif

#if USBCFG_HOST_VIDEO
#include "usb_host_video.h"
#endif

/* here hub is considered as device from host point of view */
#if USBCFG_HOST_HUB
#include "usb_host_hub.h"
#endif

class_map_t class_interface_map[] =
{
    #if USBCFG_HOST_MSC
    {
        usb_class_mass_init,
        usb_class_mass_deinit,
        usb_class_mass_pre_deinit,
        USB_CLASS_MASS_STORAGE,
        USB_SUBCLASS_MASS_UFI,
        USB_PROTOCOL_MASS_BULK,
        0xFF, 0x00, 0xFF
    },
#endif
#if USBCFG_HOST_PRINTER
    {
        usb_printer_init,
        sizeof(PRINTER_INTERFACE_STRUCT),
        USB_CLASS_PRINTER,
        USB_SUBCLASS_PRINTER,
        USB_PROTOCOL_PRT_BIDIR,
        0xFF, 0xFF, 0xFF
    },
#endif
#if USBCFG_HOST_HID
    {
        usb_class_hid_init,
        usb_class_hid_deinit,
        usb_class_hid_pre_deinit,
        USB_CLASS_HID,
        0,
        0,
        0xFF, 0x00, 0x00
    },
#endif
#if USBCFG_HOST_CDC
    {
        usb_class_cdc_acm_init,
        usb_class_cdc_acm_deinit,
        usb_class_cdc_acm_pre_deinit,
        USB_CLASS_COMMUNICATION,
        USB_SUBCLASS_COM_ABSTRACT,
        USB_PROTOCOL_COM_NOSPEC,
        0xFF, 0xFF, 0xFE
    },
    {
        usb_class_cdc_data_init,
        usb_class_cdc_data_deinit,
        usb_class_cdc_data_pre_deinit,
        USB_CLASS_DATA,
        0,
        0,
        0xFF, 0x00, 0x00
    },
#endif
#if USBCFG_HOST_AUDIO
    {
        usb_class_audio_control_init,
        usb_class_audio_control_deinit,
        usb_class_audio_control_pre_deinit,
        USB_CLASS_AUDIO,
        USB_SUBCLASS_AUD_CONTROL,
        0,
        0xFF, 0xFF, 0x00
    },
    {
        usb_class_audio_stream_init,
        usb_class_audio_stream_deinit,
        usb_class_audio_stream_pre_deinit,
        USB_CLASS_AUDIO,
        USB_SUBCLASS_AUD_STREAMING,
        0,
        0xFF, 0xFF, 0x00
    },
#endif
#if USBCFG_HOST_PHDC
    {
        usb_class_phdc_init,
        usb_class_phdc_deinit,
        usb_class_phdc_pre_deinit,
        USB_CLASS_PHDC,
        0,
        0,
        0xFF, 0x00, 0x00
    },
#endif   
#if USBCFG_HOST_HUB
    {
        usb_class_hub_init,
        usb_class_hub_deinit,
        usb_class_hub_pre_deinit,
        USB_CLASS_HUB,
        USB_SUBCLASS_HUB_NONE,
        USB_PROTOCOL_HUB_FS,
        0xFF, 0x00, 0x00
    },
#endif
#if USBCFG_HOST_VIDEO
    {
        usb_class_video_control_init,
        usb_class_video_control_deinit,
        usb_class_video_control_pre_deinit,
        USB_CLASS_VIDEO,
        USB_SUBCLASS_VIDEO_CONTROL,
        0,
        0xFF, 0xFF, 0x00
    },
    {
        usb_class_video_stream_init,
        usb_class_video_stream_deinit,
        usb_class_video_stream_pre_deinit,
        USB_CLASS_VIDEO,
        USB_SUBCLASS_VIDEO_STREAMING,
        0,
        0xFF, 0xFF, 0x00
    },
#endif
    {
        NULL,
        0,
        0, 0, 0,
        0, 0, 0
    }
};

/* EOF */
