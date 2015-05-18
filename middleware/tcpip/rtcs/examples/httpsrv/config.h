#ifndef _config_h_
#define _config_h_

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
*   Configurable information for the RTCS examples.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "rtcs_ssl.h"

/* IP address macros */
#define ENET_IPADDR_A 192
#define ENET_IPADDR_B 168
#define ENET_IPADDR_C 1
#define ENET_IPADDR_D 202

#define HTTPSRV_EXAMPLE_CGI_STACK (3000)
#define HTTPSRV_EXAMPLE_SESSIONS  (2)

/* Use this define to tell example if only one server should be used for all interfaces */
#define HTTP_USE_ONE_SERVER 1

#ifndef ENET_IPMASK
    #define ENET_IPMASK  IPADDR(255,255,255,0) 
#endif

#define DEMOCFG_USE_WIFI          BSP_ENET_WIFI_ENABLED  /* USE WIFI Interface */

#if DEMOCFG_USE_WIFI
    #include "iwcfg.h"

   #define DEMOCFG_SSID            "WiFi_Demo"
   //Possible Values managed or adhoc
   #define DEMOCFG_NW_MODE         "managed"
   //Possible vales
   // 1. "wep"
   // 2. "wpa"
   // 3. "wpa2"
   // 4. "none"
   #define DEMOCFG_SECURITY           "none"
   #define DEMOCFG_PASSPHRASE         NULL
   #define DEMOCFG_WEP_KEY            "ABCDE"
   #define DEMOCFG_WIFI_USE_WEB_PROV  BSP_ENET_WIFI_WEB_PROV_ENABLED
#if DEMOCFG_WIFI_USE_WEB_PROV
#ifdef BSP_BUTTON1
    #define WPS_TRIGGER_PIN           BSP_BUTTON1
    #define WPS_TRIGGER_MUX_GPIO      BSP_BUTTON1_MUX_GPIO
#endif
#ifdef BSP_BUTTON2
    #define PROV_TRIGGER_PIN          BSP_BUTTON2
    #define PROV_TRIGGER_MUX_GPIO     BSP_BUTTON1_MUX_GPIO
#endif
   #define DEMOCFG_WEBPARAM_SSID      "GSWebProv"
   #define DEMOCFG_WEBPARAM_CH        11
   #define DEMOCFG_WEBPARAM_USERNAME  "admin"
   #define DEMOCFG_WEBPARAM_PWD       "admin"
   #define DEMOCFG_WEBPARAM_IP        "192.168.1.100,255.255.255.0,192.168.1.1"
#endif

   //Possible values 1,2,3,4
   #define DEMOCFG_WEP_KEY_INDEX   1

#ifdef BSP_ENET_WIFI_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE       BSP_ENET_WIFI_DEVICE
#else
   #define DEMOCFG_DEFAULT_DEVICE       (1)
#endif

#endif

#ifndef DEMOCFG_DEFAULT_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE   BSP_DEFAULT_ENET_DEVICE
#endif

#if RTCSCFG_ENABLE_IP6
    #if RTCSCFG_ENABLE_IP4
        /*
        **  HTTP_INET_AF is AF_INET+AF_INET for http support IPv4+IPv6.
        **  HTTP_INET_AF is AF_INET  for http support IPv4 only.
        **  HTTP_INET_AF is AF_INET6 for http support IPv6 only.
        */
        #define HTTP_INET_AF    (AF_INET | AF_INET6)
    #else
        #define HTTP_INET_AF    AF_INET6
    #endif
#else
    #define HTTP_INET_AF    AF_INET
#endif

#define HTTP_SCOPE_ID          0 /* For any IF. */

#define MAIN_TASK_PRIORITY      9
#define MAIN_TASK_STACK_SIZE (2000)


#if ENABLE_SSL && RTCSCFG_ENABLE_SSL
    #define HTTPSRV_PORT (443)
    #define SSL_CERT_PATH "tfs:\\ssl\\server_cert.pem"
    #define SSL_KEY_PATH "tfs:\\ssl\\server_key.pem"
    static const HTTPSRV_SSL_STRUCT ssl_params = 
    {
        SSL_CERT_PATH,
        SSL_KEY_PATH
    };
#else
    #define HTTPSRV_PORT (80)
#endif

extern const HTTPSRV_PLUGIN_LINK_STRUCT plugins;
extern uint32_t init_plugins(void);

#endif // _config_h_
