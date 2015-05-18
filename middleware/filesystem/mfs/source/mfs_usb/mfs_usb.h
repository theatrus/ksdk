/**HEADER********************************************************************
* 
* Copyright (c) 2014 Freescale Semiconductor;
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
 **************************************************************************
 *
 * $FileName: msd_diskio.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *
 *
 *END************************************************************************/

#ifndef _MFS_USB_H
#define _MFS_USB_H

#include <stdint.h>
#include <nio.h>

#include <usb_host_config.h>
#include <usb.h>
#include <usb_host_stack_interface.h>
#include <usb_host_msd_ufi.h>
#define  HIGH_SPEED                        (0)

#if HIGH_SPEED
#define CONTROLLER_ID                      USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                      USB_CONTROLLER_KHCI_0
#endif

#define USBCFG_MAX_INSTANCE                                     4
/* Storage information*/
#define MSD_DEFAULT_SECTOR_SIZE                                 (512)
#define MAX_RETRY_TIMES                                         (3)
#define USBMFS_TIMEOUT                                          (5000)
     
/* USB MSD ioctl command */
#define UFI_REQUEST_SENSE_CMD                                   0x81
#define UFI_INQUIRY_CMD	                                        0x82
#define UFI_READ_FORMAT_CAPACITY_CMD                            0x83
#define UFI_READ_CAPACITY_CMD                                   0x84
#define UFI_TEST_UNIT_READY_CMD                                 0x85
#define UFI_MODE_SENSE_CMD                                      0x86
#define UFI_READ10_CMD                                          0x87

#define GET_SECTOR_COUNT                                        0x88
#define GET_SECTOR_SIZE                                         0x89
#define GET_BLOCK_SIZE                                          0x90
#define CTRL_SYNC                                               0x91

#define GET_VENDOR_INFO                                         0x92
#define GET_PRODUCT_ID                                          0x93
#define GET_PRODUCT_REV                                         0x94     

#define ERASE_BLOCK_SIZE                                        1 /* Unknown erasable block size*/

/* extend for DSTATUS */

struct mfs_device
{
    uint32_t       BLENGTH;   // logic block length
    uint32_t       BCOUNT;    // logic blocks count
    mass_command_struct_t msd_dev;
};

typedef struct mfsmsd_device 
{
    uint32_t                        dev_state;  /* Attach/detach state */
    usb_device_instance_handle      dev_handle;
    usb_interface_descriptor_handle intf_handle;
    usb_class_handle                class_handle; /* Class-specific info */
    uint8_t                         lun;
} mfsmsd_device_t;

struct file_data
{
    uint64_t LOCATION;
};

extern const NIO_DEV_FN_STRUCT usb_mfs_dev;

int _io_usb_mfs_install(char *id, mfsmsd_device_t *mass_device); //TODO: return error values like IO_DEVICE_EXISTS

#endif /* _MFS_USB_H */
