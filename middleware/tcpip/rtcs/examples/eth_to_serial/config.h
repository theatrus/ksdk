/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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


/*
** Define IP address and IP network mask
*/
#define SERIAL_SOCKET_BUFFER_SIZE 1460

#ifndef ENET_IPADDR
   #define ENET_IPADDR  IPADDR(192,168,1,202) 
#endif

#ifndef ENET_IPMASK
   #define ENET_IPMASK  IPADDR(255,255,255,0)
#endif

#ifndef ENET_IPGATEWAY
   #define ENET_IPGATEWAY  IPADDR(0,0,0,0)
#endif

#ifndef ENET_MAC
   #define ENET_MAC  {0x00,0xA7,0xC5,0xF1,0x11,0x90}
#endif

#define DEMOCFG_USE_WIFI            BSP_ENET_WIFI_ENABLED  /* USE WIFI Interface */

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
   #define DEMOCFG_SECURITY        "none"
   #define DEMOCFG_PASSPHRASE      NULL
   #define DEMOCFG_WEP_KEY         "ABCDE"
   //Possible values 1,2,3,4
   #define DEMOCFG_WEP_KEY_INDEX   1

#ifdef BSP_ENET_WIFI_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE       BSP_ENET_WIFI_DEVICE
#else
   #define DEMOCFG_DEFAULT_DEVICE       (1)
#endif

#ifdef BSP_DEFAULT_ENET_DEVICE
   #undef BSP_DEFAULT_ENET_DEVICE
#endif

   #define BSP_DEFAULT_ENET_DEVICE       DEMOCFG_DEFAULT_DEVICE

#endif //DEMOCFG_USE_WIFI

#ifndef DEMO_PORT
    #define DEMO_PORT IPPORT_TELNET
#endif

#if MQX_USE_IO_OLD
#define SERIAL_DEVICE BSP_DEFAULT_IO_CHANNEL
#else
#define SERIAL_DEVICE "tty:"
#endif
/* EOF */
