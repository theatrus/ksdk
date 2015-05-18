/**HEADER********************************************************************
*
* Copyright (c) 2011 - 2015 Freescale Semiconductor;
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
* $FileName: usb_host_config.h$
* $Version : 
* $Date    :
*
* Comments:
*
*   
*
*END************************************************************************/

#ifndef __usbhost_cnfg_h__
#define __usbhost_cnfg_h__

/** MGCT: <category name="USB Host Settings"> */

#define USBCFG_HOST_KHCI                             (1)

#define USBCFG_HOST_EHCI                             (1)

/*
** Maximum number of USB instance.
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_NUM                              (2)

/*
** Maximum number of pipes.
** MGCT: <option type="number"/>
*/ 
#define USBCFG_HOST_MAX_PIPES                        (16)

/*
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_DEFAULT_MAX_NAK_COUNT            (3000)

/*
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_CTRL_FAILED_RETRY                 (3)

/*
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_CTRL_STALL_RETRY                 (1)


/*
** Maximum power of host.
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_MAX_POWER                        (250)


/*
** Maximum configuration  number per single device
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_MAX_CONFIGURATION_PER_DEV         (2)

/*
** Maximum interface number per single configuration
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION  (5)

/*
** Maximum endpoint number per single interface
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_MAX_EP_PER_INTERFACE             (4)

/*
** If the HID class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_HID                              (1)

/*
** If the Audio class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_AUDIO                            (1)

/*
** If the MSC class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_MSC                              (1)

/*
** If the CDC class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_CDC                              (1)

/*
** If the PHDC class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_PHDC                             (1)

/*
** If the HUBclass device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_HUB                              (1)

/*
** If the HUBclass device supported
** MGCT: <option type="number"/>
*/
#define USBCFG_HOST_PRINTER                           (0)
/*
** If the Video class device supported
** MGCT: <option type="number"/>
*/
#define  USBCFG_HOST_VIDEO                            (0)

/** MGCT: </category> */

#if USBCFG_HOST_KHCI

    /** MGCT: <category name="KHCI Driver Specific Settings"> */
    
    /*
    ** KHCI task priority
    ** MGCT: <option type="number"/>
    */  
    #define USBCFG_HOST_KHCI_TASK_PRIORITY               (8)

    /*
    ** Wait time in tick for events - must be above 1.
    ** MGCT: <option type="string"/>
    */
    #define USBCFG_HOST_KHCI_WAIT_TICK                   100    // 5ms

    /*
    ** Maximum message count for KHCI.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_HOST_KHCI_TR_QUE_MSG_CNT              (10)

    /*
    ** Maximum interrupt transaction count.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_HOST_KHCI_MAX_INT_TR                  (10)
    
    /* 
    ** Allow workaround for bug in the peripheral when unaligned buffer @4B address is used
    ** MGCT: <category name="USB DMA alignment fix">
    */
    #define USBCFG_KHCI_4BYTE_ALIGN_FIX                      (1)

#if USBCFG_KHCI_4BYTE_ALIGN_FIX    
    /*
    ** The aligned buffer size for IN transactions, active when USBCFG_4BYTE_ALIGN_FIX is defined
      ** MGCT: <option type="number"/>
    */
    #define USBCFG_HOST_KHCI_SWAP_BUF_MAX                     (1024)
    /** MGCT: </category> */
#endif    
    /* 
    ** Choose Micro USB AB on TWR-K22F120M as host port. J26 and J27(2-3) should be shunt.
    ** Otherwise the host port on SER board would be selected.
    ** MGCT: <category name="USB DMA alignment fix">
    */
    #define USBCFG_HOST_PORT_NATIVE                      (1)
#endif

#ifdef USBCFG_HOST_EHCI

    
    /** MGCT: <category name="EHCI Driver Specific Settings"> */
    /*
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_USE_SW_TOGGLING         (0)

    /*
    ** Maximum number of allocated QH descriptors.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_MAX_QH_DESCRS           (8)

    /*
    ** Maximum number of allocated QTD descriptors.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_MAX_QTD_DESCRS          (8)


    /*
    ** Maximum number of allocated ISO descriptors.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_MAX_ITD_DESCRS          (16)

    /*
    ** Maximum number of allocated SSPLIT descriptors.
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_MAX_SITD_DESCRS         (16)

    /*
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_PIPE_TIMEOUT            (300)

    /*
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_FRAME_LIST_SIZE         (1024)

    /*
    ** MGCT: <option type="number"/>
    */
    #define USBCFG_EHCI_ITD_THRESHOLD           (10)

    /*
        ** MGCT: <option type="number"/>
        */
    #define USBCFG_EHCI_HS_DISCONNECT_ENABLE     (1)

    #define USBCFG_EHCI_PIN_DETECT_ENABLE         (1)

#endif // USBCFG_EHCI

/* set MACRO to 1 to enable USB COMPLIANCE test
*/
#define USBCFG_HOST_COMPLIANCE_TEST                 (0)

/* If the buffer provided by APP is cacheable
* 1 cacheable, buffer cache maintenance is needed
* 0 uncacheable, buffer cache maintenance is not needed
*/
#define    USBCFG_HOST_BUFF_PROPERTY_CACHEABLE       0

#endif
