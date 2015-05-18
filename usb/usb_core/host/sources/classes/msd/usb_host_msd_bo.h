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
* $FileName: usb_host_msd_bo.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file is a header file which defines all necessary structure to
*   be used for sending commands to mass storage devices.
*
*END************************************************************************/
#ifndef __usb_host_msd_bo_h__
#define __usb_host_msd_bo_h__

#define MSD_RECV_MAX_TRANS_LENGTH       (65536)
#define MSD_SEND_MAX_TRANS_LENGTH       (65536)

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/



/*******************************************************************
**
** Mass Storage definitions
*/

/* Error codes */
#define USB_TRANSFER_CANCELLED         0xF1
#define USB_MASS_INVALID_CBW           -1
#define USB_MASS_QUEUE_FULL            -2
#define USB_MASS_NO_MATCHING_REQUEST   -4
#define USB_MASS_INVALID_INTF_HANDLE   -5
#define USB_MASS_INVALID_BUFFER        -6
#define USB_MASS_FAILED_IN_COMMAND     -7
#define USB_MASS_FAILED_IN_DATA        -8
#define USB_MASS_FAILED_IN_STATUS      -9

/* Constants */
#define MAX_CBWCB_SIZE                 16
#define CBW_DIRECTION_IN               0x80
#define CBW_DIRECTION_OUT              0x00
#define MAX_RETRIAL_ATTEMPTS_IN_CBW    (3)
#define MAX_RETRIAL_ATTEMPTS_IN_DPHASE (3)
#define MAX_RETRIAL_ATTEMPTS_IN_CSW    (3)
#define QUEUE_SIZE                     4
#define DIRECTION_OUT                  0x00
#define DIRECTION_IN                   0x80
#define MASK_NON_DIRECTION_BITS        0x80

/* Command Status Wrapper Signature 'USBS'*/
#define CBW_SIGNATURE               0x43425355
#define CSW_SIGNATURE               0x53425355
#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02

/* some useful defines */
#define ZERO_BIT0      0xFE
#define BE_HIGH_BYTE   0    /* LSB last in UFI specs (big endian)    */
#define BE_LOW_BYTE    1    /* LSB last in UFI specs (big endian)    */
#define LE_HIGH_BYTE   1    /* LSB last in UFI specs (little endian) */
#define LE_LOW_BYTE    0    /* LSB last in UFI specs (little endian) */

/*
** high nibble in x should be 1111
** transfer the low nibble of x to low nibble of a
*/
#define TRANSFER_LOW_NIBBLE(x,a)   (a) = (uint8_t)(((x) & 0x0Fu) | ((a) & 0xF0u))

#define CSW_SIZE  0x0C
#define CBW_SIZE  0x1F
#define GET_MAX_LUN   0xFE
#define MASS_STORAGE_RESET 0xFF


#define USB_CLASS_MASS_IS_Q_EMPTY(intf_ptr)      (bool)(intf_ptr->queue.COUNT==0)


/*********************************************************************
**
** Typedefs
*/

/* UFI Typical Command Block Wrapper for Most commands */
PACKED_STRUCT_BEGIN 
struct _ufi_cbwcb
{
    uint8_t      BUFIOPCODE;                 /* 0 */
    uint8_t      BUFILUN;                    /* 1 */
    uint8_t      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint8_t      RESERVED6;                  /* 6 Reserved */
    uint8_t      BLENGTH[2];                 /* 7,8 length of the data block */
    uint8_t      RESERVED9;                  /* 9 Reserved */
    uint8_t      RESERVED10;                 /* 10 Reserved */
    uint8_t      RESERVED11;                 /* 11 Reserved */
} PACKED_STRUCT_END;

typedef struct _ufi_cbwcb ufi_cbwcb_struct_t;

/* UFI Typical Command Block Wrapper for Extended commands */
PACKED_STRUCT_BEGIN 
struct _ufi_cbwcb_extended
{
    uint8_t      BUFIOPCODE;                 /* 0 */
    uint8_t      BUFILUN;                    /* 1 */
    uint8_t      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint8_t      BLENGTH[4];                 /* 6,7,8,9 length of the data block */
    uint8_t      RESERVED10;                 /* 10 Reserved */
    uint8_t      RESERVED11;                 /* 11 Reserved */
} PACKED_STRUCT_END;

typedef struct _ufi_cbwcb_extended ufi_cbwcb_extended_struct_t;


/* define a union that covers all supported protocols.  */

PACKED_UNION_BEGIN 
union _cbwcb 
{
   ufi_cbwcb_struct_t            UFI_CBWCB;
   ufi_cbwcb_extended_struct_t   UFI_CBWCB_EXT;
   uint8_t                       MAX_COMMAND[16];
} PACKED_UNION_END;

typedef union _cbwcb cbwcb_struct_t;

/* State machine constants of Class driver */
enum _usb_class_mass_command_status
{
   STATUS_COMPLETED = USB_OK,
   STATUS_CANCELLED,
   STATUS_QUEUED_IN_DRIVER,
   STATUS_QUEUED_CBW_ON_USB,
   STATUS_FINISHED_CBW_ON_USB,
   STATUS_QUEUED_DPHASE_ON_USB,
   STATUS_FINISHED_DPHASE_ON_USB,
   STATUS_QUEUED_CSW_ON_USB,
   STATUS_FINISHED_CSW_ON_USB,
   STATUS_FAILED_IN_CSW,
   STATUS_RESET_BULK_IN,
   STATUS_RESET_BULK_OUT,
   STATUS_RESET_INTERFACE,
   STATUS_RESET_DEVICE,
   STATUS_CLEAR_BULK_PIPE,
   STATUS_FAILED_IN_CBW,
   STATUS_COMMAND_FAILED
};

typedef enum _usb_class_mass_command_status usb_class_mass_command_status;


/* Define the representation of a circular queue */
typedef struct _mass_queue_struct
{
   void*                 ELEMENTS[QUEUE_SIZE]; /* storage for the queue*/
   /* CR TBD */
   uint32_t              COUNT;
   uint8_t               FIRST;         /* index of the first element in the queue*/
   uint8_t               LAST;          /* index of the last element in the queue*/
} mass_queue_struct_t;

/* Command Block Wrapper (see USB Mass Storage specs) */
PACKED_STRUCT_BEGIN
struct _cbw_struct
{
   uint8_t               DCBWSIGNATURE[4];            /* 0-3   */
   uint8_t               DCBWTAG[4];                  /* 4-7   */
   uint8_t               DCBWDATATRANSFERLENGTH[4];   /* 8-11  */
   uint8_t               BMCBWFLAGS;                  /* 12    */
   uint8_t               BCBWLUN;                     /* 13    */
   uint8_t               BCBWCBLENGTH;                /* 14    */
   cbwcb_struct_t        CMD_BLOCK;                   /* 15-31 */
}
PACKED_STRUCT_END;

typedef struct _cbw_struct cbw_struct_t;

/* Command Status Wrapper   (see USB Mass Storage specs) */
typedef struct _csw_struct
{
    uint8_t             DCSWSIGNATURE[4];         /* 0-3    */
    uint8_t             DCSWTAG[4];               /* 4-7    */
    uint8_t             DCSWDATARESIDUE[4];       /* 8-11   */
    uint8_t             BCSWSTATUS;               /* 12     */
} csw_struct_t;

/* USB Mass Class  One Single Command Object for all protocols */
typedef struct
{
   usb_class_handle                   CLASS_PTR;      /* Class intf data pointer and key    */
   uint32_t                           LUN;           /* Logical unit number on device      */
   cbw_struct_t *                     CBW_PTR;       /* current CBW being constructed      */
   csw_struct_t *                     CSW_PTR;       /* CSW for this command               */
   void (_CODE_PTR_                   CALLBACK)
      (usb_status status,                            /* status of this command                       */
       void* p1,                                     /* pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT */
       void* p2,                                     /* pointer to the command object                */
       uint32_t buffer_length                        /* length of the data transfered if any         */
      );

   void*                              DATA_BUFFER;   /* buffer for IN/OUT for the command  */
   uint32_t                           BUFFER_LEN;    /* length of data buffer              */
   uint32_t                           BUFFER_SOFAR;  /* number of bytes trans so far */
   usb_class_mass_command_status      STATUS;        /* current status of this command     */
   usb_class_mass_command_status      PREV_STATUS;   /* previous status of this command    */
   uint32_t                           TR_BUF_LEN;    /* length of the buffer received in
                                                       currently executed TR              */
   uint8_t                            RETRY_COUNT;   /* Number of times this command tried  */
   uint8_t                            CBW_RETRY_COUNT;   /* Number of times this command tried  */
   uint8_t                            DPHASE_RETRY_COUNT;   /* Number of times this command tried  */
   uint8_t                            CSW_RETRY_COUNT;   /* Number of times this command tried  */
   uint8_t                            IS_STALL_IN_DPHASE;   /* Is stall happened in data dphase  */
   uint8_t                            TR_INDEX;      /* TR_INDEX of the TR used for search */
} mass_command_struct_t;

/*
** USB Mass Class Interface structure. This structure will be passed to all
** commands to this class driver. The structure holds all information
** pertaining to an interface on storage device. This allows the class driver
** to know which interface the command is directed for.
*/

typedef struct
{
   usb_host_handle                    host_handle; 
   usb_device_instance_handle         dev_handle;
   usb_interface_descriptor_handle    intf_handle;

   usb_pipe_handle                    control_pipe;      /* control pipe handle*/
   usb_pipe_handle                    bulk_in_pipe;      /* Bulk in pipe handle*/
   usb_pipe_handle                    bulk_out_pipe;     /* Bulk out pipe handle*/
   mass_queue_struct_t                queue;             /* structure that queues requests*/                   
   uint8_t                            interface_num;     /* interface number*/
   uint8_t                            alternate_setting; /* Alternate setting*/
   /* Here we store callback and parameter from higher level */
   tr_callback                        ctrl_callback;
   void *                             ctrl_param;
   os_mutex_handle                    mutex;
} usb_mass_class_struct_t;


#define USB_Host_MSD_lock()                OS_Mutex_lock(((usb_mass_class_struct_t*)mass_class)->mutex)
#define USB_Host_MSD_unlock()              OS_Mutex_unlock(((usb_mass_class_struct_t*)mass_class)->mutex)

#ifdef __cplusplus
extern "C" {
#endif

int32_t usb_class_mass_q_insert(usb_mass_class_struct_t* mass_class, mass_command_struct_t* pCmd);
void usb_class_mass_deleteq(usb_mass_class_struct_t* mass_class);
void usb_class_mass_get_pending_request(usb_mass_class_struct_t* mass_class, mass_command_struct_t** cmd_ptr_ptr);
void usb_class_mass_q_init(usb_mass_class_struct_t* msd_class_ptr);
usb_status usb_class_mass_init(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, usb_class_handle* class_handle_ptr);
usb_status usb_class_mass_deinit(usb_class_handle handle);
usb_status usb_class_mass_pre_deinit(usb_class_handle handle);
usb_status usb_class_mass_storage_device_command(mass_command_struct_t* cmd_ptr);
bool usb_class_mass_storage_device_command_cancel(mass_command_struct_t* cmd_ptr);
usb_status usb_class_mass_getmaxlun_bulkonly(usb_class_handle handle, uint8_t* pLUN, tr_callback callback, void* callback_param);
usb_status usb_class_mass_getvidpid(usb_class_handle handle, uint16_t* vid, uint16_t* pid);
usb_status usb_class_mass_reset_recovery_on_usb(usb_mass_class_struct_t* mass_class);
bool usb_class_mass_cancelq(usb_mass_class_struct_t* mass_class, mass_command_struct_t* pCmd);

#ifdef __cplusplus
}
#endif

#endif
