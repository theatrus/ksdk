/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 */

/*!
 * @file
 * Ping sender module
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "lwip/opt.h"

#if LWIP_SOCKET // don't build if not configured for use in lwipopts.h
// Standard C Included Files
#include <stdio.h>
// lwip Included Files
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
// SDK Included Files
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
// PING_DEBUG: Enable debugging for PING.
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

// ping target - should be a "ip_addr_t" 
#ifndef PING_TARGET
#define PING_TARGET   (netif_default?netif_default->gw:ip_addr_any)
#endif

// ping receive timeout - in milliseconds
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

// ping delay - in milliseconds
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

// ping identifier - must fit on a u16_t
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

// ping additional data size to include in the packet
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

// ping result action - no default action
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

#ifndef PING_STACKSIZE
#define PING_STACKSIZE        3000
#endif

#ifndef PING_PRIORITY
#define PING_PRIORITY          3
#endif

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// ping variables
static u16_t ping_seq_num;
static u32_t ping_time;

struct netif fsl_netif0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
    {
   { 1L,     Main_Task,      3000L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

// Prepare a echo ICMP request
static void ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    // fill the additional data buffer with some data
    for(i = 0; i < data_len; i++) 
    {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

// Ping using the socket ip
static err_t ping_send(int s, ip_addr_t *addr)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho) 
    {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ipaddr(&to.sin_addr, addr);

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

    mem_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s)
{
    char buf[64];
    int fromlen, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    fromlen = sizeof(from);

    while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) 
    {
        if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) 
        {
            iphdr = (struct ip_hdr *)buf;
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
            {
                ip_addr_t fromaddr;
                inet_addr_to_ipaddr(&fromaddr, &from.sin_addr);
                LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
                ip_addr_debug_print(PING_DEBUG, &fromaddr);
                LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\r\n", (sys_now() - ping_time)));
                // do some ping result processing
                PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
                return;
            } 
        }
    }

    if (len == 0) 
    {
        LWIP_DEBUGF( PING_DEBUG, ("ping: recv - %"U32_F" ms - timeout\r\n", (sys_now()-ping_time)));
    }

    // do some ping result processing
    PING_RESULT(0);
}

static void ping_thread(void *arg)
{
    int s;
    int timeout = PING_RCV_TIMEO;
    ip_addr_t ping_target;

    LWIP_UNUSED_ARG(arg);
    netif_set_up(&fsl_netif0);

    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) 
    {
        return;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (1) 
    {
        ping_target = PING_TARGET;

        if (ping_send(s, &ping_target) == ERR_OK) 
        {
            LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
            ip_addr_debug_print(PING_DEBUG, &ping_target);
            LWIP_DEBUGF( PING_DEBUG, ("\r\n"));

            ping_time = sys_now();
            ping_recv(s);
        } 
        else 
        {
            LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
            ip_addr_debug_print(PING_DEBUG, &ping_target);
            LWIP_DEBUGF( PING_DEBUG, (" - error\r\n"));
        }
        sys_msleep(PING_DELAY);
    }
}

void ping_init(void)
{
    sys_thread_new("ping_thread", ping_thread, NULL, PING_STACKSIZE, PING_PRIORITY);
    OSA_Start();
    while(1){}
}

static void app_low_level_init(void)
{
    // Open uart module for debug
    hardware_init();

    // Open ENET clock gate
    CLOCK_SYS_EnableEnetClock(0);
    // Select PTP timer outclk
    CLOCK_SYS_SetEnetTimeStampSrc(0, kClockTimeSrcOsc0erClk);

    // Disable the mpu
    MPU_BWR_CESR_VLD(MPU, 0);
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif
{
  ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
  
  app_low_level_init();
  OSA_Init();
  
  LWIP_DEBUGF(PING_DEBUG,("TCP/IP initializing...\r\n"));  
  tcpip_init(NULL,NULL);
  LWIP_DEBUGF(PING_DEBUG,("TCP/IP initialized.\r\n"));

  IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
  IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
  IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);

  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, tcpip_input);
  netif_set_default(&fsl_netif0);

  ping_init();
}
#endif // LWIP_SOCKET
