/*HEADER**********************************************************************
*
* Copyright 2008, 2014 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the implementation of an MQX
*   socket I/O device.
*
*
*END************************************************************************/

#include <rtcs.h>

#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <nio.h>
#include <nio/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#endif
#define SOCKIO_MIN_BUFFER 64
#define SOCKIO_MAX_BUFFER (64*1024)

typedef struct io_socket {
   uint32_t  SOCKET;
   uint32_t  COUNT;
   uint32_t  SIZE;
   char *BUFFER;
} IO_SOCKET, * IO_SOCKET_PTR;

#if MQX_USE_IO_OLD
_mqx_int _io_socket_open  (MQX_FILE_PTR, char *,  char *);
_mqx_int _io_socket_close (MQX_FILE_PTR);
_mqx_int _io_socket_read  (MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_socket_write (MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_socket_ioctl (MQX_FILE_PTR, _mqx_uint, void *);
#else // !MQX_USE_IO_OLD
static int _io_socket_open(void *dev_context, const char *dev_name, int flags, void **fp_context);
static int _io_socket_read(void *dev_context, void *fp_context, void *buf, size_t nbytes);
static int _io_socket_write(void *dev_context, void *fp_context, const void *buf, size_t nbytes);
static int _io_socket_ioctl(void *dev_context, void *fp_context, unsigned long int request, va_list ap);
static int _io_socket_close(void *dev_context, void *fp_context);
static int _io_socket_init(void *init_data, void **dev_context);
static int _io_socket_deinit(void *dev_context);

const NIO_DEV_FN_STRUCT _io_socket_dev_fn = {
    .OPEN = _io_socket_open,
    .READ = _io_socket_read,
    .WRITE = _io_socket_write,
    .LSEEK = NULL,
    .IOCTL = _io_socket_ioctl,
    .CLOSE = _io_socket_close,
    .INIT = _io_socket_init,
    .DEINIT = _io_socket_deinit,
};
#endif

#if MQX_USE_IO_OLD
/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_install
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

int32_t _io_socket_install
   (
      char *identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_socket_open,
                          _io_socket_close,
                          _io_socket_read,
                          _io_socket_write,
#if MQX_VERSION < 250
                          (int32_t(_CODE_PTR_)(MQX_FILE_PTR, uint32_t, uint32_t *))
#endif
                          _io_socket_ioctl,
                          NULL );
} /* Endbody */
#endif //MQX_USE_IO_OLD


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_open
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_socket_open
   (
      MQX_FILE_PTR fd_ptr,
      char   *open_name_ptr,
      char   *flags_ptr
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = RTCS_mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
   
   _mem_set_type(io_ptr, MEM_TYPE_IO_SOCKET);
   
   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->SOCKET = (uint32_t)flags_ptr;

   return MQX_OK;

} /* Endbody */
#else // !MQX_USE_IO_OLD
static int _io_socket_open(void *dev_context, const char *dev_name, 
                           int flags, void **fp_context) 
{
  IO_SOCKET * devc;
  const char * sock_handle_str = NULL;
  
  devc = RTCS_mem_alloc_zero(sizeof(*devc));   
  if(devc == NULL) 
  {
    errno = ENOMEM;
    return -1;
  }
  *fp_context = (void*)devc;
   
  _mem_set_type(devc, MEM_TYPE_IO_SOCKET);
  
  /* expected dev_name: "socket:SOCKET_HANDLE" 
   * where SOCKET_HANDLE is decimal string of socket handle */
  sock_handle_str = strchr(dev_name, ':');
  if(NULL == sock_handle_str)
  {
    errno = EBADF;
    return -1;
  }
  sock_handle_str++;
  devc->SOCKET = (uint32_t)strtoul(sock_handle_str, NULL , 10);
  return 0;
}
#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_close
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_socket_close
   (
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */

   IO_SOCKET_PTR  io_ptr;


   if (fd_ptr->DEV_DATA_PTR != NULL) 
   {
      io_ptr = fd_ptr->DEV_DATA_PTR;
      if (io_ptr->BUFFER != NULL) 
      {
         _mem_free(io_ptr->BUFFER);
      }
      fd_ptr->DEV_DATA_PTR = NULL;
      _mem_free(io_ptr);
   }
   return MQX_OK;

} 
#else
static int _io_socket_close(void *dev_context, void *fp_context) 
{
  IO_SOCKET * devc = (IO_SOCKET*)fp_context;
  if(devc->BUFFER)
  {
    _mem_free(devc->BUFFER);
  }  
  _mem_free(fp_context);
  return 0;
}
#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_read
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

#if MQX_USE_IO_OLD
_mqx_int _io_socket_read
   (
      MQX_FILE_PTR fd_ptr,
      char   *data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
#else // !MQX_USE_IO_OLD
static int _io_socket_read(void *dev_context, void *fp_context, void *data_ptr, size_t num) {
    //IO_SOCKET *io_ptr = (IO_SOCKET*)dev_context;
    IO_SOCKET * io_ptr = (IO_SOCKET*)fp_context;
    //IO_SOCKET_FP_CONTEXT_STRUCT *fpc = (IO_SOCKET_FP_CONTEXT_STRUCT*)fp_context;
#endif
   if (io_ptr != NULL) {
   
   // flush any pending data before reading
   if (io_ptr->COUNT) {
      send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
      io_ptr->COUNT = 0;
   }

     return (_mqx_int)recv(io_ptr->SOCKET, data_ptr, num, 0);
   } 
   return -1;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_set_send_push
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/
static _mqx_int _io_socket_set_send_push( uint32_t sock, bool value)
{
   return setsockopt(sock, SOL_TCP, OPT_SEND_PUSH, &value, sizeof(value));
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_write
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_socket_write
   (
      MQX_FILE_PTR fd_ptr,
      char   *data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr = fd_ptr->DEV_DATA_PTR;
   _mqx_int      sent = IO_ERROR;
#else
static int _io_socket_write(void *dev_context, void *fp_context, const void *data_ptr, size_t num) {
    //IO_SOCKET *io_ptr = (IO_SOCKET*)dev_context;
    IO_SOCKET * io_ptr = (IO_SOCKET*)fp_context;
    //NPIPE_FP_CONTEXT_STRUCT *fpc = (NPIPE_FP_CONTEXT_STRUCT*)fp_context;
    _mqx_int      sent = -1;
#endif
   

   if (io_ptr != NULL) {
      if ( io_ptr->SIZE ) {
         if  (io_ptr->COUNT+num > io_ptr->SIZE) {
            
            if (io_ptr->COUNT) {
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT = 0;
            }
            sent = (_mqx_int)send(io_ptr->SOCKET, (void*)data_ptr, num, 0);

         } else if (num==1) {
            // most writes are 1 byte.
            io_ptr->BUFFER[io_ptr->COUNT++] = *(char*)data_ptr;
            
            sent=1;
            
            if (io_ptr->COUNT == io_ptr->SIZE) {
               // buffer is full, flush it
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT=0;
            }
            
         } else {
            _mem_copy(data_ptr,&io_ptr->BUFFER[io_ptr->COUNT],num);
            io_ptr->COUNT+=num;
            sent=num;
         }
      } else {     
         sent = (_mqx_int)send(io_ptr->SOCKET, (void*)data_ptr, num, 0);
      } 
   }

   return sent;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_ioctl
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_socket_ioctl
   (
      MQX_FILE_PTR fd_ptr,
      _mqx_uint   cmd,
      void       *param_ptr
   )
{ /* Body */
   IO_SOCKET_PTR  io_ptr = fd_ptr->DEV_DATA_PTR;
   _mqx_int       result = IO_ERROR;
#else
#define IO_ERROR (-1)
static int _io_socket_ioctl(void *dev_context, void *fp_context, unsigned long int cmd, va_list ap) {
    //IO_SOCKET *io_ptr = (IO_SOCKET*)dev_context;
  IO_SOCKET * io_ptr = (IO_SOCKET*)fp_context;
    _mqx_int       result = IO_ERROR;
    void * param_ptr;    
    param_ptr = va_arg(ap, void*);    
#endif
   
   
   if (io_ptr != NULL) 
   {
      _mqx_int       bytes;
      uint32_t       size;
      rtcs_fd_set    rfds;
      switch (cmd) {
         case IO_IOCTL_CHAR_AVAIL:
            RTCS_FD_ZERO(&rfds);
            RTCS_FD_SET(io_ptr->SOCKET, &rfds);

            if(1 == select(1, &rfds, NULL, 0xFFFFFFFF)) /* last parameter = poll mode */
            {
              *(bool *)param_ptr = TRUE;
            } else {
              *(bool *)param_ptr = FALSE;
            } 
            result=MQX_OK;
            break;
      
         case IO_IOCTL_FLUSH_OUTPUT:
            if (io_ptr->COUNT) {
               bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               if (bytes==io_ptr->COUNT) {
                  io_ptr->COUNT = 0;
                  result = MQX_OK;
               }
            } else {
               result = MQX_OK;
            }
            break;
            
         case IO_IOCTL_SET_BLOCK_MODE:
            size = *(uint32_t *)param_ptr;
            result=MQX_OK;
            if (size != io_ptr->SIZE) {
               // First, clear out the old buffer
               if (io_ptr->BUFFER) {
                  if (io_ptr->COUNT) {
                     bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
                     io_ptr->COUNT = 0;
                  }
                  _mem_free(io_ptr->BUFFER);
                  io_ptr->BUFFER = NULL;
               }
            
               if (size) {
                  // Size is set, entering block mode
                  if (size < SOCKIO_MIN_BUFFER) {
                     size = SOCKIO_MIN_BUFFER;
                  }
                  if (size > SOCKIO_MAX_BUFFER) {
                     size = SOCKIO_MAX_BUFFER;
                  }

                  io_ptr->BUFFER = RTCS_mem_alloc(size);
                  if (io_ptr->BUFFER==NULL) {
                     size = 0;
                     result = IO_ERROR;
                  } else {
                     _io_socket_set_send_push(io_ptr->SOCKET, TRUE);
                  }
                  
               }                     

               io_ptr->SIZE = size;
            }
            break;      
            
         case IO_IOCTL_GET_BLOCK_SIZE:
            result=MQX_OK;
            *(bool *)param_ptr = io_ptr->SIZE;
            break;
            
       }      
   }
   return result;

} 

#if !MQX_USE_IO_OLD
static int _io_socket_init(void *init_data, void **dev_context) {
  (*dev_context) = NULL; /* dummy value - not used */
  //NPIPE_INIT_DATA_STRUCT *init = (NPIPE_INIT_DATA_STRUCT*)init_data;    
   
   return 0;
}

static int _io_socket_deinit(void *dev_context) {
#if 0
    IO_SOCKET *devc = dev_context;

    if(devc->BUFFER)
    {
      _mem_free(devc->BUFFER);
    }
    _mem_free(dev_context);
#endif
    return 0;
}
#endif // !MQX_USE_IO_OLD

