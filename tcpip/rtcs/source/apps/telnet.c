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
*   This file contains the Telnet client implementation.
*
*
*END************************************************************************/

#include <rtcs.h>
#include <lwevent.h>

#define EV_INBOUND_EOF 0x00000004
#define EV_OUTBOUND_EOF  0x00000008
#define EV_TELNET_ALL (EV_INBOUND_EOF | EV_OUTBOUND_EOF)

static void TELNET_inbound_task(void *, void * creator);
static void TELNET_outbound_task(void *, void * creator);

#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#include <nio.h>
#include <nio/ioctl.h>
#define MQX_FILE_PTR FILE *
#define IO_EOF EOF
extern const NIO_DEV_FN_STRUCT _io_telnet_dev_fn;
extern const NIO_DEV_FN_STRUCT _io_socket_dev_fn;
#endif
#include "telnet.h"

typedef struct telnet_context_tag
{
  LWEVENT_STRUCT_PTR lwevent_ptr;
  MQX_FILE_PTR telnetfd;
  volatile bool b_run;
  volatile bool b_run_ack1;
  volatile bool b_run_ack2;
} TELNET_CONTEXT;

/*TASK*-----------------------------------------------------------------
*
* Function Name  : TELNET_connect
* Returned Value : error code
* Comments       : A TCP/IP based Telnet Client
*
*END------------------------------------------------------------------*/

uint32_t TELNET_connect
   (
      _ip_address    ipaddress
   )
{ /* Body */
   MQX_FILE_PTR   sockfd;
   MQX_FILE_PTR   telnetfd;
   sockaddr_in    addr;
   uint32_t        sock;
   uint32_t        error;
   //bool        work;
   //int32_t         c;
   //bool b_result;
   
   _mqx_uint telnet_ev_bits;
   _mqx_uint prio;
   TELNET_CONTEXT context = {0};
#if !MQX_USE_IO_OLD
   char dev_name[20] = {0};
   char handle_str[20] = {0};   
#endif
   LWEVENT_STRUCT telnet_lwevent;
   
   context.b_run = TRUE;
   if(MQX_OK != _lwevent_create(&telnet_lwevent, 0))
   {
     return RTCSERR_OUT_OF_MEMORY;
   }
   context.lwevent_ptr = &telnet_lwevent;

   /*
   ** Install device driver for socket and telnet
   */
#if MQX_USE_IO_OLD
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");
#else
   _nio_dev_install("socket:", &_io_socket_dev_fn, NULL);
   _nio_dev_install("telnet:", &_io_telnet_dev_fn, NULL);
#endif

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      return RTCSERR_OUT_OF_SOCKETS;
   } /* Endif */

   addr.sin_family      = AF_INET;
   addr.sin_port        = 0;
   addr.sin_addr.s_addr = INADDR_ANY;
   error = bind(sock,(const sockaddr *)&addr, sizeof(addr));
   if (error != RTCS_OK) {
      return error;
   } /* Endif */

   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = ipaddress;

   error = connect(sock, (const sockaddr *)(&addr), sizeof(addr));
   if (error != RTCS_OK) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return error;
   } /* Endif */

#if MQX_USE_IO_OLD
   sockfd = fopen("socket:", (char *)sock);
#else
   strcpy(dev_name, "socket:");
   snprintf(handle_str, 20, "%i", sock);
   strncat(dev_name, handle_str , 20-strlen(dev_name));
   sockfd = fopen(dev_name, "r+");
#endif
   if (sockfd == NULL) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

#if MQX_USE_IO_OLD
   telnetfd = fopen("telnet:", (char *)sockfd);
#else
   memset(dev_name,0,sizeof(dev_name));
   memset(handle_str,0,sizeof(handle_str));
   strcpy(dev_name, "telnet:");
   snprintf(handle_str, 20, "%i", sockfd);
   strncat(dev_name, handle_str , 20-strlen(dev_name));
   telnetfd = fopen(dev_name, "r+");
#endif
   if (telnetfd == NULL) {
      fclose(sockfd);
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */
   context.telnetfd = telnetfd;   

   /* Set the console stream to the client  */
#if MQX_USE_IO_OLD
   ioctl(telnetfd, IO_IOCTL_SET_STREAM, (uint32_t *)((void *)stdin));
#else
   ioctl(fileno(telnetfd), IO_IOCTL_SET_STREAM, (uint32_t *)((void *)stdin));
#endif
   
   /* now create two processes:
    * process one to read stdin and send read characters to socket
    * process two to read from socket and write the received chars to stdout
    */
   if(MQX_OK != _task_get_priority(MQX_NULL_TASK_ID, &prio))
   {
     return RTCSERR_INVALID_PARAMETER;
   }
   if (RTCS_task_create((char*)"telnet client outbound", prio, 
                        2000, TELNET_outbound_task, (void*)&context) 
      != RTCS_OK)
  {
    return RTCSERR_OUT_OF_MEMORY;
  }
  if (RTCS_task_create((char*)"telnet client inbound", prio, 
                        2000, TELNET_inbound_task, (void*)&context) 
      != RTCS_OK)
  {
    return RTCSERR_OUT_OF_MEMORY;
  }
  
  /* wait until either of the two TELNET processes reports EOF */
  _lwevent_wait_ticks(&telnet_lwevent, EV_TELNET_ALL, FALSE, 0);
  telnet_ev_bits = _lwevent_get_signalled();
  _lwevent_clear(&telnet_lwevent, telnet_ev_bits);
#if !MQX_USE_IO_OLD
  if(EV_INBOUND_EOF == telnet_ev_bits)
  {
    /* socket reports eof or error
     * for NIO, we need to unblock the blocking fgetc(stdin)
     * to release TELNET_outbound_task.
     */
    ioctl(fileno(stdin), IOCTL_ABORT, NULL);
  }
#endif
  context.b_run = FALSE;
  
  fclose(telnetfd);
  fclose(sockfd);
  shutdown(sock, FLAG_CLOSE_TX);
  while(!context.b_run_ack1) 
  { 
    RTCS_time_delay(1);
  }
  while(!context.b_run_ack2) 
  { 
    RTCS_time_delay(1);
  }
  _lwevent_destroy(&telnet_lwevent);
  return RTCS_OK;
} /* Endbody */

static void TELNET_inbound_task(void * context, void * creator)
{
  MQX_FILE_PTR telnetfd = ((TELNET_CONTEXT *)context)->telnetfd;
  bool b_result = TRUE;
  int32_t c;
  RTCS_task_resume_creator(creator, RTCS_OK);
  
  while(((TELNET_CONTEXT *)context)->b_run)
  {
#if MQX_USE_IO_OLD  
    /* old IO (fio.h) fstatus()
     */
    b_result = fstatus(telnetfd);
#endif
    if(b_result) 
    {
      c = (int32_t)fgetc(telnetfd);
      if(c == IO_EOF) 
      {
        break;
      }
      fputc(c & 0x7F, stdout);
    }
    else
    {
      /* this executes only for old IO. 
       * for NIO we expect fgetc() is blocking
       */
      RTCS_time_delay(1);
    }
  }
  _lwevent_set(((TELNET_CONTEXT *)context)->lwevent_ptr ,EV_INBOUND_EOF);
  ((TELNET_CONTEXT *)context)->b_run_ack1 = TRUE;
}

static void TELNET_outbound_task(void * context, void * creator)
{
  MQX_FILE_PTR telnetfd = ((TELNET_CONTEXT *)context)->telnetfd;
  bool b_result = TRUE;
  int32_t c;
  RTCS_task_resume_creator(creator, RTCS_OK);

  while(((TELNET_CONTEXT *)context)->b_run)
  {
#if MQX_USE_IO_OLD  
    /* old IO (fio.h) stdin might be polled UART driver
     * thus only for this case we check if a character is available.
     * is there is no character, sleep for a tick
     */
    b_result = fstatus(stdin);
#endif
    if(b_result) 
    {
      c = (int32_t)fgetc(stdin);
#if !MQX_USE_IO_OLD
      if(EOF == c)
      {
        clearerr(stdin);
        break;
      }
#endif
      if (fputc(c & 0x7F, telnetfd) == IO_EOF)  
      {
        break;   
      }
    }
    else
    {
      /* this executes only for old IO uart driver. 
       * for NIO tty, fgetc() above is blocking function.
       */
      RTCS_time_delay(1);
    }
  }
  _lwevent_set(((TELNET_CONTEXT *)context)->lwevent_ptr ,EV_OUTBOUND_EOF);
  ((TELNET_CONTEXT *)context)->b_run_ack2 = TRUE;
}
