/**HEADER********************************************************************
 *
 * Copyright (c) 2010, 2013-2015 Freescale Semiconductor;
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
 * @file usb_descriptor.c
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file contains USB descriptors for Audio Application
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "usb_descriptor.h"
#include "audio_generator.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/* structure containing details of all the endpoints used by this device */
usb_ep_struct_t ep[AUDIO_DESC_ENDPOINT_COUNT] =
{
    {
        AUDIO_ENDPOINT,
        USB_ISOCHRONOUS_PIPE,
        USB_SEND,
        FS_ISO_OUT_ENDP_PACKET_SIZE
    }
};
usb_endpoints_t usb_desc_ep =
{
    AUDIO_DESC_ENDPOINT_COUNT,
    ep
};

static usb_if_struct_t usb_if[1];

usb_class_struct_t usb_dec_class =
{
    USB_CLASS_AUDIO,
    {
        1,
        usb_if
    }
};

/* *********************************************************************
 * definition a struct of Input/output or Feature Unit
 ************************************************************************ */

/* Struct of Terminal Input /Output or Feature Unit */

audio_ut_struct_t ut[AUDIO_UNIT_COUNT] =
{
    { 0x01, AUDIO_CONTROL_INPUT_TERMINAL },
    { 0x02, AUDIO_CONTROL_FEATURE_UNIT },
    { 0x03, AUDIO_CONTROL_OUTPUT_TERMINAL },
};

audio_units_struct_t usb_audio_unit =
{
    AUDIO_UNIT_COUNT,
    ut
};

/* ******************************* END ******************************** */

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
   DEVICE_DESCRIPTOR_SIZE,               /* "Device Descriptor Size        */
   USB_DEVICE_DESCRIPTOR,                /* "Device" Type of descriptor    */
   0x00,0x01,                            /*  BCD USB version               */
   0x00,                                 /*  Device Class is indicated in
                                             the interface descriptors     */
   0x00,                                 /*  Device Subclass is indicated
                                             in the interface descriptors  */
   0x00,                                 /*  Device Protocol               */
   CONTROL_MAX_PACKET_SIZE,              /*  Max Packet size               */
   0x04,0x25,                            /*  Vendor ID   */
   0x00, 0x02,                           /*  Product ID  */
   0x00, 0x01,                           /*  BCD Device version */
   0x01,                                 /*  Manufacturer string index     */
   0x02,                                 /*  Product string index          */
   0x00,                                 /*  Serial number string index    */
   0x01                                  /*  Number of configurations      */
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{
   CONFIG_ONLY_DESC_SIZE,           /*  Configuration Descriptor Size - always 9 bytes*/
   USB_CONFIG_DESCRIPTOR,           /* "Configuration" type of descriptor */
   CONFIG_DESC_SIZE, 0x00,          /*  Total length of the Configuration descriptor */
   0x02,                            /*  NumInterfaces */
   0x01,                            /*  Configuration Value */
   0,                               /*  Configuration Description String Index*/
   /*  Attributes.support RemoteWakeup and self power */
   (USB_DESC_CFG_ATTRIBUTES_D7_POS) | (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
   /* S08/CFv1 are both self powered (its compulsory to set bus powered)*/
   /*Attributes.support RemoteWakeup and self power*/
   0x32,                            /*  Current draw from bus */

   /* AUDIO CONTROL INTERFACE DISCRIPTOR */
   IFACE_ONLY_DESC_SIZE,            /*Size of this descriptor*/
   USB_IFACE_DESCRIPTOR,            /*INTERFACE descriptor */
   0x00,                            /* Index of this interface*/
   0x00,                            /*Index of this setting*/
   0x00,                            /*0 endpoint*/
   USB_DEVICE_CLASS_AUDIO,          /*AUDIO*/
   USB_SUBCLASS_AUDIOCONTROL,       /*AUDIO_CONTROL*/
   0x00,                            /*Unused*/
   0x00,                            /* Unused*/
 
   /* Audio class-specific interface header */
   HEADER_ONLY_DESC_SIZE,           /* bLength (9) */  
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */  
   AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype (HEADER) */  
   0x00,0x01,                       /* bcdADC (1.0) */  
   0x27,0x00,                       /* wTotalLength (43) */  
   0x01,                            /* bInCollection (1 streaming interface)  */ 
   0x01,                            /* baInterfaceNr (interface 1 is stream) */    

  /* Audio class-specific input terminal       */
   INPUT_TERMINAL_ONLY_DESC_SIZE,   /* bLength (12) */  
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */  
   AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype (INPUT_TERMINAL)  */ 
   0x01,                            /* bTerminalID (1)  */ 
   0x01,0x02,                       /* wTerminalType (radio receiver)   */
   0x00,                            /* bAssocTerminal (none) */  
   0x01,                            /* bNrChannels (2)  */ 
   0x00,0x00,                       /* wChannelConfig (left, right) */  
   0x00,                            /* iChannelNames (none) */  
   0x00,                            /* iTerminal (none) */  

  /* Audio class-specific feature unit */    
   FEATURE_UNIT_ONLY_DESC_SIZE,     /* bLength (9) */  
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */  
   AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype (FEATURE_UNIT) */  
   0x02,                            /* bUnitID (2) */  
   0x01,                            /* bSourceID (input terminal 1)  */ 
   0x01,                            /* bControlSize (1 bytes) */  
   0x03,
   0x00,                            /* Master controls */  
   0x00,                            /* Channel 0 controls  */ 
   
   /* Audio class-specific output terminal   */ 
   OUTPUT_TERMINAL_ONLY_DESC_SIZE,  /* bLength (9)*/   
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE)  */ 
   AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype (OUTPUT_TERMINAL)*/   
   0x03,                            /* bTerminalID (3) */  
   0x01,0x01,                       /* wTerminalType (USB streaming) */  
   0x00,                            /* bAssocTerminal (none)  */ 
   0x02,                            /* bSourceID (feature unit 2)  */ 
   0x00,                            /* iTerminal (none)*/   
    
    /* USB speaker standard AS interface descriptor - audio streaming operational 
(Interface 1, Alternate Setting 0) */
   IFACE_ONLY_DESC_SIZE,            /* bLength (9)*/   
   USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */  
   0x01,                            /* interface Number: 1  */
   0x00,                            /* Alternate Setting: 0  */ 
   0x00,                            /* not used (Zero Bandwidth)  */
   USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO  */ 
   USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */  
   0x00,                            /* AUDIO PROTOCOL UNDEFINED  */ 
   0x00,                            /* Unused */

    /* USB speaker standard AS interface descriptor - audio streaming operational 
   (Interface 1, Alternate Setting 1) */
   IFACE_ONLY_DESC_SIZE,            /* bLength (9) */  
   USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */  
   0x01,                            /* interface Number: 1  */
   0x01,                            /* Alternate Setting: 1  */ 
   0x01,                            /* One Endpoint.  */
   USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO */  
   USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */  
   0x00,                            /* AUDIO PROTOCOL UNDEFINED  */ 
   0x00,                            /* Unused */
   
    /* USB speaker standard General AS interface descriptor */
   AUDIO_STREAMING_IFACE_DESC_SIZE, /* bLength (7)  */ 
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */  
   AUDIO_STREAMING_GENERAL,         /* GENERAL subtype */ 
   0x03,                            /* Unit ID of output terminal  */ 
   0x00,                            /* Interface delay */
   0x02,0x00,                       /* PCM format */

   /* USB speaker audio type I format interface descriptor */
   
   AUDIO_STREAMING_TYPE_I_DESC_SIZE,/* bLength (11) */   
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */  
   AUDIO_STREAMING_FORMAT_TYPE,     /* DescriptorSubtype: AUDIO STREAMING FORMAT TYPE */ 
   AUDIO_FORMAT_TYPE_I,             /* Format Type: Type I */
   0x01,                            /* Number of Channels: one channel */ 
   0x01,                            /* SubFrame Size: one byte per audio subframe */  
   0x08,                            /* Bit Resolution: 8 bits per sample  */ 
   0x01,                            /* One frequency supported  */ 
   0x40,0x1F,0x00,                  /* 8 kHz */
   
   /*Endpoint 1 - standard descriptor*/
   ENDP_ONLY_DESC_SIZE,             /* bLength (9) */  
   USB_ENDPOINT_DESCRIPTOR,         /* Descriptor type (endpoint descriptor) */  
   AUDIO_ENDPOINT | (USB_SEND << 7),/* OUT endpoint address 1 */
   0x01,                            /* Isochronous endpoint */
   USB_uint_16_low(FS_ISO_OUT_ENDP_PACKET_SIZE), 
   USB_uint_16_high(FS_ISO_OUT_ENDP_PACKET_SIZE),/* 16 bytes  */ 
#if HIGH_SPEED
   HS_ISO_OUT_ENDP_INTERVAL,        /* bInterval(0x04): 2^x ms */
#else
   FS_ISO_OUT_ENDP_INTERVAL,        /* bInterval(0x01): 2^x ms */
#endif
   0x00,                            /* Unused */
   0x00,                            /* Unused */

   /* Endpoint 1 - Audio streaming descriptor */
   AUDIO_STREAMING_ENDP_DESC_SIZE,  /* bLength (7) */  
   USB_AUDIO_DESCRIPTOR,            /* AUDIO ENDPOINT DESCRIPTOR TYPE */  
   AUDIO_ENDPOINT_GENERAL,          /* AUDIO ENDPOINT GENERAL */
   0x00,                            /* bmAttributes: 0x00 */  
   0x00,                            /* unused */
   0x00,0x00,                       /* unused */
};

uint8_t USB_STR_0[USB_STR_0_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(USB_STR_0),
    USB_STRING_DESCRIPTOR,
    0x09,
    0x04 /*equivalent to 0x0409*/
};

uint8_t USB_STR_1[USB_STR_1_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(USB_STR_1),
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

uint8_t USB_STR_2[USB_STR_2_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(USB_STR_2),
    USB_STRING_DESCRIPTOR,
    'U', 0,
    'S', 0,
    'B', 0,
    ' ', 0,
    'A', 0,
    'U', 0,
    'D', 0,
    'I', 0,
    'O', 0,
    ' ', 0,
    'D', 0,
    'E', 0,
    'M', 0,
    'O', 0,
};

uint8_t USB_STR_n[USB_STR_n_SIZE + USB_STR_DESC_SIZE] =
{
    sizeof(USB_STR_n),
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
    0, /* Device Qualifier */
    0, /* other speed config */
    0
};

uint8_t * g_std_descriptors[USB_MAX_STD_DESCRIPTORS + 1] =
{
NULL,
    g_device_descriptor,
    g_config_descriptor,
    NULL, /* string */
    NULL, /* Interface */
    NULL, /* Endpoint */
    NULL, /* Device Qualifier */
    NULL, /* other speed config*/
    NULL
};

uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS] =
{
    sizeof(USB_STR_0),
    sizeof(USB_STR_1),
    sizeof(USB_STR_2),
    sizeof(USB_STR_n)
};

uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS + 1] =
{
    USB_STR_0,
    USB_STR_1,
    USB_STR_2,
    USB_STR_n
};

usb_language_t usb_lang[USB_MAX_SUPPORTED_LANGUAGES] =
{
    {
        (uint16_t) 0x0409,
        g_string_descriptors,
        g_string_desc_size
    }
};

usb_all_languages_t g_languages =
{
    USB_STR_0, sizeof(USB_STR_0),
    USB_MAX_SUPPORTED_LANGUAGES,
    usb_lang
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
 * @param controller_ID : Controller ID
 * @param type          : Type of descriptor requested
 * @param sub_type      : String index for string descriptor
 * @param index         : String descriptor language Id
 * @param descriptor    : Output descriptor pointer
 * @param size          : Size of descriptor returned
 *
 * @return USB_OK                              When Successfully
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * This function is used to pass the pointer of the requested descriptor
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor
(
    uint32_t handle,
    uint8_t type,
    uint8_t str_num,
    uint16_t index,
    uint8_t * *descriptor,
    uint32_t *size
    )
{
    UNUSED_ARGUMENT(handle)

    /* string descriptors are handled separately */
    if (type == USB_STRING_DESCRIPTOR)
    {
        if (index == 0)
        {
            /* return the string and size of all languages */
            *descriptor = (uint8_t *) g_languages.languages_supported_string;
            *size = g_languages.languages_supported_size;
        }
        else
        {
            uint8_t lang_id = 0;
            uint8_t lang_index = USB_MAX_LANGUAGES_SUPPORTED;

            for (; lang_id < USB_MAX_LANGUAGES_SUPPORTED; lang_id++)
            {
                /* check whether we have a string for this language */
                if (index == g_languages.usb_language[lang_id].language_id)
                {
                    /* check for max descriptors */
                    if (str_num < USB_MAX_STRING_DESCRIPTORS)
                    {
                        /* setup index for the string to be returned */
                        lang_index = str_num;
                    }
                    break;
                }
            }
            /* set return val for descriptor and size */
            *descriptor =
            (uint8_t *) g_languages.usb_language[lang_id].lang_desc[str_num];
            *size =
            g_languages.usb_language[lang_id].lang_desc_size[lang_index];
        }
    }
    else if (type < USB_MAX_STD_DESCRIPTORS + 1)
    {
        /* set return val for descriptor and size*/
        *descriptor = (uint8_t *) g_std_descriptors[type];

        /* if there is no descriptor then return error */
        if (*descriptor == NULL)
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
 * @param controller_ID : Controller ID
 * @param interface     : Interface number
 * @param alt_interface : Output alternate interface
 *
 * @return USB_OK                              When Successfully
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *This function is called by the framework module to get the current interface
 *****************************************************************************/
uint8_t USB_Desc_Get_Interface
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * alt_interface
    )
{
    UNUSED_ARGUMENT(handle)

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
 * @param handle        : handle
 * @param interface     : Interface number
 * @param alt_interface : Input alternate interface
 *
 * @return USB_OK                              When Successfully
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *This function is called by the framework module to set the interface
 *****************************************************************************/
uint8_t USB_Desc_Set_Interface
(
    uint32_t handle,
    uint8_t interface,
    uint8_t alt_interface
)
{
    UNUSED_ARGUMENT(handle)

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
 * @name  USB_Desc_Valid_Configation
 *
 * @brief The function checks whether the configuration parameter
 *        input is valid or not
 *
 * @param handle        : handle
 * @param config_val    : Configuration value
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the configuration is valid or not
 *****************************************************************************/
bool USB_Desc_Valid_Configation
(
    uint32_t handle,/*[IN] Controller ID */
    uint16_t config_val /*[IN] Configuration value */
)
{
    uint8_t loop_index = 0;

    UNUSED_ARGUMENT(handle)

    /* check with only supported val right now */
    while (loop_index < (USB_MAX_CONFIG_SUPPORTED + 1))
    {
        if (config_val == g_valid_config_values[loop_index])
        {
            return TRUE;
        }
        loop_index++;
    }
    return FALSE;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Interface
 *
 * @brief The function checks whether the interface parameter
 *        input is valid or not
 *
 * @param handle        : handle
 * @param interface     : Target interface
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the interface is valid or not
 *****************************************************************************/
bool USB_Desc_Valid_Interface
(
    uint32_t handle, /*[IN] Controller ID */
    uint8_t interface /*[IN] Target interface */
)
{
    uint8_t loop_index = 0;
    UNUSED_ARGUMENT(handle)
    /* check with only supported val right now */
    while (loop_index < USB_MAX_SUPPORTED_INTERFACES)
    {
        if (interface == g_alternate_interface[loop_index])
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
 * @param handle : handle
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - If remote wakeup supported
 *****************************************************************************
 * This function returns remote wakeup is supported or not
 *****************************************************************************/
bool USB_Desc_Remote_Wakeup
(
    uint32_t handle /* [IN] Controller ID */
)
{
    UNUSED_ARGUMENT(handle)
    return USBCFG_DEV_REMOTE_WAKEUP;
}

/* ****************************************************************************
 * Local available
 * Stock Array value of Parameter controller
 ******************************************************************************/

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Endpoints
 *
 * @brief The function returns with the list of all non control endpoints used
 *
 * @param handle : handle
 *
 * @return pointer to usb_endpoints_t
 *****************************************************************************
 * This function returns the information about all the non control endpoints
 * implemented
 *****************************************************************************/
usb_endpoints_t* USB_Desc_Get_Endpoints(uint32_t handle)
{
    return &usb_desc_ep;
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
    audio_handle_t handle, uint8_t config

) 
{
    UNUSED_ARGUMENT(handle)
    /* TODO */
    /* if a device has two or more configuration, should set the configuration*/
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
uint8_t USB_Desc_Get_Entity(audio_handle_t handle, entity_type type, uint32_t * object)
{

    switch(type)
    {
    case USB_CLASS_INFO:
        usb_if[0].index = 1;
        usb_if[0].endpoints = usb_desc_ep;
        *object = (unsigned long) &usb_dec_class;
        break;
    case USB_AUDIO_UNITS:
        *object = (unsigned long) &usb_audio_unit;
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
                ptr1.ndpt->iInterval = HS_ISO_OUT_ENDP_INTERVAL;
                ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(HS_ISO_OUT_ENDP_PACKET_SIZE);
                ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(HS_ISO_OUT_ENDP_PACKET_SIZE);
            }
            else
            {
                ptr1.ndpt->iInterval = FS_ISO_OUT_ENDP_INTERVAL;
                ptr1.ndpt->wMaxPacketSize[0] = USB_uint_16_low(FS_ISO_OUT_ENDP_PACKET_SIZE);
                ptr1.ndpt->wMaxPacketSize[1] = USB_uint_16_high(FS_ISO_OUT_ENDP_PACKET_SIZE);
            }
        }
        ptr1.word += ptr1.common->bLength;
    }

    for (int i = 0; i < AUDIO_DESC_ENDPOINT_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            ep[i].size = HS_ISO_OUT_ENDP_PACKET_SIZE;
        }
        else
        {
            ep[i].size = FS_ISO_OUT_ENDP_PACKET_SIZE;
        }
    }

    return USB_OK;
}

usb_desc_request_notify_struct_t desc_callback =
{
    USB_Desc_Get_Descriptor,
    USB_Desc_Get_Interface,
    USB_Desc_Set_Interface,
    USB_Set_Configuration,
    USB_Desc_Get_Entity
};
/* EOF */
