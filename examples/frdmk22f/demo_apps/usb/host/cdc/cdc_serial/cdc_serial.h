/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: cdc_serial.h$
 * $Version : 3.8.6.0$
 * $Date    : Sep-13-2012$
 *
 * Comments:
 *
 *   This file contains keyboard-application types and definitions.
 *
 *END************************************************************************/
#ifndef __cdc_serial_h__
#define __cdc_serial_h__

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
//#include "hostapi.h"
//#include "io.h"
#endif

/***************************************
 **
 ** Application-specific definitions
 */
#define  HIGH_SPEED           (0)

#if HIGH_SPEED
#define CONTROLLER_ID         USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID         USB_CONTROLLER_KHCI_0
#endif

/* Used to initialize USB controller */
#define HOST_CONTROLLER_NUMBER      USBCFG_DEFAULT_HOST_CONTROLLER
#define CDC_EXAMPLE_USE_HW_FLOW     1

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define DEVICE_REGISTERED_EVENT            (0x01)
#define DEVICE_CTRL_ATTACHED               (0x02)
#define DEVICE_CTRL_INTERFACED             (0x04)
#define DEVICE_DATA_ATTACHED               (0x08)
#define DEVICE_DATA_INTERFACED             (0x10)
#define DEVICE_CTRL_DETACHED               (0x20)

#define CDC_SERIAL_DEV_ATTACHED            (0x01)
#define CDC_SERIAL_DEV_REGISTERED          (0x02)
#define CDC_SERIAL_FUSB_OPENED_UART2USB    (0x04)
#define CDC_SERIAL_FUSB_OPENED_USB2UART    (0x08)
#define CDC_SERIAL_DEV_DETACH_USB2UART     (0x10)
#define CDC_SERIAL_DEV_DETACH_UART2USB     (0x20)
#define CDC_SERIAL_UART2USB_DONE           (0x40)
#define CDC_SERIAL_USB2UART_DONE           (0x80)

/* CDC status */
#define CDC_ERROR (-1)
#define CDC_OK    (0)
/* CDC app */
#define CDC_ASYNC               (1)
#define CDC_SERIAL_ECHO_BACK    (0)

/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */

typedef struct acm_device_struct
{
    cdc_class_call_struct_t CLASS_INTF; /* Class-specific info */
    os_event_handle acm_event;
} acm_device_struct_t;

typedef struct data_device_struct
{
    cdc_class_call_struct_t CLASS_INTF; /* Class-specific info */
    os_event_handle data_event;
} data_device_struct_t;

typedef enum
{
    CDC_SERIAL_FUSB_OPEND = 0,
    CDC_SERIAL_ATTACHED,
    CDC_SERIAL_DETACHED,
    CDC_SERIAL_STS_INVALID,
} CDC_SERIAL_STS_E;

typedef struct f_usb_info_struct
{
    file_cdc_t * f_usb;
    uint32_t cnt; /* how many times f_usb has been opened */
    CDC_SERIAL_STS_E state; /* state of cdc serial */
} f_usb_info_t;

typedef enum
{
    CDC_TX_WAIT = 0,
    CDC_RX_WAIT,
    CDC_TX_DONE,
    CDC_RX_DONE,
    CDC_TX_IDLE,
    CDC_RX_IDLE,
} CDC_XFER_STS_E;

typedef struct
{
    uint32_t tx_sts;
    uint32_t rx_sts;
    int32_t tx_num_done;
    int32_t rx_num_done;
} cdc_xfer_t;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    usb_status usb_host_cdc_acm_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);
    usb_status usb_host_cdc_data_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);

#ifdef __cplusplus
}
#endif

#endif
