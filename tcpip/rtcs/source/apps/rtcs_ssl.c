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
#include <cyassl/ssl.h>
#endif

/* Create SSL connection using sock, return SSL connection handle. */
uint32_t RTCS_ssl_socket(void* ctx, uint32_t sock)
{
    uint32_t   retval = RTCS_SOCKET_ERROR;

#if RTCSCFG_ENABLE_SSL
    CYASSL_CTX *ssl_ctx;
    CYASSL     *ssl_sock;
    
    ssl_ctx = (CYASSL_CTX *) ctx;
    
    /* Create the CYASSL object after TCP connect.*/
    if ((ssl_sock = CyaSSL_new(ssl_ctx)) != NULL)
    {
        /* Associate the socket (file descriptor) with the session.*/
        if (CyaSSL_set_fd(ssl_sock, sock) == SSL_SUCCESS)
        {
            retval = (uint32_t) ssl_sock;
        }
        else
        {
            CyaSSL_free(ssl_sock);
        }
    }
#endif

    return(retval);
}

/* Shutdown SSL connection. */
uint32_t RTCS_ssl_shutdown(uint32_t ssl_sock)
{
    uint32_t   retval = RTCS_SOCKET_ERROR;

#if RTCSCFG_ENABLE_SSL
    if((ssl_sock != RTCS_SOCKET_ERROR) && (CyaSSL_shutdown((CYASSL *)ssl_sock) == SSL_SUCCESS))
    {
        CyaSSL_free((CYASSL *)ssl_sock);
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
        retval = CyaSSL_recv((CYASSL *) ssl_sock, buf, len, flags);
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
        retval = CyaSSL_send((CYASSL *) ssl_sock, buf, len, flags);
    }
#endif
    return retval;
}

/* Initialize CyaSSL */
void* RTCS_ssl_init(RTCS_SSL_PARAMS_STRUCT *params)
{
    void* retval = NULL;

#if RTCSCFG_ENABLE_SSL
    CYASSL_CTX *ctx = NULL;

    if(params)
    {
        /* CyaSSL_Init(); Called by CyaSSL_CTX_new() */

    switch (params->init_type)
    {
        case RTCS_SSL_SERVER:
            /* Server method call. Allow use highest possible version from SSLv3 - TLS 1.2*/
            ctx = CyaSSL_CTX_new(CyaSSLv23_server_method());
            break;
        case RTCS_SSL_CLIENT:
            /* Client method call. Use highest possible version from SSLv3 - TLS 1.2*/
            ctx = CyaSSL_CTX_new(CyaSSLv23_client_method());
            break;
        default:
            break;
    }

        if (ctx)
        {
                /* Loading Client or Server Certificate chane.*/
            if((params->cert_file && (CyaSSL_CTX_use_certificate_chain_file(ctx, params->cert_file) != SSL_SUCCESS))
                /* Loading Private Key into the SSL context. */
                ||(params->priv_key_file && (CyaSSL_CTX_use_PrivateKey_file(ctx, params->priv_key_file, SSL_FILETYPE_PEM) != SSL_SUCCESS))
                /* Loading CA (Certificate Authority), or trusted root,  certificates.*/
                /* Used to verify certs received from peers during the SSL handshake.*/
                ||(params->ca_file && (CyaSSL_CTX_load_verify_locations(ctx, params->ca_file, NULL) != SSL_SUCCESS)) )
            {
                /* Error of loading.*/
                CyaSSL_CTX_free(ctx);
                ctx = NULL;
            }
        }

    retval = (void *) ctx;
    }
#endif

    return(retval);
}

/* Release CyaSSL */
void RTCS_ssl_release(void *ctx)
{
#if RTCSCFG_ENABLE_SSL
    if(ctx)
    {
        /*  Free the CYASSL_CTX object.*/
        CyaSSL_CTX_free((CYASSL_CTX *)ctx);

        /* CyaSSL_Cleanup(); Doesn’t have to be called: http://www.yassl.com/yaSSL/Docs-cyassl-manual-17-1-cyassl-api-init-shutdown.html */
    }
#endif
}
