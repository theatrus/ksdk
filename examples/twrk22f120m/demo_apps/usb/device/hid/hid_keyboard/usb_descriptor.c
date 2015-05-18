/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013- 2015 Freescale Semiconductor;
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
 * @brief The file contains USB descriptors for Keyboard Application
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "usb_class_hid.h"
#include "keyboard.h"
#include "usb_descriptor.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
usb_ep_struct_t g_ep[HID_DESC_ENDPOINT_COUNT] =
{
    {
        HID_ENDPOINT,
        USB_INTERRUPT_PIPE,
        USB_SEND,
        FS_INTERRUPT_OUT_ENDP_PACKET_SIZE,
    }
};

/* structure containing details of all the endpoints used by this device */
usb_endpoints_t g_usb_desc_ep =
{
    HID_DESC_ENDPOINT_COUNT,
    g_ep
};

static usb_if_struct_t g_usb_if[1];

usb_class_struct_t g_usb_dec_class =
{
    USB_CLASS_HID,
    {
        1,
        g_usb_if
    }
};

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    DEVICE_DESCRIPTOR_SIZE,               /*  Device Descriptor Size        */
    USB_DEVICE_DESCRIPTOR,                /*  Device  Type of descriptor    */
    0x00, 0x02,                           /*  BCD USB version               */
    0x00,                                 /*  Device Class is indicated in
                                              the interface descriptors     */
    0x00,                                 /*  Device Subclass is indicated
                                              in the interface descriptors  */
    0x00,                                 /*  Device Protocol               */
    CONTROL_MAX_PACKET_SIZE,              /*  Max Packet size               */
    0xA2,0x15,                            /*  Vendor ID                     */
    0x01,0x01,                            /*  Product ID (0x0101 for KBD)   */
    0x02,0x00,                            /*  BCD Device version            */
    0x01,                                 /*  Manufacturer string index     */
    0x02,                                 /*  Product string index          */
    0x00,                                 /*  Serial number string index    */
    0x01                                  /*  Number of configurations      */
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{
    CONFIG_ONLY_DESC_SIZE,  /*  Configuration Descriptor Size - always 9 bytes*/
    USB_CONFIG_DESCRIPTOR,  /* "Configuration" type of descriptor */
    CONFIG_DESC_SIZE, 0x00, /*  Total length of the Configuration descriptor */
    1,                      /*  NumInterfaces */
    1,                      /*  Configuration Value */
    0,                      /*  Configuration Description String Index*/
    (USB_DESC_CFG_ATTRIBUTES_D7_POS) | (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
    /* S08/CFv1 are both self powered (its compulsory to set bus powered)*/
    /* Attributes.support RemoteWakeup and self power */
    0x32,                   /*  Current draw from bus */

    /* Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00,
    0x00,
    HID_DESC_ENDPOINT_COUNT,
    0x03,
    0x01,
    0x01, /* 0x01 for keyboard */
    0x00,

    /* HID descriptor */
    HID_ONLY_DESC_SIZE,
    USB_HID_DESCRIPTOR,
    0x00, 0x01,
    0x00,
    0x01,
    0x22,
    0x3F, 0x00, /* report descriptor size to follow */

    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    HID_ENDPOINT | (USB_SEND << 7),
    USB_INTERRUPT_PIPE,
    FS_INTERRUPT_OUT_ENDP_PACKET_SIZE, 0x00,
    FS_INTERRUPT_OUT_ENDP_INTERVAL
};
#if HIGH_SPEED
uint8_t g_device_qualifier_descriptor[DEVICE_QUALIFIER_DESCRIPTOR_SIZE] =
{
    /* Device Qualifier Descriptor Size */
    DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
    /* Type of Descriptor */
    USB_DEVQUAL_DESCRIPTOR,
    /*  BCD USB version  */
    0x00, 0x02,
    /* bDeviceClass */
    0x00,
    /* bDeviceSubClass */
    0x00,
    /* bDeviceProtocol */
    0x00,
    /* bMaxPacketSize0 */
    CONTROL_MAX_PACKET_SIZE,
    /* bNumConfigurations */
    0x00,
    /* Reserved : must be zero */
    0x00
};

uint8_t g_other_speed_config_descriptor[OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE] =
{
    /* Length of this descriptor */
    CONFIG_ONLY_DESC_SIZE,
    /* This is a Other speed config descr */
    USB_OTHER_SPEED_DESCRIPTOR,
    /*  Total length of the Configuration descriptor */
    USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
    0x01,
    /*value used to select this configuration : Configuration Value */
    1,
    /*  Configuration Description String Index*/
    0,
    /*  Attributes.support RemoteWakeup and self power */
    (USB_DESC_CFG_ATTRIBUTES_D7_POS) | (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
    /*  Current draw from bus */
    0x32,

    /* Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00,
    0x00,
    HID_DESC_ENDPOINT_COUNT,
    0x03,
    0x01,
    0x02,
    0x00,

    /* HID descriptor */
    HID_ONLY_DESC_SIZE,
    USB_HID_DESCRIPTOR,
    0x00,0x01,
    0x00,
    0x01,
    0x22,
    0x34,0x00,

    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    HID_ENDPOINT|(USB_SEND << 7),
    USB_INTERRUPT_PIPE,
    FS_INTERRUPT_OUT_ENDP_PACKET_SIZE, 0x00,
    FS_INTERRUPT_OUT_ENDP_INTERVAL
};
#endif
uint8_t g_report_descriptor[REPORT_DESC_SIZE] =
{
    0x05, 0x01, /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x06, /* USAGE (Keyboard) */
    0xa1, 0x01, /* COLLECTION (Application) */
    0x05, 0x07, /*   USAGE_PAGE (Keyboard) */
    0x19, 0xe0, /*   USAGE_MINIMUM (Keyboard LeftControl) */
    0x29, 0xe7, /*   USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15, 0x00, /*   LOGICAL_MINIMUM (0) */
    0x25, 0x01, /*   LOGICAL_MAXIMUM (1) */
    0x75, 0x01, /*   REPORT_SIZE (1) */
    0x95, 0x08, /*   REPORT_COUNT (8) */
    0x81, 0x02, /*   INPUT (Data,Var,Abs) modifier keys (CTRL, ALT, etc...*/
    0x95, 0x01, /*   REPORT_COUNT (1) */
    0x75, 0x08, /*   REPORT_SIZE (8) */
    0x81, 0x01, /*   INPUT (Cnst,Var,Abs) filupp to byte boundary */
    0x95, 0x05, /*   REPORT_COUNT (5) */
    0x75, 0x01, /*   REPORT_SIZE (1) */
    0x05, 0x08, /*   USAGE_PAGE (LEDs) */
    0x19, 0x01, /*   USAGE_MINIMUM (Num Lock) */
    0x29, 0x05, /*   USAGE_MAXIMUM (Kana) */
    0x91, 0x02, /*   OUTPUT (Data,Var,Abs) pc->kbd */
    0x95, 0x01, /*   REPORT_COUNT (1) */
    0x75, 0x03, /*   REPORT_SIZE (3 */
    0x91, 0x01, /*   OUTPUT (Cnst,Var,Abs) filupp to byte boundary */
    0x95, 0x06, /*   REPORT_COUNT (6) */
    0x75, 0x08, /*   REPORT_SIZE (8) */
    0x15, 0x00, /*   LOGICAL_MINIMUM (0) */
    0x25, 0x65, /*   LOGICAL_MAXIMUM (101) */
    0x05, 0x07, /*   USAGE_PAGE (Keyboard) */
    0x19, 0x00, /*   USAGE_MINIMUM (Reserved (no event indicated)) */
    0x29, 0x65, /*   USAGE_MAXIMUM (Keyboard Application) */
    0x81, 0x00, /*   INPUT (Data,Ary,Abs) array for pressed keys */
    0xc0 /* END_COLLECTION */
};

/* number of strings in the table not including 0 or n. */
uint8_t g_usb_str_0[USB_STR_0_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(g_usb_str_0),
    USB_STRING_DESCRIPTOR,
    0x09,
    0x04 /*equivalent to 0x0409*/
};

uint8_t g_usb_str_1[USB_STR_1_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(g_usb_str_1),
    USB_STRING_DESCRIPTOR,
    'F', 0,
    'R', 0,
    'E', 0,
    'E', 0,
    'S', 0,
    'C', 0,
    'A', 0,
    'L', 0,
    'E', 0,
    ' ', 0,
    'S', 0,
    'E', 0,
    'M', 0,
    'I', 0,
    'C', 0,
    'O', 0,
    'N', 0,
    'D', 0,
    'U', 0,
    'C', 0,
    'T', 0,
    'O', 0,
    'R', 0,
    ' ', 0,
    'I', 0,
    'N', 0,
    'C', 0,
    '.', 0
};

uint8_t g_usb_str_2[USB_STR_2_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(g_usb_str_2),
    USB_STRING_DESCRIPTOR,
    'M', 0,
    'C', 0,
    'U', 0,
    ' ', 0,
    'K', 0,
    'E', 0,
    'Y', 0,
    'B', 0,
    'O', 0,
    'A', 0,
    'R', 0,
    'D', 0,
    ' ', 0,
    'D', 0,
    'E', 0,
    'M', 0,
    'O', 0,
    ' ', 0
};

uint8_t g_usb_str_n[USB_STR_n_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(g_usb_str_n),
    USB_STRING_DESCRIPTOR,
    'B', 0,
    'A', 0,
    'D', 0,
    ' ', 0,
    'S', 0,
    'T', 0,
    'R', 0,
    'I', 0,
    'N', 0,
    'G', 0,
    ' ', 0,
    'I', 0,
    'N', 0,
    'D', 0,
    'E', 0,
    'X', 0
};

uint32_t g_std_desc_size[USB_MAX_STD_DESCRIPTORS + 1] =
{
    0,
    DEVICE_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE,
    0, /* string */
    0, /* Interface */
    0, /* Endpoint */
#if HIGH_SPEED
    DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
    OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE,
#else
    0, /* Device Qualifier */
    0, /* other speed config */
#endif
    REPORT_DESC_SIZE
};

uint8_t *g_std_descriptors[USB_MAX_STD_DESCRIPTORS + 1] =
{
    NULL,
    g_device_descriptor,
    g_config_descriptor,
    NULL, /* string */
    NULL, /* Interface */
    NULL, /* Endpoint */
#if HIGH_SPEED
    g_device_qualifier_descriptor,
    g_other_speed_config_descriptor,
#else
    NULL, /* Device Qualifier */
    NULL, /* other speed config*/
#endif
    g_report_descriptor
};

uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS + 1] =
{
    sizeof(g_usb_str_0),
    sizeof(g_usb_str_1),
    sizeof(g_usb_str_2),
    sizeof(g_usb_str_n)
};

uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS + 1] =
{
    g_usb_str_0,
    g_usb_str_1,
    g_usb_str_2,
    g_usb_str_n
};

usb_language_t g_usb_language[USB_MAX_SUPPORTED_INTERFACES] =
{
    {
        (uint16_t) 0x0409,
        g_string_descriptors,
        g_string_desc_size
    }
};

usb_all_languages_t g_languages =
{
    g_usb_str_0,
    sizeof(g_usb_str_0),
    USB_MAX_LANGUAGES_SUPPORTED,
    g_usb_language
};

uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED + 1] = { 0, 1 };

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
 * Local Variables - None
 *****************************************************************************/

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
 * @brief The function returns the corresponding descriptor
 *
 * @param handle:        handle     
 * @param type          : type of descriptor requested
 * @param sub_type      : string index for string descriptor
 * @param index         : string descriptor language Id
 * @param descriptor    : output descriptor pointer
 * @param size          : size of descriptor returned
 *
 * @return USB_OK                      When Success
 *         USBERR_INVALID_REQ_TYPE     when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor
(
    hid_handle_t handle,
    uint8_t type,
    uint8_t str_num,
    uint16_t index,
    uint8_t * *descriptor,
    uint32_t *size
)
{
    UNUSED_ARGUMENT (handle)

    switch(type)
    {
    case USB_REPORT_DESCRIPTOR:
        {
        type = USB_MAX_STD_DESCRIPTORS;
        *descriptor = (uint8_t *) g_std_descriptors[type];
        *size = g_std_desc_size[type];
    }
        break;
    case USB_HID_DESCRIPTOR:
        {
        type = USB_CONFIG_DESCRIPTOR;
        *descriptor = (uint8_t *) (g_std_descriptors[type] +
        CONFIG_ONLY_DESC_SIZE + IFACE_ONLY_DESC_SIZE);
        *size = HID_ONLY_DESC_SIZE;
    }
        break;
    case USB_STRING_DESCRIPTOR:
        {
        if (index == 0)
        {
            /* return the string and size of all languages */
            *descriptor =
            (uint8_t *) g_languages.languages_supported_string;
            *size = g_languages.languages_supported_size;
        }
        else
        {
            uint8_t lang_id = 0;
            uint8_t lang_index = USB_MAX_LANGUAGES_SUPPORTED;

            for (; lang_id < USB_MAX_LANGUAGES_SUPPORTED; lang_id++)
            {
                /* check whether we have a string for this language */
                if (index ==
                g_languages.usb_language[lang_id].language_id)
                { /* check for max descriptors */
                    if (str_num < USB_MAX_STRING_DESCRIPTORS)
                    { /* setup index for the string to be returned */
                        lang_index = str_num;
                    }
                    break;
                }
            }
            /* set return val for descriptor and size */
            *descriptor = (uint8_t *)
            g_languages.usb_language[lang_id].lang_desc[lang_index];
            *size =
            g_languages.usb_language[lang_id].
            lang_desc_size[lang_index];
        }
    }
        break;
    default:
        if (type < USB_MAX_STD_DESCRIPTORS)
        {
            /* set return val for descriptor and size*/
            *descriptor = (uint8_t *) g_std_descriptors[type];

            /* if there is no descriptor then return error */
            *size = g_std_desc_size[type];

            if (*descriptor == NULL)
            {
                return USBERR_INVALID_REQ_TYPE;
            }

        }
        else /* invalid descriptor */
        {
            return USBERR_INVALID_REQ_TYPE;
        }
        break;
    }/* End Switch */
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
 * @return USB_OK                     When Success
 *         USBERR_INVALID_REQ_TYPE    when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Interface
(
    hid_handle_t handle,
    uint8_t interface,
    uint8_t * alt_interface
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
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
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Interface
(
    hid_handle_t handle,
    uint8_t interface,
    uint8_t alt_interface
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface] = alt_interface;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Configuration
 *
 * @brief The function set the configuration value of device
 *        
 *
 * @param handle          handle
 * @param config_val      configuration value
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************/
uint8_t USB_Set_Configuration
(
    hid_handle_t handle, uint8_t config
)
{
    UNUSED_ARGUMENT (handle)
    /* TODO */
    /* if a device has two or more configuration, should set the configuration*/ 
    return USB_OK;
}

//usb_if_struct_t usb_qq[5] = {1,usb_desc_ep};
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
uint8_t USB_Desc_Get_Entity(hid_handle_t handle, entity_type type, uint32_t * object)
{

    switch(type)
    {
    case USB_CLASS_INFO:
        g_usb_if[0].index = 1;
        g_usb_if[0].endpoints = g_usb_desc_ep;
        *object = (unsigned long) &g_usb_dec_class;
        break;
    default:
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
            if (USB_SPEED_HIGH == speed)
            {
                ptr1.ndpt->iInterval = HS_INTERRUPT_OUT_ENDP_INTERVAL;
                ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(HS_INTERRUPT_OUT_ENDP_PACKET_SIZE);
                ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(HS_INTERRUPT_OUT_ENDP_PACKET_SIZE);
            }
            else
            {
                ptr1.ndpt->iInterval = FS_INTERRUPT_OUT_ENDP_INTERVAL;
                ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(FS_INTERRUPT_OUT_ENDP_PACKET_SIZE);
                ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(FS_INTERRUPT_OUT_ENDP_PACKET_SIZE);
            }
        }
        ptr1.word += ptr1.common->bLength;
    }

    for (int i = 0; i < HID_DESC_ENDPOINT_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            g_ep[i].size = HS_INTERRUPT_OUT_ENDP_PACKET_SIZE;
        }
        else
        {
            g_ep[i].size = FS_INTERRUPT_OUT_ENDP_PACKET_SIZE;
        }
    }

    return USB_OK;
}

usb_desc_request_notify_struct_t g_desc_callback =
{
    USB_Desc_Get_Descriptor,
    USB_Desc_Get_Interface,
    USB_Desc_Set_Interface,
    USB_Set_Configuration,
    USB_Desc_Get_Entity
};
/* EOF */
