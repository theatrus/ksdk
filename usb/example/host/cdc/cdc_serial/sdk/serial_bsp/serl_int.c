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
* $FileName: serl_int.c$
* $Version : 3.8.19.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the driver functions for the interrupt driven
*   serial asynchronous character I/O.
*
*END************************************************************************/
//#include "types.h"
//#include "user_config.h"
//#include "bsp.h"
	
//#include "derivative.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "adapter.h"

#include "serial.h"
#include "io_serl_int.h"
#include "io_prv.h"
#include "charq.h"
#include "serinprv.h"
#include "fsl_uart_driver.h"

extern io_serial_int_device_struct_t sci_int_dev;
extern uart_state_t uartState;
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_int_install
* Returned Value   : uint32_t a task error code or IO_OK
* Comments         :
*    Install an interrupt driven serial device.
*
*END*----------------------------------------------------------------------*/

uint32_t _io_serial_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char             *identifier,
  
      /* [IN] The I/O init function */
      uint32_t (_CODE_PTR_ init)(void *, char *),

      /* [IN] The enable interrupts function */
      uint32_t (_CODE_PTR_ enable_ints)(void *),

      /* [IN] The I/O de-init function */
      uint32_t (_CODE_PTR_ deinit)(void *, void *),

      /* [IN] The output function */
      void    (_CODE_PTR_  putc)(void *, char),

      /* [IN] The I/O ioctl function */
      uint32_t (_CODE_PTR_ ioctl)(void *, uint32_t, void *),

      /* [IN] The I/O init data pointer */
      void                *init_data_ptr,
      
      /* [IN] The I/O queue size to use */
      uint32_t             queue_size
   )
{ /* Body */
#if 0
   io_serial_int_device_struct_t * int_io_dev_ptr;
   uint32_t                         result;

   int_io_dev_ptr = (io_serial_int_device_struct_t *)OS_Mem_alloc_zero(
      (uint32_t)sizeof(io_serial_int_device_struct_t));

   int_io_dev_ptr->dev_init          = init;
   int_io_dev_ptr->dev_enable_ints   = enable_ints;
   int_io_dev_ptr->dev_deinit        = deinit;
   int_io_dev_ptr->dev_putc          = putc;
   int_io_dev_ptr->dev_ioctl         = ioctl;
   int_io_dev_ptr->dev_init_data_ptr = init_data_ptr;
   int_io_dev_ptr->queue_size        = queue_size;
   
   result = _io_dev_install(identifier,
      _io_serial_int_open, _io_serial_int_close,
      _io_serial_int_read, _io_serial_int_write,
      _io_serial_int_ioctl,
      (void *)int_io_dev_ptr); 
   
   return result;
#endif
	return IO_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_int_uninstall
* Returned Value   : int32_t a task error code or IO_OK
* Comments         :
*    Un-Install a interrupt driven async serial device.
*
*END*----------------------------------------------------------------------*/

int32_t _io_serial_int_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   io_serial_int_device_struct_t * int_dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;

   if (int_dev_ptr->count == 0) {
      if (int_dev_ptr->dev_deinit) {

         (*int_dev_ptr->dev_deinit)(int_dev_ptr->dev_init_data_ptr,
            int_dev_ptr->dev_info_ptr);
          
      } /* Endif */
      OS_Mem_free(int_dev_ptr->in_queue);
      OS_Mem_free(int_dev_ptr->out_queue);
      
      OS_Mem_free(int_dev_ptr);
      return(IO_OK);
   } else {
      return(IO_ERROR_DEVICE_BUSY);
   } /* Endif */

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_open
* Returned Value   : int32_t task error code
* Comments         :
*    This routine initializes an interrupt I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    This memory is returned as a 'handle' to be used for all other 
*    interrupt I/O functions.
*
* 
*END**********************************************************************/

int32_t _io_serial_int_open
   (
      /* [IN] the file handle for the device being opened */
      file_device_struct_t * fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char              *open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff
      */
      char              *flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   io_serial_int_device_struct_t * int_io_dev_ptr;
   uint32_t                       result = IO_OK;
   uint32_t                       ioctl_val;

   io_dev_ptr     = fd_ptr->dev_ptr;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   
   if (int_io_dev_ptr->count) {
      /* Device is already opened */
      int_io_dev_ptr->count++;
      fd_ptr->flags = int_io_dev_ptr->flags;
      return(result);
   } /* Endif */


   int_io_dev_ptr->in_queue = (void *)OS_Mem_alloc_zero(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + int_io_dev_ptr->queue_size);
   int_io_dev_ptr->out_queue = (void *)OS_Mem_alloc_zero(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + int_io_dev_ptr->queue_size);

   _CHARQ_INIT(int_io_dev_ptr->in_queue, int_io_dev_ptr->queue_size);
   _CHARQ_INIT(int_io_dev_ptr->out_queue, int_io_dev_ptr->queue_size);
   int_io_dev_ptr->input_high_water_mark = int_io_dev_ptr->queue_size -
      int_io_dev_ptr->queue_size/8;
   int_io_dev_ptr->input_low_water_mark  = int_io_dev_ptr->queue_size/2;
   int_io_dev_ptr->flags = (uint32_t)flags;
   fd_ptr->flags      = (uint32_t)flags;

   result = (*int_io_dev_ptr->dev_init)(int_io_dev_ptr, open_name_ptr);
   
   if (result == IO_OK) {
       
      result = (*int_io_dev_ptr->dev_enable_ints)(int_io_dev_ptr->dev_info_ptr);
      
      if (int_io_dev_ptr->dev_ioctl != NULL) {
         if ((uint32_t)flags & IO_SERIAL_HW_FLOW_CONTROL) {
            ioctl_val = IO_SERIAL_RTS;
            (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
         } /* Endif */
         ioctl_val = (uint32_t)flags;
         (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_SET_FLAGS, &ioctl_val);
      }
      if (result == IO_OK && ((uint32_t)flags & IO_SERIAL_NON_BLOCKING) 
         && (uint32_t)flags & (IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO | IO_SERIAL_XON_XOFF)) {
         result = IO_ERROR_INVALID_PARAM;
      } /* Endif */
   } /* Endif */
   
   if (result != IO_OK) {
      OS_Mem_free(int_io_dev_ptr->in_queue);
      OS_Mem_free(int_io_dev_ptr->out_queue);
   }
   int_io_dev_ptr->count = 1;
   return(result);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_close
* Returned Value   : int32_t error code
* Comments         :
*    This routine closes the serial I/O channel.
* 
*END**********************************************************************/

int32_t _io_serial_int_close
   (
      /* [IN] the file handle for the device being closed */
      file_device_struct_t * fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   io_serial_int_device_struct_t * int_io_dev_ptr;
   int32_t                        result = IO_OK;
   int32_t                        ioctl_val;

   /* other task cannot break 'close' function */
   OS_Lock();

   io_dev_ptr     = fd_ptr->dev_ptr;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   /* flush the output buffer before closing */
   (*io_dev_ptr->IO_IOCTL)(fd_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);

   if (--int_io_dev_ptr->count == 0) {

      if (int_io_dev_ptr->dev_ioctl != NULL) {
         if (fd_ptr->flags & IO_SERIAL_HW_FLOW_CONTROL) {
            ioctl_val = IO_SERIAL_RTS;
            (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL, &ioctl_val);
         }
      }
      if (int_io_dev_ptr->dev_deinit) {
          
         result = (*int_io_dev_ptr->dev_deinit)(int_io_dev_ptr->dev_init_data_ptr,
            int_io_dev_ptr->dev_info_ptr);
          
      } /* Endif */
      OS_Mem_free(int_io_dev_ptr->in_queue);
      int_io_dev_ptr->in_queue = NULL;
      OS_Mem_free(int_io_dev_ptr->out_queue);
      int_io_dev_ptr->out_queue = NULL;
   } /* Endif */

   OS_Unlock();
   return(result);
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_read
* Returned Value   : int32_t number of characters read
* Comments         :
*    This routine reads characters from the input ring buffer,
*    converting carriage return ('\r') characters to newlines,
*    and then echoing the input characters.
*
*END*********************************************************************/

int32_t _io_serial_int_read
   (
      /* [IN] the handle returned from _fopen */
      file_device_struct_t * fd_ptr,

      /* [IN] where the characters are to be stored */
      char              *data_ptr,

      /* [IN] the number of characters to input */
      int32_t               num
      
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   io_serial_int_device_struct_t * int_io_dev_ptr;
   int32_t                        ioctl_val;
   unsigned char                           c = 0;
   uint32_t                       flags;
   int32_t                        i = num;
   volatile CHARQ_STRUCT          *in_queue;
   bool                           is_os_unlock = FALSE;

   io_dev_ptr     = fd_ptr->dev_ptr;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   flags          = fd_ptr->flags;

   while ( i ) {

      in_queue = int_io_dev_ptr->in_queue;
      OS_Lock();
      if(flags & IO_SERIAL_NON_BLOCKING) {
          if (_CHARQ_SIZE(in_queue) == 0) {
              num -= i;
              OS_Unlock();
              break;
          } /* Endif */
      } else {
          if(_CHARQ_SIZE(in_queue) == 0){
              OS_Unlock();
			  is_os_unlock = TRUE;
          }
          while (_CHARQ_SIZE(in_queue) == 0) {
          	/* Wait to receive data */
          } /* Endwhile */  
      } /* Endif */
	  if (is_os_unlock == TRUE){
		  OS_Lock();
		  is_os_unlock = FALSE;
	  }
      _CHARQ_DEQUEUE(in_queue,c);

      if (int_io_dev_ptr->have_stopped_input) {
         if (_CHARQ_SIZE(in_queue) < int_io_dev_ptr->input_low_water_mark) {
            if (flags & IO_SERIAL_HW_FLOW_CONTROL) {
               if (int_io_dev_ptr->dev_ioctl != NULL) {
                  ioctl_val = IO_SERIAL_RTS;
                  (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
               }
               int_io_dev_ptr->have_stopped_input = FALSE;
            } else {
               if (int_io_dev_ptr->output_enabled && !int_io_dev_ptr->have_stopped_output) {
                  int_io_dev_ptr->must_start_input = TRUE;
               } else {
                  int_io_dev_ptr->have_stopped_input = FALSE;
                  int_io_dev_ptr->output_enabled = TRUE;
                  (*int_io_dev_ptr->dev_putc)(int_io_dev_ptr, CNTL_Q);
               } /* Endif */
            } /* Endif */
         } /* Endif */
      } /* Endif */
      OS_Unlock();

      if (flags & IO_SERIAL_TRANSLATION) {
         if (c == '\r') {
            /* Start CR 387 */
            if (flags & IO_SERIAL_ECHO) {
               _io_serial_int_putc_internal(int_io_dev_ptr, (char)c, 0);
            } /* Endif */
            /* End CR 387 */
            c = '\n';
         } else if ((c == '\b') && (flags & IO_SERIAL_ECHO)) {
            _io_serial_int_putc_internal(int_io_dev_ptr, (char)'\b', 0);
            _io_serial_int_putc_internal(int_io_dev_ptr, (char)' ', 0);
         } /* Endif */
      } /* Endif */

      if (flags & IO_SERIAL_ECHO) {
         _io_serial_int_putc_internal(int_io_dev_ptr, (char)c, 0);
      } /* Endif */

      *data_ptr++ = c;
      --i;
      
   } /* Endwhile */

   return num;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_write
* Returned Value   : int32_t
* Comments         :
*    This routine writes the character to the device.
*    It also converts the C '\n' into '\n\r', if required.
*
*END**********************************************************************/

int32_t _io_serial_int_write
   (
      /* [IN] the handle returned from _fopen */
      file_device_struct_t * fd_ptr,

      /* [IN] where the characters to print out are */
      char              *data_ptr,

      /* [IN] the number of characters to output */
      int32_t               num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   io_serial_int_device_struct_t * int_io_dev_ptr;
   uint32_t                       flags;
   int32_t                        i = num;

   io_dev_ptr     = fd_ptr->dev_ptr;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;
   flags          = fd_ptr->flags;

   while ( i != 0 ) {
      if (flags & IO_SERIAL_TRANSLATION) {
         if (*data_ptr == '\n') {
            (void)_io_serial_int_putc_internal(int_io_dev_ptr, '\r', 0);
         } /* Endif */
      } /* Endif */
      if (_io_serial_int_putc_internal(int_io_dev_ptr, *data_ptr, flags)){
          data_ptr++;
          i--;
      } else {
         num -= i;
         break;
      } /* Endif */
      
   } /* Endwhile */
   
   return num;
   
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_serial_int_ioctl
* Returned Value   : int32_t
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

int32_t _io_serial_int_ioctl
   (
      /* [IN] the handle returned from _fopen */
      file_device_struct_t * fd_ptr,

      /* [IN] the ioctl command */
      uint32_t              cmd,

      /* [IN] the ioctl parameters */
      void                  *param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   io_serial_int_device_struct_t * int_io_dev_ptr;
   uint32_t                       result = IO_OK;
   uint32_t *                   uparam_ptr = (uint32_t *)param_ptr;

   io_dev_ptr     = fd_ptr->dev_ptr;
   int_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   switch (cmd) {
      case IO_IOCTL_CHAR_AVAIL:
         if ( _CHARQ_SIZE(int_io_dev_ptr->in_queue) ) {
           *((bool *)param_ptr) = TRUE;
         } else {
           *((bool *)param_ptr) = FALSE;
         } /* Endif */
      break;

      case IO_IOCTL_SERIAL_GET_FLAGS:
         *((uint32_t *)param_ptr) = int_io_dev_ptr->flags;
         break;

      case IO_IOCTL_SERIAL_SET_FLAGS:
         int_io_dev_ptr->flags = *((uint32_t *)param_ptr);
         fd_ptr->flags = *((uint32_t *)param_ptr);
         if (int_io_dev_ptr->dev_ioctl != NULL) {
            result = (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr,
               cmd, param_ptr);
         } /* Endif */
         break;

      case IO_IOCTL_SERIAL_TRANSMIT_DONE:
         *((bool *)param_ptr) = !(int_io_dev_ptr->output_enabled);
         break;
      
      case IO_IOCTL_FLUSH_OUTPUT:
         while(TRUE == uartState.isTxBusy){
             /* Wait for the last TX transfer to complete */
         };
         break;
      
      case IO_IOCTL_SERIAL_CAN_TRANSMIT:
         *uparam_ptr = (int_io_dev_ptr->out_queue->MAX_SIZE - int_io_dev_ptr->out_queue->CURRENT_SIZE);
         result = IO_OK;   
         break;

      case IO_IOCTL_SERIAL_CAN_RECEIVE:
         *uparam_ptr = (int_io_dev_ptr->in_queue->MAX_SIZE - int_io_dev_ptr->in_queue->CURRENT_SIZE);
         result = IO_OK;   
         break;
            
      default:
         if (int_io_dev_ptr->dev_ioctl != NULL) {
            result = (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr,
               cmd, param_ptr);
         } /* Endif */
      break;
   } /* Endswitch */
   return result;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_putc_internal
* Returned Value   : void
* Comments         : 
*   This function writes out the character to the device if the queue
* is empty, or it writes it to the device.  If the queue is full, this
* function will suspend the writing task.
*
*END*********************************************************************/

bool _io_serial_int_putc_internal
   (
      /* [IN] the interrupt io device information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the character to print out */
      char                     c, 
      uint32_t                flags
   )
{ /* Body */
   volatile CHARQ_STRUCT      *out_queue;
   bool                       is_os_unlock = FALSE;

   /* Start CR 388 */
#if (PSP_MEMORY_ADDRESSING_CAPABILITY > 8 )
   c &= 0xFF;
#endif
   /* End CR 388 */

   out_queue = int_io_dev_ptr->out_queue;
   OS_Lock();
   if(flags & IO_SERIAL_NON_BLOCKING) {
      if (_CHARQ_FULL(out_queue)) {
          OS_Unlock();
          return FALSE;
      } /* Endif */
   } else {
      if(int_io_dev_ptr->have_stopped_output) {
      } /* Endif */
      if(_CHARQ_FULL(out_queue)){
		  OS_Unlock();
		  is_os_unlock = TRUE;
      }
      while (_CHARQ_FULL(out_queue)) {
         /* Lets wait */
      } /* Endif */
   } /* Endif */
   if(is_os_unlock == TRUE){
	  OS_Lock();
	  is_os_unlock = FALSE;
   }

   if (int_io_dev_ptr->output_enabled || int_io_dev_ptr->have_stopped_output) {
      _CHARQ_ENQUEUE(out_queue,c);
   } else {
      int_io_dev_ptr->output_enabled = TRUE;
      (*int_io_dev_ptr->dev_putc)(int_io_dev_ptr, c);
   } /* Endif */
   OS_Unlock();
   return TRUE;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_addc
* Returned Value   : none
* Comments         :
*    This function is called by the device io interrupt handler, to add
* a character to the input queue.
*
*END*********************************************************************/

bool _io_serial_int_addc
   (
      /* [IN] the interrupt I/O context information */
      io_serial_int_device_struct_t * int_io_dev_ptr,

      /* [IN] the character to add to the input queue */
      char                            c
   )
{ /* Body */
   CHARQ_STRUCT_PTR in_queue;
   uint32_t       ioctl_val;

   if (int_io_dev_ptr->flags & IO_SERIAL_XON_XOFF) {
      if (int_io_dev_ptr->have_stopped_output) {
         if (c == CNTL_Q) {
            int_io_dev_ptr->have_stopped_output = FALSE;
            return TRUE;
         } /* Endif */
      } else {
         if (c == CNTL_S) {
            int_io_dev_ptr->have_stopped_output = TRUE;
            return TRUE;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   in_queue = int_io_dev_ptr->in_queue;
   if (_CHARQ_NOT_FULL(in_queue)) {
      _CHARQ_ENQUEUE(in_queue,c);

      if (int_io_dev_ptr->flags & (IO_SERIAL_XON_XOFF |
         IO_SERIAL_HW_FLOW_CONTROL))
      {
         if (_CHARQ_SIZE(in_queue) > int_io_dev_ptr->input_high_water_mark) {
            if (!int_io_dev_ptr->have_stopped_input) {
               if (int_io_dev_ptr->flags & IO_SERIAL_XON_XOFF) {
                  int_io_dev_ptr->must_stop_input = TRUE;
               } else {
                  if (int_io_dev_ptr->dev_ioctl != NULL) {
                     ioctl_val = IO_SERIAL_RTS;
                     (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL, &ioctl_val);
                  }
                  int_io_dev_ptr->have_stopped_input = TRUE;
               } /* Endif */
            } /* Endif */
         } else if (_CHARQ_SIZE(in_queue) < int_io_dev_ptr->input_low_water_mark) {
            if (int_io_dev_ptr->have_stopped_input) {
               if (int_io_dev_ptr->flags & IO_SERIAL_XON_XOFF) {
                  int_io_dev_ptr->must_start_input = TRUE;
               } else {
                  if (int_io_dev_ptr->dev_ioctl != NULL) {
                     ioctl_val = IO_SERIAL_RTS;
                     (*int_io_dev_ptr->dev_ioctl)(int_io_dev_ptr->dev_info_ptr, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
                  }
                  int_io_dev_ptr->have_stopped_input = FALSE;
               } /* Endif */
            } /* Endif */
         } /* Endif */
      } /* Endif */
   } else {
      /* indicate that tossed the character */
      return FALSE;
   } /* Endif */

   return TRUE;
   
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_nextc
* Returned Value   : int32_t, the next character to write out.
* Comments         :
*    This function returns the next character to send out, or -1 if
* no more output characters are available
*
*END*********************************************************************/

int32_t _io_serial_int_nextc
   (
      /* [IN] the interrupt I/O context information */
      io_serial_int_device_struct_t * int_io_dev_ptr
   )
{ /* Body */
   unsigned char c = 0;

   if (int_io_dev_ptr->flags & IO_SERIAL_XON_XOFF) {
      if (int_io_dev_ptr->must_stop_input) {
         int_io_dev_ptr->must_stop_input    = FALSE;
         int_io_dev_ptr->have_stopped_input = TRUE;
        return((int32_t)CNTL_S);
      } else if (int_io_dev_ptr->must_start_input) {
         int_io_dev_ptr->must_start_input   = FALSE;
         int_io_dev_ptr->have_stopped_input = FALSE;
         return((int32_t)CNTL_Q);
      } /* Endif */
   } /* Endif */

   if (int_io_dev_ptr->have_stopped_output
      || (! int_io_dev_ptr->output_enabled))
   {
      return(-1);
   } /* Endif */

   if (_CHARQ_EMPTY(int_io_dev_ptr->out_queue)) {
      /* No output */
      int_io_dev_ptr->output_enabled = FALSE;
      return(-1);
   }/* Endif */

   _CHARQ_DEQUEUE(int_io_dev_ptr->out_queue, c);
   return((int32_t)c);

} /* Endbody */

IO_DEVICE_STRUCT sci_io_dev = 
{
	 "sci0",
	
	 /*! \brief The I/O init function. */
	 _io_serial_int_open,
	
	 /*! \brief The I/O deinit function. */
	 _io_serial_int_close,
	
	 /*! \brief The I/O read function. */
	 _io_serial_int_read,
	
	 /*! \brief The I/O write function. */
	 _io_serial_int_write,
	
	 /*! \brief The I/O ioctl function. */
	 _io_serial_int_ioctl,
	
	 /*! \brief The function to call when uninstalling this device. */
	 NULL,
	
	 /*! \brief The I/O channel specific initialization data. */
	 &sci_int_dev,
	
	 /*! \brief Type of driver for this device. */
	 0,
};

file_device_struct_t sci_fdev = 
{
    /*! \brief The address of the Device for this stream. */
    &sci_io_dev,

    /*! \brief Device Driver specific information. */
    NULL,

    /*! \brief General control flags for this stream. */
    0,
    
    /*! \brief The current error for this stream. */
    0,

    /*! \brief The current position in the stream. */
    0,

    /*! \brief The current size of the file. */
    0,

    /*! \brief Undelete implementation. */
    0,
    /*! \brief Undelete implementation. */
    0,
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : serl_int_get_fdev
* Returned Value   :
* Comments         : This function returns the sci fdev handle.
*    
*
*END*----------------------------------------------------------------------*/
file_device_struct_t * serl_int_get_fdev(void)
{
	return &sci_fdev;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sci1_init
* Returned Value   :
* Comments         : This function initializes the SCI 1 baud rate.
*    
*
*END*----------------------------------------------------------------------*/
void sci_init(void) 
{
	_io_serial_int_open(&sci_fdev, "sci", (char *)(IO_SERIAL_XON_XOFF | IO_SERIAL_ECHO));
}

void sci_deinit(void)
{
	_io_serial_int_close(&sci_fdev);

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_PutChar
* Returned Value   :
* Comments         :
*                     This function sends a char via SCI.
*
*END*----------------------------------------------------------------------*/
#ifdef __CC_ARM
int32_t sendchar (int32_t ch)
#else
void uart_putchar (char ch)
#endif
{
#ifdef __CC_ARM
	_io_serial_int_write(&sci_fdev, (char*)&ch, 1);
#else
	_io_serial_int_write(&sci_fdev, &ch, 1);
#endif
#ifdef __CC_ARM
    return 0;
#endif    
}

void out_char (char ch)
{
#ifndef __CC_ARM
	uart_putchar(ch);
#else
  (int32_t)sendchar(ch);
#endif
}
/* EOF */
