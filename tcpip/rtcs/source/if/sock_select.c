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
* $FileName: sock_select.c$
* Comments:
*
*   This file contains the select() implementation.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "tcp_prv.h"    /* for TCP internal definitions */
#include "udp_prv.h"    /* for UDP internal definitions */
#include "socket.h"

/* definitions local for this source file */
#define SOCK_ACTIVITY_NONE              0
#define SOCK_ACTIVITY_ON_READ_FDS       1
#define SOCK_ACTIVITY_ON_WRITE_FDS      2
#define SOCK_ACTIVITY_ON_READWRITE_FDS  (SOCK_ACTIVITY_ON_READ_FDS | SOCK_ACTIVITY_ON_WRITE_FDS)

/* forward declarations for static functions */
static void SOCK_poll_fds(SOCK_SELECT_PARM_PTR);
static bool SOCK_select_activity (SOCKET_STRUCT_PTR, int32_t);
static void SOCK_select_block    (SOCK_SELECT_PARM_PTR);
static bool SOCK_select_expire   (TCPIP_EVENT_PTR);
static void SOCK_select_unblock  (SOCK_SELECT_PARM_PTR, uint16_t);

#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1
static void SOCK_build_fds(SOCK_SELECT_PARM_PTR);
#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_activity
* Returned Value  : TRUE or FALSE
* Comments        : Determine whether there is any activity on a socket.
*
*END*-----------------------------------------------------------------*/

static bool SOCK_select_activity
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      int32_t flag                          // direction flag - 0 recv, non zero send
   )
{
   TCB_STRUCT_PTR tcb_ptr;
   bool        activity = FALSE;

   /* Check closing and connected stream sockets for data */
   if (socket_ptr->TCB_PTR)
   {
      tcb_ptr = socket_ptr->TCB_PTR;
   
      if (!flag)
      {
         /*
         ** Check that there is data in the receive ring or
         ** that the socket has been closed by the peer
         */
         if ((tcb_ptr->conn_pending) || (tcb_ptr->state == CLOSED) || (GT32(tcb_ptr->rcvnxt, tcb_ptr->rcvbufseq)))
         {
            activity = TRUE;
         }
      }
      else
      {
        if(!tcb_ptr->sndlen) // TODO - remake for partialy empty send buffers
         {    
            activity = TRUE;
         }
      }

   /* Check datagram sockets for data */
   } 
   else if (socket_ptr->UCB_PTR)
   {
     if(!flag)
     {
        /*
        ** Check that there is queued data
        */
        if(socket_ptr->UCB_PTR->PHEAD)
        {
           activity = TRUE;
        } /* Endif */
     }
     else
     {
       activity = TRUE; /* for now assume we can always UDP send. */
     }

   /* TCB=UCB=NULL is a TCP connection reset by peer */
   }
   else
   {
     activity = TRUE;
   }

   return activity;

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_block
* Returned Values :
* Comments  :
*     Enqueues a select() call on the select queue.
*
*END*-----------------------------------------------------------------*/

static void SOCK_select_block
   (
      SOCK_SELECT_PARM_PTR  parms
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);

   parms->NEXT = socket_cfg_ptr->SELECT_HEAD;
   if (parms->NEXT) {
      parms->NEXT->PREV = &parms->NEXT;
   } /* Endif */
   socket_cfg_ptr->SELECT_HEAD = parms;
   parms->PREV = &socket_cfg_ptr->SELECT_HEAD;

   if (parms->timeout) {
      parms->EXPIRE.TIME    = parms->timeout;
      parms->EXPIRE.EVENT   = SOCK_select_expire;
      parms->EXPIRE.PRIVATE = parms;
      TCPIP_Event_add(&parms->EXPIRE);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_expire
* Returned Values :
* Comments  :
*     Called by the Timer.  Expire a select call.
*
*END*-----------------------------------------------------------------*/

static bool SOCK_select_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   SOCK_SELECT_PARM_PTR  parms = event->PRIVATE;

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

  parms->sock = 0;
  if(NULL != parms->readfds)
  {
    RTCS_FD_ZERO(parms->readfds);
  }
  if(NULL != parms->writefds)
  {
    RTCS_FD_ZERO(parms->writefds);
  }
  RTCSCMD_complete(parms, RTCS_OK);

  return FALSE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_unblock
* Returned Values :
* Comments  :
*     Dequeues a select() call from the select queue.
*
*END*-----------------------------------------------------------------*/

static void SOCK_select_unblock
   (
      SOCK_SELECT_PARM_PTR  parms,
      uint16_t  state      // TCP state or UDP socket flag
   )
{

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } 

   *parms->PREV = parms->NEXT;

   if (parms->timeout)
   {
      TCPIP_Event_cancel(&parms->EXPIRE);
   } 

   /* Check TCP state and UDP socket flag */
   if ( (state == CLOSED) || (state == UDP_SOCKET_CLOSE) )
   {
      RTCSCMD_complete(parms, RTCSERR_SOCK_CLOSED);
   } 
   else
   {
      RTCSCMD_complete(parms, RTCS_OK);
   }
    

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_signal
* Returned Values :
* Comments  :
*     Unblocks all select() calls blocked on a specific socket.
*
*END*-----------------------------------------------------------------*/

void _SOCK_select_signal
   (
      uint32_t  sock,
      uint16_t  state,      // TCP state or UDP socket flag
      uint32_t  flag        // direction flag - 0 recv, non zero send
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCK_SELECT_PARM_PTR       parms;
   SOCK_SELECT_PARM_PTR       nextparms;
   uint32_t                   i;
   uint32_t                   size;
   uint32_t                   poll_act_mask = SOCK_ACTIVITY_NONE;

   if (!sock)
   {
      return;
   } /* Endif */

   for (parms = socket_cfg_ptr->SELECT_HEAD; parms; parms = nextparms)
   {
      nextparms = parms->NEXT;
      poll_act_mask = SOCK_ACTIVITY_NONE;

      if((flag)&&(NULL != parms->writefds))
      {
        size = parms->sock_count;
        for(i=0; (i < RTCSCFG_FD_SETSIZE) && size ;i++)
        {
          if(parms->writefds->fd_array[i])
          {
            if(parms->writefds->fd_array[i] == sock)
            {
              RTCS_FD_ZERO(parms->writefds);
              RTCS_FD_SET(sock,parms->writefds);
              poll_act_mask = SOCK_ACTIVITY_ON_WRITE_FDS;
              parms->sock++;
              break;
            }
            size--; /* decrease with each non-zero socket handle */
          }          
        } /* Endfor */
      } /* Endif */
      
      if((!flag)&&(NULL != parms->readfds))
      {
        size = parms->sock_count;
        for(i=0; (i < RTCSCFG_FD_SETSIZE) && size ;i++)
        {
          if(parms->readfds->fd_array[i])
          {
            if(parms->readfds->fd_array[i] == sock)
            {
              RTCS_FD_ZERO(parms->readfds);
              RTCS_FD_SET(sock,parms->readfds);
              poll_act_mask |= SOCK_ACTIVITY_ON_READ_FDS;
              parms->sock++;
              break;
            }
            size--; /* decrease with each non-zero socket handle */
          }
        }
      }
      /*
       * poll_act_mask values
       * SOCK_ACTIVITY_NONE means no activity
       * SOCK_ACTIVITY_ON_READ_FDS means activity on readfds
       * SOCK_ACTIVITY_ON_WRITE_FDS means activity on writefds
       * SOCK_ACTIVITY_ON_READWRITE_FDS means activity on both readfds and writefds
       */
      if((NULL != parms->writefds)&&(SOCK_ACTIVITY_ON_READ_FDS==poll_act_mask))
      {
        RTCS_FD_ZERO(parms->writefds);
      }
      if((NULL != parms->readfds)&&(SOCK_ACTIVITY_ON_WRITE_FDS==poll_act_mask))
      { 
        RTCS_FD_ZERO(parms->readfds);
      }
      if(poll_act_mask)
      {
        SOCK_select_unblock(parms, state);
      }
   } /* Endfor */

   /*
   ** Finally, call the application callback, if one exists.
   */
   if (((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK)
   {
      ((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK(sock);
   } /* Endif */

} /* Endbody */


#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_selectset
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.
*
*END*-----------------------------------------------------------------*/
uint32_t RTCS_selectset(void *sockset, uint32_t size, uint32_t timeout)
{
  int32_t i;
  rtcs_fd_set rfds;
  /* fd_set wfds; */
  int32_t retval;
  
  RTCS_FD_ZERO(&rfds);
  /* FD_ZERO(&wfds); */
  for(i=0;i<size;i++)
  {
    RTCS_FD_SET(*(((uint32_t*)sockset)+i),&rfds);
    /* FD_SET(*(((uint32_t*)sockset)+i),&wfds); */
  }
  /* retval = select(size,&rfds,&wfds,NULL,timeout); */
  /* with select() we can (if we wish) monitor only receive activity: example on the line below */
  retval = select(size,&rfds,NULL,timeout);
  if(RTCS_ERROR == retval)
  {
    return RTCS_SOCKET_ERROR;
  }
  else if(0 == retval)
  {
    return 0; /* timeout expired */
  }
  else 
  { /* at least of of the rfds/wfds should be valid. take priority on read activity. */
    for(i=0;i<size;i++)
    {
      if(RTCS_FD_ISSET(*(((uint32_t*)sockset)+i),&rfds))
      {
        return *(((uint32_t*)sockset)+i);
      }
    }
    return 0;
  }
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_selectall
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/
uint32_t RTCS_selectall
   (
      uint32_t     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCK_SELECT_PARM   parms;
   uint32_t           error;
   rtcs_fd_set rfds;

   RTCS_FD_ZERO(&rfds);

   parms.owner   = RTCS_task_getid();
   parms.timeout = timeout;
   parms.readfds = &rfds;
   /* create socket set from all existing sockets */
   error = RTCSCMD_issue(parms, SOCK_build_fds);
   if (error) return RTCS_SOCKET_ERROR;
   
   return RTCS_selectset(rfds.fd_array, rfds.fd_count,timeout);
  
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_build_fds
* Parameters      :
*
*     SOCK_SELECT_PARM_PTR      p_param       [IN] pointer to SOCKSELECT_PARM
*
* Comments  : 
*   Cycle through all sockets in the system and build fd_set from the socket handles.
*   This is only to support RTCS_selectall() for backward compatibility.
*   Expected to be called by TCP/IP task.
*
*END*-----------------------------------------------------------------*/
static void SOCK_build_fds(SOCK_SELECT_PARM_PTR p_param)
{
  SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
  SOCKET_STRUCT_PTR          socket_ptr;
  
  /* cycle through sockets looking for one owned by this task */
  for (socket_ptr = socket_cfg_ptr->SOCKET_HEAD;
      socket_ptr;
      socket_ptr = socket_ptr->NEXT) 
  {
    /* check owner only if backward compatibility is configured. */
    /* this is specific for compatibility with RTCSCFG_SOCKET_OWNERSHIP */
    /* of RTCS_selectall() function. */
    if(SOCK_Is_owner(socket_ptr, p_param->owner))
    {
      RTCS_FD_SET((uint32_t)socket_ptr,p_param->readfds);
    }
  } /* Endfor */
  RTCSCMD_complete(p_param, RTCS_OK);
}
#endif /* RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : FD_SET
* Parameters      :
*
*     uint32_t      sock       [IN] pointer to SOCKET_STRUCT
*     fd_set        *p_fd_set  [IN] pointer to fd_set
*
* Comments  :  Add socket handle to the array pointed to by p_fd_set.
*   Insert sock to first non-zero array member.
*
*END*-----------------------------------------------------------------*/
void RTCS_FD_SET(const uint32_t sock, rtcs_fd_set * const p_fd_set)
{
  int32_t i;
  if(0==sock) return; /* socket handle is NULL */
  if(NULL == p_fd_set) return;
  /* if there is an empty place in the fd_set */  
  if(p_fd_set->fd_count<RTCSCFG_FD_SETSIZE)
  {
    /* search for the 1st empty place */
    for(i=0;i<RTCSCFG_FD_SETSIZE;i++)
    {
      if(0==p_fd_set->fd_array[i]) /* empty place found */ 
      {
        p_fd_set->fd_array[i] = sock;
        p_fd_set->fd_count++;
        break;
      }
    }
  }
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : FD_CLR
* Parameters      :
*
*     uint32_t      sock       [IN] pointer to SOCKET_STRUCT
*     fd_set        *p_fd_set  [IN] pointer to fd_set
*
* Comments  :  Remove socket handle from the array pointed to by p_fd_set.
*  Clear the array member which equals to sock.
*
*END*-----------------------------------------------------------------*/
void RTCS_FD_CLR(const uint32_t sock, rtcs_fd_set * const p_fd_set)
{
  int32_t i;
  if(0==sock) return; /* socket handle is NULL */
  if(NULL == p_fd_set) return;
  /* if there is at least one socket in the fd_set */  
  if(p_fd_set->fd_count>0)
  {
    /* search for the one that needs to be cleared */
    for(i=0;i<RTCSCFG_FD_SETSIZE;i++)
    {
      if(sock==p_fd_set->fd_array[i])
      {
        p_fd_set->fd_array[i] = 0;
        p_fd_set->fd_count--;
        break;
      }
    }
  }
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : FD_ZERO
* Parameters      :
*
*     fd_set        *p_fd_set  [IN] pointer to fd_set
*
* Comments  :  Clear out the array
*
*END*-----------------------------------------------------------------*/
void RTCS_FD_ZERO(rtcs_fd_set * const p_fd_set)
{
  int32_t i;
  if(NULL == p_fd_set) return;
  //for(i=0;i<p_fd_set->fd_count;i++)
  for(i=0;i<(sizeof(p_fd_set->fd_array)/sizeof(p_fd_set->fd_array[0]));i++)
  {
    p_fd_set->fd_array[i] = 0;
  }
  p_fd_set->fd_count=0;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : FD_ISSET
* Parameters      :
*
*     uint32_t      sock       [IN] pointer to SOCKET_STRUCT
*     fd_set        *p_fd_set  [IN] pointer to fd_set
*
* Comments  :  check if sock is the member of fd_set array
*
*END*-----------------------------------------------------------------*/
bool RTCS_FD_ISSET(const uint32_t sock, const rtcs_fd_set * const p_fd_set)
{
  int32_t i;
  if(NULL == p_fd_set) return FALSE;
  if(0==sock) return FALSE;
  if(RTCS_SOCKET_ERROR == sock) return FALSE;
  for(i=0;i<RTCSCFG_FD_SETSIZE;i++)
  {
    if(sock==(p_fd_set->fd_array[i]))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : select
* Parameters      :
*
*     int32_t      nfds       [IN] The first nfds socket handles 
*                                  are checked in each set; i.e., 
*                                  the sockets from 0 through nfds-1 
*                                  in the descriptor sets are examined.
*     fd_set *     readfds   [IN/OUT]
*       IN - array of pointers to SOCKET_STRUCT to examine for receive activity
*       OUT - readfds contains the socket handles where activity 
*           has been detected
*     fd_set *     writefds  [IN/OUT]
*       IN - array of pointers to SOCKET_STRUCT to examine for transmit activity
*       OUT - writefds contains the socket handles where activity 
*           has been detected
*     uint32_t     timeout_ms [IN]
*       if timeout_ms is zero, select() may block indefinitely.
*       if timeout_ms is 0xFFFFFFFF, select() only polls the socket descriptors
*         and returns when the actual status is determined.
*       other values determine time limit in milliseconds that the select
*         would block at max
*       

* Comments  :  Due to the restrict keyword the fd_sets must not overlap;
*  readfds, writefds shall not point to the same fd_set structure 
*  in memory.
*  
*  The select() function returns the number of ready sockets that are
*  contained in the descriptor sets, or RTCS_ERROR if an error occurred
*  (RTCS_errno is set appropriately).If the time limit expires, select() returns 0.  
*  
*  Any of readfds, writefds may be given as null pointers if
*  no descriptors are of interest.
*  
*  select() function modifies the content of readfds/writefds.
*
*  SPECIAL NOTES:
*  datagram socket in writefds is reported as always writeable.
*
*END*-----------------------------------------------------------------*/
int32_t select(int32_t nfds, 
                rtcs_fd_set *restrict readfds,
                rtcs_fd_set *restrict writefds,
                uint32_t timeout_ms)
{
  SOCK_SELECT_PARM   parms;
  uint32_t error;
  
  /* socket descriptors have to be checked by TCP/IP task */
  parms.readfds = readfds;
  parms.writefds = writefds;
  /* greater value from nfds and RTCSCFG_FD_SETSIZE */
  if(nfds<0)
  {
    return RTCSERR_SOCK_EINVAL;
  }
  
  if(RTCSCFG_FD_SETSIZE>nfds)
  {
    parms.sock_count = nfds; 
  }
  else
  {
    parms.sock_count = RTCSCFG_FD_SETSIZE;
  }
  
  parms.timeout    = timeout_ms;
  
  parms.sock = 0; /* will be used by SOCK_poll_fds to inform caller about activity */
  if(RTCS_OK != (error = RTCSCMD_issue(parms, SOCK_poll_fds)))
  {
    _task_set_error(error); /* set RTCS_errno */
    return RTCS_ERROR;
  }
  return parms.sock;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : sock_exists_in_system
* Parameters      :
*
*     uint32_t      sock       [IN] pointer to SOCKET_STRUCT
*     SOCKET_CONFIG_STRUCT_PTR        socket_cfg_ptr  [IN] pointer to socket configuration structure in system
*
* Comments  :  returns TRUE if given sock is found in the system sockets. returns FALSE otherwise.
*   
*   Expected to be called internally in this module - from TCP/IP task.
*
*END*-----------------------------------------------------------------*/
static bool sock_exists_in_system(uint32_t sock, SOCKET_CONFIG_STRUCT_PTR socket_cfg_ptr)
{
  bool b_is_found = FALSE;
  SOCKET_STRUCT_PTR socket_ptr;
  
  for (socket_ptr = socket_cfg_ptr->SOCKET_HEAD; socket_ptr; socket_ptr = socket_ptr->NEXT)
  {
    if((uint32_t)socket_ptr == sock)
    {
      b_is_found = TRUE;
      break;
    }
  }
  return b_is_found;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_correct_fds
* Parameters      :
*
*     SOCK_SELECT_PARM_PTR      p_param       [IN] pointer to SOCKSELECT_PARM
*
* Comments  :
*   Cycle through all sockets in the system and build fd_set from the socket handles.

*   Expected to be called by TCP/IP task.
*
*   Expecting p_param->readfds and p_param->writefds as input fds.
*   Content of both will be checked against invalid values (0, RTCS_SOCKET_ERR
*   and that the sockets really exist in the system).
*   
*   Non-existing socket handles are cleared out in the input arrays.
*   
*   EBADF will be returned if an invalid socket is found in either fd_set.
*
*END*-----------------------------------------------------------------*/
static uint32_t SOCK_correct_fds(SOCK_SELECT_PARM_PTR p_param)
{
  SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
  int32_t i = 0;
  uint32_t nonzero_sockcnt = 0;
  uint32_t ret_code = RTCS_OK;

  if(NULL != p_param->readfds)
  {
    nonzero_sockcnt = 0;
    for(i=0;i<sizeof(p_param->readfds->fd_array)/sizeof(p_param->readfds->fd_array[0]);i++)
    {
      if(0 != p_param->readfds->fd_array[i])
      {
        nonzero_sockcnt++;
        if(FALSE == sock_exists_in_system(p_param->readfds->fd_array[i], socket_cfg_ptr))
        {
          RTCS_FD_CLR(p_param->readfds->fd_array[i],p_param->readfds);
          ret_code = RTCSERR_SOCK_EBADF;
        }  
      }
    }
    /* catch fd_set will all zeroes or a missing sock */
    if((nonzero_sockcnt==0)||(nonzero_sockcnt<p_param->readfds->fd_count))
    {
      ret_code = RTCSERR_SOCK_EBADF;
    }    
  }

  if(NULL != p_param->writefds)
  {
      nonzero_sockcnt = 0;
      for(i=0;i<sizeof(p_param->writefds->fd_array)/sizeof(p_param->writefds->fd_array[0]);i++)
      {
        if(0 != p_param->writefds->fd_array[i])
        {
          nonzero_sockcnt++;
          if(FALSE == sock_exists_in_system(p_param->writefds->fd_array[i],socket_cfg_ptr))
          {
            RTCS_FD_CLR(p_param->writefds->fd_array[i],p_param->writefds);
            ret_code = RTCSERR_SOCK_EBADF;
          }  
        }
      }
      /* catch fd_set will all zeroes or a missing sock */
      if((nonzero_sockcnt==0)||(nonzero_sockcnt<p_param->writefds->fd_count))
      {
        ret_code = RTCSERR_SOCK_EBADF;
      }
  }
  
  return ret_code;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_poll_fds
* Parameters      :
*
*     uint32_t      sock       [IN] pointer to SOCKET_STRUCT
*     fd_set        *p_fd_set  [IN] pointer to fd_set
*
* Comments  :  Poll socket descriptors for activity.
*   If no activity is detected during poll, block the caller
*   until activity is signalled by TCP/UDP processes.
*   (may block if SOCK_select_block followed by return is called)
*   
*   Expected to be called by TCP/IP task.
*
*END*-----------------------------------------------------------------*/
static void SOCK_poll_fds(SOCK_SELECT_PARM_PTR p_parms)
{
    uint32_t                   size;
    uint32_t                   *sock_ptr;
    uint32_t                   i = 0;
    uint32_t                   poll_act_mask = SOCK_ACTIVITY_NONE;
    
    rtcs_fd_set temp_fds; /* temp storage for socket handles to output */

    /*
     * run auto correction function for input socket fd_sets.
     * it will clear out (zero) all socket handles that are invalid
     * invalidity is checked by function FD_ISSET
     * it means the handle is not RTCS_SOCKET_ERROR and that the socket exists in the system
     */
    if(RTCS_OK != SOCK_correct_fds(p_parms)) 
    {
      RTCSCMD_complete(p_parms, RTCSERR_SOCK_EBADF);
      return;
    }
        
    /* poll socket descriptors for a read activity */
    if(NULL != p_parms->readfds)
    {
      sock_ptr = p_parms->readfds->fd_array;
      size = p_parms->sock_count;
      RTCS_FD_ZERO(&temp_fds);
      for (i = 0; (i < RTCSCFG_FD_SETSIZE) && size; i++) 
      {
        if(*(sock_ptr+i) != 0) /* may be NULL (by incorrect user input or by SOCK_correct_fds) */
        {
          /* pass 0 to SOCK_select_activity last parameter - for read */
          if (SOCK_select_activity((SOCKET_STRUCT_PTR) *(sock_ptr+i), 0))
          {
            RTCS_FD_SET(*(sock_ptr+i),&temp_fds);
            poll_act_mask = SOCK_ACTIVITY_ON_READ_FDS; /* save info that read activity was detected */
            p_parms->sock++; /* number of sockets with activity */
          }
          size--; /* decrease with each non-zero socket */
        }        
      }

      if(poll_act_mask&SOCK_ACTIVITY_ON_READ_FDS)
      {
        RTCS_FD_ZERO(p_parms->readfds);
        for(i=0;i<temp_fds.fd_count;i++)
        {
          RTCS_FD_SET(temp_fds.fd_array[i],p_parms->readfds);
        }
      }

    }
    /* Poll for write activity as RTCS_selectall and RTCS_selectset wait for Rx activity only */
//#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==0
    /* Poll socket descriptors for a write activity.
     * If backward compatibility with RTCS_selectset() and RTCS_selectall() is on,
     * these non-BSD implementation, write activity was not checked here in poll function,
     * so only _SOCK_select_signal() was used.
     * particularly because of listening stream socket. as after socket(), the socket is writeable
     * tranmsit buffer is empty, this code would return writeable activity to listening socket
     * so in the call sequnce: socket()/listen(), RTCS_selectall or RTCS_selectset, accept(), the RTCS_select
     * would return with the listening socket handle.
     * and subsequent accept() would block or fail
     */
    if(NULL != p_parms->writefds)
    {
      sock_ptr = p_parms->writefds->fd_array;
      size = p_parms->sock_count;
      RTCS_FD_ZERO(&temp_fds);
      for (i = 0; (i < RTCSCFG_FD_SETSIZE) && size; i++) 
      {
         if(*(sock_ptr+i) != 0) /* may be NULL (by incorrect user input or by SOCK_correct_fds) */
         {
           /* pass 1 to SOCK_select_activity last parameter */
            if (SOCK_select_activity((SOCKET_STRUCT_PTR) *(sock_ptr+i), 1))
            { 
              poll_act_mask |= SOCK_ACTIVITY_ON_WRITE_FDS;
              p_parms->sock++;
              RTCS_FD_SET(*(sock_ptr+i),&temp_fds);
            }
            size--; /* decrease with each non-zero socket */
         }
      }
      

      if(poll_act_mask&SOCK_ACTIVITY_ON_WRITE_FDS)
      {
        RTCS_FD_ZERO(p_parms->writefds);
        for(i=0;i<temp_fds.fd_count;i++)
        {
          RTCS_FD_SET(temp_fds.fd_array[i],p_parms->writefds);
        }
      }
    }
//#endif

    /*
     * poll_act_mask values:
     * SOCK_ACTIVITY_NONE means no activity, readfds and writefds have not been changed.
     * SOCK_ACTIVITY_ON_READ_FDS means activity on readfds; writefds has not been changed.
     * SOCK_ACTIVITY_ON_WRITE_FDS means activity on writefds; readfds has not been changed.
     * SOCK_ACTIVITY_ON_READWRITE_FDS means activity on both readfds and writefds
     */
    if((NULL != p_parms->writefds)&&(SOCK_ACTIVITY_ON_READ_FDS==poll_act_mask))
    {
      RTCS_FD_ZERO(p_parms->writefds);
    }
    if((NULL != p_parms->readfds)&&(SOCK_ACTIVITY_ON_WRITE_FDS==poll_act_mask))
    { 
      RTCS_FD_ZERO(p_parms->readfds);
    }

    /*
     * if NO poll activity has been detected
     * if NOT in poll only mode
     */
    if((0 == p_parms->sock)&&(p_parms->timeout != 0xFFFFFFFF))
    {
      /* put this select() to select queue. */
      /* unblock via _SOCK_select_signal() or via SOCK_select_expire() */
      SOCK_select_block(p_parms);
      return;
    }
    RTCSCMD_complete(p_parms, RTCS_OK);
}
/* EOF */
