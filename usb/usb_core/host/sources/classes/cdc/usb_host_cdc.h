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
* $FileName: usb_host_cdc.h$
* $Version :
* $Date    :
*
* Comments:
*
*   This file defines a template structure for Class Drivers.
*
*END************************************************************************/
#ifndef __usb_host_cdc_h__
#define __usb_host_cdc_h__

#ifdef __cplusplus
extern "C" {
#endif

#define USB_CDC_SET_LINE_CODING              0x20
#define USB_CDC_GET_LINE_CODING              0x21

/*
** FILE STRUCTURE
**
** This structure defines the information kept in order to implement
** ANSI 'C' standard I/O stream.
*/
typedef struct file_struct
{

    /* The address of the Device for this stream */
    struct cdc_io_device_struct * dev_ptr;

    /* Device Driver specific information */
    void *       dev_data_ptr;

    /* General control flags for this stream */
    uint32_t     flags;

    /* The current error for this stream */
    uint32_t     error;

    /* The current position in the stream */
    uint32_t     location;

    /* The current size of the file */
    uint32_t     size;

    /* The following 2 implement undelete */
    uint32_t     ungot_character;
    bool         have_ungot_character;
} file_cdc_t;

typedef struct cdc_queue_element_struct
{

    /* next element in queue, MUST BE FIRST FIELD */
    struct queue_element_struct * next;

    /* previous element in queue, MUST BE SECOND FIELD */
    struct queue_element_struct * prev;

} cdc_queue_element_struct_t;

/*
**
** IO DEVICE STRUCT
**
** This is the structure used to store device information for an
** installed I/O driver
*/
typedef struct cdc_io_device_struct
{

    /* Used to link io_device_structs together */
    cdc_queue_element_struct_t queue_element;

    /*
    ** A string that identifies the device.  This string is matched
    ** by fopen, then the other information is used to initialize a
    ** FILE struct for standard I/O.  This string is also provided in
    ** the kernel initialization record for the default I/O channel
    */
    char *             identifier;

    /* The I/O init function */
    int32_t (_CODE_PTR_ io_open)(file_cdc_t * p1, char * p2, const char * p3);

    /* The I/O deinit function */
    int32_t (_CODE_PTR_ io_close)(file_cdc_t * p1);

    /* The I/O read function */
    int32_t (_CODE_PTR_ io_read)(file_cdc_t * p1, char * p2, int32_t p3);

    /* The I/O write function */
    int32_t (_CODE_PTR_ io_write)(file_cdc_t * p1, char * p2, int32_t p3);

    /* The I/O ioctl function */
    int32_t (_CODE_PTR_ io_ioctl)(file_cdc_t * p1, uint32_t p2, void * p3);

    /* The function to call when uninstalling this device */
    int32_t (_CODE_PTR_ io_uninstall)(struct cdc_io_device_struct * p1);

    /* The I/O channel specific initialization data */
    void *              driver_init_ptr;

    /* Type of driver for this device */
    uint32_t           driver_type;

} cdc_io_device_struct_t;
/* The following struct is for storing a class's validity-check
** code with the pointer to the data.  The address of one such
** struct is passed as a pointer to select-interface calls,
** where values for that interface get initialized.  Then the
** struct should be passed to class calls using the interface. */
typedef struct class_call_struct
{
    usb_class_intf_handle         class_intf_handle;
    uint32_t                       code_key;
    void *                         next;   /* Used by class driver or app, usually pointer to next CDC_CLASS_CALL_STRUCT */
    void *                         anchor; /* Used by class driver or app, usually pointer to first CDC_CLASS_CALL_STRUCT */
}  cdc_class_call_struct_t;

typedef struct
{
    uint32_t      baudrate;
    uint8_t       stopbits; /* 1 ~ 1bit, 2 ~ 2bits, 3 ~ 1.5bit */
    uint8_t       parity;   /* 1 ~ even, -1 ~ odd, 0 ~ no parity */
    uint8_t       databits;
} usb_cdc_uart_coding_t;

/* set output pin state */
#define USB_CDC_SET_CTRL_LINE_STATE          0x22
typedef struct
{
#define USB_ACM_LINE_STATE_DTR               0x01u
#define USB_ACM_LINE_STATE_RTS               0x02u
    uint16_t      state;
} usb_cdc_ctrl_state_t;

/* receive interrupt state */
typedef struct
{
    uint8_t       reservedA[8];
#define USB_ACM_STATE_RX_CARRIER             0x01
#define USB_ACM_STATE_TX_CARRIER             0x02
#define USB_ACM_STATE_BREAK                  0x04
#define USB_ACM_STATE_RING_SIGNAL            0x08
#define USB_ACM_STATE_FRAMING                0x10
#define USB_ACM_STATE_PARITY                 0x20
#define USB_ACM_STATE_OVERRUN                0x40
    uint8_t       bmStates;
    uint8_t         reservedB[1];
    uint8_t         reservedC[2]; /* To be removed, fix 4B align issue */
} usb_cdc_acm_state_t;

#define USB_DESC_SUBTYPE_CS_HEADER           0x00
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bcdCDC[2];
} usb_cdc_desc_header_t;

#define USB_DESC_SUBTYPE_CS_CM               0x01
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
#define USB_ACM_CM_CAP_HANDLE_MANAGEMENT     0x01
#define USB_ACM_CM_CAP_DATA_CLASS            0x02
    uint8_t   bmCapabilities;
    uint8_t   bDataInterface;
} usb_cdc_desc_cm_t;

#define USB_DESC_SUBTYPE_CS_ACM              0x02
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
#define USB_ACM_CAP_COMM_FEATURE             0x01
#define USB_ACM_CAP_LINE_CODING              0x02
#define USB_ACM_CAP_SEND_BREAK               0x04
#define USB_ACM_CAP_NET_NOTIFY               0x08
    uint8_t   bmCapabilities;
} usb_cdc_desc_acm_t;

#define USB_DESC_SUBTYPE_CS_DL               0x03
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities;
} usb_cdc_desc_dl_t;

#define USB_DESC_SUBTYPE_CS_TR               0x04
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bRingerVolSteps;
    uint8_t   bNumRingerPatterns;
} usb_cdc_desc_tr_t;

#define USB_DESC_SUBTYPE_CS_TCS              0x05
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities[4];
} usb_cdc_desc_tcs_t;

#define USB_DESC_SUBTYPE_CS_UNION            0x06
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bMasterInterface;
} usb_cdc_desc_union_t;

#define USB_DESC_SUBTYPE_CS_TOM              0x08
typedef struct
{
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities;
} usb_cdc_desc_tom_t;

typedef union
{
    usb_cdc_desc_header_t         header;
    usb_cdc_desc_cm_t             cm;
    usb_cdc_desc_acm_t            acm;
    usb_cdc_desc_dl_t             dl;
    usb_cdc_desc_tr_t             tr;
    usb_cdc_desc_tcs_t            tcs;
    usb_cdc_desc_tom_t            tom;
    usb_cdc_desc_union_t          uni;
} usb_cdc_func_desc_t;

/*
** CDC Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct
{
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    /*USB_CDC_GENERAL_CLASS                     CDC_G;*/
    usb_cdc_desc_acm_t *                      acm_desc;
    usb_cdc_desc_cm_t *                       cm_desc;
    usb_cdc_desc_header_t *                   header_desc;
    usb_cdc_desc_union_t *                    union_desc;

    usb_cdc_uart_coding_t                       uart_coding;

    usb_pipe_handle                          interrupt_pipe;
    usb_cdc_acm_state_t                         interrupt_buffer;

    usb_cdc_ctrl_state_t                        ctrl_state;

#define USB_ACM_DETACH           0x01
#define USB_ACM_CTRL_PIPE_FREE   0x02
#define USB_ACM_INT_PIPE_FREE    0x04
    os_event_handle                           acm_event;
    usb_host_handle                          host_handle;
    usb_device_instance_handle               dev_handle;
    usb_interface_descriptor_handle          intf_handle;
    uint8_t                                   intf_num;
    os_mutex_handle                           mutex;
} usb_acm_class_intf_struct_t;
typedef void (*_usb_cdc_callback)(void * param);
typedef struct
{
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    /*USB_CDC_GENERAL_CLASS                     CDC_G;*/
    /* Address of bound control interface */
    cdc_class_call_struct_t *                     BOUND_CONTROL_INTERFACE;

    uint8_t *                                  rx_buffer;
    uint8_t *                                  RX_BUFFER_DRV;
    uint8_t *                                  RX_BUFFER_APP;
    uint32_t                                   RX_BUFFER_SIZE;
    uint32_t                                   RX_READ;

    uint32_t                                   TX_SENT;

    usb_pipe_handle                           in_pipe;
    usb_pipe_handle                           out_pipe;

    char *                                  device_name;

#define USB_DATA_DETACH            0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
    os_event_handle                        data_event;
    usb_host_handle                          host_handle;
    usb_device_instance_handle               dev_handle;
    usb_interface_descriptor_handle          intf_handle;
    uint8_t                                   intf_num;
    os_mutex_handle                           mutex;
    tr_callback                               ctrl_callback;
    void *                                    ctrl_callback_param;
    _usb_cdc_callback                         data_tx_cb;
    _usb_cdc_callback                         data_rx_cb;
    bool                                      is_rx_xferring;
    bool                                      is_tx_xferring;
} usb_data_class_intf_struct_t, * USB_DATA_CLASS_INTF_STRUCT_PTR;

typedef struct
{
    cdc_class_call_struct_t *   CLASS_PTR;
    tr_callback             CALLBACK_FN;
    void *                 CALLBACK_PARAM;
} cdc_command_t;

typedef struct cdc_serial_init
{
#define USB_UART_BLOCKING          (0x0000)
#define USB_UART_NO_BLOCKING       (0x0001)
    /* NOTE!!!
    ** HW control flow has one weakness: we don't know exactly when DCD signal got log. 0 or
    ** log.1. This is because interrupts pipe is polled in time intervals in some milliseconds.
    ** HW flow control is here only just for compatibility purposes.
    ** As for output pins, we set always DTR and RTS signal for HW flow control. The user can
    ** still use usb_class_cdc_set_acm_ctrl_state to control those signals manually.
    ** Note also, that additional HW control flow is done on USB,
    ** which implies from USB specification (NAK responding).
    */
#define USB_UART_HW_FLOW           (0x0002)
    uint32_t               flags;
} cdc_serial_init_t;

/* Mutex handling */
#define USB_CDC_ACM_lock()                OS_Mutex_lock(((usb_acm_class_intf_struct_t *)if_acm_ptr)->mutex)
#define USB_CDC_ACM_unlock()              OS_Mutex_unlock(((usb_acm_class_intf_struct_t *)if_acm_ptr)->mutex)
#define USB_CDC_DATA_lock()               OS_Mutex_lock(((usb_data_class_intf_struct_t *)if_data_ptr)->mutex)
#define USB_CDC_DATA_unlock()             OS_Mutex_unlock(((usb_data_class_intf_struct_t *)if_data_ptr)->mutex)
/* Class specific functions exported by CDC class driver */
extern usb_status usb_class_cdc_acm_init(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, usb_class_handle* class_handle_ptr);
extern usb_status usb_class_cdc_acm_deinit(usb_class_handle handle);
extern usb_status usb_class_cdc_acm_pre_deinit(usb_class_handle handle);
extern usb_status usb_class_cdc_acm_use_lwevent(cdc_class_call_struct_t* ccs_ptr, os_event_handle acm_event);
extern usb_status usb_class_cdc_data_init(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, usb_class_handle* class_handle_ptr);
extern usb_status usb_class_cdc_data_deinit(usb_class_handle handle);
extern usb_status usb_class_cdc_data_pre_deinit(usb_class_handle handle);
extern usb_status usb_class_cdc_data_use_lwevent(cdc_class_call_struct_t* ccs_ptr, os_event_handle data_event);

extern cdc_class_call_struct_t * usb_class_cdc_get_ctrl_interface(void* intf_handle);
extern cdc_class_call_struct_t * usb_class_cdc_get_data_interface(void* intf_handle);
extern usb_status usb_class_cdc_get_acm_line_coding(cdc_class_call_struct_t* ccs_ptr, usb_cdc_uart_coding_t* uart_coding_ptr);
extern usb_status usb_class_cdc_set_acm_line_coding(cdc_class_call_struct_t* ccs_ptr, usb_cdc_uart_coding_t* uart_coding_ptr);
extern usb_status usb_class_cdc_set_acm_ctrl_state(cdc_class_call_struct_t* ccs_ptr, uint8_t dtr, uint8_t rts);
extern usb_status usb_class_cdc_get_acm_descriptors
(
    usb_device_instance_handle      dev_handle,
    usb_interface_descriptor_handle intf_handle,
    usb_cdc_desc_acm_t * *       acm_desc,
    usb_cdc_desc_cm_t * *        cm_desc,
    usb_cdc_desc_header_t * *    header_desc,
    usb_cdc_desc_union_t * *     union_desc
);
extern usb_status usb_class_cdc_set_acm_descriptors
(
    cdc_class_call_struct_t *            ccs_ptr,
    usb_cdc_desc_acm_t *             acm_desc,
    usb_cdc_desc_cm_t *              cm_desc,
    usb_cdc_desc_header_t *          header_desc,
    usb_cdc_desc_union_t *           union_desc
);
extern usb_status usb_class_cdc_get_ctrl_descriptor
(
    usb_device_instance_handle      dev_handle,
    usb_interface_descriptor_handle intf_handle,
    interface_descriptor_t* *   if_desc_ptr
);
extern usb_status usb_class_cdc_bind_data_interfaces(usb_device_instance_handle dev_handle, cdc_class_call_struct_t* ccs_ptr);
extern usb_status usb_class_cdc_unbind_data_interfaces(cdc_class_call_struct_t* ccs_ptr);
extern usb_status usb_class_cdc_bind_acm_interface(cdc_class_call_struct_t* ccs_ptr, interface_descriptor_t* if_desc);
extern usb_status usb_class_cdc_unbind_acm_interface(cdc_class_call_struct_t* ccs_ptr);
extern usb_status usb_class_cdc_init_ipipe(cdc_class_call_struct_t* acm_instance);
extern uint32_t usb_class_cdc_intf_validate(void * param);
#ifdef __cplusplus
}
#endif

#endif
