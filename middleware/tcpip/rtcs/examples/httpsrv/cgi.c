/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   Example of shell using RTCS.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include <ctype.h>
#include <stdlib.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#endif

#define CGI_DATA_LENGTH_MAX (96)

#if ENABLE_SSL && RTCSCFG_ENABLE_SSL
    /* When secure web server is on, put CGI callback cgi_rtc_data() processing to default
     * server's script handler.
     * This is to save some RAM that would be otherwise needed for the separate task stack
     * and task descriptor.
     */
    #define CGI_RTC_DATA_STACK    0
#else
    /* put CGI processing for cgi_rtc_data() callback to separate task */
    #define CGI_RTC_DATA_STACK    1500
#endif

char cgi_data[CGI_DATA_LENGTH_MAX+1];

static void cgi_urldecode(char* url);
static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_example(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int ssi_date_time(HTTPSRV_SSI_PARAM_STRUCT* param);
static bool cgi_get_varval(char *var_str, char *var_name, char *var_val, uint32_t length);

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    { "rtcdata", cgi_rtc_data, CGI_RTC_DATA_STACK},
    { "get", cgi_example, 0},
    { "post", cgi_example, 0},
    { 0, 0 }    // DO NOT REMOVE - last item - end of table
};

const HTTPSRV_SSI_LINK_STRUCT ssi_lnk_tbl[] = {
 { "date_time", ssi_date_time },
 { 0, 0 } 
};


static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param)
{
    #define BUFF_SIZE sizeof("00\n00\n00\n")
    HTTPSRV_CGI_RES_STRUCT response;
    TIME_STRUCT time;
    uint32_t min;
    uint32_t hour;
    uint32_t sec;
    
    char str[BUFF_SIZE];
    uint32_t length = 0;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    _time_get(&time);
    
    sec = time.SECONDS % 60;
    min = time.SECONDS / 60;
    hour = min / 60;
    min %= 60;

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = HTTPSRV_CODE_OK;
    /* 
    ** When the keep-alive is used we have to calculate a correct content length
    ** so the receiver knows when to ACK the data and continue with a next request.
    ** Please see RFC2616 section 4.4 for further details.
    */
    
    /* Calculate content length while saving it to buffer */                                  
    length = snprintf(str, BUFF_SIZE, "%ld\n%ld\n%ld\n", hour, min, sec);          
    response.data = str;
    response.data_length = length;
    response.content_length = response.data_length;
    /* Send response */
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

/* Example Common Gateway Interface callback. */
static _mqx_int cgi_example(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};

    response.ses_handle = param->ses_handle;
    response.status_code = HTTPSRV_CODE_OK;
   
    if (param->request_method == HTTPSRV_REQ_GET)
    {
        char *c;
        
        /* Replace '+' with spaces. */
        while ((c = strchr(cgi_data, '+')) != NULL)
        {
            *c = ' ';
        }
        response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
        response.data = cgi_data;
        response.data_length = strlen(cgi_data);
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);
    }
    else if (param->request_method == HTTPSRV_REQ_POST)
    {
        uint32_t length = 0;
        uint32_t read;
        char     buffer[sizeof("post_input = ")+CGI_DATA_LENGTH_MAX] = {0};
        
        length = param->content_length;
        read = HTTPSRV_cgi_read(param->ses_handle, buffer, (length > sizeof(buffer)) ? sizeof(buffer) : length);

        if (read > 0)
        {
            cgi_get_varval(buffer, "post_input", cgi_data, sizeof(cgi_data));
            cgi_urldecode(cgi_data);
        }

        /* Write the response using chunked transmission coding. */
        response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
        /* Set content length to -1 to indicate unknown content length. */
        response.content_length = -1;
        response.data = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data = "<html><head><title>POST successfull!</title>";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data = "<meta http-equiv=\"refresh\" content=\"0; url=cgi.html\"></head><body></body></html>";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data_length = 0;
        HTTPSRV_cgi_write(&response);
    }

    return (response.content_length);
}

static bool cgi_get_varval(char *src, char *var_name, char *dst, uint32_t length)
{
    char *name;
    bool result;
    uint32_t index;
    uint32_t n_length;

    result = false;
    dst[0] = 0;
    name = src;

    n_length = strlen(var_name);

    while ((name = strstr(name, var_name)) != 0)
    {
        if (name[n_length] == '=')
        {
            name += n_length + 1;

            index = strcspn(name, "&");
            if (index >= length)
            {
                index = length-1;
            }
            strncpy(dst, name, index);
            dst[index] = '\0';
            result = true;
            break;
        }
        else
        {
            name = strchr(name, '&');
        }
    }

    return(result);
}

/* Example Server Side Include callback. */
static _mqx_int ssi_date_time(HTTPSRV_SSI_PARAM_STRUCT* param)
{
    if (strcmp(param->com_param, "time") == 0)
    {
        HTTPSRV_ssi_write(param->ses_handle, __TIME__, strlen(__TIME__));
    }
    else if (strcmp(param->com_param, "date") == 0)
    {
        HTTPSRV_ssi_write(param->ses_handle, __DATE__, strlen(__DATE__));
    }
    return(0);
}

/* Decode URL encoded string in place. */
static void cgi_urldecode(char* url)
{
    char* src = url;
    char* dst = url;

    while(*src != '\0')
    {
        if ((*src == '%') && (isxdigit((int)*(src+1))) && (isxdigit((int)*(src+2))))
        {
            *src = *(src+1);
            *(src+1) = *(src+2);
            *(src+2) = '\0';
            *dst++ = strtol(src, NULL, 16);
            src += 3;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}
