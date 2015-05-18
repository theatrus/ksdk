/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: usb_host_phdc.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains definitions for  USB PHDC class.
 *   Reference Specification:
 *   "Universal Serial Bus Device Class Definition for Personal Healthcare Devices" version 1.0
 *   November 2008, from USB Implementers Forum.
 *
 *END************************************************************************/
#ifndef __USB_HOST_PHDC_H__
#define __USB_HOST_PHDC_H__

#include "usb_host_stack_interface.h"

/* Constant Definitions*********************************************************/
#define METADATA_PREAMBLE_SIGNATURE_SIZE    16

/* Descriptor related */

/* Standard descriptors */
#define USB_DEVICE_DESCRIPTOR               0x01
#define USB_CONFIG_DESCRIPTOR               0x02
#define USB_STRING_DESCRIPTOR               0x03
#define USB_IFACE_DESCRIPTOR                0x04
#define USB_ENDPOINT_DESCRIPTOR             0x05

/* PHDC descriptors */
#define USB_PHDC_CLASSFUNCTION_DESCRIPTOR   0x20
#define USB_PHDC_QOS_DESCRIPTOR             0x21
#define PHDC_11073PHD_FUNCTION_DESCRIPTOR   0x30
#define PHDC_METADATA_DESCRIPTOR            0x22

#define USB_CLASS_PHDC                      0x0F

/* Control Requests Types */
#define PHDC_GET_STATUS_BMREQTYPE           0xA1
#define PHDC_GET_STATUS_BREQ                0x00
#define PHDC_SET_FEATURE_BMREQTYPE          0x21
#define PHDC_SET_FEATURE_BREQ               0x03
#define PHDC_CLEAR_FEATURE_BMREQTYPE        0x21
#define PHDC_CLEAR_FEATURE_BREQ             0x01

/* Metadata feature */
#define PHDC_FEATURE_METADATA               0x01  
/* QoS information encoding feature */
#define PHDC_QoS_ENCODING_VERSION           0x01 
/* PHDC specific status codes for the application callbacks (passed through the callback_param) */
#define USB_PHDC_RX_OK                      0x00   /* Rx OK code. All went fine on this transfer */ 
#define USB_PHDC_TX_OK                      0x00   /* Tx OK code. All went fine on this transfer */
#define USB_PHDC_CTRL_OK                    0x00   /* CTRL OK code. All went fine on this transfer */
#define USB_PHDC_RX_ERR_METADATA_EXPECTED   0x01   /* Rx ERR code. Metadata was expected, instead plain data was received */
#define USB_PHDC_ERR                        0x7F   /* USB Standard error returned. Check the usb_status code */
#define USB_PHDC_ERR_ENDP_CLEAR_STALL       0xFF   /* The PHDC failed in clearing the Endpoint STALL (due to an USB STALL ERROR) */

/* Standard USB endpoint related */
#define ENDPOINT_TRANSFER_TYPE          0x03
#define ENDPOINT_DIRECTION              0x80

#define ENDPOINT_CONTROL                0x00
#define ENDPOINT_ISOCHRONOUS            0x01
#define ENDPOINT_BULK                   0x02
#define ENSPOINT_INTERRUPT              0x03

#define ENDPOINT_DIR_IN                 0x80
#define ENDPOINT_DIR_OUT                0x00

/* Type Definitions*********************************************************/

/* Endpoint descriptor structure as defined by the USB standard */
typedef struct usb_phdc_desc_endpoint_type
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t bendpointaddress;
    uint8_t bmattributes;
    uint16_t wmaxpacketsize;
    uint8_t binterval;
} usb_phdc_desc_endpoint_t;

/* PHDC class descriptor structure as defined by the Healthcare standard */
typedef struct usb_phdc_desc_class_type
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t bphdcdatacode;
    uint8_t bmcapability;
} usb_phdc_class_type_t;

/* QoS descriptor structure as defined by the PHDC class specification. */
typedef struct usb_phdc_desc_qos_type
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t bqosencodingversion;
    uint8_t bmlatencyreliability;
} usb_phdc_desc_qos_t;

/* Metadata descriptor structure as defined by the PHDC class specification. */
typedef struct usb_phdc_desc_metadata_type
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t* bopaquedata;
} usb_phdc_desc_metadata_t;

/* QoS and Metadata Linked-List element */
typedef struct usb_phdc_qos_metadata_list_type
{
    usb_phdc_desc_endpoint_t *pendpointdesc;
    usb_phdc_desc_qos_t *pqosdesc;
    usb_phdc_desc_metadata_t *pmetadatadesc;
    struct usb_phdc_qos_metadata_list_type *pnext;
} usb_phdc_desc_qos_metadata_list_t;

/* Function extension descriptor (device specialization) structure as defined by the PHDC class specification. */
typedef struct usb_phdc_desc_fcn_ext_type
{
    uint8_t blength;
    uint8_t bdescriptortype;
    uint8_t breserved;
    uint8_t bnumdevspecs;
    uint16_t* wdevspecializations;
} usb_phdc_desc_fcn_ext_t;

/* PHDC required type for parameter passing to the PHDC transfer functions (Send / Receive/ Ctrl). 
 * A pointer to this type is required when those functions are called, 
 * pointer which will be also transmitted back to the application when the corresponding callback 
 * function is called by the PHDC through the callback_param_ptr.
 */
typedef struct usb_phdc_param_type
{
    /* [APP->PHDC] phdc call struct pointer */
    usb_class_handle class_ptr;
    tr_callback callback_fn;
    void* callback_param;
    /* [PHDC->APP] USB status code. usb_status (standard) code when the transfer is finished. Not valid until the callback is called */
    usb_status status;
    /* [APP->PHDC] length of buffer (only for PHDC Send/Recv requests) */
    uint32_t buff_size;
    /* [PHDC->APP] USB transaction index. Used to identify the Send/Recv transaction */
    uint32_t tr_index;
    /* [PHDC->APP] USB transaction index. Used to identify the Send/Recv transaction */
    usb_pipe_handle tr_pipe_handle;
    /* [APP->PHDC] Boolean for metadata transfers (only for PHDC Send request) */
    bool metadata;
    /* [APP->PHDC] the type of the request (only for PHDC Ctrl requests) */
    uint8_t classRequestType;
    /* [APP->PHDC] QoS for receive transfers (only for PHDC Recv request) */
    uint8_t qos;
    /* [PHDC->APP] USB PHDC status code. USB PHDC (specific) code when the transfer is finished. Not valid until the callback is called */
    uint8_t usb_phdc_status;
    /* [APP->PHDC] data buffer (only for PHDC Send/Recv requests) */
    uint8_t* buff_ptr;
} usb_phdc_param_t;

/* Callback function pointer keeping the current transaction parameters. It contains a pointer to a USB_PHDC_PARAM struct. */
typedef void (*phdc_callback)(usb_phdc_param_t *call_param);

/*
 * PHDC Class Interface structure. This structure will be passed to
 * all commands to this class driver.
 */
typedef struct _usb_phdc_class_intf_struct_type
{
    usb_host_handle host_handle;
    usb_device_instance_handle dev_handle;
    usb_interface_descriptor_handle intf_handle;
    /* Pipes */
    usb_pipe_handle control_pipe; /* control pipe */
    usb_pipe_handle bulk_in_pipe; /* bulk IN pipe */
    usb_pipe_handle bulk_out_pipe; /* bulk OUT pipe */
    usb_pipe_handle int_in_pipe; /* interrupt IN pipe */
    /* Descriptors */
    usb_phdc_desc_qos_metadata_list_t *qos_metadata_list;
    usb_phdc_desc_fcn_ext_t *fcn_ext_desc;
    /* Callbacks */
    phdc_callback send_callback; /* Send app callback */
    phdc_callback recv_callback; /* Receive app callback */
    phdc_callback ctrl_callback; /* Send ctrl app callback */
    /* Number of transfers until next metadata on the Bulk OUT. Required on transmit */
    os_mutex_handle mutex;
    uint32_t running;
    bool preamble_capability;
    /* Status */
    /* Only one SET_FEATURE/CLEAR_FEATURE request at the time */
    bool set_clear_request_pending;
    /* Metadata feature set for the device */
    bool device_feature_set;
    uint8_t num_transf_bulk_out;
    /* Number of transfers until next metadata on the Bulk IN. Required on receive */
    uint8_t num_transf_bulk_in;
    /* Configuration */
    uint8_t phdc_data_code;
} usb_phdc_class_struct_t;

/* Internal structure keeping the Metadata preamble fields as defined by the PHDC class specification. */
typedef struct usb_phdc_metadata_preamble_type
{
    uint8_t a_signature[METADATA_PREAMBLE_SIGNATURE_SIZE];
    uint8_t b_num_transfers;
    uint8_t b_qos_encoding_version;
    uint8_t bm_latency_reliability;
    uint8_t b_opaque_data_size;
    uint8_t* b_opaque_data;
} usb_phdc_metadat_prreamble_t;

/* Function Declarations *********************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

usb_status usb_class_phdc_init
(
    /* [IN]  the device handle related to the class driver */
    usb_device_instance_handle      dev_handle,
    /* [IN]  the interface handle related to the class driver */
    usb_interface_descriptor_handle intf_handle,
    /* [OUT] printer call struct pointer */
    usb_class_handle*                    class_handle_ptr
);
usb_status usb_class_phdc_deinit(usb_class_handle handle);
usb_status usb_class_phdc_pre_deinit(usb_class_handle handle);
usb_status usb_class_phdc_set_callbacks(usb_class_handle handle, phdc_callback sendCallback, phdc_callback recvCallback, phdc_callback ctrlCallback);
usb_status usb_class_phdc_send_control_request(usb_phdc_param_t* call_param_ptr);
usb_status usb_class_phdc_recv_data(usb_phdc_param_t *call_param_ptr);
usb_status usb_class_phdc_send_data(usb_phdc_param_t *call_param_ptr);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_PHDC_H__ */
