#ifndef __httpsrv_plugins_h__
#define __httpsrv_plugin_h__

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
*   This file contains HTTP server plugins definitions.
*
*END************************************************************************/

#include <httpsrv_prv.h>

#ifdef __cplusplus
extern "C" {
#endif

void* httpsrv_ws_alloc(HTTPSRV_SESSION_STRUCT *session);

void *ws_plugin_create_context(void *message);
void ws_plugin_destroy_context(void *plugin_context);
void ws_plugin_wait(void *plugin_context);
void ws_plugin_process(void *message, void *plugin_context);

HTTPSRV_PLUGIN_CALLBACKS ws_plugin_callbacks = 
{
    ws_plugin_create_context,
    ws_plugin_destroy_context,
    ws_plugin_wait,
    ws_plugin_process
};

#ifdef __cplusplus
}
#endif

#endif
