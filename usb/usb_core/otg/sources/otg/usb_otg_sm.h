/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 Freescale Semiconductor;
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
 * $FileName: usb_otg_sm.h$
 * $Version : 
 * $Date    : 
 *
 * Comments : This is the header file for the OTG State machine
 *
 *         
 *****************************************************************************/
#ifndef USB_OTG_SM_H_
#define USB_OTG_SM_H_

/* Public constants */

/* Timeout definitions in ms */
#define TB_SESSEND_SRP                    1500
#define TB_SE0_SRP                        1000
#define TB_DATA_PLS                       7
#define TB_DATA_PLS_MIN                   5
#define TB_DATA_PLS_MAX                   10
#define TB_SRP_FAIL                       5500
#define TB_AIDL_BDIS                      5
#define TB_ASE0_BRST                      155

#define TA_VBUS_RISE                      100
#define TA_VBUS_FALL                      1000
#define TA_WAIT_BCON                      2000 
#define TA_AIDL_BDIS                      500
#define TA_SUSPEND_RESUME                 2
#define TA_BIDL_ADIS                      190
#define TA_BCON_LDB                       100
#define TA_BCON_SDB                       1
#define TA_BCON_SDB_WIN                   100
#define T_HOST_REQ_POLL                   1500

/* USB OTG SM state definitions */
#define USB_OTG_DEVSTATE_UNDEFINED          0xFF
#define USB_OTG_SM_UNDEFINED                0xFF
#define USB_OTG_SM_B_IDLE_SESS_DETECT       0x00
#define USB_OTG_SM_B_IDLE_SESS_END_DETECT   0x01
#define USB_OTG_SM_B_IDLE_SE0_STABLE_WAIT   0x02
#define USB_OTG_SM_B_IDLE_SRP_START_ARMED   0x03
#define USB_OTG_SM_B_SRP_PULSE              0x10
#define USB_OTG_SM_B_SRP_VBUS_WAIT          0x11
#define USB_OTG_SM_B_PERI_BUS_SUSP_DETECT   0x20
#define USB_OTG_SM_B_PERI_BUS_SUSP_WAIT     0x21
#define USB_OTG_SM_B_PERI_HNP_ARMED         0x22
#define USB_OTG_SM_B_PERI_HNP_START         0x23
#define USB_OTG_SM_B_PERI_HNP_ACONN         0x24
#define USB_OTG_SM_B_HOST                   0x30
#define USB_OTG_SM_A_IDLE                   0x00
#define USB_OTG_SM_A_WAIT_VRISE             0x01
#define USB_OTG_SM_A_WAIT_BCON              0x02
#define USB_OTG_SM_A_VBUS_ERR               0x03
#define USB_OTG_SM_A_WAIT_VFALL             0x04
#define USB_OTG_SM_A_HOST                   0x10
#define USB_OTG_SM_A_SUSPEND                0x20
#define USB_OTG_SM_A_SUSPEND_RESUME         0x21
#define USB_OTG_SM_A_PERIPHERAL             0x30

/* Public types */

/* Public functions */
extern void _usb_otg_sm(usb_otg_handle otg_handle);
extern void _usb_otg_sm_a(usb_otg_handle otg_handle);
extern void _usb_otg_sm_b(usb_otg_handle otg_handle);
extern void _usb_otg_id_chg_a(usb_otg_handle otg_handle);
extern void _usb_otg_id_chg_b(usb_otg_handle otg_handle);
extern usb_status _usb_otg_load_host(usb_otg_handle otg_handle);
extern usb_status _usb_otg_load_device(usb_otg_handle otg_handle);
extern void _usb_otg_unload_host(usb_otg_handle otg_handle);
extern void _usb_otg_unload_device(usb_otg_handle otg_handle);
extern void _usb_otg_unload_active(usb_otg_handle otg_handle);
extern uint32_t _usb_otg_hnp_poll_req(usb_otg_handle handle);

#endif /* USB_OTG_SM_H_ */
