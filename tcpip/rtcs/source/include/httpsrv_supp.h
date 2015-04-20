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
*   HTTPSRV support functions header.
*
*
*END************************************************************************/

#ifndef HTTP_SUPP_H_
#define HTTP_SUPP_H_

#define ERR_PAGE_FORMAT "<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n<BODY><H1>%s</H1>\n</BODY></HTML>\n"
#include "httpsrv_prv.h"
#include "httpsrv.h"

#ifdef __cplusplus
extern "C" {
#endif

_mqx_int httpsrv_read(HTTPSRV_SESSION_STRUCT *session, char *dst, _mqx_int len);
_mqx_int httpsrv_write(HTTPSRV_SESSION_STRUCT *session, char *src, _mqx_int len);
int32_t httpsrv_ses_flush(HTTPSRV_SESSION_STRUCT *session);

void httpsrv_sendhdr(HTTPSRV_SESSION_STRUCT *session, _mqx_int content_len, bool has_entity);
void httpsrv_sendfile(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
void httpsrv_send_err_page(HTTPSRV_SESSION_STRUCT *session, const char* title, const char* text);
void httpsrv_process_req_hdr_line(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT* session, char* buffer);
void httpsrv_process_req_method(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, char* buffer);
HTTPSRV_AUTH_REALM_STRUCT* httpsrv_req_realm(HTTPSRV_STRUCT *server, char* path);
int httpsrv_check_auth(HTTPSRV_AUTH_REALM_STRUCT* realm, HTTPSRV_AUTH_USER_STRUCT* user);
uint32_t httpsrv_check_request(HTTPSRV_SESSION_STRUCT *session);
char* httpsrv_unalias(HTTPSRV_ALIAS* table, char* path, char** new_root);
char* httpsrv_get_full_path(char* path, char* root);
int32_t httpsrv_destroy_server(HTTPSRV_STRUCT* server);
HTTPSRV_STRUCT* httpsrv_create_server(HTTPSRV_PARAM_STRUCT* params);
HTTPSRV_PLUGIN_STRUCT* httpsrv_get_plugin(HTTPSRV_PLUGIN_LINK_STRUCT* table, char* resource);
void httpsrv_process_plugin(HTTPSRV_SESSION_STRUCT *session);
uint32_t httpsrv_recv(HTTPSRV_SESSION_STRUCT *session, char *buffer, uint32_t length, uint32_t flags);
uint32_t httpsrv_send(HTTPSRV_SESSION_STRUCT *session, char *buffer, uint32_t length, uint32_t flags);
#if MQX_USE_IO_OLD == 0
size_t httpsrv_fsize(FILE *file);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HTTP_SUPP_H_ */
