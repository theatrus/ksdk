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
* $FileName: usb_host_ch9.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains definitions for standard device requests,
*   i.e. USB specification Chapter 9 requests.
*
*END************************************************************************/
#ifndef __host_ch9_h__
#define __host_ch9_h__

/*----------------------------------------------------------------**
** Chapter 9.4 Standard Device Requests -- all devices            **
** See Table 9-3 p. 250 of USB 2.0 spec for combinations          **
**   of request type bitfields with requests, wvalue, windex etc. **
**----------------------------------------------------------------*/

/*
NOTE!!!
DEPRECATED: Use USB_DESC_TYPE_xxx constant from usb_desc.h !
#define  DESCTYPE_DEVICE         0x1
#define  DESCTYPE_CONFIG         0x2
#define  DESCTYPE_STRING         0x3
#define  DESCTYPE_INTERFACE      0x4
#define  DESCTYPE_ENDPOINT       0x5
#define  DESCTYPE_QUALIFIER      0x6
#define  DESCTYPE_OTHER_SPEED    0x7
#define  DESCTYPE_INTF_POWER     0x8
#define  DESCTYPE_OTG            0x9
*/ 

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/

#define  ENDPOINT_HALT        0
#define  DEVICE_REMOTE_WAKEUP 1
#define  TEST_MODE            2

/* States of device instances on the device list */

/* initial device state */
#define  DEVSTATE_INITIAL        0x00
/* device descriptor [0..7]*/
#define  DEVSTATE_DEVDESC8       0x01
/* address set */
#define  DEVSTATE_ADDR_SET       0x02
/* full device descriptor */
#define  DEVSTATE_DEV_DESC       0x03
/* config descriptor [0..7] */
#define  DEVSTATE_GET_CFG9       0x04
/* check otg descriptor */
#define  DEVSTATE_CHK_OTG        0x05
/* full config desc. read in */
#define  DEVSTATE_CFG_READ       0x06
/* config set */
#define  DEVSTATE_SET_CFG        0x07
/* Select interface done */
#define  DEVSTATE_SET_INTF       0x08
/* enumeration done */
#ifdef USBCFG_OTG
#define  DEVSTATE_SET_HNP        0x0A
#define  DEVSTATE_SET_HNP_OK     0x0B
#define  DEVSTATE_ENUM_OK        0x0C
#else
#define  DEVSTATE_ENUM_OK        0x09
#endif

/* OTG related features */
#define OTG_B_HNP_ENABLE      3
#define OTG_A_HNP_SUPPORT     4
/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

//usb_status _usb_host_register_ch9_callback(usb_device_instance_handle, tr_callback, void*);
usb_status _usb_host_ch9_clear_feature(usb_device_instance_handle dev_handle, uint8_t req_type, uint8_t intf_endpt, uint16_t feature);
usb_status _usb_host_ch9_get_configuration(usb_device_instance_handle dev_handle, uint8_t* buffer);
usb_status _usb_host_ch9_get_descriptor
(
    /* usb device */
    usb_device_instance_handle   dev_handle,
    /* descriptor type & index */
    uint16_t                       type_index,
    /* Language ID or 0 */
    uint16_t                       lang_id,
    /* buffer length */
    uint16_t                       buflen,
    /* descriptor buffer */
    uint8_t *                     buffer
);
usb_status _usb_host_ch9_get_interface(usb_device_instance_handle dev_handle, uint8_t interface, uint8_t* buffer);
usb_status _usb_host_ch9_get_status(usb_device_instance_handle dev_handle, uint8_t req_type, uint16_t intf_endpt, uint8_t* buffer);
usb_status _usb_host_ch9_set_address(usb_device_instance_handle dev_handle);
usb_status _usb_host_ch9_set_configuration(usb_device_instance_handle dev_handle, uint16_t config);
usb_status _usb_host_ch9_set_descriptor(usb_device_instance_handle dev_handle, uint16_t type_index, uint16_t lang_id, uint16_t buflen, uint8_t* buffer);
usb_status _usb_host_ch9_set_feature(usb_device_instance_handle dev_handle, uint8_t req_type, uint8_t intf_endpt, uint16_t feature);
usb_status _usb_host_ch9_set_interface(usb_device_instance_handle dev_handle, uint8_t alternate, uint8_t intf);
usb_status _usb_host_ch9_synch_frame(usb_device_instance_handle dev_handle, uint8_t interface, uint8_t* buffer);
//usb_status _usb_hostdev_cntrl_request(usb_device_instance_handle, usb_setup_t*, uint8_t *, tr_callback, void*);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
