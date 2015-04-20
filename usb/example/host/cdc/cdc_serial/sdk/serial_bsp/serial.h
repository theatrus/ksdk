/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: serial.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
/* Incoming and outgoing data not processed */
#define IO_SERIAL_RAW_IO             (0)

/* Perform xon/xoff processing */
#define IO_SERIAL_XON_XOFF           (0x01)

/*
** Perform translation :
**	  outgoing \n to CR\LF
**	  incoming CR to \n
**	  incoming backspace erases previous character
*/
#define IO_SERIAL_TRANSLATION        (0x02)

/* echo incoming characters */
#define IO_SERIAL_ECHO               (0x04)

/* Perform hardware flow control processing */
#define IO_SERIAL_HW_FLOW_CONTROL    (0x08)

/*	*/
#define IO_SERIAL_NON_BLOCKING       (0x10)

/* RS485 flags */
#define IO_SERIAL_HW_485_FLOW_CONTROL (0x20)

/* Multidrop flag */
#define IO_SERIAL_MULTI_DROP          (0x40)

/* Serial I/O IOCTL commands */
#define IO_IOCTL_SERIAL_GET_FLAGS        (0x01)
#define IO_IOCTL_SERIAL_SET_FLAGS        (0x02)
#define IO_IOCTL_SERIAL_GET_BAUD         (0x03)
#define IO_IOCTL_SERIAL_SET_BAUD         (0x04)
#define IO_IOCTL_SERIAL_GET_STATS        (0x05)
#define IO_IOCTL_SERIAL_CLEAR_STATS      (0x06)
#define IO_IOCTL_SERIAL_TRANSMIT_DONE    (0x07)
#define IO_IOCTL_SERIAL_GET_CONFIG       (0x08)

#define IO_IOCTL_SERIAL_GET_HW_SIGNAL    (0x09)
#define IO_IOCTL_SERIAL_SET_HW_SIGNAL    (0x0A)
#define IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL  (0x0B)
/* Standard HW signal names used with GET/SET/CLEAR HW SIGNAL */
#define IO_SERIAL_CTS                    (1)
#define IO_SERIAL_RTS                    (2)
#define IO_SERIAL_DTR                    (4)
#define IO_SERIAL_DSR                    (8)
#define IO_SERIAL_DCD                    (0x10)
#define IO_SERIAL_RI                     (0x20)
#define IO_SERIAL_BRK                    (0x40)

#define IO_IOCTL_SERIAL_SET_DATA_BITS    (0x0C)
#define IO_IOCTL_SERIAL_GET_DATA_BITS    (0x0D)
/* Value used with SET DATA BITS is just the integer number of bits */

#define IO_IOCTL_SERIAL_SET_STOP_BITS    (0x0E)
#define IO_IOCTL_SERIAL_GET_STOP_BITS    (0x0F)
/* Standard names used with SET STOP BITS */
#define IO_SERIAL_STOP_BITS_1            (1)
#define IO_SERIAL_STOP_BITS_1_5          (2)
#define IO_SERIAL_STOP_BITS_2            (3)

#define IO_IOCTL_SERIAL_SET_PARITY       (0x10)
#define IO_IOCTL_SERIAL_GET_PARITY       (0x11)
/* Standard parity names used with SET PARITY */
#define IO_SERIAL_PARITY_NONE            (1)
#define IO_SERIAL_PARITY_ODD             (2)
#define IO_SERIAL_PARITY_EVEN            (3)
#define IO_SERIAL_PARITY_FORCE           (4)
#define IO_SERIAL_PARITY_MARK            (5)
#define IO_SERIAL_PARITY_SPACE           (6)
#define IO_SERIAL_PARITY_MULTI_DATA      (7)
#define IO_SERIAL_PARITY_MULTI_ADDRESS   (8)

#define IO_IOCTL_SERIAL_START_BREAK      (0x12)
#define IO_IOCTL_SERIAL_STOP_BREAK       (0x13)
#define IO_IOCTL_SERIAL_TX_DRAINED       (0x14)

#define IO_IOCTL_SERIAL_CAN_TRANSMIT     (0x15)
#define IO_IOCTL_SERIAL_CAN_RECEIVE      (0x16)

#define IO_IOCTL_SERIAL_DISABLE_RX       (0x17)
#define IO_IOCTL_SERIAL_WAIT_FOR_TC      (0x18)

#define IO_IOCTL_SERIAL_SET_RTL          (0x19)
#define IO_IOCTL_SERIAL_GET_RTL          (0x1A)
#define IO_IOCTL_SERIAL_SET_IRDA_TX      (0x1B)
#define IO_IOCTL_SERIAL_SET_IRDA_RX      (0x1C)
#define IO_IOCTL_SERIAL_SET_ADDRESS_DEVICE      ( 0x1D)
#define IO_IOCTL_SERIAL_GET_ADDRESS_DEVICE      ( 0x1E)

/* Standard IOCTL commands */
#define IO_IOCTL_GET_FLAGS          (0x01)
#define IO_IOCTL_SET_FLAGS          (0x02)
#define IO_IOCTL_FLUSH_OUTPUT       (0x03)
#define IO_IOCTL_GET_STATS          (0x04)
#define IO_IOCTL_CLEAR_STATS        (0x05)
#define IO_IOCTL_CHAR_AVAIL         (0x06)
#define IO_IOCTL_SEEK               (0x07)
/* EOF */
