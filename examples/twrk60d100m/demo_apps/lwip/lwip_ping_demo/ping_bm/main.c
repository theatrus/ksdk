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

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */
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
// SDK Included Files
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"

// PING_DEBUG: Enable debugging for PING
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

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

struct netif fsl_netif0;

// ping variables
static u16_t ping_seq_num;
static u32_t ping_time;
static struct raw_pcb *ping_pcb;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

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

// Ping using the raw ip
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);

    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
      pbuf_header(p, -PBUF_IP_HLEN) == 0) 
    {
        iecho = (struct icmp_echo_hdr *)p->payload;

        if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
        {
            LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
            LWIP_DEBUGF( PING_DEBUG, ("%u.%u.%u.%u ",((u8_t *)addr)[0],((u8_t *)addr)[1],((u8_t *)addr)[2],((u8_t *)addr)[3]));
            LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\r\n", (sys_now()-ping_time)));
            // do some ping result processing
            PING_RESULT(1);
            pbuf_free(p);
            return 1; // eat the packet
        }
        // not eaten, restore original packet
        pbuf_header(p, PBUF_IP_HLEN);
    }

    return 0; // don't eat the packet
}

static void ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
    struct pbuf *p;
    struct icmp_echo_hdr *iecho;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

    LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
    LWIP_DEBUGF( PING_DEBUG, ("%u.%u.%u.%u \r",((u8_t *)addr)[0],((u8_t *)addr)[1],((u8_t *)addr)[2],((u8_t *)addr)[3]));
    LWIP_DEBUGF( PING_DEBUG, ("\r\n"));
    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

    p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
    if (!p) 
    {
        return;
    }
    
    if ((p->len == p->tot_len) && (p->next == NULL)) 
    {
        iecho = (struct icmp_echo_hdr *)p->payload;
        ping_prepare_echo(iecho, (u16_t)ping_size);

        raw_sendto(raw, p, addr);
        ping_time = sys_now();
    }
    pbuf_free(p);
}

static void ping_timeout(void *arg)
{
    struct raw_pcb *pcb = (struct raw_pcb*)arg;
    ip_addr_t ping_target = PING_TARGET;

    LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

    ping_send(pcb, &ping_target);

    sys_timeout(PING_DELAY, ping_timeout, pcb);
}

static void ping_raw_init(void)
{
    ping_pcb = raw_new(IP_PROTO_ICMP);
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

    raw_recv(ping_pcb, ping_recv, NULL);
    raw_bind(ping_pcb, IP_ADDR_ANY);
    sys_timeout(PING_DELAY, ping_timeout, ping_pcb);
}

void ping_send_now()
{
    ip_addr_t ping_target = PING_TARGET;
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
    ping_send(ping_pcb, &ping_target);
}

void ping_init(void)
{
#if !ENET_RECEIVE_ALL_INTERRUPT
    uint32_t devNumber = 0; 
    enet_dev_if_t * enetIfPtr;
#if LWIP_HAVE_LOOPIF
    devNumber = fsl_netif0.num - 1;
#else
    devNumber = fsl_netif0.num;
#endif
    enetIfPtr = (enet_dev_if_t *)&enetDevIf[devNumber];
#endif
    ping_raw_init();
    while(1)
    {
#if !ENET_RECEIVE_ALL_INTERRUPT
        ENET_receive(enetIfPtr);
#endif
        sys_check_timeouts();
    }
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

/*!
 * @brief main function
 */
int main(void)
{
  ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

  app_low_level_init();
  OSA_Init();
  lwip_init();

  IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
  IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
  IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);
  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, ethernet_input);
  netif_set_default(&fsl_netif0);
  netif_set_up(&fsl_netif0);

  ping_init();

  return 0;
}
#endif
