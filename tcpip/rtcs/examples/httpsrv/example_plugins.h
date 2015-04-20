#ifndef _example_plugins_h_
#define _example_plugins_h_
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
*   Plugin definitions for HTTP server example.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "httpsrv_ws.h"

#define CHAT_ID_LENGTH        (16)
#define CHAT_OK               (0)
#define CHAT_FAIL             (1)
#define CHAT_USERNAME_LENGTH  (12)
#define CHAT_NUM_CLIENTS      (2)

typedef struct chat_user_struct
{
    /* Websocket handle */
    uint32_t handle;
    /* Username */
    char     name[CHAT_USERNAME_LENGTH+1];
    /* User ID */
    uint8_t  id[CHAT_ID_LENGTH+1];
}CHAT_USER_STRUCT;

uint32_t echo_connect(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t echo_disconnect(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t echo_message(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t echo_error(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t chat_connect(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t chat_disconnect(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t chat_message(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t chat_error(void* param, WS_USER_CONTEXT_STRUCT context);
uint32_t init_plugins(void);
uint32_t chat_add_user(WS_USER_CONTEXT_STRUCT *context);
uint32_t chat_del_user(WS_USER_CONTEXT_STRUCT *context);
uint32_t chat_add_id(uint8_t *src, char* msg, uint32_t handle);
uint32_t chat_strip_id(char *message, uint8_t *id);
bool chat_get_id(char *message, uint8_t *id);
CHAT_USER_STRUCT * chat_find_id(uint8_t *id);
#endif
