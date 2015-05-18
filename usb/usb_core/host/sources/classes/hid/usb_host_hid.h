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
 * $FileName: usb_host_hid.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file defines a template structure for Class Drivers.
 *
 *END************************************************************************/
#ifndef __usb_host_hid_h__
#define __usb_host_hid_h__

#define LONG_ITEM                      0xFE
#define SHORT_ITEM                     0x03

#define MAIN_INPUT                     0x80
#define MAIN_OUTPUT                    0x90
#define MAIN_FEATURE                   0xB0
#define MAIN_COLLECTION                0xA0
#define MAIN_END_COLLECTION            0xC0

#define GLOBAL_USAGE_PAGE              0x04
#define GLOBAL_LOGICAL_MIN             0x14
#define GLOBAL_LOGICAL_MAX             0x24
#define GLOBAL_PHYSICAL_MIN            0x34
#define GLOBAL_PHYSICAL_MAX            0x44
#define GLOBAL_UNIT_COMPONENT          0x54
#define GLOBAL_UNIT                    0x64
#define GLOBAL_REPORT_SIZE             0x74
#define GLOBAL_REPORT_ID               0x84
#define GLOBAL_REPORT_COUNT            0x94
#define GLOBAL_PUSH                    0xA4
#define GLOBAL_POP                     0xB4

#define LOCAL_USAGE                    0x08
#define LOCAL_USAGE_MINIMUM            0x18
#define LOCAL_USAGE_MAXIMUM            0x28
#define LOCAL_DESIGNATOR_INDEX         0x38
#define LOCAL_DESIGNATOR_MIN           0x48
#define LOCAL_DESIGNATOR_MAX           0x58
#define LOCAL_STRING_INDEX             0x78
#define LOCAL_STRING_MIN               0x88
#define LOCAL_STRING_MAX               0x98
#define LOCAL_SET_DELIMITER            0xA8

#define USAGE_PAGE_UNDEFINED           0x00
#define USAGE_PAGE_GENERIC_DESKTOP     0x01
#define USAGE_PAGE_KEYBOARD            0x07
#define USAGE_PAGE_LEDS                0x08
#define USAGE_PAGE_BUTTONS             0x09

#define DESKTOP_USAGE_POINTER          0x01
#define DESKTOP_USAGE_MOUSE            0x02
#define DESKTOP_USAGE_KEYBOARD         0x06
#define DESKTOP_USAGE_X                0x30
#define DESKTOP_USAGE_Y                0x31
#define DESKTOP_USAGE_Z                0x32
#define DESKTOP_USAGE_WHEEL            0x38

#define INPUT_DATA_CONSTBIT            0x01
#define INPUT_DATA_VAR_BIT             0x02
#define INPUT_ABS_RELATIVE_BIT         0x04
#define INPUT_NO_WRAP_BIT              0x08
#define INPUT_NON_LINEAR_BIT           0x10
#define INPUT_PREF_STATE_NO_PREF_BIT   0x20
#define INPUT_NO_NULL_STATE_BIT        0x40
#define INPUT_BIT_FIELD_BUF_BYTES_BIT  0x100

#define GET_REPORT                     0x01
#define GET_IDLE                       0x02
#define GET_PROTOCOL                   0x03
#define SET_REPORT                     0x09
#define SET_IDLE                       0x0A
#define SET_PROTOCOL                   0x0B

/* structure for HID class descriptor */
typedef struct _hid_descriptor_struct
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t bcdhid[2];
    uint8_t bcountrycode;
    uint8_t bnumdescriptor;
    uint8_t bclassdescriptortype;
    uint8_t bclassdescriptorlength[2];
} hid_descriptor_struct_t;

typedef struct _hid_class_descriptor
{
    uint8_t classdescriptortype;
    uint8_t descriptorlength[2];
} hid_class_descriptor_struct_t;

/*
 ** HID Class Interface structure. This structure will be passed to
 ** all commands to this class driver.
 */

typedef struct _usb_hid_class
{
    usb_host_handle host_handle;
    usb_device_instance_handle dev_handle;
    usb_interface_descriptor_handle intf_handle;
    /* Only 1 command can be issued at one time */
    usb_pipe_handle in_pipe;
    /* Here we store callback and parameter from higher level */
    tr_callback ctrl_callback;
    void* ctrl_param;
    tr_callback recv_callback;
    void* recv_param;
    uint32_t running;
    bool in_setup;
} usb_hid_class_struct_t;

typedef struct _hid_command
{
    usb_class_handle class_ptr;
    tr_callback callback_fn;
    void* callback_param;
} hid_command_t;

/* Class specific functions exported by HID class driver */
#ifdef __cplusplus
extern "C"
{
#endif

extern usb_status usb_class_hid_init(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, usb_class_handle* class_handle_ptr);
extern usb_status usb_class_hid_deinit(usb_class_handle handle);
extern usb_status usb_class_hid_pre_deinit(usb_class_handle handle);
extern usb_status usb_class_hid_get_idle(hid_command_t* com_ptr, uint8_t rid, uint8_t* idle_rate);
extern usb_status usb_class_hid_get_protocol(hid_command_t* com_ptr, uint8_t* protocol);
extern usb_status usb_class_hid_set_protocol(hid_command_t* com_ptr, uint8_t protocol);
extern usb_status usb_class_hid_get_report(hid_command_t* com_ptr, uint8_t rid, uint8_t rtype, void* buf, uint16_t blen);
extern usb_status usb_class_hid_set_idle(hid_command_t* com_ptr, uint8_t rid, uint8_t duration);
extern usb_status usb_class_hid_set_report(hid_command_t* com_ptr, uint8_t rid, uint8_t rtype, void* buf, uint16_t blen);
extern usb_status usb_class_hid_recv_data(hid_command_t* com_ptr, uint8_t* buffer, uint16_t length);
extern usb_status usb_class_hid_get_descriptor(hid_command_t* com_ptr, uint8_t type, uint8_t* buf, uint16_t len);
#ifdef __cplusplus
}
#endif

#endif
