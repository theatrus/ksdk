/*HEADER*********************************************************************
* 
* Copyright (c) 2008, 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: io_prv.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file includes the private definitions for the I/O subsystem.
*
*END************************************************************************/
#ifndef __io_prv_h__
#define __io_prv_h__


/*--------------------------------------------------------------------------*/
/*
 *                            CONSTANT DEFINITIONS
 */


/* Flag meanings */

/* Is the stream at EOF? */
#define IO_FLAG_TEXT        (4)
#define IO_FLAG_AT_EOF      (8)

/* Maximum name check length */
#define IO_MAXIMUM_NAME_LENGTH (1024)


/*
 * Error codes
 */
#define IO_OK                      (0x00)
#define IO_DEVICE_EXISTS           (0x01)
#define IO_DEVICE_DOES_NOT_EXIST   (0x02)
#define IO_ERROR_READ              (0x03)
#define IO_ERROR_WRITE             (0x04)
#define IO_ERROR_SEEK              (0x05)
#define IO_ERROR_WRITE_PROTECTED   (0x06)
#define IO_ERROR_READ_ACCESS       (0x07)
#define IO_ERROR_WRITE_ACCESS      (0x08)
#define IO_ERROR_SEEK_ACCESS       (0x09)
#define IO_ERROR_INVALID_IOCTL_CMD (0x0A)
#define IO_ERROR_DEVICE_BUSY       (0x0B)
#define IO_ERROR_DEVICE_INVALID    (0x0C)

#define IO_ERROR_TIMEOUT           (0x0D)
#define IO_ERROR_INQUIRE           (0x0E)

#define IO_ERROR_NOTALIGNED        (0x0F)

#define IO_ERROR_INVALID_PARAM     (0x10)

/* other I/O flags */
#define IO_PERIPHERAL_PIN_MUX_ENABLE                (0x01)
#define IO_PERIPHERAL_PIN_MUX_DISABLE               (0x02)
#define IO_PERIPHERAL_CLOCK_ENABLE                  (0x04)
#define IO_PERIPHERAL_CLOCK_DISABLE                 (0x08)
#define IO_PERIPHERAL_MODULE_ENABLE                 (0x10)
#define IO_PERIPHERAL_WAKEUP_ENABLE                 (0x20)
#define IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE    (0x40)

/*--------------------------------------------------------------------------*/
/*
 *                            DATATYPE DECLARATIONS
 */


/*--------------------------------------------------------------------------*/
/*
 *                    TYPEDEFS FOR * FUNCTIONS
 */
struct io_device_struct;

typedef int32_t (* IO_OPEN_FPTR)( file_device_struct_t *, char *, char *);
typedef int32_t (* IO_CLOSE_FPTR)( file_device_struct_t *);
typedef int32_t (* IO_READ_FPTR)( file_device_struct_t *, char *, int32_t);
typedef int32_t (* IO_WRITE_FPTR)( file_device_struct_t *, char *, int32_t);
typedef int32_t (* IO_IOCTL_FPTR)( file_device_struct_t *, uint32_t, void *);
typedef int32_t (* IO_UNINSTALL_FPTR)( struct io_device_struct *);

/* IO DEVICE STRUCT */
/*!
 * \brief This is the structure used to store device information for an
 * installed I/O driver.
 */
typedef struct io_device_struct
{

   /*! \brief Used to link io_device_structs together. */
   //QUEUE_ELEMENT_STRUCT QUEUE_ELEMENT;

   /*!
    * \brief A string that identifies the device.
    *
    * This string is matched by fopen, then the other information is used to
    * initialize a FILE struct for standard I/O. This string is also provided in
    * the kernel initialization record for the default I/O channel.
    */
   char             *IDENTIFIER;

   /*! \brief The I/O init function. */
   IO_OPEN_FPTR     IO_OPEN;

   /*! \brief The I/O deinit function. */
   IO_CLOSE_FPTR    IO_CLOSE;

   /*! \brief The I/O read function. */
   IO_READ_FPTR     IO_READ;

   /*! \brief The I/O write function. */
   IO_WRITE_FPTR    IO_WRITE;

   /*! \brief The I/O ioctl function. */
   IO_IOCTL_FPTR    IO_IOCTL;

   /*! \brief The function to call when uninstalling this device. */
  IO_UNINSTALL_FPTR IO_UNINSTALL;

   /*! \brief The I/O channel specific initialization data. */
   void            *DRIVER_INIT_PTR;

   /*! \brief Type of driver for this device. */
   uint32_t        DRIVER_TYPE;

}IO_DEVICE_STRUCT, * IO_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
 *                            FUNCTION PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
