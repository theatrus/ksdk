#ifndef __tftpsrv_prv_h__
#define __tftpsrv_prv_h__
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
*   Private header for TFTP server application.
*
*
*END************************************************************************/

#include "tftpsrv.h"

#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif

#define TFTPSRV_ERR_UNKNOWN            (0)
#define TFTPSRV_ERR_FILE_NOT_FOUND     (-1)
#define TFTPSRV_ERR_ACCESS_VIOLATION   (-2)
#define TFTPSRV_ERR_DISK_FULL          (-3)
#define TFTPSRV_ERR_ILLEGAL_OP         (-4)
#define TFTPSRV_ERR_ILLEGAL_TID        (-5)
#define TFTPSRV_ERR_FILE_EXISTS        (-6)
#define TFTPSRV_ERR_ILLEGAL_USER       (-7)
#define TFTPSRV_ERR_NO_FILE            (-8)
#define TFTPSRV_ERR_TIMEOUT            (-9)

#define TFTPSRV_SERVER_STACK_SIZE   (2000)
#define TFTPSRV_SESSION_STACK_SIZE  (2000)
#define TFTPSRV_FILENAME_MAX_LENGTH (128)

#define TFTPSRV_FATAL                   (1)

#define TFTPSRV_RD_MAX               (30000)
#define TFTPSRV_RC_MAX               (10)
#define TFTPSRV_RC_INIT              (1000)

/*
 * Session process function prototype
 */
typedef void(*TFTPSRV_SES_FUNC)(void* server, void *session);

/*
 * TFTP transaction state
 */
typedef enum tftpsrv_session_state
{
    TFTPSRV_STATE_SEND_DATA,
    TFTPSRV_STATE_SEND_ACK,
    TFTPSRV_STATE_READ_DATA,
    TFTPSRV_STATE_READ_ACK
}TFTPSRV_SESSION_STATE;

/*
* TFTP session structure
*/
typedef struct tftpsrv_session_struct
{
    volatile bool         valid;        /* non zero (1) = session is valid - data in this entry is valid */
    volatile _mqx_int     sock;         /* Session socket */
    char                  *buffer;      /* Pointer to session buffer */
    uint16_t              buffer_ack[2];/* Buffer for ACKs */
    TFTPSRV_SES_FUNC      process_func; /* Session process function */
    #if MQX_USE_IO_OLD
    MQX_FILE_PTR          file;         /* File used in transaction */
    #else
    FILE                  *file;        /* File used in transaction */
    #endif
    sockaddr              client_sin;   /* Information about remote host. */
    uint16_t              n_block;      /* Serial number of transaction block. */
    TFTPSRV_SESSION_STATE state;        /* Session state. */
    bool                  last;         /* Flag signalizing last packet. */
    uint32_t              recv_timeout; /* Time for which server will wait for data. */
    uint32_t              rt_count;     /* number of retransmissions. */
} TFTPSRV_SESSION_STRUCT;

/*
 * Parameters for server task
 */
typedef struct tftpsrv_server_task_param
{
    uint32_t             handle;    /* [out] Server handle, non-zero if initialization was successful */
    TFTPSRV_PARAM_STRUCT *params;   /* [in] Server parameters */
}TFTPSRV_SERVER_TASK_PARAM;

/*
 * TFTP server context structure
 */
typedef struct tftpsrv_struct
{
    TFTPSRV_PARAM_STRUCT   params;          /* server parameters */
    uint32_t               sock_v4;         /* listening socket for IPv4 */
    uint32_t               sock_v6;         /* listening socket for IPv6 */
    TFTPSRV_SESSION_STRUCT **session;       /* array of pointers to sessions */
    volatile _task_id      server_tid;      /* Server task ID */
    volatile _task_id      *ses_tid;        /* Session task IDs */
    LWSEM_STRUCT           tid_sem;         /* Semaphore for session TID array locking */
    LWSEM_STRUCT           ses_cnt;         /* Session counter */
}TFTPSRV_STRUCT;

typedef struct tftpsrv_ses_task_param
{
    TFTPSRV_STRUCT *server;   /* Pointer to server context. */
    uint32_t       sock;      /* Socket with detected activity. */
}TFTPSRV_SES_TASK_PARAM;

#ifdef __cplusplus
extern "C" {
#endif

void tftpsrv_server_task(void *dummy, void *creator);
TFTPSRV_STRUCT* tftpsrv_create_server(TFTPSRV_PARAM_STRUCT* params);
int32_t tftpsrv_destroy_server(TFTPSRV_STRUCT* server);
#if MQX_USE_IO_OLD
int32_t tftpsrv_open_file(uint16_t opcode, char *filename, char *root, char *filemode, MQX_FILE_PTR *file_ptr);
#else
int32_t tftpsrv_open_file(uint16_t opcode, char *filename, char *root, char *filemode, FILE **file_ptr);
#endif
void tftpsrv_error(TFTPSRV_SESSION_STRUCT *session, int32_t error_code, uint32_t flags);
int32_t tftpsrv_send_data(TFTPSRV_SESSION_STRUCT *session);
int32_t tftpsrv_send_ack(TFTPSRV_SESSION_STRUCT *session);
int32_t tftpsrv_recv_data(TFTPSRV_SESSION_STRUCT *session);
int32_t tftpsrv_recv_ack(TFTPSRV_SESSION_STRUCT *session);

void tftpsrv_transaction_prologue(void* server_v, void *session_v);
void tftpsrv_transaction_epilogue(void* server_v, void *session_v);

#ifdef __cplusplus
}
#endif

#endif
