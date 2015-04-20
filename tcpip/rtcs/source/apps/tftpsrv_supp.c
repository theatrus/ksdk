/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains the TFTP server support functions.
*
*
*END************************************************************************/

#include "tftp.h"
#include "tftpsrv_prv.h"
#include <limits.h>

static uint32_t tftpsrv_set_params(TFTPSRV_STRUCT *server, TFTPSRV_PARAM_STRUCT *params);
static uint32_t tftpsrv_init_socket(TFTPSRV_STRUCT *server, uint16_t family);
static char* tftpsrv_create_path(char *root, char *filename);
static void tftpsrv_normalize_path(char* path);
static uint32_t tftpsrv_rt_step(TFTPSRV_SESSION_STRUCT *session);
static inline void tftpsrv_rt_reset(TFTPSRV_SESSION_STRUCT *session);
static int32_t tftpsrv_send(uint32_t sock, char* data, uint32_t data_size, uint32_t flags);
static int32_t tftpsrv_recv(TFTPSRV_SESSION_STRUCT *session, char* data, uint32_t data_size);

/*
 * Allocate server structure, init sockets, etc.
 */
TFTPSRV_STRUCT* tftpsrv_create_server(TFTPSRV_PARAM_STRUCT* params)
{
    TFTPSRV_STRUCT *server = NULL;
    uint32_t error;
    uint32_t error4 = TFTPSRV_OK;
    uint32_t error6 = TFTPSRV_OK;


    if ((server = _mem_alloc_zero(sizeof(TFTPSRV_STRUCT))) == NULL)
    {
        return(NULL);
    }
    _mem_set_type(server, MEM_TYPE_TFTPSRV_SERVER_STRUCT);

    error = _lwsem_create(&server->tid_sem, 1);
    if (error != MQX_OK)
    {
        goto EXIT;
    }

    error = tftpsrv_set_params(server, params);
    if (error != TFTPSRV_OK)
    {
        goto EXIT;
    }
    
    /* Allocate space for session pointers */
    server->session = _mem_alloc_zero(sizeof(TFTPSRV_SESSION_STRUCT*) * server->params.max_ses);
    if (server->session == NULL)
    {
        goto EXIT;
    }

    /* Allocate space for session task IDs */
    server->ses_tid = _mem_alloc_zero(sizeof(_rtcs_taskid) * server->params.max_ses);
    if (server->ses_tid == NULL)
    {
        goto EXIT;
    }

    /* Init sockets. */
    if (server->params.af & AF_INET)
    {
        /* Setup IPv4 server socket */
        error4 = tftpsrv_init_socket(server, AF_INET);
    }
    if (server->params.af & AF_INET6)
    {
        /* Setup IPv6 server socket */
        error6 = tftpsrv_init_socket(server, AF_INET6);
    }

    if ((error4 != TFTPSRV_OK) || (error6 != TFTPSRV_OK))
    {
        goto EXIT;
    }

    return(server);
    EXIT:
    tftpsrv_destroy_server(server);
    return(server);
}

/*
 * Close sockets, free memory etc.
 */
int32_t tftpsrv_destroy_server(TFTPSRV_STRUCT* server)
{
    uint32_t n = 0;
    bool     wait = FALSE;
    
    if (server == NULL)
    {
        return(RTCS_ERROR);
    }

    /* Invalidate sessions (this is signal for session tasks to end them) */
    for (n = 0; n < server->params.max_ses; n++)
    {
        if (server->session[n])
        {
            server->session[n]->valid = FALSE;
        }
    }
    /* Wait until all session tasks end */
    do
    {
        wait = FALSE;
        for (n = 0; n < server->params.max_ses; n++)
        {
            if (server->ses_tid[n])
            {
                wait = TRUE;
                break;
            }
        }
        _sched_yield();
    }while(wait);

    
    _lwsem_destroy(&server->tid_sem);
    
    _mem_free((void*) server->ses_tid);
    server->ses_tid = NULL;
    _mem_free(server->session);
    server->session = NULL;
    return(RTCS_OK);
}

/*
** Internal function for server parameters initialization
**
** IN:
**      TFTPSRV_STRUCT* server - server structure pointer
**
**      TFTPSRV_PARAM_STRUCT* params - pointer to user parameters if there are any
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. TFTPSRV_OK if everything went right, positive number otherwise
*/
static uint32_t tftpsrv_set_params(TFTPSRV_STRUCT *server, TFTPSRV_PARAM_STRUCT *params)
{
    server->params.port = IPPORT_TFTP;
    #if RTCSCFG_ENABLE_IP4
    server->params.ipv4_address.s_addr = 0;
    server->params.af |= AF_INET;
    #endif
    #if RTCSCFG_ENABLE_IP6  
    server->params.ipv6_address = in6addr_any;
    server->params.ipv6_scope_id = 0;
    server->params.af |= AF_INET6;
    #endif
    server->params.max_ses = RTCSCFG_TFTPSRV_SES_CNT;
    server->params.server_prio = RTCSCFG_TFTPSRV_SERVER_PRIO;
    server->params.root_dir = "tfs:";
    /* If there is parameters structure copy nonzero values to server */
    if (params != NULL)
    {
        if (params->port)
            server->params.port = params->port;
        #if RTCSCFG_ENABLE_IP4
        if (params->ipv4_address.s_addr != 0)
            server->params.ipv4_address = params->ipv4_address;
        #endif
        #if RTCSCFG_ENABLE_IP6
        if (params->ipv6_address.s6_addr != NULL)
            server->params.ipv6_address = params->ipv6_address;
        if (params->ipv6_scope_id)
            server->params.ipv6_scope_id = params->ipv6_scope_id;
        #endif
        if (params->af)
            server->params.af = params->af;
        if (params->max_ses)
            server->params.max_ses = params->max_ses;
        if (params->server_prio)
            server->params.server_prio = params->server_prio;
        if (params->root_dir)
        {
            server->params.root_dir = params->root_dir;  
        }
    }

    return(TFTPSRV_OK);
}

/*
** Function for socket initialization (both IPv4 and IPv6)
**
** IN:
**      TFTPSRV_STRUCT* server - server structure pointer
**
**      uint16_t      family - IP protocol family
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. TFTPSRV_OK if everything went right, positive number otherwise
*/
static uint32_t tftpsrv_init_socket(TFTPSRV_STRUCT *server, uint16_t family)
{
    uint32_t error;
    sockaddr sin_sock;
    uint32_t sock = 0;

    _mem_zero(&sin_sock, sizeof(sockaddr));
    #if RTCSCFG_ENABLE_IP4
    if (family == AF_INET) /* IPv4 */
    {
       
        if ((server->sock_v4 = socket(AF_INET, SOCK_DGRAM, 0)) == (uint32_t)RTCS_ERROR)
        {
            return(TFTPSRV_CREATE_FAIL);
        }
        ((sockaddr_in *)&sin_sock)->sin_port   = server->params.port;
        ((sockaddr_in *)&sin_sock)->sin_addr   = server->params.ipv4_address;
        ((sockaddr_in *)&sin_sock)->sin_family = AF_INET;
        sock = server->sock_v4;
    }
    else
    #endif    
    #if RTCSCFG_ENABLE_IP6   
    if (family == AF_INET6) /* IPv6 */
    {
        if ((server->sock_v6 = socket(AF_INET6, SOCK_DGRAM, 0)) == (uint32_t)RTCS_ERROR)
        {
            return(TFTPSRV_CREATE_FAIL);
        }
        ((sockaddr_in6 *)&sin_sock)->sin6_port      = server->params.port;
        ((sockaddr_in6 *)&sin_sock)->sin6_family    = AF_INET6;
        ((sockaddr_in6 *)&sin_sock)->sin6_scope_id  = server->params.ipv6_scope_id;
        ((sockaddr_in6 *)&sin_sock)->sin6_addr      = server->params.ipv6_address;
        sock = server->sock_v6;
    }
    else
    #endif    
    {
        return(TFTPSRV_BAD_FAMILY);
    }

    /* Bind socket */
    error = bind(sock, &sin_sock, sizeof(sin_sock));
    if(error != RTCS_OK)
    {
        return(TFTPSRV_BIND_FAIL);
    }

    return(TFTPSRV_OK);
}

/*
 * Open file from filesystem in specified mode.
 */
int32_t tftpsrv_open_file(uint16_t opcode, char *filename, char *root, char *filemode,
#if MQX_USE_IO_OLD
      MQX_FILE_PTR  *file_ptr
#else
      FILE  **file_ptr
#endif
   )
{
    int32_t   error;
    uint32_t  file_error;
    char      open_flags[4] = {0};
    char      *path;

    error = TFTPSRV_OK;

    path = tftpsrv_create_path(root, filename);
    if (path == NULL)
    {
        error = TFTPSRV_ERR_UNKNOWN;
        goto EXIT;
    }
    /* 
     * The new C standard (C2011, which is not part of C++) adds a new standard 
     * subspecifier ("x"), that can be appended to any "w" specifier (to form "wx",
     * "wbx", "w+x" or "w+bx"/"wb+x"). This subspecifier forces the function to 
     * fail if the file exists, instead of overwriting it.
     */
    #if MQX_USE_IO_OLD
    open_flags[1] = 'x';
    #endif

    if (opcode == TFTP_OPCODE_RRQ)
    {
        open_flags[0] = 'r';
        error = TFTPSRV_ERR_FILE_NOT_FOUND;
    }
    else if (opcode == TFTP_OPCODE_WRQ)
    {
        open_flags[0] = 'w';
        error = TFTPSRV_ERR_FILE_EXISTS;
    }
    else
    {
        error = TFTPSRV_ERR_ILLEGAL_OP;
        goto EXIT;
    }

    if (strcmp(filemode, "octet") == 0)
    {
        open_flags[1] = 'b';
        #if MQX_USE_IO_OLD
        open_flags[2] = 'x';
        #endif
    }

    *file_ptr = RTCS_io_open(path, open_flags, &file_error);
    _mem_free(path);
    path = NULL;
    if (*file_ptr != NULL)
    {
        error = TFTPSRV_OK;
    }

    EXIT:
    return(error);
}

/*
 * Create full path in filesystem.
 */
static char* tftpsrv_create_path(char *root, char *filename)
{
    char *path;

    uint32_t root_length;
    uint32_t filename_length;
    uint32_t path_length;

    root_length = strlen(root);
    filename_length = strlen(filename);
    path_length = root_length + filename_length + 1;

    if (root[root_length-1] != '\\')
    {
        path_length++;
    }
    path = (char *) RTCS_mem_alloc_zero(path_length);
    if (path == NULL)
    {
        goto EXIT;
    }
    _mem_copy(root, path, root_length);
    if (root[root_length-1] != '\\')
    {
        path[root_length] = '\\';
        root_length++;
    }
    _mem_copy(filename, path+root_length, filename_length);
    
    tftpsrv_normalize_path(path);

    EXIT:
    return(path);
}

/*
 * Normalize filesystem path.
 */
static void tftpsrv_normalize_path(char* path)
{
    char        *bs = path; /* backslash location. */
    char        *prev_bs = path; /* previous backslash location. */
    bool        init = true;
    uint32_t    offset = 0;

    /* Replace path segments for listing one level up */
    while((bs = strchr(prev_bs+offset, '\\')) != NULL)
    {
        if (init)
        {
            init = false;
            prev_bs = bs;
        }
        /* Encountered "\.." sequence. */
        if ((bs[1] == '.') && (bs[2] == '.'))
        {
            uint32_t index;
            
            index = (bs[3] == '\0') ? 1 : 0;
            memmove(prev_bs+index, bs+3, strlen(bs+3)+1);
            bs = prev_bs;
            offset = 0;
        }
        /* Encountered "..\" sequence. */
        else if ((bs[-1] == '.') && (bs[-2] == '.') && (bs[1] == '\0'))
        {
            memmove(prev_bs, bs, strlen(bs)+1);
            bs = prev_bs;
            offset = 0;
        }
        /* Encountered "\\" sequence. */
        else if (bs[1] == '\\')
        {
            memmove(bs, bs+1, strlen(bs)+1);
            offset = 0;
        }
        else
        {
            offset = 1;
        }
        prev_bs = bs;
    }
}

/*
 * Send error to client.
 */
void tftpsrv_error(TFTPSRV_SESSION_STRUCT *session, int32_t error_code, uint32_t flags)
{
    char     *message;
    char     *buffer;
    uint32_t size;
    
    message = NULL;

    switch(error_code)
    {
        case TFTPSRV_ERR_FILE_NOT_FOUND:
            message = TFTP_ERR_NOT_FOUND_STRING;
            break;
        case TFTPSRV_ERR_ACCESS_VIOLATION:
            message = TFTP_ERR_ACCESS_STRING;
            break;
        case TFTPSRV_ERR_DISK_FULL:
            message = TFTP_ERR_DISK_FULL_STRING;
            break;
        case TFTPSRV_ERR_ILLEGAL_OP:
            message = TFTP_ERR_ILLEGAL_OP_STRING;
            break;
        case TFTPSRV_ERR_ILLEGAL_TID:
            message = TFTP_ERR_BAD_TID_STRING;
            break;
        case TFTPSRV_ERR_FILE_EXISTS:
            message = TFTP_ERR_EXISTS_STRING;
            break;
        case TFTPSRV_ERR_ILLEGAL_USER:
            message = TFTP_ERR_NO_USER_STRING;
            break;
        case TFTPSRV_ERR_TIMEOUT:
            message = TFTP_ERR_TIMEOUT_STRING;
            break;
        case TFTPSRV_ERR_UNKNOWN:
        default:
            message = TFTP_ERR_SERVER_STRING;
            break;
    }
    /* Write message to buffer and send it */
    buffer = session->buffer;
    *((uint16_t *) buffer) = htons(TFTP_OPCODE_ERROR);
    buffer += sizeof(uint16_t);
    *((uint16_t *) buffer) = htons(-error_code);
    buffer += sizeof(uint16_t);
    size = TFTP_HEADER_SIZE;
    _mem_copy(message, buffer, strlen(message)+1);
    size += strlen(message)+1;

    tftpsrv_send(session->sock, session->buffer, size, 0);

    if (flags == TFTPSRV_FATAL)
    {
        session->process_func = tftpsrv_transaction_epilogue;
    }
}

/*
 * Send data packet.
 */
int32_t tftpsrv_send_data(TFTPSRV_SESSION_STRUCT *session)
{
    uint32_t length;
    char     *buffer;
    int32_t  size;

    buffer = session->buffer;
    *((uint16_t *) buffer) = htons(TFTP_OPCODE_DATA);
    buffer += sizeof(uint16_t);
    *((uint16_t *) buffer) = htons(session->n_block);
    buffer += sizeof(uint16_t);
    length = TFTP_HEADER_SIZE;
#if MQX_USE_IO_OLD
    fseek(session->file, (session->n_block-1)*TFTP_DATA_SIZE, IO_SEEK_SET);
#else
    fseek(session->file, (session->n_block-1)*TFTP_DATA_SIZE, SEEK_SET);
#endif
    size = fread((void *) buffer, 1, TFTP_DATA_SIZE, session->file);
    if (size >= 0)
    {
        length += size;
        if 
            (
                (length < TFTP_DATA_SIZE+TFTP_HEADER_SIZE) ||
                (session->n_block == USHRT_MAX)
            )
        {
            session->last = true;
        }
        return(tftpsrv_send(session->sock, session->buffer, length, 0));
    }
    else
    {
        tftpsrv_error(session, TFTPERR_UNKNOWN, TFTPSRV_FATAL);
        return(RTCS_ERROR);
    }
}

/*
 * Receive data packet.
 */
int32_t tftpsrv_recv_data(TFTPSRV_SESSION_STRUCT *session)
{
    int32_t  retval;
    uint16_t n_block;
    uint16_t opcode;
    uint32_t length;
    char     *buffer;
    int32_t  size;

    /* Read data from socket */
    retval = tftpsrv_recv(session, session->buffer, TFTP_MAX_MESSAGE_SIZE);
    if 
        (
            (retval == 0) ||
            (retval == RTCS_ERROR) ||
            (retval == TFTPSRV_ERR_ILLEGAL_TID)
        )
    {
        goto EXIT;
    }
    /* Report timeout to client. */
    else if (retval == TFTPSRV_ERR_TIMEOUT)
    {
        tftpsrv_error(session, TFTPSRV_ERR_TIMEOUT, TFTPSRV_FATAL);
        retval = RTCS_ERROR;
        goto EXIT;
    }
    else
    {
        tftpsrv_rt_reset(session);
    }

    buffer = session->buffer;
    opcode = ntohs(*((uint16_t *) buffer));
    buffer += sizeof(uint16_t);
    n_block = ntohs(*((uint16_t *) buffer));
    buffer += sizeof(uint16_t);

    /* Write valid data to file. */
    if 
    (
        (n_block == (session->n_block+1)) &&
        (opcode == TFTP_OPCODE_DATA)
    )
    {
        session->n_block++;
        length = retval - TFTP_HEADER_SIZE;
        if (length < TFTP_DATA_SIZE)
        {
            session->last = true;
        }
#if MQX_USE_IO_OLD
        fseek(session->file, (session->n_block-1)*TFTP_DATA_SIZE, IO_SEEK_SET);
#else
        fseek(session->file, (session->n_block-1)*TFTP_DATA_SIZE, SEEK_SET);
#endif
        size = fwrite((void *) buffer, 1, length, session->file);
        if (size < 0)
        {
            tftpsrv_error(session, TFTPERR_UNKNOWN, TFTPSRV_FATAL);
            retval = RTCS_ERROR;
        }
        else if (size < length)
        {
            tftpsrv_error(session, TFTPSRV_ERR_DISK_FULL, TFTPSRV_FATAL);
            retval = RTCS_ERROR;
        }
    }
    else
    {
        retval = RTCS_ERROR;
    }
    
    EXIT:
    return(retval);
}

/*
 * Reset retransmission mechanism
 */
static inline void tftpsrv_rt_reset(TFTPSRV_SESSION_STRUCT *session)
{
    session->recv_timeout = TFTPSRV_RC_INIT;
    session->rt_count = 0;
}

/*
 * Receive acknowledgment packet.
 */
int32_t tftpsrv_recv_ack(TFTPSRV_SESSION_STRUCT *session)
{
    int32_t     retval;

    retval = tftpsrv_recv(session, (char *) session->buffer_ack, TFTP_HEADER_SIZE);
    if 
    (
        (retval == 0) ||
        (retval == RTCS_ERROR) ||
        (retval == TFTPSRV_ERR_ILLEGAL_TID)
    )
    {
        goto EXIT;
    }
    /* Report timeout to client. */
    else if (retval == TFTPSRV_ERR_TIMEOUT)
    {
        tftpsrv_error(session, TFTPSRV_ERR_TIMEOUT, TFTPSRV_FATAL);
        retval = RTCS_ERROR;
        goto EXIT;
    }
    /* Increase number of expected block and reset retransmission mechanism. */
    else if (retval == TFTP_HEADER_SIZE)
    {
        uint16_t    n_block;
        uint16_t    opcode;

        opcode = ntohs(session->buffer_ack[0]);
        n_block = ntohs(session->buffer_ack[1]);
        if 
            (
                (n_block == session->n_block) &&
                (opcode == TFTP_OPCODE_ACK)
            )
        {
            session->n_block++;
        }

        tftpsrv_rt_reset(session);
    }

    EXIT:
    return(retval);
}

/*
 * Send acknowledgment packet.
 */
int32_t tftpsrv_send_ack(TFTPSRV_SESSION_STRUCT *session)
{
    char     *buffer;
    uint32_t length;
    
    buffer = session->buffer;
    *((uint16_t *) buffer) = htons(TFTP_OPCODE_ACK);
    buffer += sizeof(uint16_t);
    *((uint16_t *) buffer) = htons(session->n_block);
    length = TFTP_HEADER_SIZE;
    
    return(tftpsrv_send(session->sock, session->buffer, length, 0));
}

/*
 * TFTPSRV wrapper for send function.
 */
static int32_t tftpsrv_send(uint32_t sock, char* data, uint32_t data_size, uint32_t flags)
{
    return(send(sock, data, data_size, flags));
}

/*
 * TFTPSRV wrapper for receive function.
 */
static int32_t tftpsrv_recv(TFTPSRV_SESSION_STRUCT *session, char* data, uint32_t data_size)
{
    sockaddr    remote_sin = {0};
    uint16_t    sin_size;
    int32_t     retval;
    int32_t     error;
    rtcs_fd_set rfds;

    RTCS_FD_ZERO(&rfds);
    RTCS_FD_SET(session->sock, &rfds);

    /* Wait for incoming ACK. */
    error = select(1, &rfds, NULL, session->recv_timeout);
    if ((error == RTCS_ERROR) || (error == 0))
    {
        /* timeout or error */
        retval = error;
        goto EXIT;
    }
    else
    {
        /* data received */
        tftpsrv_rt_reset(session);
    }
    sin_size = sizeof(remote_sin);
    retval = recvfrom(session->sock, data, data_size, 0, &remote_sin, &sin_size);

    /* Verify the sender's address and port */
    if (memcmp((void *) &remote_sin, (void *) &session->client_sin, sizeof(sockaddr)) != 0)
    {
        tftpsrv_error(session, TFTPSRV_ERR_ILLEGAL_TID, TFTPSRV_FATAL);
        retval = TFTPSRV_ERR_ILLEGAL_TID;
    }

    EXIT:
    /* Prepare next retransmission. */
    session->recv_timeout = tftpsrv_rt_step(session);
    if (session->recv_timeout == 0)
    {
       retval = TFTPSRV_ERR_TIMEOUT;
    }
    return(retval);
}

/*
 * Do one step in retransmission mechanism.
 */
static uint32_t tftpsrv_rt_step(TFTPSRV_SESSION_STRUCT *session)
{
    uint32_t retval;

    retval = 2*session->recv_timeout;
    session->rt_count++;

    if ((retval > TFTPSRV_RD_MAX) || (session->rt_count > TFTPSRV_RC_MAX))
    {
        retval = 0;
    }
    return(retval);
}
