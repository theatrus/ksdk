/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   HTTPSRV tasks and session processing.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "httpsrv_prv.h"
#include "httpsrv_supp.h"
#include "httpsrv_script.h"
#include "lwmsgq.h"
#include "message.h"
#include "rtcs_ssl.h"
#if !MQX_USE_IO_OLD
#include <strings.h>
#endif

#define HTTPSRV_SESSION_TASK_NAME "HTTP server session"
#define HTTPSRV_PLUGIN_HANDLER_NAME "HTTP server plugin handler"

static int httpsrv_readreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_processreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_response(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);

static HTTPSRV_SESSION_STRUCT* httpsrv_ses_alloc(HTTPSRV_STRUCT *server);
static void httpsrv_ses_free(HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_ses_close(HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_ses_init(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, const int sock);

static uint32_t httpsrv_wait_for_conn(HTTPSRV_STRUCT *server);
static uint32_t httpsrv_accept(uint32_t sock);

static void httpsrv_session_task(void *init_ptr, void *creator);
static void httpsrv_plugin_run(void *server_ptr, void *session_ptr);
static void httpsrv_plugin_task(void *init_ptr, void *creator);
static void httpsrv_plugin_fail(HTTPSRV_PLUGIN_MSG *msg);

/*
** HTTPSRV main task which creates new task for each new client request
*/
void httpsrv_server_task(void *init_ptr, void *creator) 
{
    HTTPSRV_STRUCT            *server;
    _mqx_uint                 res;
    HTTPSRV_SERVER_TASK_PARAM *server_params;

    server_params = (HTTPSRV_SERVER_TASK_PARAM *) init_ptr;

    server = httpsrv_create_server(server_params->params);
    if (server == NULL)
    {
       goto QUIT;
    }
    
    server->server_tid = _task_get_id();

    res = _lwsem_create(&server->ses_cnt, server->params.max_ses);  
    if (res != MQX_OK)
    {
        goto ERROR;
    }
    
    server_params->handle = (uint32_t) server;
    
    RTCS_task_resume_creator(creator, RTCS_OK);
    
    while (1) 
    {
        uint32_t connsock = 0;
        uint32_t new_sock = 0;

        /* limit number of opened sessions */
        _lwsem_wait(&server->ses_cnt);

        /* Get socket with incoming connection (IPv4 or IPv6) */
        connsock = httpsrv_wait_for_conn(server);
        if (connsock == RTCS_SOCKET_ERROR)
        {
            _sched_yield();
            break;
        }

        new_sock = httpsrv_accept(connsock);

        if (new_sock != RTCS_SOCKET_ERROR)
        {
            HTTPSRV_SES_TASK_PARAM  ses_param;
            _mqx_uint               error;

            ses_param.server = server;
            ses_param.sock = new_sock;
            /* Try to create task for session */
            error = RTCS_task_create(HTTPSRV_SESSION_TASK_NAME, server->params.server_prio, 
                    #if RTCSCFG_ENABLE_SSL
                                    (server->ssl_ctx != NULL) ? HTTPSRV_SSL_SESSION_STACK_SIZE : HTTPSRV_SESSION_STACK_SIZE, 
                    #else
                                    HTTPSRV_SESSION_STACK_SIZE,
                    #endif
                                    httpsrv_session_task, &ses_param);
            if (MQX_OK != error)
            {
            	shutdown(new_sock, FLAG_ABORT_CONNECTION);
                _lwsem_post(&server->ses_cnt);
            }
        }
        else
        {
            _lwsem_post(&server->ses_cnt);
            /* We probably run out of sockets. Wait some time then try again to prevent session tasks resource starvation */
            _time_delay(150);
        }
    }
ERROR:
    httpsrv_destroy_server(server);
    _lwsem_destroy(&server->ses_cnt);
    server->server_tid = 0;
    if (server != NULL)
    {
        _mem_free(server);
    }
QUIT:
    RTCS_task_resume_creator(creator, (uint32_t)RTCS_ERROR);
}

/*
 * Wait for incoming connection, return socket with activity or error.
 */
static uint32_t httpsrv_wait_for_conn(HTTPSRV_STRUCT *server)
{
    rtcs_fd_set       rfds;
    int32_t           retval;

    RTCS_FD_ZERO(&rfds);
    RTCS_FD_SET(server->sock_v4, &rfds);
    RTCS_FD_SET(server->sock_v6, &rfds);

    retval = select(2, &rfds, NULL, 0);
    if (retval == RTCS_ERROR)
    {
        retval = RTCS_SOCKET_ERROR;
        goto EXIT;
    }

    if(RTCS_FD_ISSET(server->sock_v4, &rfds))
    {
        retval = server->sock_v4;
    }
    else if(RTCS_FD_ISSET(server->sock_v6, &rfds))
    {
        retval = server->sock_v6;
    }
    EXIT:
    return(retval);
}

/*
 * Accept connection from client.
 */
static uint32_t httpsrv_accept(uint32_t sock)
{
    struct sockaddr  remote_addr;
    unsigned short   length;

    _mem_zero(&remote_addr, sizeof(remote_addr));
    length = sizeof(remote_addr);
    return(accept(sock, (sockaddr *) &remote_addr, &length));
}

/*
** Session task.
** This task is responsible for session creation, processing and cleanup.
*/
static void httpsrv_session_task(void *init_ptr, void *creator) 
{
    HTTPSRV_STRUCT* server = ((HTTPSRV_SES_TASK_PARAM*) init_ptr)->server;
    uint32_t sock = ((HTTPSRV_SES_TASK_PARAM*) init_ptr)->sock;
    HTTPSRV_SESSION_STRUCT *session;
    uint32_t i;
    _task_id tid = _task_get_id();

    /* Find empty session */
    _lwsem_wait(&server->tid_sem);

    for (i = 0; i < server->params.max_ses; i++)
    {
        if (server->session[i] == NULL)
        {
            break;
        }
    }
    
    if (i == server->params.max_ses)
    {
        RTCS_task_resume_creator(creator, (uint32_t) RTCS_ERROR);
        _lwsem_post(&server->tid_sem);
        _lwsem_post(&server->ses_cnt);
        return;
    }
  
    /* Save task ID - used for indication of running task */
    server->ses_tid[i] = tid;
    /* Access to array done. Unblock other tasks. */
    _lwsem_post(&server->tid_sem);
    
    /* Allocate session */
    session = httpsrv_ses_alloc(server);
        
    if (session) 
    {
        server->session[i] = session;

        RTCS_task_resume_creator(creator, RTCS_OK);       
        httpsrv_ses_init(server, session, sock);
    
        /* Disable keep-alive for last session so we have at least one session free (not blocked by keep-alive timeout) */
        if (i == server->params.max_ses - 1)
        {
            session->keep_alive_enabled = FALSE;
        }

        while (session->valid) 
        {
            /* Run state machine for session */
            session->process_func(server, session);
            _sched_yield();
        }
        /* If session is not closed close it */
        if (session->state != HTTPSRV_SES_CLOSE)
        {
            httpsrv_ses_close(session);
        }
        httpsrv_ses_free(session);
        server->session[i] = NULL;
    }
    else 
    {
        RTCS_task_resume_creator(creator, (uint32_t) RTCS_ERROR);
    }

    /* Null tid => task is no longer running */
    _lwsem_wait(&server->tid_sem);
    server->ses_tid[i] = 0;
    _lwsem_post(&server->tid_sem);
    /* Cleanup and end task */
    _lwsem_post(&server->ses_cnt);
}

/*
** Task for CGI/SSI handling.
*/
void httpsrv_script_task(void *param, void *creator)
{
    HTTPSRV_STRUCT* server = (HTTPSRV_STRUCT*) param;

    server->script_tid = _task_get_id();

    /* Create lwmsgq for script messages. */
    server->script_msgq = RTCS_mem_alloc_zero(sizeof(LWMSGQ_STRUCT) + server->params.max_ses*sizeof(HTTPSRV_SCRIPT_MSG)*sizeof(_mqx_max_type));
    if 
    (
        (server->script_msgq == NULL) ||
        (MQX_OK != _lwmsgq_init(server->script_msgq, server->params.max_ses, sizeof(HTTPSRV_SCRIPT_MSG)/sizeof(_mqx_max_type)))
    )
    {
        RTCS_task_resume_creator(creator, (uint32_t)RTCS_ERROR);
        goto EXIT;
    }

    RTCS_task_resume_creator(creator, RTCS_OK);

    /* Read messages */
    while(1)
    {
        HTTPSRV_SCRIPT_MSG      message;
        HTTPSRV_FN_CALLBACK     user_function;
        HTTPSRV_FN_LINK_STRUCT* table;
        uint32_t                stack_size;
        char*                   separator;
        uint32_t                retval;

        retval = _lwmsgq_receive(server->script_msgq, (_mqx_max_type*) &message, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
        if (retval != MQX_OK)
        {
            break;
        }

        /* NULL name and session => exit task */
        if ((message.name == NULL) && (message.session == NULL))
        {
            break;
        }

        /*
        * There are two options:
        * 1. User set stack size to 0 and script callback will be run from this task. 
        * 2. User set stack size > 0 and script callback will be run in separate task.
        */
        switch (message.type)
        {
            case HTTPSRV_CGI_CALLBACK:
                table = (HTTPSRV_FN_LINK_STRUCT*) server->params.cgi_lnk_tbl;
                user_function = httpsrv_find_callback(table, message.name, &stack_size);
                
                /* Option No.1a - Run User CGI function here. */
                if (user_function && (stack_size == 0))
                {
                    httpsrv_call_cgi((HTTPSRV_CGI_CALLBACK_FN) user_function, &message);
                    httpsrv_ses_flush(message.session);
                }
                break;

            case HTTPSRV_SSI_CALLBACK:
                table = (HTTPSRV_FN_LINK_STRUCT*) server->params.ssi_lnk_tbl;

                /* Set separator to null character temporarily. */
                separator = strchr(message.name, ':');
                if (separator != NULL)
                {
                    *separator = '\0';
                }

                user_function = httpsrv_find_callback(table, message.name, &stack_size);

                if (separator != NULL)
                {
                    *separator = ':';
                }

                /* Option No.1b - Run User SSI function here. */
                if ((user_function != NULL) && (stack_size == 0))
                {
                    httpsrv_call_ssi((HTTPSRV_SSI_CALLBACK_FN) user_function, &message);
                    httpsrv_ses_flush(message.session);
                }
                break;

            default:
                break;
        }

        /* Option No.2 Run script in detached task. */
        if (user_function && (stack_size > 0))
        {
            HTTPSRV_DET_TASK_PARAM task_params;

            task_params.session = message.session;
            task_params.user_function = user_function;
            task_params.stack_size = stack_size;
            task_params.type = message.type;
            task_params.script_name = message.name;
            httpsrv_detach_script(&task_params);
        }
        _task_ready(_task_get_td(message.ses_tid));
    }
    
    if (server->script_msgq != NULL)
    {
        _lwmsgq_deinit(server->script_msgq);
        _mem_free(server->script_msgq);
    }

EXIT:
    server->script_tid = 0;
}

/*
** Run user script separately with independent stack. Each session can create
** such task. This makes parallel script processing possible. If session
** timeout occurs durring script processing, this task is destroyed.
*/
void httpsrv_detached_task(void *param, void *creator)
{
    HTTPSRV_SESSION_STRUCT* session;
    HTTPSRV_FN_CALLBACK     user_function;
    HTTPSRV_DET_TASK_PARAM* task_params = (HTTPSRV_DET_TASK_PARAM*) param;
    HTTPSRV_CALLBACK_TYPE   type;
    _mqx_uint               error;
    HTTPSRV_SCRIPT_MSG      call_param;

    session = task_params->session;
    session->script_tid = _task_get_id();
    user_function = task_params->user_function;
    type = task_params->type;
    call_param.session = task_params->session;
    call_param.name = task_params->script_name;

    /* Lock session, so socket is accessible as long as required by user function. */
    error = _lwsem_wait(&session->lock);
    if (error != MQX_OK)
    {
        RTCS_task_resume_creator(creator, error);
    }
    RTCS_task_resume_creator(creator, MQX_OK);

    switch (type)
    {
        case HTTPSRV_CGI_CALLBACK:
            httpsrv_call_cgi((HTTPSRV_CGI_CALLBACK_FN) user_function, &call_param);
            httpsrv_ses_flush(session);
            break;

        case HTTPSRV_SSI_CALLBACK:
            httpsrv_call_ssi((HTTPSRV_SSI_CALLBACK_FN) user_function, &call_param);
            httpsrv_ses_flush(session);
            break;

        default:
            break;
    }

    session->script_tid = 0;
    /* Callback returned, we can unlock the session. */
    _lwsem_post(&session->lock);
}

/*
** HTTP session state machine
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
void httpsrv_http_process(void *server_ptr, void *session_ptr)
{
    uint32_t               time_interval;
    int                    result;
    HTTPSRV_STRUCT         *server = (HTTPSRV_STRUCT*) server_ptr;
    HTTPSRV_SESSION_STRUCT *session = (HTTPSRV_SESSION_STRUCT*) session_ptr;  
    
    time_interval = 0;  
    if (!session->valid)
    {
        session->state = HTTPSRV_SES_CLOSE;
        return;
    }
    else
    {
        uint32_t time_now;
        /* check session timeout */
        time_now = RTCS_time_get();
        time_interval = RTCS_timer_get_interval(session->time, time_now);
        if (time_interval > session->timeout)
        {
            if (session->script_tid != 0)
            {
                _task_destroy(session->script_tid);
                session->script_tid = 0;
            }
            session->state = HTTPSRV_SES_CLOSE;
        }
    }
        
    switch (session->state)
    {
        case HTTPSRV_SES_WAIT_REQ:
            result = httpsrv_readreq(server, session);
            
            if ((result < 0) && (!session->keep_alive) && (HTTPSRV_SES_CLOSE != session->state))
            {
                session->state = HTTPSRV_SES_END_REQ;
            }
            else if ((result == 0) && (session->request.process_header == 0))
            {
                session->state = HTTPSRV_SES_PROCESS_REQ;
            }
            break;
        case HTTPSRV_SES_PROCESS_REQ:
            httpsrv_processreq(server, session);
            break;

        case HTTPSRV_SES_RESP:
            httpsrv_response(server, session);
            session->time = RTCS_time_get();
            break;

        case HTTPSRV_SES_END_REQ:
            if (!session->keep_alive)
            {
                session->state = HTTPSRV_SES_CLOSE;
            }
            else
            {
                httpsrv_wait_for_cgi(session);
                /* Re-init session */
                session->state = HTTPSRV_SES_WAIT_REQ;
                if (session->response.file)
                {
                    fclose(session->response.file);
                }
                _mem_zero(&session->response, sizeof(session->response));
                session->request.process_header = 1;
                if (session->request.auth.user_id != NULL)
                {
                    _mem_free(session->request.auth.user_id);
                }
                session->request.auth.user_id = NULL;
                session->request.auth.password = NULL;
                session->time = RTCS_time_get();
                session->timeout = HTTPSRVCFG_KEEPALIVE_TO;
                session->keep_alive = 1;
            }
            break;
        case HTTPSRV_SES_CLOSE:
            /*
             * If session did not time out; wait for lock. If waiting
             * for lock timed-out, check session timeout again.
             */
            if (time_interval < session->timeout)
            {
                if (httpsrv_wait_for_cgi(session) == MQX_LWSEM_WAIT_TIMEOUT)
                {
                    break;
                }
            }
            httpsrv_ses_close(session);
            session->valid = FALSE;
            break;
        default:
            /* invalid state */
            session->valid = FALSE;
            break;
    }
}

/*
** Function for request parsing
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      int - zero if request is valid, negative value if invalid.
*/
static int httpsrv_readreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    char* line_start = session->buffer.data;
    char *query_str;
    int read;
    char* line_end = NULL;
    uint32_t unprocessed_size = 0;
    uint32_t req_lines = 0;
    
    /* Begin */
    while (session->request.process_header)
    {
        /* Read data */
        read = httpsrv_recv(session, session->buffer.data+session->buffer.offset, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset, 0);
        if ((read == RTCS_ERROR) || (read == 0))
        {
            uint32_t error;
            
            error = RTCS_geterror(session->sock);
            /* Client closed connection so we close it too */
            if (error == RTCSERR_TCP_CONN_CLOSING)
            {
                session->keep_alive = 0;
                session->state = HTTPSRV_SES_CLOSE;
            }
            /*
             * If reading timed out, set number of read bytes to 0
             * to prevent session closing.
             */
            else if (error == RTCSERR_TCP_TIMED_OUT)
            {
               read = 0;
            }
            break;
        }
        unprocessed_size += read;

        /* Process buffer line by line. End of line is \n or \r\n */
        while (session->request.process_header && ((line_end = strchr(line_start, '\n')) != NULL))
        {
            /* Null terminate the line */
            *line_end = '\0';
            if ((line_end != session->buffer.data) && (*(line_end-1) == '\r'))
            {
                *(line_end-1) = '\0';
            }
            req_lines++;
            /* Substract line length from size of unprocessed data */
            unprocessed_size -= (line_end-line_start+1);

            /* Found an empty line => end of header */
            if (strlen(line_start) == 0)
            {
                session->request.process_header = FALSE;
            }
            else
            {
                if (req_lines == 1) 
                {
                    httpsrv_process_req_method(server, session, line_start);
                }
                else
                {
                    httpsrv_process_req_hdr_line(server, session, line_start);
                }
            }
            /* Set start of next line after end of current line */
            line_start = line_end+1;
            /* Check buffer boundary */
            if (line_start > (session->buffer.data+HTTPSRV_SES_BUF_SIZE_PRV))
            {
                line_start = session->buffer.data+HTTPSRV_SES_BUF_SIZE_PRV;
            }
        }
        /* There were no valid lines in buffer */
        if ((unprocessed_size >= HTTPSRV_SES_BUF_SIZE_PRV) && (req_lines == 0))
        {
            session->buffer.offset = 0;
            unprocessed_size = 0;
        }
        /* We are not at the end of buffer and there are some data after the end of last line. */
        else if (unprocessed_size != 0)
        {
            /* Copy rest of data to begining of buffer and save offset for next reading. */
            memmove(session->buffer.data, line_start, unprocessed_size);
            line_start = session->buffer.data;
            session->buffer.offset = unprocessed_size; 
        }
        /* Clear the buffer so we don't have some old data there. */
        _mem_zero(session->buffer.data+session->buffer.offset, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset);
    }
    /* End */

    /* Save query string */
    query_str = strchr(session->request.path, '?');
    if (NULL != query_str)
    {
        *query_str = '\0';
        query_str++;
        session->request.query = query_str;
    }

    return (0 > read) ? read : 0;
}

/*
** Function for request processing
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/

static void httpsrv_processreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    char *suffix;
    char* root_dir = NULL;
    char* full_path;
    char* new_path;

    session->state = HTTPSRV_SES_RESP;

    /* Check request validity */
    if (httpsrv_check_request(session) != HTTPSRV_OK)
    {
        return;
    }

    /* Check authentication */
    session->response.auth_realm = httpsrv_req_realm(server, session->request.path);
    if (session->response.auth_realm != NULL)
    {
        if (!httpsrv_check_auth(session->response.auth_realm, &session->request.auth))
        {
            session->response.status_code = HTTPSRV_CODE_UNAUTHORIZED;
            if (session->request.auth.user_id != NULL)
            {
                _mem_free(session->request.auth.user_id);
                session->request.auth.user_id = NULL;
                session->request.auth.password = NULL;
            }
            return;
        }
    }
    
    /* Get rid of alias if there is any */
    new_path = httpsrv_unalias(server->params.alias_tbl, session->request.path, &root_dir);
    if (new_path != session->request.path)
    {
        memmove(session->request.path, new_path, strlen(new_path)+1);
    }

    if (root_dir == NULL)
    {
        root_dir = server->params.root_dir;
    }

    /* Check if resource is plugin */
    session->plugin = httpsrv_get_plugin(server->params.plugins, session->request.path);
    if (session->plugin != NULL)
    {
        httpsrv_process_plugin(session);
        return;
    }

    /* Check if requested resource is CGI script */
    if ((suffix = strrchr(session->request.path, '.')) != 0)
    {
        if ((0 == strcasecmp(suffix, ".cgi")) && server->params.script_stack)
        {
            *suffix = '\0';
            httpsrv_process_cgi(server, session, session->request.path+1); /* +1 because of slash */
            *suffix = '.';
            session->state = HTTPSRV_SES_END_REQ;
            return;
        }
    }

    /* If client requested root, set requested path to index page */
    if (session->request.path[0] == '/' && session->request.path[1] == '\0')
    {
        uint32_t offset;
        uint32_t length;
        char*    index = server->params.index_page;
        uint32_t max_length = server->params.max_uri;

        length = strlen(index);
        if (length > max_length)
        {
            length = max_length;
        }

        offset = ((index[0] == '\\') || (index[0] == '/')) ? 1 : 0;
        _mem_copy(index+offset, session->request.path+1, length);
    }

    /* Get full file path */
    full_path = httpsrv_get_full_path(session->request.path, root_dir);
    if (full_path == NULL)
    {
        session->response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
        return;
    }

    session->response.file = fopen(full_path, "r");
    session->response.len = 0;
    if (!session->response.file)
    {
        session->response.status_code = HTTPSRV_CODE_NOT_FOUND;
    }
    _mem_free(full_path);
}

/*
** Function for HTTP sending response
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/

static void httpsrv_response(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{   
    switch (session->response.status_code)
    {
        case HTTPSRV_CODE_UPGRADE:
            if (session->request.upgrade_to == HTTPSRV_WS_PROTOCOL)
            {
                ws_handshake(session->ws_handshake);
            }
            session->process_func = httpsrv_plugin_run;
            httpsrv_sendhdr(session, 0, 0);
            break;
        case HTTPSRV_CODE_OK:
            if (session->request.method == HTTPSRV_REQ_HEAD)
            {
                #if MQX_USE_IO_OLD
                httpsrv_sendhdr(session, session->response.file->SIZE, 0);
                #else
                httpsrv_sendhdr(session, httpsrv_fsize(session->response.file), 0);
                #endif
                session->state = HTTPSRV_SES_END_REQ;
            }
            else
            {
                httpsrv_sendfile(server, session);
            }
            break;
        case HTTPSRV_CODE_UNAUTHORIZED:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "401 Unauthorized", "Unauthorized!");
            break;
        case HTTPSRV_CODE_FORBIDDEN:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "403 Forbidden", "Forbidden!");
            break;
        case HTTPSRV_CODE_URI_TOO_LONG:
            session->keep_alive = 0;
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "414 Uri too long", "Requested URI too long!");
            break;
        case HTTPSRV_CODE_UPGRADE_REQUIRED:
        case HTTPSRV_CODE_BAD_REQ:
        case HTTPSRV_CODE_NO_LENGTH:
        case HTTPSRV_CODE_INTERNAL_ERROR:
        case HTTPSRV_CODE_NOT_IMPLEMENTED:
            session->keep_alive = 0;
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_sendhdr(session, 0, 0);
            break;
        default:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "404 Not Found", "Requested URL not found!");
            break;
    }
}

/*
** Function for session allocation
**
** IN:
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      HTTPSRV_SESSION_STRUCT* - pointer to allocated session. Non-zero if allocation was OK, NULL otherwise
*/
static HTTPSRV_SESSION_STRUCT* httpsrv_ses_alloc(HTTPSRV_STRUCT *server)
{
    HTTPSRV_SESSION_STRUCT *session = NULL;

    if (server)
    {
        session = _mem_alloc_zero(sizeof(HTTPSRV_SESSION_STRUCT));
        if (session)
        {
            _mem_set_type(session, MEM_TYPE_HTTPSRV_SESSION_STRUCT);
            /* Alloc URI */
            session->request.path = _mem_alloc_zero(server->params.max_uri + 1);
            if (NULL == session->request.path)
            {
                goto ERROR;
            } 
            _mem_set_type(session->request.path, MEM_TYPE_HTTPSRV_URI);
            /* Alloc session buffer */
            session->buffer.data = _mem_alloc_zero(sizeof(char)*HTTPSRV_SES_BUF_SIZE_PRV);
            if (NULL == session->buffer.data)
            {
                goto ERROR;
            }
        }
    }

    return session;

ERROR:
    if (session->request.path)
    {
        _mem_free(session->request.path);
        session->request.path = NULL;
    }
    if (session->buffer.data)
    {
        _mem_free(session->buffer.data);
        session->buffer.data = NULL;
    }
    _mem_free(session);
    return NULL;
}

/*
** Function used to free session structure
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_free(HTTPSRV_SESSION_STRUCT *session)
{
    if (session)
    {
        if (session->request.path)
        {
            _mem_free(session->request.path);
        }
        if(session->request.auth.user_id)
        {
            _mem_free(session->request.auth.user_id);
        }
        if(session->buffer.data)
        {
            _mem_free(session->buffer.data);
        }
        if (session->ws_handshake)
        {
            _mem_free(session->ws_handshake);
        }
        _mem_free(session);
    }
}

/*
** Function used to init session structure
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters)
**      const int sock - socket handle used for communication with client
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_init(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, const int sock)
{
    if (server && session)
    {
    #if RTCSCFG_ENABLE_SSL
        if (server->ssl_ctx != 0)
        {
            session->ssl_sock = RTCS_ssl_socket(server->ssl_ctx, sock);
        }
    #endif
        session->state = HTTPSRV_SES_WAIT_REQ;
        session->sock = sock;
        session->valid = TRUE;
        session->request.process_header = 1;
        session->timeout = HTTPSRVCFG_SES_TO;
        session->keep_alive_enabled = HTTPSRVCFG_KEEPALIVE_ENABLED;
        session->keep_alive = session->keep_alive_enabled;
        session->time = RTCS_time_get();
        session->process_func = httpsrv_http_process;
        _lwsem_create(&session->lock, 1);
    }
}

/*
** Function used to close session
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_close(HTTPSRV_SESSION_STRUCT *session)
{
    if (session != NULL)
    {
        if (session->response.file)
        {
            fclose(session->response.file);
            session->response.file = NULL;
        }
    #if RTCSCFG_ENABLE_SSL
        if (session->ssl_sock != 0)
        {
            RTCS_ssl_shutdown(session->ssl_sock);
        }
    #endif
        if (session->sock != RTCS_SOCKET_ERROR)
        {
            shutdown(session->sock, FLAG_CLOSE_TX);
        }
        _lwsem_destroy(&session->lock);
    }
}

/*
** Run plugin - start plugin handler task send message to invoke plugin
** functions.
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_plugin_run(void *server_ptr, void *session_ptr)
{
    HTTPSRV_SESSION_STRUCT    *session = (HTTPSRV_SESSION_STRUCT*) session_ptr;
    HTTPSRV_STRUCT            *server = (HTTPSRV_STRUCT*) server_ptr;
    HTTPSRV_PLUGIN_MSG        message;
    bool                      plugin_blocked;

    plugin_blocked = false;

    /* If plugin handler task is not running -> start it. */
    if (server->plugin_tid == 0)
    {
        HTTPSRV_PLUGIN_TASK_PARAM params;

        params.server = server;
        if (RTCS_task_create(HTTPSRV_PLUGIN_HANDLER_NAME, server->params.server_prio, 
                    #if RTCSCFG_ENABLE_SSL
                                (server->ssl_ctx != NULL) ? HTTPSRV_SSL_SESSION_STACK_SIZE : HTTPSRV_SESSION_STACK_SIZE, 
                    #else
                                HTTPSRV_SSL_SESSION_STACK_SIZE,
                    #endif
                                httpsrv_plugin_task, &params) != RTCS_OK)
        {
            goto ERROR;
        }
        plugin_blocked = true;
    }

    /* Transfer session buffer to plugin handler. */
    if (_mem_transfer(session->buffer.data, _task_get_id(), server->plugin_tid) != MQX_OK)
    {
        goto ERROR;
    }

    /* Send message to plugin handler - start plugin servicing. */
    message.sock = session->sock;
    message.buffer = (uint8_t *) session->buffer.data;
    message.plugin = session->plugin;
    message.callbacks = session->p_callbacks;

    if (_lwmsgq_send(server->plugin_queue, (_mqx_max_type *) &message, LWMSGQ_SEND_BLOCK_ON_FULL) != MQX_OK)
    {
        goto ERROR;
    }

    if (plugin_blocked)
    {
        _task_ready(_task_get_td(server->plugin_tid));
    }
    /* 
     * Memory has been transfered to plugin task, so set buffer data pointer to 
     * NULL to prevent deallocation during session freeing.
     */
    session->buffer.data = NULL;
    session->sock = RTCS_SOCKET_ERROR;
    /* 
     * Terminate session task. It is no longer needed. Plugin task is ran 
     * instead
     */
ERROR:
    session->valid = FALSE;
}

/*
 * Task for handling plugins.
 */
static void httpsrv_plugin_task(void *init_ptr, void *creator)
{
    HTTPSRV_PLUGIN_TASK_PARAM *params;
    HTTPSRV_STRUCT            *server;
    HTTPSRV_PLUGIN_MSG        message;
    /* There is only one plugin now, so only one context is created. */
    void                      *pl_context;
    HTTPSRV_PLUGIN_CALLBACKS  *pl_callbacks;

    params = (HTTPSRV_PLUGIN_TASK_PARAM *) init_ptr;
    server = params->server;
    server->plugin_tid = _task_get_id();
    pl_context = NULL;
    pl_callbacks = NULL;

    /* Create queue for plugin messages. */
    server->plugin_queue = RTCS_mem_alloc_zero(sizeof(LWMSGQ_STRUCT) + HTTPSRV_PLUGIN_NUM_MESSAGES*sizeof(HTTPSRV_PLUGIN_MSG)*sizeof(_mqx_max_type));
    if (server->plugin_queue == NULL)
    {
        RTCS_task_resume_creator(creator, (uint32_t)RTCS_ERROR);
        return;
    }

    if (MQX_OK != _lwmsgq_init(server->plugin_queue, HTTPSRV_PLUGIN_NUM_MESSAGES, sizeof(HTTPSRV_PLUGIN_MSG)/sizeof(_mqx_max_type)))
    {
        _mem_free(server->plugin_queue);
        server->plugin_queue = NULL;
        RTCS_task_resume_creator(creator, (uint32_t)RTCS_ERROR);
        return;
    }

    RTCS_task_resume_creator(creator, RTCS_OK);
    /* 
     * Block plugin task. It will be unblocked by session task that created it 
     * when first message is send to message queue.
     */
    _task_block();

    /* Process messages - run plugin. */
    while(1)
    {
        uint32_t error;

        error = _lwmsgq_receive(server->plugin_queue, (_mqx_max_type *) &message, LWMSGQ_TIMEOUT_FOR, 0, NULL);

        if (error == MQX_OK)
        {
            pl_callbacks = message.callbacks;

            if (pl_context == NULL)
            {
                pl_context = pl_callbacks->create(&message);
            }
            /* Allocation failed. */
            if (pl_context == NULL)
            {
                httpsrv_plugin_fail(&message);
                continue;
            }
            pl_callbacks->process(&message, pl_context);
            
            /* If plugin is set to NULL terminate plugin handler task. */
            if (message.plugin == NULL)
            {
                break;
            }
        }
        else if (error == LWMSGQ_EMPTY)
        {
            /* Wait for activity. */
            if(pl_callbacks)
            {
                pl_callbacks->wait(pl_context);
            }
        }        
        else
        {
            /* Some error occurred, terminate plugin task. */
            break;
        }
    }

    /* Cleanup */
    if (pl_callbacks)
    {
        pl_callbacks->destroy(pl_context);
    }
    _lwmsgq_deinit(server->plugin_queue);
    _mem_free(server->plugin_queue);
    server->plugin_queue = NULL;
    server->plugin_tid = 0;
}

/*
 * Cleanup in case plugin creation fails
 */
static void httpsrv_plugin_fail(HTTPSRV_PLUGIN_MSG *msg)
{
    shutdown(msg->sock, FLAG_CLOSE_TX);
    _mem_free(msg->buffer);
}
