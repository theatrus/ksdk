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
#include "usb_class_audio.h"
#include "usb_device_stack_interface.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
 
#define BCD_USB_VERSION                  (0x0200)
 
/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (18)
#if USBCFG_AUDIO_CLASS_2_0
#define CONFIG_DESC_SIZE                   141//132// (0x6D+8) 
#else
#define CONFIG_DESC_SIZE                   (0x6D)
#endif
#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
#define REPORT_DESC_SIZE                  (50)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define AUDIO_ONLY_DESC_SIZE              (9)
#define ENDP_ONLY_DESC_SIZE               (9)
#define HEADER_ONLY_DESC_SIZE             (9)
#define INPUT_TERMINAL_ONLY_DESC_SIZE     (12)
#define OUTPUT_TERMINAL_ONLY_DESC_SIZE    (9)
#define FEATURE_UNIT_ONLY_DESC_SIZE       (9)
#define AUDIO_STREAMING_IFACE_DESC_SIZE   (7)
#define AUDIO_STREAMING_ENDP_DESC_SIZE    (7)
#define AUDIO_FORMAT_TYPE_I_DESC_SIZE      (11)

#define USB_DEVICE_CLASS_AUDIO            (0x01)
#define USB_SUBCLASS_AUDIOCONTROL         (0x01)
#define USB_SUBCLASS_AUDIOSTREAM          (0x02)

#define AUDIO_INTERFACE_DESCRIPTOR_TYPE   (0x24)

#define AUDIO_CONTROL_HEADER              (0x01)
#define AUDIO_CONTROL_INPUT_TERMINAL      (0x02)
#define AUDIO_CONTROL_OUTPUT_TERMINAL     (0x03)
#define AUDIO_CONTROL_FEATURE_UNIT        (0x06)
#define AUDIO_STREAMING_GENERAL           (0x01)
#define AUDIO_STREAMING_FORMAT_TYPE       (0x02)
#define AUDIO_FORMAT_TYPE_I               (0x01)
#define AUDIO_ENDPOINT_GENERAL            (0x01)

/* Audio data format */
#define AUDIO_FORMAT_CHANNELS             (0x02)
#define AUDIO_FORMAT_BITS                 (16)
#define AUDIO_FORMAT_SIZE                 (0x02)
#define AUDIO_FORMAT_SAMPLE_RATE_48K      (0xBB80)
#define AUDIO_FORMAT_SAMPLE_RATE_8K       (0x1F40)
#define AUDIO_FORMAT_SAMPLE_RATE_44K1     (0xAC44)
#define AUDIO_FORMAT_SAMPLE_RATE_32K      (0x7D00)

/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS               (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS    (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED          (1)

/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS        (4)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED       (1)

#if USBCFG_AUDIO_CLASS_2_0
#define AUDIO_UNIT_COUNT                  (4)
#else
#define AUDIO_UNIT_COUNT                  (3)
#endif

#define AUDIO_DESC_ENDPOINT_COUNT         (1)
#define AUDIO_ISOCHRONOUS_ENDPOINT        (2)

#define HS_ISO_OUT_ENDP_PACKET_SIZE      (64)
#define FS_ISO_OUT_ENDP_PACKET_SIZE      (64)
#define HS_ISO_OUT_ENDP_INTERVAL         (0x04)
#define FS_ISO_OUT_ENDP_INTERVAL         (0x01)

/* string descriptors sizes */
#define USB_STR_DESC_SIZE (2)
#define USB_STR_0_SIZE  (2)
#define USB_STR_1_SIZE  (56)
#define USB_STR_2_SIZE  (28)
#define USB_STR_n_SIZE  (32)

/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR     (1)
#define USB_CONFIG_DESCRIPTOR     (2)
#define USB_STRING_DESCRIPTOR     (3)
#define USB_IFACE_DESCRIPTOR      (4)
#define USB_ENDPOINT_DESCRIPTOR   (5)
#define USB_DEVQUAL_DESCRIPTOR    (6)
#define USB_AUDIO_DESCRIPTOR      (0x25)
#define USB_REPORT_DESCRIPTOR     (0x22)

#define USB_MAX_SUPPORTED_INTERFACES     (1)
#define USB_MAX_SUPPORTED_LANGUAGES     (1)
#define CONTROL_MAX_PACKET_SIZE          (64)

#if USBCFG_AUDIO_CLASS_2_0
#define AUDIO_CONTROL_CLOCK_SOURCE_UNIT                  (0x0A)
#define AUDIO_CONTROL_CLOCK_SELECTOR_UNIT                (0x0B)
#define AUDIO_CONTROL_CLOCK_MULTIPLIER_UNIT              (0x0C)
#define AUDIO_CONTROL_SAMPLE_RATE_CONVERTER_UNIT         (0x0D)
#endif

/******************************************************************************
 * Types
 *****************************************************************************/


/******************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

uint8_t USB_Desc_Get_Descriptor(uint32_t handle, uint8_t type, uint8_t str_num, uint16_t index, uint8_t * *descriptor, uint32_t *size);
uint8_t USB_Desc_Get_Interface(uint32_t handle, uint8_t interface, uint8_t * alt_interface);
uint8_t USB_Desc_Set_Interface(uint32_t handle, uint8_t interface, uint8_t alt_interface);
bool USB_Desc_Valid_Configation(uint32_t handle, uint16_t config_val);
bool USB_Desc_Valid_Interface(uint32_t handle, uint8_t interface);

bool USB_Desc_Remote_Wakeup(uint32_t handle);
uint8_t USB_Desc_Set_Speed
(
    uint32_t handle,
    uint16_t speed
);

#ifdef __cplusplus
}
#endif

#endif
