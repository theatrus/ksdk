/*HEADER**********************************************************************
* Copyright (c) 2013 Freescale Semiconductor;
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
*   Example HTTP server using RTCS Library.
*
*
*END************************************************************************/

#include <rtcs.h>
#include <ipcfg.h>
#include <shell.h>

#if PLATFORM_SDK_ENABLED
  #include <stdio.h>
  #include <fsl_os_abstraction.h>
  #include <fsl_gpio_driver.h>
  #include <board.h>
  #include <nio_tfs.h>
  #include <fsl_enet_rtcs_adapter.h>
  #include "app_rtos.h"
  #include "main.h"
#else
  #include "tfs.h"
#endif

#include "config.h"
#include "rtcs_ssl.h"

#if DEMOCFG_WIFI_USE_WEB_PROV
#define SWITCH_TASK             3
#endif

#if !PLATFORM_SDK_ENABLED
  #ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
  #error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in mqx_sdk_config.h and recompile BSP with this option.
  #endif
#endif

#ifndef BSP_ENET_DEVICE_COUNT
#error Ethernet devices not defined!
#endif

#if BSP_ENET_DEVICE_COUNT == 0
#error No ethernet devices defined!
#endif

#if DEMOCFG_WIFI_USE_WEB_PROV && !BSP_ENET_WIFI_WEB_PROV_ENABLED
#error Need BSP_ENET_WIFI_WEB_PROV_ENABLED to be enabled
#endif

/*
 * Authentication users
 */
static const HTTPSRV_AUTH_USER_STRUCT users[] = {
    {"developer", "freescale"},
    { NULL, NULL} /* Array terminator */
};

/*
 * Authentication information.
 */
static const HTTPSRV_AUTH_REALM_STRUCT auth_realms[] = {
    { "Please use uid:developer pass:freescale to login", "auth.html", HTTPSRV_AUTH_BASIC, users},
    { NULL, NULL, HTTPSRV_AUTH_INVALID, NULL} /* Array terminator */
};

/* add SHELL */

/*
 * Enabled shell commands
 */
const SHELL_COMMAND_STRUCT Shell_commands[] = {
   { "help",      Shell_help }, 
   { "ipconfig",  Shell_ipconfig },   
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },      
#endif
#if DEMOCFG_USE_WIFI
   { "iwconfig",  Shell_iwconfig },
#endif
   { "?",         Shell_command_list },
   { NULL,        NULL }
};

#if !PLATFORM_SDK_ENABLED
void main_task(uint32_t);
#if DEMOCFG_WIFI_USE_WEB_PROV
void Switch_Task(uint32_t);
#endif

/*
** MQX initialization information
*/

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority,   Name,    Attributes,          Param, Time Slice */
    { 1,            main_task,  MAIN_TASK_STACK_SIZE,    MAIN_TASK_PRIORITY,          "Main",         MQX_AUTO_START_TASK, 0,     0 },
#if DEMOCFG_WIFI_USE_WEB_PROV
    { SWITCH_TASK,  Switch_Task, 2000,   MAIN_TASK_PRIORITY-1,          "Switch",       0, 0,     0 },
#endif
    { 0 }
};

typedef uint32_t task_param_t;
#endif

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Returned Value : void
* Comments       :
*
*END*************************************************************************/

void main_task ( task_param_t init_data )
{
    int32_t                              error;
    uint32_t                             server[BSP_ENET_DEVICE_COUNT];
    extern const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[];
    extern const HTTPSRV_SSI_LINK_STRUCT ssi_lnk_tbl[];
#if MQX_USE_IO_OLD
    extern const TFS_DIR_ENTRY           tfs_data[];
#else
    extern const NIO_TFS_DIR_ENTRY       nio_tfs_data[];
#endif
    HTTPSRV_PARAM_STRUCT                 params[BSP_ENET_DEVICE_COUNT];
    uint32_t                             ip_addr[BSP_ENET_DEVICE_COUNT];    
    uint32_t                             i = 0;
    char*                                indexes[BSP_ENET_DEVICE_COUNT] = {0};
    uint8_t                              n_devices = BSP_ENET_DEVICE_COUNT;
    #if !HTTP_USE_ONE_SERVER
    uint8_t                              n_servers = BSP_ENET_DEVICE_COUNT;
    #else
    uint8_t                              n_servers = 1;
    #endif

    /* Init RTCS */
    _RTCSPCB_init = 4;
    _RTCSPCB_grow = 2;
    _RTCSPCB_max = 20;
    
    _RTCSTASK_stacksize = 4500;
    
    error = RTCS_create();
    if (error != RTCS_OK) 
    {
        printf("RTCS failed to initialize, error = 0x%lX\n", (uint32_t)error);
        _task_block();
    }

    _IP_forward = TRUE;

    /* Bind IP address for each interface */
    for (i = 0; (i < n_devices) && (n_devices != 0); i++)
    {
        _enet_address           address;
        uint32_t                 phy_addr;
        #if RTCSCFG_ENABLE_IP4
        IPCFG_IP_ADDRESS_DATA	ip_data; 
        #endif
        phy_addr = i;
        if (i == DEMOCFG_DEFAULT_DEVICE)
          ip_addr[i] = IPADDR(ENET_IPADDR_A,ENET_IPADDR_B,ENET_IPADDR_C,ENET_IPADDR_D);
        else
          ip_addr[i] = IPADDR(ENET_IPADDR_A,ENET_IPADDR_B,ENET_IPADDR_C+i+1,ENET_IPADDR_D);

        ENET_get_mac_address(phy_addr, ip_addr[i], address);

        /* Init ENET device */
        error = ipcfg_init_device (phy_addr, address);
        if (error != RTCS_OK) 
        {
            printf("IPCFG: Device n.%ld init failed. Error = 0x%lX\n", i, (uint32_t)error);
            _task_set_error(MQX_OK);
            n_devices--;
            i--;
            continue;
        }

#if DEMOCFG_USE_WIFI
        if (phy_addr == DEMOCFG_DEFAULT_DEVICE)
        {
            iwcfg_set_essid(phy_addr, DEMOCFG_SSID);
            if ((strcmp(DEMOCFG_SECURITY,"wpa") == 0)||strcmp(DEMOCFG_SECURITY,"wpa2") == 0)
            {
                iwcfg_set_passphrase (phy_addr,DEMOCFG_PASSPHRASE);
            }
            if (strcmp(DEMOCFG_SECURITY,"wep") == 0)
            {
                iwcfg_set_wep_key (phy_addr,DEMOCFG_WEP_KEY,
                                 strlen(DEMOCFG_WEP_KEY),DEMOCFG_WEP_KEY_INDEX);
            }
            iwcfg_set_sec_type(phy_addr, DEMOCFG_SECURITY);
            iwcfg_set_mode(phy_addr, DEMOCFG_NW_MODE);
            iwcfg_commit(phy_addr);
        }
#endif

        #if RTCSCFG_ENABLE_IP4
        ip_data.ip      = ip_addr[i];
        ip_data.mask    = ENET_IPMASK;
        ip_data.gateway = 0;
        /* Bind IPv4 address */
        error = ipcfg_bind_staticip (phy_addr, &ip_data);
        if (error != RTCS_OK) 
        {
            printf("\nIPCFG: Failed to bind IP address. Error = 0x%lX", (uint32_t)error);
            _task_block();
        }
        #endif

        indexes[i] = (char*) _mem_alloc_zero(sizeof("\\index_x.html"));
        if (indexes[i] == NULL)
        {
            printf("\n Failed to allocate memory.");
            _task_block();
        }
    }  

#if MQX_USE_IO_OLD
    error = _io_tfs_install("tfs:", tfs_data);
    if (error != IO_OK)
#else
    NIO_TFS_INIT_DATA_STRUCT tfs_init = { nio_tfs_data };
    if (_nio_dev_install("tfs:", &nio_tfs_dev_fn, &tfs_init, &error) == NULL)
#endif
    {
        printf("\n Failed to initialize Trivial File System");
        _task_block();
    }

    /* Start HTTP server on each interface */
    for (i = 0; i < n_servers; i++)
    {
        char *secure_string;
        
        _mem_zero(&params[i], sizeof(HTTPSRV_PARAM_STRUCT));
        params[i].af = HTTP_INET_AF;  //IPv4, IPv6 or from config.h

        #if RTCSCFG_ENABLE_IP6
        /* Set interface number here. Zero is any. */
        params[i].ipv6_scope_id = HTTP_SCOPE_ID;
        #endif

        sprintf(indexes[i], "\\index.html");
        params[i].root_dir = "tfs:\\web";
        params[i].index_page = indexes[i];
        params[i].auth_table = auth_realms;
    #if ENABLE_SSL && RTCSCFG_ENABLE_SSL
        secure_string = "S";
    #else
        secure_string = "";
    #endif
        printf("Starting HTTP%s server No.%ld on IP", secure_string, i);
        /*
        ** If there is only one server listen on any IP address 
        ** so address can change in runtime (DHCP etc.).
        ** Otherwise we will use static IP for server.
        */
        if (HTTP_INET_AF & AF_INET)
        {
            #if RTCSCFG_ENABLE_IP4
            if ((i == 0) && (n_servers == 1))
            {
                params[i].ipv4_address.s_addr = INADDR_ANY;
            }
            else
            {
                params[i].ipv4_address.s_addr = ip_addr[i];
            }
            /* Print active IPv4 address */
            printf(" %lu.%lu.%lu.%lu", IPBYTES(ip_addr[i]));
            #endif
        }
        if (HTTP_INET_AF & AF_INET6)
        {
            #if RTCSCFG_ENABLE_IP6 
            IPCFG6_GET_ADDR_DATA data;
            char prn_addr6[RTCS_IP_ADDR_STR_SIZE];

            ipcfg6_get_addr(0, 0, &data);

            if(inet_ntop(AF_INET6, &(data.ip_addr), prn_addr6, sizeof(prn_addr6)))
            {
                /* Print active IPv6 address */
                printf("%s %s", (HTTP_INET_AF & AF_INET) ? " and" : "", prn_addr6);
            }
            #endif
        }
        printf(", port %d", HTTPSRV_PORT);
        /*
        ** Maximum default number of available sockets in RTCS is six.
        ** Maximum of two are used for HTTP server as listening sockets (one for IPv4 and one for IPv6).
        ** So we take two of sockets and use them for sessions.
        */
        params[i].max_ses = HTTPSRV_EXAMPLE_SESSIONS;
        params[i].cgi_lnk_tbl = cgi_lnk_tbl;
        params[i].ssi_lnk_tbl = ssi_lnk_tbl;
        #if HTTPSRVCFG_WEBSOCKET_ENABLED
        params[i].plugins = &plugins;
        #endif 
        params[i].port = HTTPSRV_PORT;
        #if ENABLE_SSL && RTCSCFG_ENABLE_SSL
        params[i].ssl_params = &ssl_params;
        #endif
        init_plugins();
        /* There are a lot of static data in CGIs so we have to set large enough stack for script handler */
        params[i].script_stack = HTTPSRV_EXAMPLE_CGI_STACK;
        server[i] = HTTPSRV_init(&params[i]);
        
        printf("...");
        if (server[i] == 0)
        {
            printf("[FAIL]\n");
        }
        else
        {
            printf("[OK]\n");
        }
    }
#if DEMOCFG_WIFI_USE_WEB_PROV
    _task_create(0, SWITCH_TASK, DEMOCFG_DEFAULT_DEVICE);
#endif

    /* HTTP server initialization is done so we can start shell */
    for (;;)  
    {
        /* Run the shell */
        Shell(Shell_commands, NULL);
        printf("Shell quit, restarting...\n");
    }
}

#if DEMOCFG_WIFI_USE_WEB_PROV
/*TASK*-----------------------------------------------------------------
*
* Function Name  : Switch_task
* Returned Value : void
* Comments       : This task is to demo WiFi provisioning using multiple methods
*   1. Embedded web server (push button SW2)
*   2. WiFi Protected Setup (WPS) (push button SW1)
* Either of these methods can be used to connect the Tower System to an access point. 
* Alternatively, the information for the wireless access point can be hard-coded into MQX demo software.
*
*END------------------------------------------------------------------*/

void Switch_Task(uint32_t myInput)
{
    int input = 0;
    int32_t              error;
    ENET_ESSID           webProv_ssid;
    uint8_t              channel;
    uint32_t             mode;
    ENET_MEDIACTL_PARAM  webParam;
    ENET_MEDIACTL_PARAM  webGetParam;
    uint8_t              buff[32];
    uint8_t              buff1[32];
    _enet_handle         ehandle;

    LWGPIO_STRUCT wps_port;
    LWGPIO_STRUCT prov_port;

    ehandle = ENET_get_device_handle(ENET_default_params[myInput].ENET_IF->MAC_NUMBER);
#ifdef WPS_TRIGGER_PIN
    lwgpio_init(&wps_port, WPS_TRIGGER_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_attribute(&wps_port, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    lwgpio_set_functionality(&wps_port, WPS_TRIGGER_MUX_GPIO);
#endif

#ifdef PROV_TRIGGER_PIN
    lwgpio_init(&prov_port, PROV_TRIGGER_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_attribute(&prov_port, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    lwgpio_set_functionality(&prov_port, PROV_TRIGGER_MUX_GPIO);
#endif

    while (TRUE)
    {
        // Check each input for a low-high transition (button press)
    #ifdef WPS_TRIGGER_PIN
        if (lwgpio_get_value(&wps_port) == LWGPIO_VALUE_LOW) {
            printf("Wps triggered -Please Wait...\n");
            error = ENET_mediactl (ehandle,ENET_SET_WPS_ENABLE,&mode);
        }
    #endif
    #ifdef PROV_TRIGGER_PIN
        if (lwgpio_get_value(&prov_port) == LWGPIO_VALUE_LOW) {
            printf("Web Provisioning triggered -Please Wait...\n");
            webGetParam.data = buff;
            webGetParam.length =0;

            error = ENET_mediactl (ehandle,ENET_GET_WEB_PROV_PARAM_USRNAME,&webGetParam);

            if(webGetParam.length == 0){
                webGetParam.data = DEMOCFG_WEBPARAM_USERNAME;
                webGetParam.length = strlen(webGetParam.data);
            }
            error = ENET_mediactl (ehandle,ENET_SET_WEB_PROV_PARAM_USRNAME,&webGetParam);

            webGetParam.data = buff1;
            webGetParam.length =0;
            error = ENET_mediactl (ehandle,ENET_GET_WEB_PROV_PARAM_PWD,&webGetParam);

            if(webGetParam.length == 0){
                webGetParam.data = DEMOCFG_WEBPARAM_PWD;
                webGetParam.length = strlen(webGetParam.data);
            }
            error = ENET_mediactl (ehandle,ENET_SET_WEB_PROV_PARAM_PWD,&webGetParam);

            channel = DEMOCFG_WEBPARAM_CH;
            error = ENET_mediactl (ehandle,ENET_SET_WEB_PROV_PARAM_CH,&channel);

            webProv_ssid.flags = 1;
            webProv_ssid.essid = DEMOCFG_WEBPARAM_SSID;
            webProv_ssid.length = strlen(webProv_ssid.essid);
            error = ENET_mediactl (ehandle,ENET_SET_WEB_PROV_PARAM_SSID,&webProv_ssid);

            webParam.data = DEMOCFG_WEBPARAM_IP;
            webParam.length = strlen(webParam.data);
            error = ENET_mediactl (ehandle,ENET_SET_WEB_PROV_ENABLE,&webParam);
            _time_delay(1000);
        }
    #endif
        _time_delay(200);
    }
}
#endif
