/**HEADER********************************************************************
* 
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* Comments:
*
*   Simple websocket echo plugin for HTTPSRV example application.
*
*END************************************************************************/

#include <rtcs.h>
#include <lwsem.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#endif
#include "example_plugins.h"

/*
 * Plugin structure for echo plugin.
 */
WS_PLUGIN_STRUCT ws_echo_plugin = {
    echo_connect,
    echo_message,
    echo_error,
    echo_disconnect,
    NULL
};

/*
 * Plugin structure for chat plugin.
 */
WS_PLUGIN_STRUCT ws_chat_plugin = {
    chat_connect,
    chat_message,
    chat_error,
    chat_disconnect,
    NULL
};

HTTPSRV_PLUGIN_STRUCT echo_plugin = {
    HTTPSRV_WS_PLUGIN,
    (void *) &ws_echo_plugin
};

HTTPSRV_PLUGIN_STRUCT chat_plugin = {
    HTTPSRV_WS_PLUGIN,
    (void *) &ws_chat_plugin
};

/*
 * Server plugins
 */
const HTTPSRV_PLUGIN_LINK_STRUCT plugins[] = {
    {"/echo", &echo_plugin},
    {"/chat", &chat_plugin},
    {NULL, NULL}
};

/* Lock used for locking of chat_clients array. */
static LWSEM_STRUCT chat_lock;
/* Array of all connected users. */
static CHAT_USER_STRUCT chat_clients[CHAT_NUM_CLIENTS];

/*
 * Echo plugin code - simple plugin which echoes any message it receives back to
 * client.
 */
uint32_t echo_connect(void* param, WS_USER_CONTEXT_STRUCT context)
{
    printf("WebSocket echo client connected.\r\n");
    return(0);
}

uint32_t echo_disconnect(void* param, WS_USER_CONTEXT_STRUCT context)
{
    printf("WebSocket echo client disconnected.\r\n");
    return(0);
}

uint32_t echo_message(void* param, WS_USER_CONTEXT_STRUCT context)
{
    WS_send(&context);
    
    if (context.data.type == WS_DATA_TEXT)
    {
       /* Print received text message to console. */
       context.data.data_ptr[context.data.length] = 0;
       printf("WebSocket message received:\r\n%s\r\n", context.data.data_ptr); 
    }
    else
    {
       /* Inform user about binary message. */
       printf("WebSocket binary data with length of %d bytes received.", context.data.length);
    }
    
    return(0);
}

uint32_t echo_error(void* param, WS_USER_CONTEXT_STRUCT context)
{
    printf("WebSocket error: 0x%X.\r\n", context.error);
    return(0);
}

/*
 * Chat plugin code.
 */

/*
 * Callback called when user connects to chat server. 
 */
uint32_t chat_connect(void* param, WS_USER_CONTEXT_STRUCT context)
{
    printf("Chat user connected.\r\n");
    if (chat_add_user(&context) != CHAT_OK)
    {
        WS_close(context.handle);
    }
    return(0);
}

/*
 * Callback called when user disconnects from chat server. 
 */
uint32_t chat_disconnect(void* param, WS_USER_CONTEXT_STRUCT context)
{
    chat_del_user(&context);
    printf("Chat user disconnected.\r\n");
    return(0);
}

/*
 * Callback called whenever message is received by chat server. 
 * This function adds user if required, call ID->name replacement and then
 * broadcasts received message to all valid users.
 */
uint32_t chat_message(void* param, WS_USER_CONTEXT_STRUCT context)
{
    uint32_t         i;
    char             *message;
    uint8_t          id[CHAT_ID_LENGTH+1] = {0};
    CHAT_USER_STRUCT *user;
    
    /* Create buffer for received message. */
    message = (char*) _mem_alloc_zero(context.data.length+1);
    if (message == NULL)
    {
        return (uint32_t)RTCS_ERROR;
    }

    /* Copy message to buffer*/
    _mem_copy(context.data.data_ptr, message, context.data.length);
    context.data.data_ptr = (uint8_t *) message;
    
    /* If there is no ID in message, return. */
    if (!chat_get_id(message, id))
    {
        return (uint32_t) RTCS_OK;
    }
    user = chat_find_id(id);
    if (user == NULL)
    {
        /* If user with message ID is not present, add him to users array. */
        if(RTCS_OK != chat_add_id(id, message, context.handle))
        {
            return (uint32_t)RTCS_ERROR;
        }
    }
    context.data.length = chat_strip_id(message, id);

    /* Send received message to all valid clients. */
    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        if (chat_clients[i].handle != 0)
        {
            context.handle = chat_clients[i].handle;
            WS_send(&context);
        }
    }

    /* Cleanup. */
    _mem_free(message);
    return (RTCS_OK);
}

/*
 * Initialize webserver plugins.
 */
uint32_t init_plugins(void)
{
    _lwsem_create(&chat_lock, 1);
    return(0);
}

/*
 * Callback called on error in chat plugin.
 */
uint32_t chat_error(void* param, WS_USER_CONTEXT_STRUCT context)
{
    printf("Chat connection error: 0x%X.\r\n", context.error);
    return(0);
}

/*
 * Get user ID from received chat message.
 */
bool chat_get_id(char *message, uint8_t *id)
{
    char *start;
    bool retval;

    retval = FALSE;

    /* Find ID start. */
    start = strchr(message, '.');
    if (start != NULL)
    {
        char *end;

        /* Find ID end. */
        end = strchr(start, ':');
        if (end != NULL)
        {
            uint32_t length;

            /* Copy ID. */
            length = ((end-start) < CHAT_ID_LENGTH) ? (end-start) : CHAT_ID_LENGTH;
            _mem_copy(start+1, id, length);
            retval = TRUE;
        }
    }
    return(retval);
}

/*
 * Find user with ID, return NULL if no user is found.
 */
CHAT_USER_STRUCT * chat_find_id(uint8_t *id)
{
    uint32_t         i;
    CHAT_USER_STRUCT *retval;

    retval = NULL;

    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        /* If user ID matches message ID, return user structure. */
        if (!memcmp(chat_clients[i].id, id, CHAT_ID_LENGTH))
        {
            retval = chat_clients + i;
            break;
        }
    }
    return(retval);
}

/*
 * Add user ID and user name to appropriate handle.
 */
uint32_t chat_add_id(uint8_t *src, char* msg, uint32_t handle)
{
    char             * start;
    CHAT_USER_STRUCT * user = NULL;
    uint32_t         i;
    
    /* Find client with selected handle. */
    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        if (chat_clients[i].handle == handle)
        {
            user = chat_clients + i;
            break;
        }
    }
    
    if(NULL == user)
    {
      return (uint32_t)RTCS_ERROR;
    }

    /* Copy message ID to user ID. */
    _mem_copy(src, user->id, strlen((char*) src));
    msg += strlen((char*) src)+strlen("0.");

    /* Find user's name. */
    start = strchr(msg, ' ');
    if (start != NULL)
    {
        char     *end;
        uint32_t length;
        
        start++;
        end = strchr(start, ' ');
        if (end != NULL)
        {
            /* Copy user's name to user structure. */
            length = ((end-start) < CHAT_USERNAME_LENGTH) ? (end-start) : CHAT_USERNAME_LENGTH;
            _mem_copy(start, user->name, length);
        }
    }
    
    return RTCS_OK;
}

/*
 * Replace user ID with user name in message, return length of message.
 */
uint32_t chat_strip_id(char *message, uint8_t *id)
{
    uint32_t i;
    char     *end;

    end = strchr(message, ':');

    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        if (!memcmp(chat_clients[i].id, id, CHAT_ID_LENGTH))
        {
            uint32_t length;

            _mem_copy(chat_clients[i].name, message, strlen(chat_clients[i].name));
            length = strlen(chat_clients[i].name) + ((message+strlen(message))-end);
            memmove(message+strlen(chat_clients[i].name), end, (message+strlen(message))-end);
            return(length);
        }
    }
    return(0);
}

/*
 * Add new user to users array.
 */
uint32_t chat_add_user(WS_USER_CONTEXT_STRUCT *context)
{
    uint32_t i;
    uint32_t retval;

    retval = CHAT_OK;

    _lwsem_wait(&chat_lock);

    /* Set variables in clients array. */
    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        if (chat_clients[i].handle == 0)
        {
            chat_clients[i].handle = context->handle;
            break;
        }
    }

    if (i == CHAT_NUM_CLIENTS)
    {
        retval = CHAT_FAIL;
    }
    _lwsem_post(&chat_lock);
    return(retval);
}

/*
 * Remove chat user from users array.
 */
uint32_t chat_del_user(WS_USER_CONTEXT_STRUCT *context)
{
    uint32_t i;

    _lwsem_wait(&chat_lock);

    /* Reset variables in clients array. */
    for (i = 0; i < CHAT_NUM_CLIENTS; i++)
    {
        if (chat_clients[i].handle == context->handle)
        {
            _mem_zero(chat_clients+i, sizeof(CHAT_USER_STRUCT));
            break;
        }
    }
    _lwsem_post(&chat_lock);
    return(CHAT_OK);
}
