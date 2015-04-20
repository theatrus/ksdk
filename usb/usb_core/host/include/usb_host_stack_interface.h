/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 Freescale Semiconductor;
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
* $FileName: usb_host_stack_interface.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#ifndef __usb_host_stack_interface_h__
#define __usb_host_stack_interface_h__

/* Prototypes */
#define  USB_SERVICE_HOST_RESUME             (0x01)
#define  USB_SERVICE_SYSTEM_ERROR            (0x02)
#define  USB_SERVICE_SPEED                   (0x03)
#define  USB_SERVICE_ATTACH                  (0x04)
#define  USB_SERVICE_DETACH                  (0x05)
#define  USB_SERVICE_STALL_PACKET            (0x06)
#define  USB_SERVICE_ENUMERATION_DONE        (0x07)
#define  USB_SERVICE_1MS_TIMER               (0x08)

/*------------------------------**
** Class / SubClass / Protocol  **
**------------------------------*/

#define  USB_CLASS_AUDIO            1
#define  USB_SUBCLASS_UNDEFINED     0
#define  USB_SUBCLASS_AUD_CONTROL   1
#define  USB_SUBCLASS_AUD_STREAMING 2
#define  USB_SUBCLASS_AUD_MIDI_STRM 3
#define  USB_PROTOCOL_UNDEFINED     0

#define  USB_CLASS_COMMUNICATION    2
#define  USB_SUBCLASS_COM_DIRECT    1
#define  USB_SUBCLASS_COM_ABSTRACT  2
#define  USB_SUBCLASS_COM_TELEPHONE 3
#define  USB_SUBCLASS_COM_MULTICHAN 4
#define  USB_SUBCLASS_COM_CAPI      5
#define  USB_SUBCLASS_COM_ETHERNET  6
#define  USB_SUBCLASS_COM_ATM_NET   7
#define  USB_PROTOCOL_COM_NOSPEC    0
#define  USB_PROTOCOL_COM_V25       1
#define  USB_PROTOCOL_COM_HAYES     1
#define  USB_PROTOCOL_COM_AT        1
#define  USB_PROTOCOL_COM_VENDOR    0xFF

#define  USB_CLASS_PRINTER          7
#define  USB_SUBCLASS_PRINTER       1
#define  USB_PROTOCOL_PRT_UNIDIR   1
#define  USB_PROTOCOL_PRT_BIDIR     2
#define  USB_PROTOCOL_PRT_1284      3

#define  USB_CLASS_MASS_STORAGE     8
#define  USB_SUBCLASS_MASS_RBC      1
#define  USB_SUBCLASS_MASS_ATAPI    2
#define  USB_SUBCLASS_MASS_QIC157   3
#define  USB_SUBCLASS_MASS_UFI      4
#define  USB_SUBCLASS_MASS_SFF8070I 5
#define  USB_SUBCLASS_MASS_SCSI     6
#define  USB_PROTOCOL_MASS_IRRPT    0
#define  USB_PROTOCOL_MASS_NOIRRPT  1
#define  USB_PROTOCOL_MASS_BULK     0x50

#define  USB_CLASS_HID              3
#define  USB_SUBCLASS_HID_NONE      0
#define  USB_SUBCLASS_HID_BOOT      1
#define  USB_PROTOCOL_HID_NONE      0
#define  USB_PROTOCOL_HID_KEYBOARD  1
#define  USB_PROTOCOL_HID_MOUSE     2
#define  USB_PROTOCOL_HID_BOOT_E    0
#define  USB_PROTOCOL_HID_REPORT_E  1

#define  USB_CLASS_HUB              9
#define  USB_SUBCLASS_HUB_NONE      0
#define  USB_PROTOCOL_HUB_LS        0
#define  USB_PROTOCOL_HUB_FS        0
#define  USB_PROTOCOL_HUB_HS_SINGLE 1
#define  USB_PROTOCOL_HUB_HS_MULTI  2
#define  USB_PROTOCOL_HUB_ALL       0xFF

#define  USB_CLASS_DATA             0x0A
   /* No data subclasses, set to 0 */
#define  USB_PROTOCOL_DATA_I430     0x30
#define  USB_PROTOCOL_DATA_HDLC     0x31
#define  USB_PROTOCOL_DATA_TRANS    0x32
#define  USB_PROTOCOL_DATA_Q921M    0x50
#define  USB_PROTOCOL_DATA_Q921     0x51
#define  USB_PROTOCOL_DATA_Q921TM   0x52
#define  USB_PROTOCOL_DATA_V42BIS   0x90
#define  USB_PROTOCOL_DATA_EUROISDN 0x91
#define  USB_PROTOCOL_DATA_V120     0x92
#define  USB_PROTOCOL_DATA_CAPI20   0x93
#define  USB_PROTOCOL_DATA_HOST     0xFE
#define  USB_PROTOCOL_DATA_CDC      0xFE
#define  USB_PROTOCOL_DATA_VENDOR   0xFF

typedef void * class_handle;

#define  REQ_TYPE_DEVICE    0x00
#define  REQ_TYPE_INTERFACE 0x01
#define  REQ_TYPE_ENDPOINT  0x02

/* Standard USB requests, see Chapter 9 */
#define  REQ_GET_STATUS          0
#define  REQ_CLEAR_FEATURE       1
#define  REQ_SET_FEATURE         3
#define  REQ_SET_ADDRESS         5
#define  REQ_GET_DESCRIPTOR      6
#define  REQ_SET_DESCRIPTOR      7
#define  REQ_GET_CONFIGURATION   8
#define  REQ_SET_CONFIGURATION   9
#define  REQ_GET_INTERFACE       10
#define  REQ_SET_INTERFACE       11
#define  REQ_SYNCH_FRAME         12

/* Combine one of the 3 above with one of the following 2 */
#define  REQ_TYPE_IN        0x80
#define  REQ_TYPE_OUT       0x00

/* Also for class requests set the following bit */
#define  REQ_TYPE_CLASS     0x20

/* Define function type for class device instance initialization */
typedef usb_status (_CODE_PTR_ intf_init_call) (usb_device_instance_handle, usb_interface_descriptor_handle, class_handle*);
/* Define function type for class device instance deinitialization, internal */
typedef usb_status (_CODE_PTR_ intf_deinit_call) (class_handle);
/* Define function type for class device instance deinitialization, internal */
typedef usb_status (_CODE_PTR_ intf_pre_deinit_call) (class_handle);

/* The following define the internal mapping from interface
** class / sub-class / protocol to class drivers 
*/



/*-------------------------------------------------------------**
** Transaction callback parameters:                            **
**    void*     pipe handle                                  **
**    void*     parameter specified by higher level          **
**    uint8_t *   pointer to buffer containing data (TX or RX) **
**    uint32_t     length of data transferred                   **
**    usb_status  status, preferably USB_OK or USB_DONE etc.   **
**-------------------------------------------------------------*/
typedef void  (_CODE_PTR_ tr_callback)(void*, void *, uint8_t *, uint32_t, usb_status);

/*-------------------------------------------------------------------**
** Isochronous Transaction callback parameters:                      **
**    PIPE_DESCRIPTOR_STRUCT_PTR pipe handle                         **
**    tr_struct_t*              transaction handle                  **
**    usb_status  status, preferably USB_OK or USB_DONE etc.         **
**-------------------------------------------------------------------*/
typedef void (_CODE_PTR_ iso_tr_callback)(struct pipe_struct *, struct tr_struct *, usb_status);

/*---------------------------------------------------------**
** Attach/Detach callback parameters:                      **
**    usb_device_instance_handle      new/removed device  **
**    usb_interface_descriptor_handle matching class etc. **
**   uint32_t                           code (attach etc.). **
**---------------------------------------------------------*/
typedef void (_CODE_PTR_ event_callback)(usb_device_instance_handle, usb_interface_descriptor_handle, uint32_t);

/*!
 * @brief USB setup packet structure.
 *
 * All USB devices respond to requests from the host on the device's Default Control Pipe.
 * These requests are made using control transfers. The request and the request's
 * parameters are sent to the device in the Setup packet. The host is responsible
 * for establishing the values passed in the fields listed in the struct.
 *
 */
typedef struct
{
   uint8_t                      bmrequesttype;    /*!< Characteristics of request*/
   uint8_t                      brequest;         /*!< Specific request*/
   uint8_t                      wvalue[2];        /*!< Word-sized field that varies according to request*/
   uint8_t                      windex[2];        /*!< Word-sized field that varies according to request; typically used to pass an index or offset.*/
   uint8_t                      wlength[2];       /*!< Number of bytes to transfer if there is a Data stage.*/
} usb_setup_t;


/*!
 * @brief USB transaction information structure.
 *
 * TR struct represents a transfer. A TR will be allocated, filled and send to HCI when
 * apps, class drivers or controller start a transfer. And HCI decomposes the TR into
 * transactions and then does the actual transmission.
 *
 */
typedef struct tr_struct
{
   struct tr_struct*            next;                /*!< A pointer to save next TR address*/
   uint32_t                     status;              /*!< Save TR status*/
   uint32_t                     tr_index;            /*!< Transfer number on this pipe   */
   uint8_t *                    tx_buffer;           /*!< Address of buffer containing the data to be transmitted (including OUT data phase of control transfers) */
   uint8_t *                    rx_buffer;           /*!< Address of buffer to receive data */
   uint32_t                     tx_length;           /*!< Length of data to transmit. For control transfers, the length of data for the data phase */
   uint32_t                     rx_length;           /*!< Length of data to be received. For control transfers, this indicates the length of data for the data phase */
   usb_setup_t                  setup_packet;        /*!< Setup packet raw data */
   uint32_t                     transfered_length;   /*!< The data length has been transferred*/
   tr_callback                  callback;            /*!< The callback function to be invoked when a transfer is completed or an error is to be reported */
   void*                        callback_param;      /*!< The second parameter to be passed into the callback function when it is invoked */
   void*                        hw_transaction_head; /*!< used only for EHCI */
   void*                        hw_transaction_tail; /*!< used only for EHCI */
   bool                         send_phase;          /*!< Second phase of setup packet: Send/Receive */
   uint8_t                      occupied;            /*!< Is used or not.*/
   uint8_t                      setup_status;        /*!< Setup transfer status*/
   uint8_t                      no_of_itds_sitds;    /*!< The numbers of itds or sitds for the TR.*/
   uint8_t                      setup_first_phase;   /*!< Is the setup packet is sent.*/
} tr_struct_t;

/*!
 * @brief USB pipe initialization parameter structure.
 *
 * Pipe init struct is used to set pipe params when calling usb_host_open_pipe(). 
 *
 */
typedef struct pipe_init_struct
{
   void*                        dev_instance;     /*!< The device instance of this pipe.*/
   uint32_t                     flags;            /*!< Pipe flags*/
   uint16_t                     max_packet_size;  /*!< Max pipe's packet size*/
   uint16_t                     nak_count;        /*!< Max NAK retry count. MUST be zero for interrupt*/
   uint8_t                      interval;         /*!< Interval for polling pipe for data transfer.*/
   uint8_t                      endpoint_number;  /*!< The device's ep number of this pipe.*/
   uint8_t                      direction;        /*!< The pipe direction. 0-USB_RECV/1-USB_SEND*/
   uint8_t                      pipetype;         /*!< The transfer type of this pipe. USB_CONTROL_PIPE-(0x00)/USB_ISOCHRONOUS_PIPE-(0x01)/USB_BULK_PIPE-(0x02)/USB_INTERRUPT_PIPE-(0x03)*/
} pipe_init_struct_t;

/*!
 * @brief USB class specific information structure.
 *
 * Define the class init/deinit function and the corresponding class code. 
 *
 */
typedef struct class_map
{
   intf_init_call               class_init;       /*!< class driver initialization- entry  of the class driver */
   intf_deinit_call             class_deinit;     /*!< class driver de-initialization*/
   intf_pre_deinit_call         class_pre_deinit; /*!< class driver pre-initialization*/
   uint8_t                      class_code;       /*!< interface descriptor class, */
   uint8_t                      sub_class_code;   /*!< sub-class*/
   uint8_t                      protocol_code;    /*!< protocol*/
   uint8_t                      class_code_mask;  /*!< masks ANDed with class etc. */
   uint8_t                      sub_code_mask;    /*!< mask = 0x00 matches any   */
   uint8_t                      prot_code_mask;   /*!< mask = 0xFF must be exact */
} class_map_t;

/*!
 * @brief USB class driver information structure.
 *
 * Give information for one class or device drive. 
 *
 */
typedef struct driver_info
{
   uint8_t                      idVendor[2];      /*!< Vendor ID per USB-IF */
   uint8_t                      idProduct[2];     /*!< Product ID per manufacturer */
   uint8_t                      bDeviceClass;     /*!< Class code, 0xFF if any */
   uint8_t                      bDeviceSubClass;  /*!< Sub-Class code, 0xFF if any */
   uint8_t                      bDeviceProtocol;  /*!< Protocol, 0xFF if any */
   uint8_t                      reserved;         /*!< Alignment padding */
   event_callback               attach_call;      /*!< Attach callback function*/
} usb_host_driver_info_t;

/*!
 * @brief USB device endpoint structure.
 *
 * Strucutre Representing Endpoints and number of endpoints user want.
 *
 */
typedef struct _usb_device_ep_struct
{
    endpoint_descriptor_t*     lpEndpointDesc;    /*!< Pointer to endpoint descriptor*/
    uint16_t                   endpointExlength;  /*!< Extended endpoint length*/
    uint8_t*                   endpointEx;        /*!< Extended endpoint*/
} usb_device_ep_struct_t;

/*!
 * @brief USB device interface structure.
 *
 * Strucutre Representing interface user want.
 *
 */
typedef struct _usb_device_interface_struct
{
    interface_descriptor_t*    lpinterfaceDesc;    /*!< Pointer to interface descriptor*/
    uint16_t                   interfaceExlength;  /*!< Extended interface length*/
    uint8_t *                  interfaceEx;        /*!< Extended interface*/
    uint8_t                    ep_count;           /*!< Number of endpoint in one interface*/
    usb_device_ep_struct_t     ep[USBCFG_HOST_MAX_EP_PER_INTERFACE];    /*!< Array of endpoint structure belongs to the interface*/
} usb_device_interface_struct_t;



#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initialize usb host.
 *
 * The function calls an HCI function to initialize the USB Host hardware and install an
 * ISR that services all interrupt sources on the USB Host hardware.
 * The function also allocates and initializes all internal host-specific data structures
 * and USB Host internal data and returns a USB Host controller handle for subsequent
 * use with other USB Host API functions.
 *
 * @param controller_id controller ID
 * KHCI 0 --- 0
 * KHCI 1 --- 1
 * EHCI 0 --- 2
 * EHCI 1 --- 3
 * @param handle host handle
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_init (uint8_t, usb_host_handle *);

/*!
 * @brief Un-Initialize usb host.
 *
 * The function calls an HCI function to stop the specified USB Host controller. Call
 * the function when the services of the USB Host controller are no longer required
 * or if the USB Host controller needs to be re-configured.
 *
 * @param handle host handle
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_deinit(usb_host_handle);

/*!
 * @brief Open usb host pipe.
 *
 * This function is used by the application to open a pipe. The pipe detailed information
 * is included in the pipe_init_ptr, and it points to PIPE_INIT_STRUCT.
 *
 * @param handle USB host handle
 * @param pipe_handle_ptr returned pipe handle
 * @param pipe_init_ptr parameter to initialize the pipe,
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_open_pipe(usb_host_handle, usb_pipe_handle *, pipe_init_struct_t*);

/*!
 * @brief Close usb host pipe.
 *
 * This function is used by the application to close an opened pipe so that the pipe resource can be free.
 *
 * @param handle USB host handle
 * @param pipe_handle_ptr returned pipe handle
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_close_pipe(usb_host_handle, usb_pipe_handle);

/*!
 * @brief Send data through target pipe.
 *
 * This function is used by the application to send data through target pipe that is
 * assigned by the pipe_handle parameter. The detailed data about the address, the
 * length, the transfer type is assigned in the TR structure.
 *
 * @param handle USB host handle
 * @param pipe_handle pipe handle
 * @param tr_ptr pointer to TR
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_send_data(usb_host_handle, usb_pipe_handle, tr_struct_t*);

/*!
 * @brief Send a setup through target pipe.
 *
 * This function is used by the application to send a setup through target pipe, which
 *  always the control pipe 0. 
 *
 * @param handle USB host handle
 * @param pipe_handle pipe handle
 * @param tr_ptr pointer to TR
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_send_setup(usb_host_handle, usb_pipe_handle, tr_struct_t*);

/*!
 * @brief Receive data through the target pipe.
 *
 * This function is used by the application to receive data through the target pipe that is
 * assigned by the pipe_handle parameter. The detailed data about the address, the
 * length, the transfer type is assigned in the TR structure.
 *
 * @param handle USB host handle
 * @param pipe_handle pipe handle
 * @param tr_ptr pointer to TR
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_recv_data(usb_host_handle, usb_pipe_handle, tr_struct_t*);

/*!
 * @brief Control the BUS status.
 *
 * This function is used by the application to control the BUS status, for example, to
 * suspend the BUS or resume the BUS. Currently this function is not implemented yet.
 *
 * @param handle USB host handle
 * @param bcontrol control code of the BUS
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_bus_control(usb_host_handle, uint8_t);

/*!
 * @brief Open the selected interface.
 *
 * This function is used by the application to open the selected interface, and the
 * corresponding class driver handle will be obtained through the class_handle_ptr
 * parameter that can be used for the following transfer.
 *
 * @param handle USB host handle
 * @param dev_handle attached device handle
 * @param intf_handle interface handle to be opened
 * @param class_handle_ptr class handle associated to the interface
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_open_dev_interface(usb_host_handle, usb_device_instance_handle, usb_interface_descriptor_handle, class_handle*);

/*!
 * @brief Close the selected interface.
 *
 * This function is used by the application to close the selected interface.
 *
 * @param handle USB host handle
 * @param dev_handle attached device handle
 * @param intf_handle interface handle to be closed
 * @param class_handle class handle associated to the interface
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_close_dev_interface(usb_host_handle, usb_device_instance_handle, usb_interface_descriptor_handle, class_handle);

/*!
 * @brief Register services for types of events .
 *
 * This function registers a callback routine for a specified event.
 *
 * @param handle Handle to the USB device
 * @param type type of event or endpoint number to service
 * @param service Pointer to the service's callback function
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_register_service (usb_host_handle, uint8_t, void(_CODE_PTR_ service)(void*, uint32_t));

/*!
 * @brief Un-Register services for types of events .
 *
 * This function un-registers a callback routine for a specified event.
 *
 * @param handle Handle to the USB device
 * @param type type of event or endpoint number to service
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_unregister_service (usb_host_handle, uint8_t);

/*!
 * @brief Registers a callback function for ch9.
 *
 * This function registers a callback function that will be called 
 *  to notify the user of a ch9 command completion. This should be used 
 *  only after enumeration is completed.
 *
 * @param handle Handle to the USB device
 * @param dev_handle usb device.
 * @param callback Callback upon completion .
 * @param callback_param User provided callback param.
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_register_ch9_callback(usb_host_handle, usb_device_instance_handle, tr_callback, void*);

/*!
 * @brief Un-Registers a callback function for ch9.
 *
 * This function un-registers a callback function that will be called 
 *  to notify the user of a ch9 command completion. This should be used 
 *  only after enumeration is completed.
 *
 * @param handle Handle to the USB device
 * @param dev_handle usb device.
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_unregister_ch9_callback(usb_host_handle, usb_device_instance_handle);

/*!
 * @brief Register a driver for a device.
 *
 * This function is used by the application to register a driver for a device with a
 * particular vendor ID, product ID, class, subclass, and protocol code.
 *
 * @param handle USB host
 * @param info_table_ptr Device info table
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_register_driver_info(usb_host_handle, void*);

/*!
 * @brief Register a callback function to get all the information of unsupported device.
 *
 * This function is used by the application to register a callback function to get all the
 * information of unsupported device.
 *
 * @param handle USB host
 * @param unsupported_device_notify callback function to get the unsupported device notification
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_register_unsupported_device_notify(usb_host_handle, event_callback);

/*!
 * @brief Get a valid TR.
 *
 * This function is used by the application to get a valid TR that will be used in the
 * following transfer. Meanwhile, the TR callback and callback parameter are provided
 * to be initialized.
 *
 * @param handle USB host handle
 * @param callback callback function that will be called when TR completed
 * @param callback_param callback parameter to the callback function
 * @param tr_ptr_ptr return the pointer to TR
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_get_tr(usb_host_handle, tr_callback, void*, tr_struct_t* *);

/*!
 * @brief Release a TR.
 *
 * This function is used by the application to release a TR so that the TR resource can
 * be free in the USB stack.
 *
 * @param handle USB host handle
 * @param tr_ptr pointer to TR to be released.
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_release_tr(usb_host_handle, tr_struct_t*);

/*!
 * @brief Cancel all the uncompleted TRs.
 *
 * This function is used by the application to cancel all the uncompleted TRs in a target
 * pipe. 
 * Note: There is no API provided by the stack to cancel a specific TR in a target pipe.
 * The tr_ptr parameter is not used in this API now, but we can extend this API in the
 * future to cancel a specific TR, so we keep tr_ptr parameter here.
 *
 * @param handle USB host handle
 * @param pipe_handle pipe handle
 * @param tr_ptr pointer to TR
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_cancel(usb_host_handle, usb_pipe_handle, tr_struct_t*);

/*!
 * @brief Remove device when error happened.
 *
 * This function is used to remove device when error happened.
 *
 * @param handle USB host handle
 * @param dev_handle USB device
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_dev_remove(usb_host_handle, usb_device_instance_handle);

/*!
 * @brief Get the host handle of the device.
 *
 * This function is used to get the host handle of the device.
 *
 * @param dev_handle usb device.
 * @param handle host handle.
 * @return USB_OK-Success/Others-Fail
 */
usb_status usb_host_get_host_handle(usb_device_instance_handle, usb_host_handle *);

#ifdef __cplusplus
}
#endif

#endif
