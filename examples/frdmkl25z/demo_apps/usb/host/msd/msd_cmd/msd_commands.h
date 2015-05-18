/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: msc_commands.h$
 * $Version : 3.0.1.0$
 * $Date    : Sep-23-2008$
 *
 * Comments:
 *
 *   This file contains mass storage class application types and definitions.
 *
 *END************************************************************************/
#ifndef __msc_commands_h__
#define __msc_commands_h__

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS          16
#define  MAX_FRAME_SIZE                    512
#define  HOST_CONTROLLER_NUMBER            0

#define BUFF_IN_SIZE                   0x40c

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACE_OPENED       (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)
#define  USB_DEVICE_INTERFACE_CLOSED       (9)

#define  HIGH_SPEED                        (0)

#if HIGH_SPEED
#define CONTROLLER_ID                      USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                      USB_CONTROLLER_KHCI_0
#endif

#define CTRL_SYNC            0    /* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT     1    /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE      2    /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE       3    /* Get erase block size (for only f_mkfs()) */
#define ERASE_BLOCK_SIZE     1    /* Unknown erasable block size*/

/* 
 ** This macro is unsupported in Vybrid series board.
 **/
#define TEST_SECTOR_READ_WRITE_SPEED     (0)

#if TEST_SECTOR_READ_WRITE_SPEED

#ifndef  DWT_CR_CYCCNTENA
#define  DWT_CR_CYCCNTENA                (1 <<  0)
#endif

#ifndef  DWT_CR
#define  DWT_CR      *((volatile uint32_t *)0xE0001000)                   /* Data Watchpoint and Trace (DWT) Control Register     */                
#endif

#ifndef  DWT_CYCCNT
#define  DWT_CYCCNT  *((volatile uint32_t *)0xE0001004)                   /* Data Watchpoint and Trace (DWT) Cycle Count Register */
#endif

#ifndef  DEMCR
#define  DEMCR       *((volatile uint32_t *)0xE000EDFC)
#endif

#define DEMCR_TRCENA                     (1 << 24)
#define DEMCR_MON_REQ                    (1 << 19)
#define DEMCR_MON_STEP                   (1 << 18)
#define DEMCR_MON_PEND                   (1 << 17)
#define DEMCR_MON_EN                     (1 << 16)

#define TEST_SECTOR_READ1                (0)
#define TEST_SECTOR_READ2                (0)
#define TEST_SECTOR_READ4                (0)
#define TEST_SECTOR_READ_MULTI           (1)
#define TEST_SECTOR_WRITE1               (0)
#define TEST_SECTOR_WRITE2               (0)
#define TEST_SECTOR_WRITE4               (0)
#define TEST_SECTOR_WRITE_MULTI          (1)

#endif
/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */

typedef struct _device_struct
{
    uint32_t dev_state; /* Attach/detach state */
    usb_device_instance_handle dev_handle;
    usb_interface_descriptor_handle intf_handle;
    usb_class_handle CLASS_HANDLE; /* Class-specific info */
} device_struct_t;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    usb_status usb_host_mass_device_event
    (
        usb_device_instance_handle dev_handle,
        usb_interface_descriptor_handle intf_handle,
        uint32_t event_code
    );

#if 0 
    static void usb_host_mass_ctrl_callback
    (
    usb_pipe_handle,
    pointer,
    uchar_ptr,
    uint_32,
    uint_32
    );
#endif

    void callback_bulk_pipe(usb_status status,
        void * p1,
        void * p2,
        uint32_t buffer_length
        );

    static void usb_host_mass_test_storage(void);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */

