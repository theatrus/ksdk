/**HEADER********************************************************************
 * 
 * Copyright (c) 2004 - 2009, 2013- 2014 Freescale Semiconductor;
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
 * @file usb_descriptor.c
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file contains USB descriptors and functions
 *
 *****************************************************************************/

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

/******************************************************************************
 * Includes
 *****************************************************************************/

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/

#define REMOTE_WAKEUP_STATUS_MASK           (0x0002)
//#define BUS_POWERED                         (0x80)
//#define SELF_POWERED                        (0x40)
//#define SELF_POWER_BIT_SHIFT                (6)

/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (18)
#define CONFIG_DESC_SIZE                  (34)
#define REPORT_DESC_SIZE                  (63)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define HID_ONLY_DESC_SIZE                (9)
#define ENDP_ONLY_DESC_SIZE               (7)

#if HIGH_SPEED
#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE    (10)
#define OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE  (CONFIG_DESC_SIZE)  
#endif

/* HID buffer size */
#define HID_BUFFER_SIZE                         (8)
/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS                 (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS      (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED                (1)

/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS              (3)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED             (1)

#define HID_DESC_ENDPOINT_COUNT                 (1)
#define HID_ENDPOINT                            (1)
#define HID_DESC_INTERFACE_COUNT                (1)
#define HS_INTERRUPT_OUT_ENDP_PACKET_SIZE       (16)
#define FS_INTERRUPT_OUT_ENDP_PACKET_SIZE       (16)
#define HS_INTERRUPT_OUT_ENDP_INTERVAL          (0x07) /* 2^(7-1) = 8ms */
#define FS_INTERRUPT_OUT_ENDP_INTERVAL          (0x08)

/* string descriptors sizes */
#define USB_STR_DESC_SIZE                       (2)
#define USB_STR_0_SIZE                          (2)
#define USB_STR_1_SIZE                          (56)
#define USB_STR_2_SIZE                          (38)
#define USB_STR_n_SIZE                          (32)

/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR                   (1)
#define USB_CONFIG_DESCRIPTOR                   (2)
#define USB_STRING_DESCRIPTOR                   (3)
#define USB_IFACE_DESCRIPTOR                    (4)
#define USB_ENDPOINT_DESCRIPTOR                 (5)
#define USB_HID_DESCRIPTOR                      (0x21)
#define USB_REPORT_DESCRIPTOR                   (0x22)

#if HIGH_SPEED
    #define USB_DEVQUAL_DESCRIPTOR              (6)
    #define USB_OTHER_SPEED_DESCRIPTOR          (7)
#endif

#define USB_MAX_SUPPORTED_INTERFACES            (1)

/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern bool USB_Desc_Remote_Wakeup(uint8_t controller_ID);

extern void* USB_Desc_Get_Endpoints(uint8_t controller_ID);
extern uint8_t USB_Desc_Get_Descriptor(
    hid_handle_t handle,
    uint8_t type,
    uint8_t str_num,
    uint16_t index,
    uint8_t * *descriptor,
    uint32_t *size);

extern uint8_t USB_Desc_Get_Interface(
    hid_handle_t handle,
    uint8_t interface,
    uint8_t * alt_interface);

extern uint8_t USB_Desc_Set_Interface(
    hid_handle_t handle,
    uint8_t interface,
    uint8_t alt_interface);

extern bool USB_Desc_Valid_Configation(hid_handle_t handle,
    uint16_t config_val);

extern bool USB_Desc_Valid_Interface(
    uint8_t controller_ID,
    uint8_t interface);
uint8_t USB_Desc_Set_Speed
(
    uint32_t handle,
    uint16_t speed
    );

#endif
