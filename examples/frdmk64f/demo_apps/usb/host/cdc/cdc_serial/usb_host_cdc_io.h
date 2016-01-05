/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
* $FileName: usb_host_cdc_io.h$
* $Version :
* $Date    :
*
* Comments:
*
*   This file is the header file for the standard formatted I/O library 
*   provided with mqx.
*   $Header:fio.h, 8, 5/14/2004 4:00:24 PM, $
*   $NoKeywords$
*
*END************************************************************************/
#ifndef __usb_host_cdc_io_h__
#define __usb_host_cdc_io_h__

/* Include for variable length argument functions */
#include <stdarg.h>
//#include "osadapter_cfg.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/
#define IO_OK                      (0)
/* Error code returned by I/O functions */
#define IO_ERROR                   (-1)
/* Serial I/O IOCTL commands */
#define CDC_IOCTL_CHAR_AVAIL            (0x30)
#define CDC_IOCTL_SERIAL_GET_FLAGS      (0x31)
#define CDC_IOCTL_SERIAL_SET_FLAGS      (0x32)
#define CDC_IOCTL_SERIAL_GET_STOP_BITS  (0x33)
#define CDC_IOCTL_SERIAL_SET_STOP_BITS  (0x34)
#define CDC_IOCTL_SERIAL_GET_DATA_BITS  (0x35)
#define CDC_IOCTL_SERIAL_SET_DATA_BITS  (0x36)
#define CDC_IOCTL_SERIAL_GET_BAUD       (0x37)
#define CDC_IOCTL_SERIAL_SET_BAUD       (0x38)

#define CDC_IO_SERIAL_STOP_BITS_1   (0x40)
#define CDC_IO_SERIAL_STOP_BITS_1_5 (0x41)
#define CDC_IO_SERIAL_STOP_BITS_2   (0x42)

/* CDC Xfer status */
typedef struct
{
    tr_struct_t * tr_ptr_tx;
    tr_struct_t * tr_ptr_rx;
    char * data_ptr_tx;
    char * data_ptr_rx;
    uint32_t num_tx;
    uint32_t num_rx;
    uint32_t num_left_tx;
    uint32_t num_left_rx;
    void (*tx_cb)(void* param);
    void (*rx_cb)(void* param);
}_cdc_io_xfer_sts_t;

/* Maximum line size for scanf */

/* Definitions for filesystem table */
/* map function names to mqx function names */
/* CR1343 & CR1455 #ifndef __cplusplus */

/*--------------------------------------------------------------------------*/
/*
**                        MACRO DECLARATIONS
*/
#define USB_INVALID_PARAMETER               (0x0C)
/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*--------------------------------------------------------------------------*/
/*
**                      FUNCTION PROTOTYPES
*/
int32_t _io_cdc_serial_char_avail(file_cdc_t * fd_ptr);
int32_t _io_cdc_serial_open(file_cdc_t *fd_ptr, char *open_name_ptr, const char *flags);
int32_t _io_cdc_serial_close(file_cdc_t *fd_ptr);
int32_t _io_cdc_serial_read(file_cdc_t *fd_ptr, char *data_ptr, int32_t num);
int32_t _io_cdc_serial_read_async(file_cdc_t *fd_ptr, char *data_ptr, int32_t num, _usb_cdc_callback cb_ptr);
int32_t _io_cdc_serial_write(file_cdc_t *fd_ptr, char *data_ptr, int32_t num);
int32_t _io_cdc_serial_write_async(file_cdc_t *fd_ptr, char *data_ptr, int32_t num, _usb_cdc_callback cb_ptr);
int32_t _io_cdc_serial_ioctl(file_cdc_t *fd_ptr, uint32_t cmd, void *param_ptr);
usb_status usb_class_cdc_install_driver(cdc_class_call_struct_t *data_instance, char *device_name);
usb_status usb_class_cdc_uninstall_driver(cdc_class_call_struct_t *data_instance);
#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif
