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
*   SSL adaptation layer for RTCS
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_SSL
#include <ssl.h>
#endif

#include <mutex.h>

/* number of times the wrapper attemts to call:
   - WolfSSL_accept()
   - WolfSSL_recv() during shutdown sequence
   before it gives up and error is reported to the upper layer
 */
#define MAX_RTCS_SSL_REPEAT_TRIALS 10

/*
 * This mutext is a workaround for MMCAU library calls.
 * RTCS has multithreaded HTTP server, so multiple tasks(threads) might want to access MMCAU
 * at the same time.
 * Once this will be solved by a cryptograpic library, (that will protect hardware accelerator
 * with a mutex), g_ssl_mutex in this RTCS wrapper can be removed.
 */
static MUTEX_ATTR_STRUCT g_mutexattr;
static MUTEX_STRUCT g_ssl_mutex = {0};

/* Create SSL connection using sock, return SSL connection handle. */
uint32_t RTCS_ssl_socket(void* ctx, uint32_t sock)
{
    uint32_t   retval = RTCS_SOCKET_ERROR;

#if RTCSCFG_ENABLE_SSL
    WOLFSSL_CTX *ssl_ctx;
    WOLFSSL     *ssl_sock;
    int i;

    ssl_ctx = (WOLFSSL_CTX *) ctx;

    /* Create the WOLFSSL object after TCP connect.*/
    _mutex_lock(&g_ssl_mutex);
    if ((ssl_sock = wolfSSL_new(ssl_ctx)) != NULL)
    {
        /* Associate the socket (file descriptor) with the session.*/
        if (wolfSSL_set_fd(ssl_sock, sock) == SSL_SUCCESS)
        {
#if 1
          for (i=0; i<MAX_RTCS_SSL_REPEAT_TRIALS; i++)
          {
            if (SSL_SUCCESS == wolfSSL_accept(ssl_sock))
            {
                retval = (uint32_t) ssl_sock;
                break;
            }
            else
            {
                uint32_t sockerr = RTCS_geterror(sock);
                if (RTCSERR_TCP_TIMED_OUT != sockerr)
                {
                    uint32_t soerr;
                    uint32_t sosz = sizeof(soerr);
                    getsockopt(sock, SOL_SOCKET, SO_ERROR, &soerr, &sosz);
                    wolfSSL_free(ssl_sock);
                    break;
                }
                _mutex_unlock(&g_ssl_mutex);
                _sched_yield();
                _mutex_lock(&g_ssl_mutex);
            }
          }

          if (i == MAX_RTCS_SSL_REPEAT_TRIALS)
          {
              wolfSSL_free(ssl_sock);
          }
#else
            retval = (uint32_t) ssl_sock;
#endif
        }
        else
        {
            wolfSSL_free(ssl_sock);
        }
    }
    _mutex_unlock(&g_ssl_mutex);
#endif
    return(retval);
}

/* Shutdown SSL connection. */
uint32_t RTCS_ssl_shutdown(uint32_t ssl_sock)
{
    uint32_t   retval = RTCS_SOCKET_ERROR;

#if RTCSCFG_ENABLE_SSL
    char buf[80];
    uint32_t sock;
    uint32_t sockerr;
    int cnt = MAX_RTCS_SSL_REPEAT_TRIALS;

    if (ssl_sock != RTCS_SOCKET_ERROR)
    {
        /* Do recv() to catch possible "close notify". Firefox 35.0 does this.
         * It returns also when peer closes connection or tcp receive timeout elapses.
         *
         */
        _mutex_lock(&g_ssl_mutex);
        while((0 < wolfSSL_recv((WOLFSSL *) ssl_sock, buf, 80, 0)) && (cnt > 0))
        { 
            cnt--;
        };
        sock = (uint32_t)wolfSSL_get_fd((WOLFSSL *) ssl_sock);
        sockerr = RTCS_geterror(sock);
        if (RTCSERR_TCP_TIMED_OUT == sockerr)
        {
            uint32_t soerr;
            uint32_t sosz = sizeof(soerr);
            /* reading socket error also clears the error code from the socket,
               * which is the solely purpose of this getsockopt() function call.
               * because upper layer typically repeats recv/send request if it sees RTCSERR_TCP_TIMED_OUT
               * be we want to shutdown and close in this case.
               */
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &soerr, &sosz);
        }

        wolfSSL_free((WOLFSSL *)ssl_sock);
        _mutex_unlock(&g_ssl_mutex);
        retval = RTCS_OK;
    }
#endif

    return retval;
}

/* Receive data from SSL layer - return error, SSL is disabled. */
int32_t RTCS_ssl_recv(uint32_t ssl_sock, void *buf, uint32_t len, uint32_t flags)
{
    int32_t     retval = RTCS_ERROR;

#if RTCSCFG_ENABLE_SSL
    if(ssl_sock != RTCS_SOCKET_ERROR)
    {
        _mutex_lock(&g_ssl_mutex);
        retval = wolfSSL_recv((WOLFSSL *) ssl_sock, buf, len, flags);
        _mutex_unlock(&g_ssl_mutex);
    }

    /* Translate WolfSSL return value to RTCS return value.
     * This is opposite to WolfSSL's function TranslateReturnCode() that is used for recv() calls.
     */
    if (0 == retval)
    {
      uint32_t    sock;
      uint32_t    sockerr;
      int cyasslerr = wolfSSL_get_error((WOLFSSL *) ssl_sock, 0);
      sock = (uint32_t)wolfSSL_get_fd((WOLFSSL *) ssl_sock);
      sockerr = RTCS_geterror(sock);
      if ((RTCSERR_TCP_CONN_RLSD == sockerr) || (RTCSERR_TCP_CONN_RESET == sockerr))
      {
        return -1;
      }
      
      /* return code zero in case peer closes connection or sends "close notify" */
      if ((RTCSERR_TCP_CONN_CLOSING == sockerr) || (SSL_ERROR_ZERO_RETURN == cyasslerr))
      {
          retval = -1;

          if (RTCSERR_TCP_TIMED_OUT == sockerr)
          {
              uint32_t soerr;
              uint32_t sosz = sizeof(soerr);
              /* reading socket error also clears the error code from the socket,
               * which is the solely purpose of this getsockopt() function call.
               * because upper layer typically repeats recv/send request if it sees RTCSERR_TCP_TIMED_OUT
               * be we want to shutdown and close in this case.
               */
              getsockopt(sock, SOL_SOCKET, SO_ERROR, &soerr, &sosz);
          }
      }
    }
    if (retval < 0)
    {
        retval = -1;
    }

#endif
    return retval;
}

/* Send data through SSL layer - return error, SSL is disabled. */
int32_t RTCS_ssl_send(uint32_t ssl_sock, void *buf, uint32_t len, uint32_t flags)
{
    int32_t     retval = RTCS_ERROR;
#if RTCSCFG_ENABLE_SSL
    if(ssl_sock != RTCS_SOCKET_ERROR)
    {
        _mutex_lock(&g_ssl_mutex);
        retval = wolfSSL_send((WOLFSSL *) ssl_sock, buf, len, flags);
        _mutex_unlock(&g_ssl_mutex);
    }

    /* Translate WolfSSL return value to RTCS return value.
     * This is opposite to WolfSSL's function TranslateReturnCode() that is used for send() calls.
     */
    if (0 == retval)
    {
      uint32_t    sock;
      uint32_t    sockerr;
      int cyasslerr = wolfSSL_get_error((WOLFSSL *) ssl_sock, 0);
      sock = (uint32_t)wolfSSL_get_fd((WOLFSSL *) ssl_sock);
      sockerr = RTCS_geterror(sock);
      if ((RTCSERR_TCP_CONN_RLSD == sockerr) || (RTCSERR_TCP_CONN_RESET == sockerr))
      {
        return -1;
      }
      
      /* return code zero in case peer closes connection or sends "close notify" */
      if ((RTCSERR_TCP_CONN_CLOSING == sockerr) || (SSL_ERROR_ZERO_RETURN == cyasslerr))
      {
          retval = -1;

          if (RTCSERR_TCP_TIMED_OUT == sockerr)
          {
              uint32_t soerr;
              uint32_t sosz = sizeof(soerr);
              /* reading socket error also clears the error code from the socket,
               * which is the solely purpose of this getsockopt() function call.
               * because upper layer typically repeats recv/send request if it sees RTCSERR_TCP_TIMED_OUT
               * be we want to shutdown and close in this case.
               */
              getsockopt(sock, SOL_SOCKET, SO_ERROR, &soerr, &sosz);
          }
      }
    }
    if (retval < 0)
    {
      retval = -1;
    }
#endif
    return retval;
}

/* Initialize WolfSSL */
void* RTCS_ssl_init(RTCS_SSL_PARAMS_STRUCT *params)
{
    void* retval = NULL;

#if RTCSCFG_ENABLE_SSL
    WOLFSSL_CTX *ctx = NULL;

    if(params)
    {
        /* WolfSSL_Init(); Called by WolfSSL_CTX_new() */

    switch (params->init_type)
    {
        case RTCS_SSL_SERVER:
            /* Server method call. Allow use highest possible version from SSLv3 - TLS 1.2*/
            ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
            break;
        case RTCS_SSL_CLIENT:
            /* Client method call. Use highest possible version from SSLv3 - TLS 1.2*/
            ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
            break;
        default:
            break;
    }

        if (ctx)
        {
                /* Loading Client or Server Certificate chane.*/
            if((params->cert_file && (wolfSSL_CTX_use_certificate_chain_file(ctx, params->cert_file) != SSL_SUCCESS))
                /* Loading Private Key into the SSL context. */
                ||(params->priv_key_file && (wolfSSL_CTX_use_PrivateKey_file(ctx, params->priv_key_file, SSL_FILETYPE_PEM) != SSL_SUCCESS))
                /* Loading CA (Certificate Authority), or trusted root,  certificates.*/
                /* Used to verify certs received from peers during the SSL handshake.*/
                ||(params->ca_file && (wolfSSL_CTX_load_verify_locations(ctx, params->ca_file, NULL) != SSL_SUCCESS)) )
            {
                /* Error of loading.*/
                wolfSSL_CTX_free(ctx);
                ctx = NULL;
            }
        }

    retval = (void *) ctx;
    }
#endif

    /* SSL mutex is MCU wide
     */
    if (_mutex_get_wait_count(&g_ssl_mutex) == MAX_MQX_UINT)
    {
        _mutatr_init(&g_mutexattr);
        _mutex_init(&g_ssl_mutex, &g_mutexattr);
    }

    return(retval);
}

/* Release WolfSSL */
void RTCS_ssl_release(void *ctx)
{
#if RTCSCFG_ENABLE_SSL
    if(ctx)
    {
        /*  Free the CYASSL_CTX object.*/
        wolfSSL_CTX_free((WOLFSSL_CTX *)ctx);

        /* WolfSSL_Cleanup(); Doesn’t have to be called: http://www.yassl.com/yaSSL/Docs-cyassl-manual-17-1-cyassl-api-init-shutdown.html */
    }
#endif
}
