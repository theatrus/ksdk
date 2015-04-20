#ifndef __serinprv_h__
#define __serinprv_h__
/**HEADER********************************************************************
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
* $FileName: serinprv.h$
* $Version : 3.8.8.0$
* $Date    : Aug-29-2011$
*
* Comments:
*
*   This file includes the private definitions for the interrupt
*   driven serial I/O drivers.
*
*END************************************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** Xon/Xoff protocol characters
*/
#define CNTL_S   ((char) 0x13)  /* Control S == XOFF.   */
#define CNTL_Q   ((char) 0x11)  /* Control Q == XON.    */

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*---------------------------------------------------------------------
**
** IO SERIAL INT DEVICE STRUCT
**
** This structure used to store information about a interrupt serial io device
** for the IO device table
*/
typedef struct io_serial_int_device_struct
{

   /* The I/O init function */
   uint32_t (_CODE_PTR_ dev_init)(void *, char *);

   /* The enable interrupts function */
   uint32_t (_CODE_PTR_ dev_enable_ints)(void *);

   /* The I/O deinit function */
   uint32_t (_CODE_PTR_ dev_deinit)(void *, void *);

   /* The output function, used to write out the first character */
   void    (_CODE_PTR_ dev_putc)(void *, char);

   /* The ioctl function, (change bauds etc) */
   uint32_t (_CODE_PTR_ dev_ioctl)(void *, uint32_t, void *);

   /* The I/O channel initialization data */
   void               *dev_init_data_ptr;
   
   /* Device specific information */
   void               *dev_info_ptr;

   /* The queue size to use */
   uint32_t             queue_size;
   
   /* Open count for number of accessing file descriptors */
   uint32_t             count;

   /* Open flags for this channel */
   uint32_t             flags;

   /* The input queue */
   volatile CHARQ_STRUCT_PTR    in_queue;

   /* The input waiting tasks */
   void               *in_waiting_tasks;

   /* The output queue */
   volatile CHARQ_STRUCT_PTR    out_queue;

   /* The output waiting tasks */
   void               *out_waiting_tasks;

   /* Has output been started */
   bool             output_enabled;
   
   /* Protocol flag information */
   uint32_t             have_stopped_output;
   uint32_t             have_stopped_input;
   uint32_t             must_stop_input;
   uint32_t             must_start_input;
   uint32_t             input_high_water_mark;
   uint32_t             input_low_water_mark;
   uint32_t             must_stop_output;
   
#if MQX_ENABLE_LOW_POWER
   
   /* Low power related state information */
   IO_SERIAL_LPM_STRUCT  lpm_info;
   
#endif

	//LWSEM_STRUCT					 LWSEM;
} io_serial_int_device_struct_t;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Interrupt I/O prototypes */
extern int32_t  _io_serial_int_open(file_device_struct_t *, char *, char *);
extern int32_t  _io_serial_int_close(file_device_struct_t *);
extern int32_t  _io_serial_int_read(file_device_struct_t *, char *, int32_t);
extern int32_t  _io_serial_int_write(file_device_struct_t *, char *, int32_t);
extern int32_t  _io_serial_int_ioctl(file_device_struct_t *, uint32_t, 
   void *);
extern int32_t _io_serial_int_uninstall(IO_DEVICE_STRUCT_PTR);

/* Callback Functions called by lower level interrupt I/O interrupt handlers */
extern bool _io_serial_int_addc(io_serial_int_device_struct_t *, char);
extern int32_t  _io_serial_int_nextc(io_serial_int_device_struct_t *);

/* Internal helper functions */
extern bool    _io_serial_int_putc_internal(io_serial_int_device_struct_t *, 
   char, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
