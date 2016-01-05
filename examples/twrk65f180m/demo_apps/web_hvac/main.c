/*
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "lwip/opt.h"

#include <stdio.h>
#include <string.h>

#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"

#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"
#include "index_html.h"
#include "hvac_html.h"
#include "hvac_input_html.h"
#include "hvac.h"
#include "hvac_public.h"
#include "hvac_private.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG         LWIP_DBG_ON
#endif
#ifndef HTTPD_STACKSIZE
#define HTTPD_STACKSIZE        3000
#endif
#ifndef HTTPD_PRIORITY
#define HTTPD_PRIORITY          3
#endif

#ifndef HVAC_TASK_STACK
#define HVAC_TASK_STACK             1400U
#endif
#ifndef HVAC_PRIORITY
#define HVAC_PRIORITY          4
#endif

#ifndef HEARTBEAT_TASK_STACK
#define HEARTBEAT_TASK_STACK             1400U
#endif
#ifndef HEARTBEAT_PRIORITY
#define HEARTBEAT_PRIORITY          5
#endif

OSA_TASK_DEFINE(hvac, HVAC_TASK_STACK);
OSA_TASK_DEFINE(heart_beat, HEARTBEAT_TASK_STACK);

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
#define REPLY_MSG "%s\n%lu.%lu &deg;%c\n%lu.%lu &deg;%c\n%s\n%s\n%s\n%s\n"
char feedback_msg[50] = {0};

struct netif fsl_netif0;

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { 1L,     Main_Task,      3000L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
    { HVAC_TASK,    HVAC_Task,         1400,    9,      "HVAC",     MQX_AUTO_START_TASK,    0,      0           },
    { ALIVE_TASK,   HeartBeat_Task,    1500,   10,      "HeartBeat",                  0,    0,      0           },
    { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

bool index_address(const char *buf, const char *sub_str)
{
    return (strstr(buf, sub_str) != NULL);
}

int32_t parse_configs(const char *buf, char *name, char *var_in, int32_t var_in_len) {
    const char *var = buf;
    int res = 0;
    int idx;

    var_in[0] = 0;

    while ((var = strstr(var, name)) != 0) {
        if (*(var + strlen(name)) == '=') {
            var += strlen(name) + 1;

            idx = (int)strcspn(var, "&");
            strncpy(var_in, var, (unsigned long)idx);
            var_in[idx] = 0;
            res = 1;
            break;
        }
        else {
            var = strchr(var, '&');
        }
    }

    return res;
}

static void handle_request(struct netconn *conn, const char *buf_in)
{
    extern HVAC_STATE  HVAC_State;
    extern HVAC_PARAMS HVAC_Params;
    char hvac[10], unit[10], fan[10], t[40];
    uint32_t temp, temp_fract = 0;
    if (index_address(buf_in, "GET /"))
    {
        // Header first.
        netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);
        if (index_address(buf_in, "GET / "))
            // Home page.
            netconn_write(conn, index_html, index_html_len-1, NETCONN_NOCOPY);
        else if (index_address(buf_in, "GET /hvac.html"))
            // HVAC page.
            netconn_write(conn, hvac_html, hvac_html_len-1, NETCONN_NOCOPY);
        else if (index_address(buf_in, "GET /hvac_input.html"))
            // HVAC setting page.
            netconn_write(conn, hvac_input_html, hvac_input_html_len-1, NETCONN_NOCOPY);
        else if (index_address(buf_in, "GET /hvacdata."))
        {
            sprintf(feedback_msg, REPLY_MSG, HVAC_HVACModeName(HVAC_GetHVACMode()), \
                HVAC_Params.DesiredTemperature/10, \
                HVAC_Params.DesiredTemperature%10, \
                HVAC_GetTemperatureSymbol(), \
                HVAC_State.ActualTemperature/10, \
                HVAC_State.ActualTemperature%10, \
                HVAC_GetTemperatureSymbol(), \
                (HVAC_State.FanOn) ? "on" : "auto", \
                HVAC_GetOutput(HVAC_FAN_OUTPUT) ? "on" : "off", \
                HVAC_GetOutput(HVAC_HEAT_OUTPUT) ? "on" : "off", \
                HVAC_GetOutput(HVAC_COOL_OUTPUT) ? "on" : "off");
            // Current information.
            netconn_write(conn, feedback_msg, sizeof(feedback_msg)-1, NETCONN_NOCOPY);
        }
    }
    else if (index_address(buf_in, "POST /"))
    {
        if (parse_configs(buf_in, "hvac", hvac, sizeof(hvac)) && \
            parse_configs(buf_in, "unit", unit, sizeof(unit)) && \
            parse_configs(buf_in, "fan", fan, sizeof(fan)) && \
            parse_configs(buf_in, "temp", t, sizeof(t)))
        {
            if (strcmp(hvac,"heat") == 0) {
                HVAC_SetHVACMode(HVAC_Heat);
            }
            else if (strcmp(hvac,"cool") == 0) {
                HVAC_SetHVACMode(HVAC_Cool);
            }
            else {
                HVAC_SetHVACMode(HVAC_Off);
            }

            if (strcmp(unit,"f") == 0)
                HVAC_SetTemperatureScale(Fahrenheit);
            else
                HVAC_SetTemperatureScale(Celsius);


            if (strcmp(fan,"auto") == 0) {
                HVAC_SetFanMode(Fan_Automatic);
            }
            else {
                HVAC_SetFanMode(Fan_On);
            }
            if (sscanf(t, "%lu.%lu", &temp, &temp_fract) >= 1) {
                if (temp_fract<10) {
                    HVAC_SetDesiredTemperature(temp * 10 + temp_fract);
                }
            }
        }
        netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);
        netconn_write(conn, hvac_html, hvac_html_len-1, NETCONN_NOCOPY);
    }
}

/*!
 * @brief Serve one HTTP connection accepted in the http thread.
 */
static void http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    // Read the data from the port, blocking if nothing yet there.
    // We assume the request (the part we care about) is in one netbuf
    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void**)&buf, &buflen);
        handle_request(conn, buf);
    }
    // Close the connection (server closes in HTTP)
    netconn_close(conn);

    // Delete the buffer (netconn_recv gives us ownership,
    // so we have to make sure to deallocate the buffer)
    netbuf_delete(inbuf);
}

/*!
 * @brief The main function containing server thread.
 */
static void
http_server_netconn_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);
    netif_set_up(&fsl_netif0);
    // Create a new TCP connection handle
    conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);

    // Bind to port 80 (HTTP) with default IP address
    netconn_bind(conn, NULL, 80);

    // Put the connection into LISTEN state
    netconn_listen(conn);

    do {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while(err == ERR_OK);
    LWIP_DEBUGF(HTTPD_DEBUG,
    ("http_server_netconn_thread: netconn_accept received error %d, shutting down", err));
    netconn_close(conn);
    netconn_delete(conn);
}

/*!
 * @brief Initialize the HTTP server.
 */
void http_server_netconn_init()
{
    sys_thread_new("http_server_netconn", http_server_netconn_thread, NULL, HTTPD_STACKSIZE, HTTPD_PRIORITY);
    OSA_Start();
}

/*!
 * @brief Initialize hardware.
 */
static void app_low_level_init(void)
{
    // Open UART module for debug
    hardware_init();

    // Open ENET clock gate
    CLOCK_SYS_EnableEnetClock(0);
    // Select PTP timer OUTCLK
    CLOCK_SYS_SetEnetTimeStampSrc(0, kClockTimeSrcOsc0erClk);

    // Disable the mpu
    MPU_BWR_CESR_VLD(MPU, 0);
}

/*!
 * @brief Main function.
 */
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif

{
    ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

    app_low_level_init();

    // Set priority for ISRs
    NVIC_SetPriority(ENET_1588_Timer_IRQn, 6U);
    NVIC_SetPriority(ENET_Transmit_IRQn, 6U);
    NVIC_SetPriority(ENET_Receive_IRQn, 6U);
    NVIC_SetPriority(ENET_Error_IRQn, 6U);
    NVIC_SetPriority(ADC0_IRQn, 6U);

    OSA_Init();

    LWIP_DEBUGF(HTTPD_DEBUG,("TCP/IP initializing...\r\n"));
    tcpip_init(NULL,NULL);
    LWIP_DEBUGF(HTTPD_DEBUG,("TCP/IP initialized.\r\n"));
    IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
    IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
    IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);

    netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&fsl_netif0);

#ifndef FSL_RTOS_MQX
    if (OSA_TaskCreate(HVAC_Task,
                       (uint8_t*)"HVAC_Task",
                       HVAC_TASK_STACK,
                       hvac_stack,
                       HVAC_PRIORITY,
                       (task_param_t)NULL,
                       false,
                       &hvac_task_handler))
    {
        PRINTF("OSA task create failed!\r\n");
    }

    if (OSA_TaskCreate(HeartBeat_Task,
                       (uint8_t*)"HeartBeat_Task",
                       HEARTBEAT_TASK_STACK,
                       heart_beat_stack,
                       HEARTBEAT_PRIORITY,
                       (task_param_t)NULL,
                       false,
                       &heart_beat_task_handler))
    {
        PRINTF("OSA task create failed!\r\n");
    }
#endif
    http_server_netconn_init();
    for(;;);
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
