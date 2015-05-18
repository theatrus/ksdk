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
*   This file contains the RTCS shell.
*
*
*END************************************************************************/


#include <rtcs.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#endif
#include <ftpc.h>
#include <shell.h>
#include <sh_rtcs.h>
#if SHELLCFG_USES_MFS
#include <sh_mfs.h>
#endif
#include "config.h"

#if ! SHELLCFG_USES_RTCS
#error This application requires SHELLCFG_USES_RTCS defined non-zero in mqx_sdk_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif

const SHELL_COMMAND_STRUCT Shell_commands[] = {
    { "arpadd",    Shell_arpadd },
    { "arpdel",    Shell_arpdel },
    { "arpdisp",   Shell_arpdisp },
#if RTCSCFG_ENABLE_NAT
    { "dnat",      Shell_dnat },
#endif
    { "echosrv",   Shell_echosrv},
    { "echo",      Shell_echo},
    { "email",     Shell_smtp },
    { "gate",      Shell_gate },
    { "gethbn",    Shell_get_host_by_name },
    { "getname",   Shell_getname },
    { "getrt",     Shell_getroute },
    { "ipconfig",  Shell_ipconfig },
    { "llmnr",  Shell_llmnrsrv },
#if DEMOCFG_USE_WIFI
    { "iwconfig",  Shell_iwconfig },
#endif
#if RTCSCFG_ENABLE_NAT
    { "natinit",   Shell_natinit },
    { "natinfo",   Shell_natinfo },
#endif
    { "netstat",   Shell_netstat },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
#if RTCSCFG_ENABLE_UDP
    { "sendto",    Shell_sendto },
#endif
#if RTCSCFG_ENABLE_SNMP
    { "snmpd",     Shell_SNMPd },
#endif
    { "telnetcln", Shell_telnetcln },
    { "telnetsrv", Shell_telnetsrv },
    { "ftp",       Shell_FTP_client },
    { "ftpsrv",    Shell_ftpsrv },
#if RTCSCFG_ENABLE_UDP
    { "tftpcln",   Shell_tftpcln },
    { "tftpsrv",   Shell_tftpsrv},
#endif
#if DEMOCFG_ENABLE_PPP
    { "ppp"    ,  Shell_ppp},
#endif
    { "walkrt",    Shell_walkroute },

/* Filesystem commands */
#if SHELLCFG_USES_MFS
#if DEMOCFG_ENABLE_RAMDISK
    { "cd",        Shell_cd },
    { "copy",      Shell_copy },
    { "create",    Shell_create },
    { "del",       Shell_del },
    { "dir",       Shell_dir },
    { "format",    Shell_format },
    { "mkdir",     Shell_mkdir },
    { "pwd",       Shell_pwd },
    { "read",      Shell_read },
    { "ren",       Shell_rename },
    { "rmdir",     Shell_rmdir },
    { "write",     Shell_write },
#endif
#endif
    { "help",      Shell_help },
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const SHELL_COMMAND_STRUCT Telnetsrv_shell_commands[] = {
    { "exit",      Shell_exit },
    { "gethbn",    Shell_get_host_by_name },
    { "getrt",     Shell_getroute },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
    { "telnetcln", Shell_telnetcln },
#if RTCSCFG_ENABLE_UDP
    { "tftp",      Shell_tftpcln },
#endif
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const FTPc_COMMAND_STRUCT FTPc_commands[] = {
   { "ascii",        FTPc_ascii       },
   { "binary",       FTPc_binary      },
   { "bye",          FTPc_bye         },
   { "cd",           FTPc_cd          },
   { "close",        FTPc_close       },
   { "delete",       FTPc_delete      },
   { "get",          FTPc_get         },
   { "help",         FTPc_help        },
   { "ls",           FTPc_ls          },
   { "mkdir",        FTPc_mkdir       },
   { "open",         FTPc_open        },
   { "pass",         FTPc_pass        },
   { "put",          FTPc_put         },
   { "pwd",          FTPc_pwd         },
   { "remotehelp",   FTPc_remotehelp  },
   { "rename",       FTPc_rename      },
   { "rmdir",        FTPc_rmdir       },
   { "user",         FTPc_user        },
   { NULL,   NULL }
};


/* EOF */
