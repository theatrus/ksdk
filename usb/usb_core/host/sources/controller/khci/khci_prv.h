/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: khci_prv.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains the macros, function prototypes and data structure
*   definitions required by the KHCI USB device driver.
*
*END************************************************************************/
#ifndef __khci_prv_h__
#define __khci_prv_h__

enum
{
    TR_CTRL,
    TR_IN,
    TR_OUT,
    TR_OUT_HNDSK,
    TR_IN_HNDSK
};

enum
{
    USB_ATTACH_STATE_IDLE,
    USB_ATTACH_STATE_BEGIN,
    USB_ATTACH_STATE_SPEED_DETECTION,
    USB_ATTACH_STATE_SPEED_DETECTION_BEGIN,
    USB_ATTACH_STATE_SPEED_DETECTION_TIMEOUT,
    USB_ATTACH_STATE_RESET,
    USB_ATTACH_STATE_RESET_DONE,
    USB_ATTACH_STATE_ENABLE_SOF,
    USB_ATTACH_STATE_SOF_ENABLED,
    USB_ATTACH_STATE_DONE
};

/* Transaction type */
typedef enum
{
    TR_MSG_UNKNOWN,     // unknow - not used
    TR_MSG_SETUP,       // setup transaction
    TR_MSG_SEND,        // send trnasaction
    TR_MSG_RECV         // receive transaction
} tr_msg_type_t;

typedef enum
{
    TR_MSG_IDLE ,    // namal transfer
    TR_MSG_NAK,       // nak transfer 
    TR_BUS_TIMEOUT,
} tr_msg_state_t;

typedef enum
{
    KHCI_TR_GET_MSG,
    KHCI_TR_START_TRANSMIT,
    KHCI_TR_TRANSMITING,
    KHCI_TR_TRANSMIT_DONE,
    KHCI_TR_NONE
} khci_tr_state_t;

typedef enum
{
    TYPE_INT,
    TYPE_NAK
} que_type_t;


#define TIMEOUT_NODATA          500
#define TIMEOUT_TOHOST          5000
#define TIMEOUT_TODEVICE        5000
#define TIMEOUT_OTHER           5000
#define TIMEOUT_DEFAULT         500

#define NAK_RETRY_TIME          1
#define RETRY_TIME              3


/* Transaction message */
typedef struct
{
      tr_msg_type_t     type;           /* transaction type */
      pipe_struct_t*    pipe_desc;      /* pointer to pipe descriptor */
      tr_struct_t*      pipe_tr;        /* pointer to transaction */
      tr_msg_state_t    msg_state;
      uint32_t          frame;          /* record the transfer timout */
      uint32_t          naktimeout;     /* record the transfer timout */
      uint8_t           retry;
} tr_msg_struct_t;

/* Interrupt transaction message queue */
typedef struct tr_int_que_itm_struct
{
    uint32_t        frame;         /* next time to fire interrupt */
    uint16_t        period;        /* period in [ms] for interrupt endpoint */
    tr_msg_struct_t msg;           /* atom transaction message */  
} tr_int_que_itm_struct_t;

#define BDT_BASE                ((uint32_t*)(bdt))
#define BD_PTR(ep, rxtx, odd)   ((((uint32_t)BDT_BASE) & 0xfffffe00) | ((ep & 0x0f) << 5) | ((rxtx & 1) << 4) | ((odd & 1) << 3))

#define BD_CTRL(ep,rxtx,odd)    (*((uint32_t*)BD_PTR(ep, rxtx, odd)))
#define BD_CTRL_RX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 0, odd)))
#define BD_CTRL_TX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 1, odd)))

#define BD_ADDR(ep,rxtx,odd)    (*((uint32_t*)BD_PTR(ep, rxtx, odd) + 1))
#define BD_ADDR_RX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 0, odd) + 1))
#define BD_ADDR_TX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 1, odd) + 1))

/* Define USB buffer descriptor definitions in case of their lack */
#ifndef USB_BD_BC
#   define USB_BD_BC(n)                 ((n & 0x3ff) << 16)
#   define USB_BD_OWN                   0x80
#   define USB_BD_DATA01(n)             ((n & 1) << 6)
#   define USB_BD_DATA0                 USB_BD_DATA01(0)
#   define USB_BD_DATA1                 USB_BD_DATA01(1)
#   define USB_BD_KEEP                  0x20
#   define USB_BD_NINC                  0x10
#   define USB_BD_DTS                   0x08
#   define USB_BD_STALL                 0x04
#   define USB_BD_PID(n)                ((n & 0x0f) << 2)
#endif

#ifndef USB_TOKEN_TOKENPID_SETUP
#   define USB_TOKEN_TOKENPID_OUT                   USB_TOKEN_TOKENPID(0x1)
#   define USB_TOKEN_TOKENPID_IN                    USB_TOKEN_TOKENPID(0x9)
#   define USB_TOKEN_TOKENPID_SETUP                 USB_TOKEN_TOKENPID(0xD)
#endif

typedef struct _usb_khci_host_state_struct
{
   uint32_t                            controller_id;
   void*                               upper_layer_handle;
   void*                               dev_ptr;
   pipe_struct_t*                      pipe_descriptor_base_ptr;
   /* event from KHCI peripheral */
   os_event_handle                     khci_event_ptr;
   /* Pipe, that had latest NAK respond, usefull to cut bandwidth for interrupt pipes sending no data */
   pipe_struct_t*                      last_to_pipe;
   /* Interrupt transactions */
   tr_int_que_itm_struct_t             tr_int_que[USBCFG_HOST_KHCI_MAX_INT_TR];
   tr_int_que_itm_struct_t             tr_nak_que[USBCFG_HOST_KHCI_MAX_INT_TR];
   os_msgq_handle                      tr_que;
   os_msgq_handle                      tr_iso_que;
   os_msgq_handle                      tr_que_bak;
   /* RX/TX buffer descriptor toggle bits */
   uint8_t                             rx_bd;
   uint8_t                             tx_bd;
   uint8_t                             vector_number;
   int8_t                              device_attached;
   uint8_t                             sof_threshold;
   uint8_t                             speed;
   uint8_t                             device_attach_phy;
   uint8_t                             reserve;
   uint32_t                            device_attach_state;
   uint32_t                            usbRegBase;
} usb_khci_host_state_struct_t, * ptr_usb_khci_host_state_struct_t;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
