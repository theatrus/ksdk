/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include "lwip/opt.h"

#if LWIP_UDP
// Standard C Included Files
#include <stdio.h>
// lwip Included Files
#include "lwip/udp.h"
#include "lwip/debug.h"
#include "netif/etharp.h"
#include "lwip/init.h"
// SDK Included Files
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct 
ip_addr *addr, u16_t port)
{
    char buffer[100];
    if (p != NULL) 
    {
        if(pbuf_copy_partial(p, buffer, p->tot_len,0) != p->tot_len) 
        {
            LWIP_DEBUGF(LWIP_DBG_ON, ("pbuf_copy_partial failed\r\n"));
        } 
        else 
        {
            buffer[p->tot_len] = '\0';
            LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\r\n", buffer));
        }
        // send received packet back to sender
        udp_sendto(pcb, p, addr, port);
        // free the pbuf
        pbuf_free(p);
    }
}

void udp_echo_init(void)
{
    struct udp_pcb * pcb;

    // get new pcb
    pcb = udp_new();
    if (pcb == NULL) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
        return;
    }

    // bind to any IP address on port 7
    if (udp_bind(pcb, IP_ADDR_ANY, 7) != ERR_OK) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
        return;
    }

    // set udp_echo_recv() as callback function
    // for received packets
    udp_recv(pcb, udp_echo_recv, NULL);
}

static void app_low_level_init(void)
{
    // Open uart module for debug
    hardware_init();
    configure_enet_pins(BOARD_ENET_INSTANCE);
    dbg_uart_init();
    
    // Open ENET clock gate
    CLOCK_SYS_EnableEnetClock(0);
    // Select PTP timer outclk
    CLOCK_SYS_SetEnetTimeStampSrc(0, kClockTimeSrcOsc0erClk);

    // Disable the mpu
    BW_MPU_CESR_VLD(MPU_BASE, 0);
}

/*!
 * @brief main function
 */
int main(void)
{
    struct netif fsl_netif0;
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
    udp_echo_init();
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
    for(;;)
    {
#if !ENET_RECEIVE_ALL_INTERRUPT
        ENET_receive(enetIfPtr);
#endif
    }
}
#endif
