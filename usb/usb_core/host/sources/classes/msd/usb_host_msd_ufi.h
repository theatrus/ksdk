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
 * $FileName: usb_host_msd_ufi.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file is a supporting header file for UFI.c which implements the UFI
 *   command set for mass storage applications
 *
 *END************************************************************************/
#ifndef __usb_host_msd_ufi_h__
#define __usb_host_msd_ufi_h__

#include "usb_host_msd_bo.h"

/* UFI specific error codes */
#define INVALID_UFI_REQUEST               (-1)

/* a should be two byte number */
#define HIGH_BYTE(a)                      ((a) >> 8)
#define LOW_BYTE(a)                       ((a) & 0xFF)

/*some useful macros for UFI command block*/
#define TRANSFER_LUN_TO_HIGH_3_BITS(x,l)   x &= ((l << 5)   | 0x1F)

/* Operation code for the UFI Commands (floppy drives) */
#define UFI_FORMAT_UNIT                   (0x04)
#define FORMAT_LUN_HEADER_BITS            (0x17)
#define UFI_READ_CAPACITY                 (0x25)
#define READ_CAPACITY_LUN_HEADER_BITS     (0x00)
#define UFI_READ_FORMAT_CAPACITY          (0x23)
#define UFI_INQUIRY                       (0x12)
#define INQUIRY_LUN_HEADER_BITS           (0x00)
#define UFI_MODE_SELECT                   (0x55)
#define MODE_SELECT_LUN_HEADER_BITS       (0x10)
#define UFI_MODE_SENSE                    (0x5A)
#define MODE_SENSE_LUN_HEADER_BITS        (0x00)
#define PAGE_CURRENT_VALUE                (0x00)
#define PAGE_CHANGEABLE_VALUE             (0x01)
#define PAGE_DEFAULT_VALUE                (0x10)
#define PAGE_SAVED_VALUE                  (0x11)

#define PAGE_CODE_ALL_PAGES               (0x3F)

#define UFI_PREVENT_ALLOW_MEDIUM_ROMVAL   (0x1E)
#define UFI_READ10                        (0x28)
#define READ10_LUN_HEADER_BITS            (0x00)
#define UFI_READ12                        (0xA8)
#define READ12_LUN_HEADER_BITS            (0x00)
#define UFI_REQUEST_SENSE                 (0x03)
#define UFI_REZERO_UNIT                   (0x01)
#define UFI_SEEK10                        (0x2B)
#define UFI_SEND_DIAGNOSTIC               (0x1D)
#define SEND_DIAG_LUN_HEADER_BITS         (0x00)
#define UFI_START_STOP                    (0x1B)
#define UFI_STOP_MEDIA                    (0x00)
#define UFI_START_MEDIA_AND_GET_FORMAT    (0x01)

#define UFI_TEST_UNIT_READY               (0x00)
#define UFI_VERIFY                        (0x2F)
#define VERIFY_LUN_HEADER_BITS            (0x00)

#define UFI_WRITE10                       (0x2A)
#define UFI_WRITE_LUN_HEADER_BITS         (0x00)
#define UFI_WRITE12                       (0xAA)
#define UFI_WRITE_AND_VERIFY              (0x2E)

#define DEFAULT_INTERLEAVE                (0x00)

/* USB Mass storage FORMAT UNIT Command information */

typedef struct _defect_list_header_struct
{
    uint8_t RESERVED1;
    uint8_t BBIT_INFO_HEADER;
    #define DEFAULT_BIT_INFO               (0xA0)
    uint8_t BLENGTH_MSB;
    uint8_t BLENGTH_LSB;
#define DEFAULT_LENGTH_MSB             (0x00)
#define DEFAULT_LENGTH_LSB             (0x08)
} defect_list_header_struct_t;

typedef struct _format_capacity_descriptor_struct
{
    uint8_t NNUM_BLOCKS[4];
    uint8_t RESERVED;
    uint8_t NBLOCK_LENGTH[3];
} format_capacity_descriptor_struct_t;

typedef struct _format_unit_parameter_block_struct
{
    defect_list_header_struct_t DEF_LIST_HEADER;
    format_capacity_descriptor_struct_t FMT_CAPACITY_DESC;
} format_unit_parameter_block_struct_t;

/* USB Mass storage READ CAPACITY Command information */

/* USB Mass storage Read10 Command information */
typedef struct _mass_storage_read_capacity_cmd_struct
{
    uint8_t BLLBA[4]; /* Last Logical Block Address */
    uint8_t BLENGTH[4]; /*Block length */
} mass_storage_read_capacity_cmd_struct_t;

/* USB Mass storage READ FORMAT CAPACITY Command information */

typedef struct _capacity_list_header
{
    uint8_t RESERVED[3];
    uint8_t BLENGTH[1]; /* Capacity list length */
} capacity_list_header_struct_t;

typedef struct _current_capacity_descriptor
{
    uint8_t NNUM_BLOCKS[4];
    uint8_t BDESC_CODE; /* Only last two bits are used */
#define  UNFORMATTED_MEDIA          (0x01)
#define  FORMATTED_MEDIA            (0x02)
#define  NO_MEDIA                   (0x03)
    uint8_t NBLOCK_LENGTH[3];
} current_capacity_descriptor_struct_t;

typedef struct _capacity_list
{
    capacity_list_header_struct_t LIST_HEADER; /* 4 bytes */
    current_capacity_descriptor_struct_t CAPACITY_DESCRIPTOR; /* 8 bytes */
    format_capacity_descriptor_struct_t FMT_CAPACITY_DESC; /* 8 bytes */
} capacity_list_struct_t;

/* USB Mass storage INQUIRY Command information */

typedef struct _inquiry_data_format
{
    /*device type currently connected to Logical Unit */
    uint8_t BDEVICE_TYPE;
    #define   DIRECT_ACCESS_DEVICE     (0x00)
#define   NONE                     (0x1F)

    /*removable media bit only bit 7 is used. rest reserved*/
    uint8_t BRMB;

    /*ISO, ECMA, ANSI Version bits*/
    uint8_t BVERSION_SPEC;
    #define DEFAULT_VERSION_SPEC       (0x00)

    /*Response data format */
    uint8_t BRESP_DATA_FORMAt;
    #define DEFAULT_RESNPOSE_FORMAT    (0x01)

    /* length of parameters */
    uint8_t BADITIONAL_LENGTH;
    #define DEFAULT_LENGTH             (0x1F)

    uint8_t RESERVED1;
    uint8_t RESERVED2;
    uint8_t RESERVED3;

    uint8_t BVID[8]; /* Vendor Identification String */
    uint8_t BPID[16]; /* Product Identification String */
    uint8_t BPRODUCT_REV[4]; /* Product revision level */

} inquiry_data_format_struct_t;

/* USB Mass storage MODE SELECT Command information */
typedef union _read_write_recovery_page
{
    uint8_t FIELDS[12];
} read_write_recovery_page_struct_t;

typedef union _mode_select_page
{
    read_write_recovery_page_struct_t READ_WRITE_PAGE;
} mode_select_page_struct_t;

typedef struct _mode_param_header
{
   uint8_t  BLENGTH[2];    /* Mode Data Length */
   uint8_t  BMEDIA_TYPE;   /* Media type code */
   #define DEFAULT_MEDIA_TYPE       (0x00)
   #define DD_720KB                 (0x1E)
   #define HD_125MB                 (0x93)
   #define DD_144MB                 (0x94)
   uint8_t  WP_DPOFUA;
   #define WP_DPOFUA_HEADER         (0xEF)
} mode_param_header_struct_t;

typedef struct _mode_select_parameter_list
{
    mode_param_header_struct_t MODE_PARAM_HEADER;
    #define      DEFAULT_NO_OF_PAGES (0x01)
    mode_select_page_struct_t PAGES[DEFAULT_NO_OF_PAGES];
} mode_select_parameter_list_struct_t;

/* USB Mass storage REQUEST SENSE Command information */

typedef struct _req_sense_data_format
{
   /* Valid and Error code*/
   uint8_t   BERROR_CODE;           /* 0 */
   #define REQUEST_SENSE_NOT_VALID  (0x00)
   #define REQUEST_SENSE_IS_VALID   (0x01)
   #define REQUEST_SENSE_ERROR_CODE (0x70)
   uint8_t   RESERVED1;             /* 1 */
   uint8_t   BSENSE_KEY;            /* 2  Only lower 4 bits are used */
   uint8_t   BINFO[4];              /* 3,4,5,6 Information  data format */
   uint8_t   BADITIONAL_LENGTH;     /* 7 additional sense length */
   uint8_t   RESERVED2[4];          /* 8,9,10,11*/
   uint8_t   BADITIONAL_CODE;       /* 12 Additional sense code */
   uint8_t   BADITIONAL_QUALIFIEr;  /* 13 Additional sense code qualifier */
   uint8_t   RESERVED3[4];          /* 14,15,16,17*/
} req_sense_data_format_struct_t;

/* USB Mass storage WRITE10 Command information */
typedef struct _mass_storage_write10_cmd_struct
{
    uint8_t BLBA[4];
    uint8_t BTRANSFER_LENGTH[2];
} mass_storage_write10_cmd_struct_t;

/* USB Mass storage WRITE12 Command information */
typedef struct _mass_storage_write12_cmd_struct
{
    uint8_t BLBA[4];
    uint8_t BTRANSFER_LENGTH;
} mass_storage_write12_cmd_struct_t;

/* USB Mass storage SEEK10 Command information */
typedef struct _mass_storage_seek10_struct
{
    uint8_t BLBA[4];
} mass_storage_seek10_strcut_t;

/* USB Mass storage SEND DIAGNOSTIC Command information */
typedef struct _mass_storage_send_diagnostic_struct
{
    bool SELF_TEST;
} mass_storage_send_diagnostic_struct_t;

/* USB Mass storage START-STOP UNIT Command information */
typedef struct _mass_storage_start_stop_unit_struct
{
    bool START;
    bool LOEJ;
} mass_storage_start_stop_unit_struct_t;

/* USB Mass storage VERIFY Command information */
typedef struct _mass_storage_verify_cmd_struct
{
    uint8_t BLBA[4];
    uint8_t BTRANSFER_LENGTH[2];
} mass_storage_verify_cmd_struct_t;

/* UFI commands */

/* Used by host-side to send the READ CAPACITY command */
#define usb_mass_ufi_read_capacity(                                       \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ_CAPACITY,                                          \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ_CAPACITY_LUN_HEADER_BITS),    \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the FORMAT UNIT command */
#define usb_mass_ufi_format_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t*  */             cmd_ptr,                      \
                                                                          \
      /* [IN] Track number to be formatted (see UFI specs) */             \
      /* uint8_t */                          track_num,                    \
                                                                          \
      /* [IN] Interleave number (see UFI specs) */                        \
      /* uint8_t */                          interleave,                   \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* FORMAT_UNIT_PARAMETER_BLOCK_PTR */ format_ptr                    \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_FORMAT_UNIT,                                            \
      (uint8_t)(((cmd_ptr)->LUN << 5) | FORMAT_LUN_HEADER_BITS),           \
      (uint32_t)(((track_num) << 24) | (HIGH_BYTE((interleave)) << 16) |   \
         (LOW_BYTE((interleave)) << 8)),                                  \
      (uint32_t)((format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0),\
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (uint8_t *)(format_ptr),                                            \
      (format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0)

/* Used by host-side to send the FORMAT CAPACITY command */
#define usb_mass_ufi_format_capacity(                                     \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ_FORMAT_CAPACITY,                                   \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))

/*
 ** Used by host-side to send the INQUIRY command, to request information
 ** regarding parameters of the UFI device itself
 */
#define usb_mass_ufi_inquiry(                                             \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_INQUIRY,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | INQUIRY_LUN_HEADER_BITS),          \
      (uint32_t)(((buf_len) & 0xFF) << 8),                                 \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the MODE SELECT command */
#define usb_mass_ufi_mode_select(                                         \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */             cmd_ptr,                       \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* void* */                        buf_ptr,                       \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                        buf_len                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_MODE_SELECT,                                            \
      (uint8_t)(((cmd_ptr)->LUN << 5) | MODE_SELECT_LUN_HEADER_BITS),      \
      (uint32_t)0,                                                         \
      (uint32_t)((buf_ptr) ? ((buf_len) << 8) : 0),                        \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (uint8_t *)(buf_ptr),                                               \
      (buf_ptr) ? (buf_len) : 0)

/* Used by host-side to send the MODE SENSE command */
#define usb_mass_ufi_mode_sense(                                          \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */         cmd_ptr,                           \
                                                                          \
      /*[IN] Page control byte 2 bits   */                                \
      /* uint8_t */                     bPC,                               \
                                                                          \
      /*[IN] Page code 6 bit          */                                  \
      /* uint8_t */                     bPage_code,                        \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_MODE_SENSE,                                             \
      (uint8_t)(((cmd_ptr)->LUN << 5) | MODE_SENSE_LUN_HEADER_BITS),       \
      (uint32_t)((((bPC) << 6) | ((bPage_code) & 0x3F)) << 24),            \
      (uint32_t)((buf_ptr) ? ((buf_len) << 8) : 0),                        \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_ptr) ? (buf_len) : 0)

/* Used by host-side to send the PREVENT-ALLOW MEDIUM REMOVAL command */
#define usb_mass_ufi_prevent_allow_medium_removal(                        \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint8_t */                  bPrevent /*1 or 0 values */           \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_PREVENT_ALLOW_MEDIUM_ROMVAL,                            \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)(((bPrevent) & 0x01) << 8),                                \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the READ(10) command, to get data from device */
#define usb_mass_ufi_read_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ10,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ10_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the READ(12) command, to get data from device */
#define usb_mass_ufi_read_12(                                             \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_READ12,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | READ12_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)(num_of_blocks),                                           \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the REQUEST SENSE command */
#define usb_mass_ufi_request_sense(                                       \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len                               \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_REQUEST_SENSE,                                          \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)(((buf_len) & 0xFF) << 8),                                 \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_IN,                                           \
      (uint8_t *)(buf_ptr),                                               \
      (buf_len))

/* Used by host-side to send the REZERO UNIT command */
#define usb_mass_ufi_rezero_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_REZERO_UNIT,                                            \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the SEEK command */
#define usb_mass_ufi_seek_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] block address (see UFI specs) */                            \
      /* uint32_t */                 bBlock_address                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_SEEK10,                                                 \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the SEND DIAGNOSTIC command */
#define usb_mass_ufi_send_diagnostic(                                     \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint8_t */                  bSelf_test                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_SEND_DIAGNOSTIC,                                        \
      (uint8_t)((((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS) |      \
         (((bSelf_test) & 0xFF) << 2)),                                   \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the START-STOP command */
#define usb_mass_ufi_start_stop(                                          \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* Load Object Bit */                                               \
      /* uint8_t */                  bLoej,                                \
                                                                          \
      /* Start Stop bit  */                                               \
      /* uint8_t */                  bStart                                \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_START_STOP,                                             \
      (uint8_t)(((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS),        \
      (uint32_t)(((((bLoej) << 1) | (bStart)) & 0x3) << 8),                \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the TEST UNIT READY command */
#define usb_mass_ufi_test_unit_ready(                                     \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_TEST_UNIT_READY,                                        \
      (uint8_t)((cmd_ptr)->LUN << 5),                                      \
      (uint32_t)0,                                                         \
      (uint32_t)0,                                                         \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the VERIFY command */
#define usb_mass_ufi_verify(                                              \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] address of the block to verify */                           \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* Length of the data to verify */                                  \
      /*uint16_t  */               dLength                                 \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_VERIFY,                                                 \
      (uint8_t)(((cmd_ptr)->LUN << 5) | VERIFY_LUN_HEADER_BITS),           \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((dLength) << 8),                                          \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)

/* Used by host-side to send the WRITE(10) command, to send data to device */
#define usb_mass_ufi_write_10(                                            \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE10,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the WRITE(12) command, to send data to device */
#define usb_mass_ufi_write_12(                                            \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE12,                                                \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)(num_of_blocks),                                          \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))

/* Used by host-side to send the WRITE AND VERIFY command */
#define usb_mass_ufi_write_and_verify(                                    \
      /* [IN] command object allocated by application*/                   \
      /* mass_command_struct_t* */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint32_t */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* void* */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint32_t */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks */                  \
      /* uint32_t */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint8_t)UFI_WRITE_AND_VERIFY,                                       \
      (uint8_t)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint32_t)(bBlock_address),                                          \
      (uint32_t)((num_of_blocks) << 8),                                    \
      (uint8_t)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))

/* Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    extern usb_status usb_mass_ufi_generic
    (
        /* [IN] command object allocated by application*/
      mass_command_struct_t*      cmd_ptr,
      uint8_t                     opcode,
      uint8_t                     lun,
      uint32_t                    lbaddr,
      uint32_t                    blen,

      uint8_t                     cbwflags,

      uint8_t *                   buf,
      uint32_t                    buf_len
    );

    extern bool usb_mass_ufi_cancel
    (
        mass_command_struct_t* cmd_ptr
    );
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
