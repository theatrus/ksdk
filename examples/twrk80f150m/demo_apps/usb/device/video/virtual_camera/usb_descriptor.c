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
* $FileName: usb_descriptor.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB descriptors and functions
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"

#include "usb_descriptor.h"
#include "virtual_camera.h"


/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/ 
usb_ep_struct_t iso_ep[VIDEO_DESC_ISO_ENDPOINT_COUNT] = 
{  
    {
        VIDEO_ISO_ENDPOINT,
        USB_ISOCHRONOUS_PIPE,
        USB_SEND,
        VIDEO_ISO_ENDPOINT_PACKET_SIZE
    }
};

/* structure containing details of all the endpoints used by this device */ 
const usb_endpoints_t usb_desc_iso_ep =
{
    VIDEO_DESC_ISO_ENDPOINT_COUNT,
    iso_ep
};

usb_ep_struct_t control_ep[VIDEO_DESC_CTL_ENDPOINT_COUNT] = 
{  
    {
        VIDEO_CTL_ENDPOINT,
        USB_INTERRUPT_PIPE,
        USB_SEND,
        FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE
    }
};

/* structure containing details of all the endpoints used by this device */ 
const usb_endpoints_t usb_desc_control_ep =
{
    VIDEO_DESC_CTL_ENDPOINT_COUNT,
    control_ep
};

/* *********************************************************************
* definition a struct of Input Terminal, Output Terminal or Feature Unit
************************************************************************ */
video_ut_struct_t  video_ut[VIDEO_UNIT_COUNT] = 
{
    {0x02, VC_INPUT_TERMINAL,  ITT_CAMERA},
    {0x05, VC_PROCESSING_UNIT, 0x0000},
    {0x03, VC_OUTPUT_TERMINAL, TT_STREAMING},
};

/* Struct of Input Terminal, Output Terminal or Feature Unit */
video_units_struct_t usb_video_unit = 
{ 
    VIDEO_UNIT_COUNT,
    video_ut,
};

static usb_if_struct_t usb_if[3] = 
{
    {
        VIDEO_CTL_IF_INDEX,
        {VIDEO_DESC_CTL_ENDPOINT_COUNT, control_ep},
        0,
    },
    {
        VIDEO_STREAM_IF_INDEX,
        {0,NULL},
        0,
    },
    {
        VIDEO_STREAM_IF_INDEX,
        {VIDEO_DESC_ISO_ENDPOINT_COUNT, iso_ep},
        1,
    },
};

static const usb_class_struct_t usb_dec_class[USB_MAX_CONFIG_SUPPORTED] =
{
    {
        USB_CLASS_VIDEO,
        {
             3, /* Count */
             usb_if
        }
    },
};

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    /* Device Descriptor */
    DEVICE_DESCRIPTOR_SIZE,               /* "Device Descriptor Size        */
    USB_DEVICE_DESCRIPTOR,                /* "Device" Type of descriptor    */
    0x00, 0x02,                           /*  BCD USB version               */
    0xEF,                                 /*  Miscellaneous Device Class    */
    0x02,                                 /*  Common Class  */
    0x01,                                 /*  Inteface Association Descriptor */
    CONTROL_MAX_PACKET_SIZE,              /*  Control endpoint packet size is 8 bytes */
    0xA2,0x15,                            /*  Vendor ID                     */
#if USBCFG_VIDEO_CLASS_1_1
    0x0C,0x0E,                            /*  Product ID                    */
#else
    0x0B,0x0E,                            /*  Product ID                    */
#endif
    0x00,0x03,                            /*  BCD Device version            */
    0x01,                                 /*  Manufacturer string index     */
    0x02,                                 /*  Product string index          */
    0x00,                                 /*  Serial number string index    */
    0x01                                  /*  Number of configurations      */
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{
    /* Configuration Descriptor */
    CONFIG_ONLY_DESC_SIZE,                /* Size of this desciptor in bytes */
    USB_CONFIG_DESCRIPTOR,                /* DEVICE descriptor */
    USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
                                          /* Length of total configuration block */
    USB_MAX_SUPPORTED_INTERFACES,         /* This device has two interface */
    0x01,                                 /* ID of this configuration */
    0x00,                                 /* Unused */
    (USB_DESC_CFG_ATTRIBUTES_D7_POS) | (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
                                          /* Bus-powered device, no remote wakeup */
    0xFA,                                 /* 500mA maximum power consumption */

    /* Interface Association Descriptor */
    IAD_DESC_SIZE,                        /* Size of this descriptor */
    USB_IFACE_ASSOCIATION_DESCRIPTOR,     /* INTERFACE ASSOCIATION Descriptor */
    0x00,                                 /* Interface number ofthe VideoControl
                                             interface that is associated with this function */
    0x02,                                 /* Number of contiguous Video interfaces
                                             that are associated with this function */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEO_INTERFACE_COLLECTION,        /* SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                                 /* Not used */
    0x02,                                 /* Index to string descriptor */

    /* Standard VC Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size of this descriptor */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_CTL_IF_INDEX,                   /* Index of this interface */
    0x00,                                 /* Index of this setting */
    VIDEO_DESC_CTL_ENDPOINT_COUNT,        /* 1 endpoint (interrupt endpoint) */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOCONTROL,                      /* SC_VIDEOCONTROL */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x02,                                 /* Index of this string descriptor */

    /* Class-specific VC Inteface Descriptor */
    VC_HEADER_ONLY_DESC_SIZE,             /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_HEADER,                            /* VC_HEADER subtype */
#if USBCFG_VIDEO_CLASS_1_1
    0x10,0x01,                            /* Revision of class specification */
#else
    0x00,0x01,                            /* Revision of class specification */
#endif
    0x32,0x00,                            /* Total size of class-specific descriptor */
    0x80,0x8D,0x5B,0x00,                  /* This device will provide timestamps and
                                            and a device clock reference based on 6MHz clock */
    0x01,                                 /* Number of streaming interfaces */
    VIDEO_STREAM_IF_INDEX,                /* VideoStreaming interface 1 belongs to
                                             this VideoControl interface */

    /* Input Terminal Descriptor (Camera) */
    INPUT_TERMINAL_ONLY_DESC_SIZE,        /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_INPUT_TERMINAL,                    /* VC_INPUT_TERMINAL subtype */
    0x02,                                 /* ID of this input terminal */
    0x01,0x02,                            /* ITT_CAMERA type. This terminal is a
                                             Camera terminal representing the CCD sensor */
    0x00,                                 /* No association */
    0x00,                                 /* Unused */
    0x00,0x00,                            /* No optical zoom supported */
    0x00,0x00,                            /* No optical zoom supported */
    0x00,0x00,                            /* No optical zoom supported */
    0x02,                                 /* The size of bmControls is 2 bytes */
    0x00,0x00,                            /* No controls are supported */

    /* Output Terminal Descriptor */
    OUTPUT_TERMINAL_ONLY_DESC_SIZE,       /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_OUTPUT_TERMINAL,                   /* VC_OUTPUT_TERMINAL */
    0x03,                                 /* ID of this descriptor */
    0x01,0x01,                            /* TT_STREAMING type.
                                             This terminal is a USB streaming terminal */
    0x00,                                 /* No association */
    0x02,                                 /* The input pin of this unit */
    0x00,                                 /* Unused */

    /* Processing Uint Descriptor */
    PROCESSING_UNIT_ONLY_DESC_SIZE,       /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_PROCESSING_UNIT,                   /* VC_PROCESSING_UNIT */
    0x05,                                 /* ID of this unit */
    0x02,                                 /* This input pin of this is connected to
                                             unit with ID 0x02 */
    0x00,0x00,                            /* unused */
#if USBCFG_VIDEO_CLASS_1_5
    0x03,                                 /* Size of the bmControls field */
    0x01,0x00,0x00,                       /* Brightness control supported */
#else
    0x02,                                 /* Size of the bmControls field */
    0x01,0x00,                            /* Brightness control supported */
#endif
    0x00,                                 /* Unused iProcessing*/
#if USBCFG_VIDEO_CLASS_1_1
    0x00,                                 /* Ignore bmVideoStandards */
#endif

    /* Standard Interrupt Endpoint Descriptor */
    INTERRUPT_ENDPOINT_DESC_SIZE,         /* Size of this descriptor */
    USB_ENDPOINT_DESCRIPTOR,              /* ENDPOINT descriptor */
    VIDEO_CTL_ENDPOINT|(USB_SEND << 7),   /* IN endpoint 1 */
    USB_INTERRUPT_PIPE,                   /* Interrupt transfer type */
    USB_uint_16_low(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE), USB_uint_16_high(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE),
    FS_VIDEO_CTL_ENDP_INTERVAL,


    /* Standard VS Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_STREAM_IF_INDEX,                /* Index of this interface */
    0x00,                                 /* Index of this alternate setting */
    0x00,                                 /* 0 endpoints - no bandwidth used */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOSTREAMING,                    /* SC_VIDEOSTREAMING */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x00,                                 /* Unused */

    /* Class-specific VS Header Descriptor (Input) */
    0x0E,                                 /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_INPUT_HEADER,                      /* VS_INPUT_HEADER */
    0x01,                                 /* One format descriptor follows */
    0x55,0x00,                            /* Total size of class-specific VideoStream
                                             interface descriptor */
    VIDEO_ISO_ENDPOINT|(USB_SEND << 7),   /* Address of the isochronous endpoint
                                             used for video data */
    0x00,                                 /* No dynamic format change supported */
    0x03,                                 /* This Video Stream interface supplies
                                             terminal ID 3 (Output Terminal)*/
    VS_STILL_IMAGE_CAPTURE_METHOD,        /* Device supports still image capture method 2 */
    0x00,                                 /* Hardware trigger not supported for still image capture */
    0x00,                                 /* Hardware trigger should initiate a still image capture */
    0x01,                                 /* Size of the bmaControls field */
    0x00,                                 /* No VideoStreaming specific controls are supported for Format 0 */
    
    /* Class-specifc VS Format Descriptor */
    0x0B,                                 /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_FORMAT_MJPEG,                      /* VS_FORMAT_MJPEG */ 
    VS_FORMAT_MJPEG_DESC_INDEX,           /* Second format descriptor */
    0x01,                                 /* One frame descriptor for this format follows */
    0x01,                                 /* Used fixed size samples */
    0x01,                                 /* Default frame index is 1 */
    0x00,                                 /* Non-interlaced stream - not required */
    0x00,                                 /* Non-interlaced stream - not required */
    0x00,                                 /* Non-interlaced stream */
    0x00,                                 /* No restrictions imposed on
                                             the duplication of this video stream*/

    /* Class-specific VS Frame Descriptor */
    VS_FRAME_MJPEG_DESC_SIZE,             /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_FRAME_MJPEG,                       /* VS_FRAME_MJPEG */
    0x01,                                 /* First frame descriptor */
    0x00,                                 /* Still images using capture method 1 are
                                             supported at this frame setting.*/
    /*  D1: Fixed frame-rate*/
    0xB0,0x00,                            /* Width of frame is 176 pixels */
    0x90,0x00,                            /* Height of frame is 144 pixels */

    0x00,0xF0,0x1E,0x00,                  /* Min bit rate in bits/s */
    0x00,0xA0,0xB9,0x00,                  /* Max bit rate in bits/s */

    0x00,0xC6,0x00,0x00,                  /* Max video or still frame size */

    0x2A,0x2C,0x0A,0x00,                  /* Default frame interval is 15fps */

    0x06,                                 /* Continuous frame interval*/
    0x15,0x16,0x05,0x00,                  /* frame interval 0 is 30fps */
    0x80,0x1A,0x06,0x00,                  /* frame interval 1 is 25fps */
    0x20,0xA1,0x07,0x00,                  /* frame interval 2 is 20fps */
    0x2A,0x2C,0x0A,0x00,                  /* frame interval 3 is 15fps */
    0x40,0x42,0x0F,0x00,                  /* frame interval 4 is 10fps */
    0x80,0x84,0x1E,0x00,                  /* frame interval 5 is 05fps */
    
//    0x06,                                 /* Size */
//    CS_INTERFACE,                         /* CS_INTERFACE */
//    VS_COLORFORMAT,                       /* VS_COLORFORMAT */
//    0x01,                                 /* BT.709, sRGB (default) */
//    0x01,                                 /* BT.709 (default) */
//    0x04,                                 /* SMPTE 170M (BT.601, default) */
    
    /* Class-specific VS Still Frame Descriptor */
    VS_STILL_IMAGE_FRAME_ONLY_DESC_SIZE,  /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_STILL_IMAGE_FRAME,                 /* VS_STILL_IMAGE_FRAME */
    0x00,                                 /* The address of the bulk endpoint used, only for Method 3 */
    0x01,                                 /* Number of Image Size patterns */
    0xB0,0x00,                            /* Width of frame is 176 pixels */
    0x90,0x00,                            /* Height of frame is 144 pixels */
    0x00,                                 /* The number of Compression pattern of this format is 0 */
    
    /* Standard VS Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_STREAM_IF_INDEX,                /* Index of this interface */
    0x01,                                 /* Index of this alternate setting */
    VIDEO_DESC_ISO_ENDPOINT_COUNT,        /* 1 endpoints now banwidth used */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOSTREAMING,                    /* SC_VIDEOSTREAMING */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x00,                                 /* Unused */

    /* Standard VS Isochronous Video Data Endpoint Descriptor */
    VIDEO_STREAMING_ENDP_DESC_SIZE,       /* Size */
    USB_ENDPOINT_DESCRIPTOR,              /* USB_ENDPOINT_DESCRIPTOR */
    VIDEO_ISO_ENDPOINT|(USB_SEND << 7),   /* IN endpoint 2 */
    (USB_ISOCHRONOUS_PIPE|ISOCH_ASYNC),   /* Isochronous tranfer type */
    USB_uint_16_low(VIDEO_ISO_ENDPOINT_PACKET_SIZE), USB_uint_16_high(VIDEO_ISO_ENDPOINT_PACKET_SIZE),
                                          /* Max packet size */
    FS_VIDEO_STREAM_ENDP_INTERVAL
};

#if HIGH_SPEED
uint8_t  g_device_qualifier_descriptor[DEVICE_QUALIFIER_DESCRIPTOR_SIZE] =
{
   /* Device Qualifier Descriptor Size */
   DEVICE_QUALIFIER_DESCRIPTOR_SIZE, 
   /* Type of Descriptor */
   USB_DEVQUAL_DESCRIPTOR,           
   /*  BCD USB version  */  
   USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
   /* bDeviceClass */
   0xEF,
   /* bDeviceSubClass */
   0x02,      
   /* bDeviceProtocol */
   0x01,
   /* bMaxPacketSize0 */
   CONTROL_MAX_PACKET_SIZE,          
   /* bNumConfigurations */
   DEVICE_OTHER_DESC_NUM_CONFIG_SUPPORTED,
   /* Reserved : must be zero */ 
   0x00                              
};

uint8_t  g_other_speed_config_descriptor[CONFIG_DESC_SIZE] =
{
    /* Configuration Descriptor */
    CONFIG_ONLY_DESC_SIZE,                /* Size of this desciptor in bytes */
    USB_OTHER_SPEED_DESCRIPTOR,           /* DEVICE descriptor */
    USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
                                          /* Length of total configuration block */
    USB_MAX_SUPPORTED_INTERFACES,         /* This device has two interface */
    0x01,                                 /* ID of this configuration */
    0x00,                                 /* Unused */
    (USB_DESC_CFG_ATTRIBUTES_D7_POS) | (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
                                          /* Bus-powered device, no remote wakeup */
    0xFA,                                 /* 500mA maximum power consumption */

    /* Interface Association Descriptor */
    IAD_DESC_SIZE,                        /* Size of this descriptor */
    USB_IFACE_ASSOCIATION_DESCRIPTOR,     /* INTERFACE ASSOCIATION Descriptor */
    0x00,                                 /* Interface number ofthe VideoControl
                                             interface that is associated with this function */
    0x02,                                 /* Number of contiguous Video interfaces
                                             that are associated with this function */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEO_INTERFACE_COLLECTION,        /* SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                                 /* Not used */
    0x02,                                 /* Index to string descriptor */

    /* Standard VC Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size of this descriptor */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_CTL_IF_INDEX,                   /* Index of this interface */
    0x00,                                 /* Index of this setting */
    VIDEO_DESC_CTL_ENDPOINT_COUNT,        /* 1 endpoint (interrupt endpoint) */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOCONTROL,                      /* SC_VIDEOCONTROL */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x02,                                 /* Index of this string descriptor */

    /* Class-specific VC Inteface Descriptor */
    VC_HEADER_ONLY_DESC_SIZE,             /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_HEADER,                            /* VC_HEADER subtype */
#if USBCFG_VIDEO_CLASS_1_1
    0x10,0x01,                            /* Revision of class specification */
#else
    0x00,0x01,                            /* Revision of class specification */
#endif
    0x32,0x00,                            /* Total size of class-specific descriptor */
    0x80,0x8D,0x5B,0x00,                  /* This device will provide timestamps and
                                            and a device clock reference based on 6MHz clock */
    0x01,                                 /* Number of streaming interfaces */
    VIDEO_STREAM_IF_INDEX,                /* VideoStreaming interface 1 belongs to
                                             this VideoControl interface */

    /* Input Terminal Descriptor (Camera) */
    INPUT_TERMINAL_ONLY_DESC_SIZE,        /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_INPUT_TERMINAL,                    /* VC_INPUT_TERMINAL subtype */
    0x02,                                 /* ID of this input terminal */
    0x01,0x02,                            /* ITT_CAMERA type. This terminal is a
                                             Camera terminal representing the CCD sensor */
    0x00,                                 /* No association */
    0x00,                                 /* Unused */
    0x00,0x00,                            /* No optical zoom supported */
    0x00,0x00,                            /* No optical zoom supported */
    0x00,0x00,                            /* No optical zoom supported */
    0x02,                                 /* The size of bmControls is 2 bytes */
    0x00,0x00,                            /* No controls are supported */

    /* Output Terminal Descriptor */
    OUTPUT_TERMINAL_ONLY_DESC_SIZE,       /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_OUTPUT_TERMINAL,                   /* VC_OUTPUT_TERMINAL */
    0x03,                                 /* ID of this descriptor */
    0x01,0x01,                            /* TT_STREAMING type.
                                             This terminal is a USB streaming terminal */
    0x00,                                 /* No association */
    0x02,                                 /* The input pin of this unit */
    0x00,                                 /* Unused */

    /* Processing Uint Descriptor */
    PROCESSING_UNIT_ONLY_DESC_SIZE,       /* Size of this descriptor */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VC_PROCESSING_UNIT,                   /* VC_PROCESSING_UNIT */
    0x05,                                 /* ID of this unit */
    0x02,                                 /* This input pin of this is connected to
                                             unit with ID 0x02 */
    0x00,0x00,                            /* unused */
#if USBCFG_VIDEO_CLASS_1_5
    0x03,                                 /* Size of the bmControls field */
    0x01,0x00,0x00,                       /* Brightness control supported */
#else
    0x02,                                 /* Size of the bmControls field */
    0x01,0x00,                            /* Brightness control supported */
#endif
    0x00,                                 /* Unused iProcessing*/
#if USBCFG_VIDEO_CLASS_1_1
    0x00,                                 /* Ignore bmVideoStandards */
#endif

    /* Standard Interrupt Endpoint Descriptor */
    INTERRUPT_ENDPOINT_DESC_SIZE,         /* Size of this descriptor */
    USB_ENDPOINT_DESCRIPTOR,              /* ENDPOINT descriptor */
    VIDEO_CTL_ENDPOINT|(USB_SEND << 7),   /* IN endpoint 1 */
    USB_INTERRUPT_PIPE,                   /* Interrupt transfer type */
    USB_uint_16_low(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE), USB_uint_16_high(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE),
    FS_VIDEO_CTL_ENDP_INTERVAL,


    /* Standard VS Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_STREAM_IF_INDEX,                /* Index of this interface */
    0x00,                                 /* Index of this alternate setting */
    0x00,                                 /* 0 endpoints - no bandwidth used */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOSTREAMING,                    /* SC_VIDEOSTREAMING */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x00,                                 /* Unused */

    /* Class-specific VS Header Descriptor (Input) */
    0x0E,                                 /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_INPUT_HEADER,                      /* VS_INPUT_HEADER */
    0x01,                                 /* One format descriptor follows */
    0x55,0x00,                            /* Total size of class-specific VideoStream
                                             interface descriptor */
    VIDEO_ISO_ENDPOINT|(USB_SEND << 7),   /* Address of the isochronous endpoint
                                             used for video data */
    0x00,                                 /* No dynamic format change supported */
    0x03,                                 /* This Video Stream interface supplies
                                             terminal ID 3 (Output Terminal)*/
    VS_STILL_IMAGE_CAPTURE_METHOD,        /* Device supports still image capture method 2 */
    0x00,                                 /* Hardware trigger not supported for still image capture */
    0x00,                                 /* Hardware trigger should initiate a still image capture */
    0x01,                                 /* Size of the bmaControls field */
    0x00,                                 /* No VideoStreaming specific controls are supported for Format 0 */
    
    /* Class-specifc VS Format Descriptor */
    0x0B,                                 /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_FORMAT_MJPEG,                      /* VS_FORMAT_MJPEG */ 
    VS_FORMAT_MJPEG_DESC_INDEX,           /* Second format descriptor */
    0x01,                                 /* One frame descriptor for this format follows */
    0x01,                                 /* Used fixed size samples */
    0x01,                                 /* Default frame index is 1 */
    0x00,                                 /* Non-interlaced stream - not required */
    0x00,                                 /* Non-interlaced stream - not required */
    0x00,                                 /* Non-interlaced stream */
    0x00,                                 /* No restrictions imposed on
                                             the duplication of this video stream*/

    /* Class-specific VS Frame Descriptor */
    VS_FRAME_MJPEG_DESC_SIZE,             /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_FRAME_MJPEG,                       /* VS_FRAME_MJPEG */
    0x01,                                 /* First frame descriptor */
    0x00,                                 /* Still images using capture method 1 are
                                             supported at this frame setting.*/
    /*  D1: Fixed frame-rate*/
    0xB0,0x00,                            /* Width of frame is 176 pixels */
    0x90,0x00,                            /* Height of frame is 144 pixels */

    0x00,0xF0,0x1E,0x00,                  /* Min bit rate in bits/s */
    0x00,0xA0,0xB9,0x00,                  /* Max bit rate in bits/s */

    0x00,0xC6,0x00,0x00,                  /* Max video or still frame size */

    0x2A,0x2C,0x0A,0x00,                  /* Default frame interval is 15fps */

    0x06,                                 /* Continuous frame interval*/
    0x15,0x16,0x05,0x00,                  /* frame interval 0 is 30fps */
    0x80,0x1A,0x06,0x00,                  /* frame interval 1 is 25fps */
    0x20,0xA1,0x07,0x00,                  /* frame interval 2 is 20fps */
    0x2A,0x2C,0x0A,0x00,                  /* frame interval 3 is 15fps */
    0x40,0x42,0x0F,0x00,                  /* frame interval 4 is 10fps */
    0x80,0x84,0x1E,0x00,                  /* frame interval 5 is 05fps */
    
    /* Class-specific VS Still Frame Descriptor */
    VS_STILL_IMAGE_FRAME_ONLY_DESC_SIZE,  /* Size */
    CS_INTERFACE,                         /* CS_INTERFACE */
    VS_STILL_IMAGE_FRAME,                 /* VS_STILL_IMAGE_FRAME */
    0x00,                                 /* The address of the bulk endpoint used, only for Method 3 */
    0x01,                                 /* Number of Image Size patterns */
    0xB0,0x00,                            /* Width of frame is 176 pixels */
    0x90,0x00,                            /* Height of frame is 144 pixels */
    0x00,                                 /* The number of Compression pattern of this format is 0 */
    
    /* Standard VS Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,                 /* Size */
    USB_IFACE_DESCRIPTOR,                 /* INTERFACE descriptor type */
    VIDEO_STREAM_IF_INDEX,                /* Index of this interface */
    0x01,                                 /* Index of this alternate setting */
    VIDEO_DESC_ISO_ENDPOINT_COUNT,        /* 1 endpoints now banwidth used */
    CC_VIDEO,                             /* CC_VIDEO */
    SC_VIDEOSTREAMING,                    /* SC_VIDEOSTREAMING */
#if USBCFG_VIDEO_CLASS_1_5
    PC_PROTOCOL_15,                       /* PC_PROTOCOL_15 */
#else
    PC_PROTOCOL_UNDEFINED,                /* Not used, PC_PROTOCOL_UNDEFINED */
#endif
    0x00,                                 /* Unused */

    /* Standard VS Isochronous Video Data Endpoint Descriptor */
    VIDEO_STREAMING_ENDP_DESC_SIZE,       /* Size */
    USB_ENDPOINT_DESCRIPTOR,              /* USB_ENDPOINT_DESCRIPTOR */
    VIDEO_ISO_ENDPOINT|(USB_SEND << 7),   /* IN endpoint 2 */
    USB_ISOCHRONOUS_PIPE,                 /* Isochronous tranfer type */
    USB_uint_16_low(VIDEO_ISO_ENDPOINT_PACKET_SIZE), USB_uint_16_high(VIDEO_ISO_ENDPOINT_PACKET_SIZE),
                                          /* Max packet size */
    FS_VIDEO_STREAM_ENDP_INTERVAL
};
#endif

uint8_t USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] =     
{ sizeof(USB_STR_0),    
  USB_STRING_DESCRIPTOR, 
  0x09,
  0x04/*equiavlent to 0x0409*/ 
};
                                    
/*  Manufacturer string */                                    
uint8_t USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] = 
{  sizeof(USB_STR_1),          
   USB_STRING_DESCRIPTOR,
   'F',0,
   'R',0,
   'E',0,
   'E',0,
   'S',0,
   'C',0,
   'A',0,
   'L',0,
   'E',0,
   ' ',0,
   'S',0,
   'E',0,
   'M',0,
   'I',0,
   'C',0,
   'O',0,
   'N',0,
   'D',0,
   'U',0,
   'C',0,
   'T',0,
   'O',0,
   'R',0,
   ' ',0,
   'I',0,
   'N',0,
   'C',0,
   '.',0 
};

/*  Product string */
uint8_t USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] = 
{  
    sizeof(USB_STR_2),
    USB_STRING_DESCRIPTOR,
    'U',0,
    'S',0,
    'B',0,
    ' ',0,
    'V',0,
    'I',0,
    'D',0,
    'E',0,
    'O',0,
    ' ',0,
    'D',0,
    'E',0,
    'M',0,
    'O',0,
};

uint8_t USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] =
{  
   sizeof(USB_STR_n),         
   USB_STRING_DESCRIPTOR,
   'B',0,
   'A',0,
   'D',0,
   ' ',0,
   'S',0,
   'T',0,
   'R',0,
   'I',0,
   'N',0,
   'G',0,
   ' ',0,
   'I',0,
   'N',0,
   'D',0,
   'E',0,
   'X',0                               
};


uint32_t g_std_desc_size[USB_MAX_STD_DESCRIPTORS+1] =
{ 
    0,
    DEVICE_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE,
    0, /* string */
    0, /* Interfdace */
    0, /* Endpoint */
#if HIGH_SPEED
    DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE
#else                                         
     0, /* Device Qualifier */
     0 /* other spped config */
#endif
};
                                             
uint8_t *g_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] = 
{
    NULL,
    g_device_descriptor,
    g_config_descriptor,
    NULL, /* string */
    NULL, /* Interfdace */
    NULL, /* Endpoint */
#if HIGH_SPEED
    g_device_qualifier_descriptor,
    g_other_speed_config_descriptor
#else
    NULL, /* Device Qualifier */
    NULL /* other spped config*/
#endif
}; 
   
uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS+1] = 
    { sizeof(USB_STR_0),
      sizeof(USB_STR_1),
      sizeof(USB_STR_2),
      sizeof(USB_STR_n)
    };   
                                             
uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] = 
    { USB_STR_0,
      USB_STR_1,
      USB_STR_2,
      USB_STR_n
    };    

usb_language_t usb_language[USB_MAX_SUPPORTED_INTERFACES] = 
{{ (uint16_t)0x0409,g_string_descriptors, g_string_desc_size}};
                                                                                                   
usb_all_languages_t g_languages = 
    { USB_STR_0, sizeof(USB_STR_0), USB_MAX_LANGUAGES_SUPPORTED,
      usb_language 
    };

uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1] = {0,1};

/****************************************************************************
 * Global Variables
 ****************************************************************************/
static uint8_t g_alternate_interface[USB_MAX_SUPPORTED_INTERFACES];
                                                        
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes 
 *****************************************************************************/

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
#if 0
 DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback = 
{
   0xFF,
   USB_Desc_Get_Descriptor,
   USB_Desc_Get_Endpoints,
   USB_Desc_Get_Interface,
   USB_Desc_Set_Interface,
   USB_Desc_Valid_Configation,
   USB_Desc_Remote_Wakeup,
   NULL,
   NULL 
};
#endif
 

 
 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Descriptor
 *
 * @brief The function returns the correponding descriptor
 *
 * @param handle:        handle     
 * @param type:          type of descriptor requested     
 * @param str_num:       string index for string descriptor     
 * @param index:         string descriptor language Id     
 * @param descriptor:    output descriptor pointer
 * @param size:          size of descriptor returned
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor
(
    uint32_t handle, // this is not of type usb_device_handle because of some bug in CW 7.1 RegABI
    uint8_t type,
    uint8_t str_num, 
    uint16_t index,
    uint8_t * *descriptor,
    uint32_t *size
) 
{
    UNUSED_ARGUMENT (handle)
    
    /* string descriptors are handled saperately */
    if (type == USB_STRING_DESCRIPTOR)
    { 
        if(index == 0) 
        {  
            /* return the string and size of all languages */      
            *descriptor = (uint8_t *)g_languages.languages_supported_string;
            *size = g_languages.languages_supported_size;            
        } 
        else 
        {
            uint8_t lang_id=0;
            uint8_t lang_index=USB_MAX_LANGUAGES_SUPPORTED;
            
            for(;lang_id< USB_MAX_LANGUAGES_SUPPORTED;lang_id++) 
            {
                /* check whether we have a string for this language */
                if(index == g_languages.usb_language[lang_id].language_id) 
                {   /* check for max descriptors */
                    if(str_num < USB_MAX_STRING_DESCRIPTORS) 
                    {   /* setup index for the string to be returned */
                        lang_index=str_num;                 
                    }                    
                    break;                    
                }
            }
            
            /* set return val for descriptor and size */
            *descriptor = (uint8_t *)
                g_languages.usb_language[lang_id].lang_desc[lang_index];
            *size = 
                g_languages.usb_language[lang_id].lang_desc_size[lang_index];
        }        
    }
    else if (type < USB_MAX_STD_DESCRIPTORS+1)
    {
        /* set return val for descriptor and size*/
        *descriptor = (uint8_t *)g_std_descriptors [type];
       
        /* if there is no descriptor then return error */
        if(*descriptor == NULL) 
        {
            return USBERR_INVALID_REQ_TYPE;
        }        
        *size = g_std_desc_size[type];                
    }
    else /* invalid descriptor */
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    return USB_OK;  
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Interface
 *
 * @brief The function returns the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number     
 * @param alt_interface:  output alternate interface     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Interface
(
    uint32_t handle, 
    uint8_t interface, 
    uint8_t * alt_interface
)
{   
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get alternate interface*/
        *alt_interface = g_alternate_interface[interface];
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Interface
 *
 * @brief The function sets the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number     
 * @param alt_interface:  input alternate interface     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Interface
(
    uint32_t handle, 
    uint8_t interface, 
    uint8_t alt_interface
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface]=alt_interface;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Configation
 *
 * @brief The function checks whether the configuration parameter 
 *        input is valid or not
 *
 * @param handle          handle    
 * @param config_val      configuration value     
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************/
bool USB_Desc_Valid_Configation(uint32_t handle,uint16_t config_val)
{
    uint8_t     loop_index=0;
    UNUSED_ARGUMENT (handle)
    /* check with only supported val right now */
    while(loop_index < (USB_MAX_CONFIG_SUPPORTED+1)) 
    {
        if(config_val == g_valid_config_values[loop_index]) 
        {          
            return TRUE;
        }
        loop_index++;
    }
    return FALSE;    
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Remote_Wakeup
 *
 * @brief The function checks whether the remote wakeup is supported or not
 *
 * @param handle:        handle     
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - if remote wakeup supported
 *****************************************************************************/
bool USB_Desc_Remote_Wakeup(uint32_t handle) 
{
    UNUSED_ARGUMENT (handle)
    return USBCFG_DEV_REMOTE_WAKEUP;    
}           

/**************************************************************************//*!
 *
 * @name  USB_Set_Configation
 *
 * @brief The function checks whether the configuration parameter 
 *        input is valid or not
 *
 * @param handle          handle     
 * @param config_val      configuration value     
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************/
uint8_t USB_Set_Configation
(
        video_handle_t handle, uint8_t config

) 
{
    UNUSED_ARGUMENT (handle)

    return USB_OK;
}


/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Entity
 *
 * @brief The function retrieves the entity specified by type.
 *
 * @param handle            handle     
 *
 * @return USB_OK  - if success
 *****************************************************************************/
uint8_t USB_Desc_Get_Entity(video_handle_t handle,entity_type type, uint32_t * object)
{

    switch(type)  
    {
        case USB_CLASS_INFO:
            *object = (unsigned long)&usb_dec_class;
            break;
        case USB_VIDEO_UNITS:
            *object = (unsigned long)&usb_video_unit;
            break;
        case USB_VIDEO_CTL_IF_INDEX_INFO:
            *object = 0xff;
            if (handle == (uint32_t)virtual_camera.video_handle)
            {
                *object = (uint32_t)VIDEO_CTL_IF_INDEX;
                break;
            }
            break;
        case USB_VIDEO_STREAM_IF_INDEX_INFO:
            *object = 0xff;
            if (handle == (uint32_t)virtual_camera.video_handle)
            {
                *object = (uint32_t)VIDEO_STREAM_IF_INDEX;
                break;
            }
            break;
        default :
            break; 
    }/* End Switch */    
    return USB_OK;  
  
}


/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Speed
 *
 * @brief The function is used to set device speed
 *
 * @param handle:         handle     
 * @param speed:          speed    
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Speed
(
    uint32_t handle,
    uint16_t speed
)
{
    descriptor_union_t ptr1, ptr2;
    UNUSED_ARGUMENT(handle);

    ptr1.pntr = g_config_descriptor;
    ptr2.pntr = g_config_descriptor + CONFIG_DESC_SIZE;

    while (ptr1.word < ptr2.word)
    {
        if (ptr1.common->bDescriptorType == USB_DESC_TYPE_EP)
        {
            if (USB_INTERRUPT_PIPE == (ptr1.ndpt->bmAttributes && EP_TYPE_MASK))
            {
                if (USB_SPEED_HIGH == speed)
                {
                    ptr1.ndpt->iInterval = HS_VIDEO_CTL_ENDP_INTERVAL;
                    ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                    ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                }
                else
                {
                    ptr1.ndpt->iInterval = FS_VIDEO_CTL_ENDP_INTERVAL;
                    ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                    ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                }
            }
            else if (USB_ISOCHRONOUS_PIPE == (ptr1.ndpt->bmAttributes && EP_TYPE_MASK))
            {
                if (USB_SPEED_HIGH == speed)
                {
                    ptr1.ndpt->iInterval = HS_VIDEO_STREAM_ENDP_INTERVAL;
//                    ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
//                    ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                }
                else
                {
                    ptr1.ndpt->iInterval = FS_VIDEO_STREAM_ENDP_INTERVAL;
//                    ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
//                    ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE);
                }
            }
        }
        ptr1.word += ptr1.common->bLength;
    }

    for (int i = 0; i < VIDEO_DESC_CTL_ENDPOINT_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            control_ep[i].size = HS_VIDEO_CTL_ENDPOINT_PACKET_SIZE;
        }
        else
        {
            control_ep[i].size = FS_VIDEO_CTL_ENDPOINT_PACKET_SIZE;
        }
    }

    return USB_OK;
}

usb_desc_request_notify_struct_t  desc_callback = 
{
   USB_Desc_Get_Descriptor,
   USB_Desc_Get_Interface,
   USB_Desc_Set_Interface,
   USB_Set_Configation,
   USB_Desc_Get_Entity
}; 
/* EOF */
