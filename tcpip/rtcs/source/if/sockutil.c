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
*   This file contains utility functions for managing
*   socket structures.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "tcp_prv.h"
#include "ip_prv.h"
#include "socket.h"
#include <string.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Get_sock_struct
* Returned Value  : none
* Comments        : Get a socket structure, initialize it and return
*                     address of socket.
*
*END*-----------------------------------------------------------------*/

SOCKET_STRUCT_PTR   SOCK_Get_sock_struct
   (
      RTCS_SOCKET_CALL_STRUCT_PTR   type,
      _rtcs_taskid                  owner

   )
{ /* Body */
  RTCS_DATA_PTR              RTCS_data_ptr;
  SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
  SOCKET_STRUCT_PTR          socket_ptr = NULL;

  RTCS_data_ptr = RTCS_get_data();
  socket_cfg_ptr = RTCS_data_ptr->SOCKET_CFG;
  
#if RTCSCFG_ENABLE_TCP || RTCSCFG_ENABLE_UDP
  if(
    #if RTCSCFG_ENABLE_TCP
      ((uint32_t)type == SOCK_STREAM)
      #if RTCSCFG_ENABLE_UDP
      ||
      #endif      
    #endif
    #if RTCSCFG_ENABLE_UDP
      ((uint32_t)type == SOCK_DGRAM)
    #endif
    )
  {
    socket_ptr = RTCS_part_alloc_zero(RTCS_data_ptr->RTCS_socket_partition);
  }
#endif  

   if ( socket_ptr != NULL ) {

      RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);
      socket_cfg_ptr->CURRENT_SOCKETS++;
      
#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1
      #if RTCSCFG_SOCKET_OWNERSHIP
      SOCK_Add_owner(socket_ptr, owner);
      #endif
#endif
      
      /*
      ** link in this socket into the linked list of active sockets
      */
      if ( socket_cfg_ptr->SOCKET_HEAD != NULL ) {

         ((SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL)->NEXT =
            (void *)socket_ptr;
         socket_ptr->PREV = (SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL;

      } else {
         socket_cfg_ptr->SOCKET_HEAD = (void *)socket_ptr;
         socket_ptr->PREV = NULL;

      } /* Endif */

      socket_ptr->NEXT = NULL;
      socket_cfg_ptr->SOCKET_TAIL = (void *)socket_ptr;
      RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

      socket_ptr->VALID = SOCKET_VALID;

      /*
      ** set the default socket options
      */
      socket_ptr->CONNECT_TIMEOUT  = DEFAULT_CONNECT_TIMEOUT;
      socket_ptr->RETRANSMISSION_TIMEOUT = DEFAULT_RETRANSMISSION_TIMEOUT;
      socket_ptr->SEND_TIMEOUT     = DEFAULT_SEND_TIMEOUT;
      socket_ptr->RECEIVE_TIMEOUT  = DEFAULT_RECEIVE_TIMEOUT;
      socket_ptr->RECEIVE_PUSH     = DEFAULT_PUSH;
      socket_ptr->SEND_NOWAIT      = DEFAULT_SEND_NOWAIT;
      /* replace DEFAULT_SEND_WAIT by MSG_WAITACK flag for stream socket send() */
      socket_ptr->SEND_WAIT        = FALSE; /* was DEFAULT_SEND_WAIT;*/ 
      socket_ptr->SEND_PUSH        = DEFAULT_PUSH;
      socket_ptr->RECEIVE_NOWAIT   = DEFAULT_RECEIVE_NOWAIT;
      socket_ptr->CHECKSUM_BYPASS  = DEFAULT_CHECKSUM_BYPASS;
      socket_ptr->ACTIVE           = DEFAULT_ACTIVE;
      socket_ptr->TBSIZE           = DEFAULT_TBSIZE;
      socket_ptr->RBSIZE           = DEFAULT_RBSIZE;
      socket_ptr->MAXRTO           = DEFAULT_MAXRTO;
      socket_ptr->MAXRCV_WND       = DEFAULT_MAXRCV_WND;
      socket_ptr->KEEPALIVE        = DEFAULT_KEEPALIVE;
      socket_ptr->NOWAIT           = DEFAULT_NOWAIT;
      socket_ptr->NO_NAGLE_ALGORITHM  = DEFAULT_NO_NAGLE_ALGORITHM;
      socket_ptr->NOSWRBUF         = DEFAULT_NOSWRBUF;
      socket_ptr->CALL_BACK        = DEFAULT_CALL_BACK;
      socket_ptr->TYPE_MIRROR      = (uint32_t)type;
      socket_ptr->APPLICATION_CALLBACK  = DEFAULT_CALLBACK;
      socket_ptr->TIMEWAIT_TIMEOUT = DEFAULT_TIMEWAIT_TIMEOUT;
      socket_ptr->DELAY_ACK = DEFAULT_DELAY_ACK;
#if RTCSCFG_ENABLE_IP6
      socket_ptr->LINK_OPTIONS.TX.HOP_LIMIT_MULTICAST = DEFAULT_IP6_MULTICAST_HOPS;
#endif

      socket_ptr->PROTOCOL     = type;

   } /* Endif */

   return(socket_ptr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Free_sock_struct
* Returned Value  : none
* Comments        : Delink and release a socket structure.
*
*END*-----------------------------------------------------------------*/

void SOCK_Free_sock_struct
   (
      SOCKET_STRUCT_PTR          socket_ptr
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   
   if (socket_ptr->VALID == 0)
   {
      return;
   }
#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1
   #if RTCSCFG_SOCKET_OWNERSHIP //FSL AB
       SOCKET_OWNER_STRUCT_PTR  owner_ptr;
       SOCKET_OWNER_STRUCT_PTR  free_ptr;
   #endif
#endif

   socket_ptr->VALID = 0;

#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1
#if RTCSCFG_SOCKET_OWNERSHIP
   owner_ptr = socket_ptr->OWNERS.NEXT;
   while (owner_ptr != NULL) {
      free_ptr = owner_ptr;
      owner_ptr = owner_ptr->NEXT;
      _mem_free(free_ptr);
   } /* Endwhile */
#endif
#endif

   RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);

   /*
   ** Fix up the head/next pointer of our predecessor.
   */
   if ( socket_ptr->PREV == NULL ) {
      socket_cfg_ptr->SOCKET_HEAD = socket_ptr->NEXT;
   } else {
      (socket_ptr->PREV)->NEXT = socket_ptr->NEXT;
   } /* Endif */

   /*
   ** Fix up the tail/prev pointer of our successor.
   */
   if ( socket_ptr->NEXT == NULL ) {
      socket_cfg_ptr->SOCKET_TAIL = socket_ptr->PREV;
   } else {
      (socket_ptr->NEXT)->PREV = socket_ptr->PREV;
   } /* Endif */

   /*
   ** Release the socket structure memory.
   */
   RTCS_part_free(socket_ptr);

   socket_cfg_ptr->CURRENT_SOCKETS--;
   RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

} /* Endbody */

/**
 * @brief Call IP_route_find()/IP6_route_find()
 * 
 * Call IP_route_find/IP6_route_find for the <tt>struct sockaddr</tt> given in @p saddr_ptr.
 * IP_route_find() is called when @c sockaddr is @c sockaddr_in (@c AF_INET family).
 * IP_route_find6() is called when @c sockaddr is @c sockadddr_in6 (@c AF_INET6 family).
 * 
 * @param[in] saddr_ptr pointer to <tt>struct sockaddr</tt>
 *                   
 * @return _ip_address result of IP_route_find(), for @c AF_INET family, typecasted to <tt>void*</tt>.
 * @return (in6_addr*) result of IP6_route_find(), for @c AF_INET6 family, typecasted to <tt>void*</tt>.
 * @return @c NULL otherwise.
 */
void * SOCKADDR_route_find(const sockaddr * saddr_ptr)
{
  void * retval = NULL; 
  /* for AF_INET, retval is _ip_address */
  /* for AF_INET6, retval is (in6_addr*) */
#if RTCSCFG_ENABLE_IP4
  #if RTCSCFG_ENABLE_IP6
  if(AF_INET == saddr_ptr->sa_family) 
  {
  #endif
    retval = (void*)IP_route_find(SOCKADDR_get_ipaddr4(saddr_ptr), 0);
  #if RTCSCFG_ENABLE_IP6
  }
  #endif
#endif
#if RTCSCFG_ENABLE_IP6
  #if RTCSCFG_ENABLE_IP4
  else if(AF_INET6 == saddr_ptr->sa_family)
  {
  #endif
    retval = IP6_route_find(SOCKADDR_get_ipaddr6(saddr_ptr));
  #if RTCSCFG_ENABLE_IP4
  }
  #endif
#endif
  return retval;
}

/**
 * @brief Return sockaddr from UDP/TCP layer to socket layer.
 *
 * If any of the function parameters is given as a @c NULL pointer, the function will only return. 
 * 
 * @param[in] sock_struct_ptr @c SOCKET_STRUCT_PTR to a socket to determine address family AF_INET/AF_INET6.
 * @param[out] name Pointer to destination <tt>struct sockaddr</tt>. 
 * @param[in] addrfrom Pointer to source <tt>struct sockaddr</tt>. 
 * @param[out] namelen Points to size of data to be copied from source to destination in bytes.
 *  
 * @return None.
 */
void SOCKADDR_return_addr(const SOCKET_STRUCT_PTR sock_struct_ptr, sockaddr * name, const sockaddr * addrfrom, uint16_t * namelen)
{
  uint16_t to_copy = 0;
  if((name)&&(namelen)&&sock_struct_ptr&&addrfrom)
  {
    if(sock_struct_ptr->AF == AF_INET)
    {
      to_copy = sizeof(sockaddr_in);
    }
    else if(sock_struct_ptr->AF == AF_INET6)
    {
      to_copy = sizeof(sockaddr_in6);
    }
#if RTCSCFG_CHECK_ADDRSIZE
    if(*namelen < to_copy)
    {
      to_copy = *namelen;
    }
#endif
    memcpy(name, addrfrom, to_copy);
    *namelen = to_copy;
  }  
}

/**
 * @brief Check validity of struct sockaddr.
 *
 * Check if socket and struct sockaddr have the same protocol family. 
 * 
 * @param[in] sock @c SOCKET_STRUCT_PTR to a socket to determine address family AF_INET/AF_INET6.
 * @param[in] localaddr Pointer to <tt>struct sockaddr</tt> to be checked. 
 *  
 * @return Zero (RTCS_OK) if valid.
 * @return Specific error code in case of error.
 */
uint32_t SOCKADDR_check_valid(uint32_t sock, const sockaddr* localaddr)
{
    uint32_t retval = 0;
    SOCKET_STRUCT_PTR sock_struct_ptr = (SOCKET_STRUCT_PTR)sock;
    
    if((NULL == localaddr) || (NULL == sock_struct_ptr))
    {
      retval = RTCSERR_SOCK_INVALID_PARAMETER;
      goto RETURN;
    }

    if(localaddr->sa_family != sock_struct_ptr->AF)
    {
      retval = RTCSERR_SOCK_INVALID_AF;
    }
    
RETURN:
    return(retval);
}

/**
 * @brief Check size of struct sockaddr.
 *
 * Check if addrlen has sufficient size for sockaddr_in/sockaddr_in6 depending on family. 
 *  
 * @param[in] localaddr Pointer to <tt>struct sockaddr</tt> to be checked. 
 * @param[in] addrlen Length in bytes to be checked. 
 *  
 * @return Zero (RTCS_OK) if struct sockaddr has sufficient size.
 * @return Specific error code otherwise.
 */
uint32_t SOCKADDR_check_addr(const sockaddr* localaddr, uint16_t addrlen)
{
    uint32_t retval = 0;
    if(NULL == localaddr)
    {
      retval = RTCSERR_SOCK_INVALID_PARAMETER;
      goto RETURN;
    }

    #if RTCSCFG_CHECK_ADDRSIZE
    if (localaddr->sa_family == AF_INET) 
    {
        if (addrlen < sizeof(sockaddr_in)) 
        {
            retval = RTCSERR_SOCK_SHORT_ADDRESS;
        }
    }
    else if (localaddr->sa_family == AF_INET6) 
    {
        if (addrlen < sizeof(struct sockaddr_in6)) 
        {
            retval = RTCSERR_SOCK_SHORT_ADDRESS;
        }
    }
    #endif
RETURN:
    return(retval);
}

#if RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT==1
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Remove_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Remove_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint32_t                 i;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            owner_ptr->TASK[i] = NULL;
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
#else
   return TRUE;
#endif
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Is_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Is_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint32_t                 i;

   owner_ptr = &socket_ptr->OWNERS;


   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
#else
   return TRUE;
#endif
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Add_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Add_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr, new_owner_ptr;
   uint32_t                 i;
   void                  **saved_ptr = NULL;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } else if ((owner_ptr->TASK[i] == 0) && (saved_ptr == NULL)) {
            saved_ptr = &owner_ptr->TASK[i];
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */

   if (saved_ptr != NULL) {
      *saved_ptr = task_ptr;
   } else {
      new_owner_ptr = RTCS_mem_alloc_zero(sizeof(SOCKET_OWNER_STRUCT));
      if (new_owner_ptr == NULL) {
         return FALSE;
      } /* Endif */

      _mem_set_type(new_owner_ptr, MEM_TYPE_SOCKET_OWNER_STRUCT);

      new_owner_ptr->TASK[0] = task_ptr;
      owner_ptr->NEXT = new_owner_ptr;
   } /* Endif */
   return TRUE;
#else
   return TRUE;
#endif
} /* Endbody */
#endif /* RTCSCFG_BACKWARD_COMPATIBILITY_RTCSSELECT */


/* EOF */
