/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FSL_USB_KHCI_HAL_H__
#define __FSL_USB_KHCI_HAL_H__

#include<stdint.h>
#include<stdbool.h>
#include<assert.h>
#include"fsl_device_registers.h"


#define usb_hal_khci_bdt_set_address(bdt_base, ep, direction, odd, address) \
    *((uint32_t*)((bdt_base & 0xfffffe00) | (((uint32_t)ep & 0x0f) << 5) | (((uint32_t)direction & 1) << 4) | (((uint32_t)odd & 1) << 3)) + 1) = address


#define usb_hal_khci_bdt_set_control(bdt_base, ep, direction, odd, control) \
    *(uint32_t*)((bdt_base & 0xfffffe00) | (((uint32_t)ep & 0x0f) << 5) | (((uint32_t)direction & 1) << 4) | (((uint32_t)odd & 1) << 3)) = control

#define usb_hal_khci_bdt_get_address(bdt_base, ep, direction, odd) \
    (*((uint32_t*)((bdt_base & 0xfffffe00) | (((uint32_t)ep & 0x0f) << 5) | (((uint32_t)direction & 1) << 4) | (((uint32_t)odd & 1) << 3)) + 1))


#define usb_hal_khci_bdt_get_control(bdt_base, ep, direction, odd) \
    (*(uint32_t*)((bdt_base & 0xfffffe00) | (((uint32_t)ep & 0x0f) << 5) | (((uint32_t)direction & 1) << 4) | (((uint32_t)odd & 1) << 3)))


/*!
 * @addtogroup usb_khci_hal
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/
 
#if defined(__cplusplus)
      extern "C" {
#endif


/*! 
 * @name Initialization
 * @{
 */

/*!
 * @brief Init the BDT page register from the BDT table
 *
 * @param baseAddr   USB baseAddr id
 * @param bdtAddress   the BDT address resides in memory
 */
static inline void usb_hal_khci_set_buffer_descriptor_table_addr(uint32_t baseAddr, uint32_t bdtAddress)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_BDTPAGE1(_baseAddr,(uint8_t)((uint32_t)bdtAddress >> 8));
    USB_WR_BDTPAGE2(_baseAddr,(uint8_t)((uint32_t)bdtAddress >> 16));
    USB_WR_BDTPAGE3(_baseAddr,(uint8_t)((uint32_t)bdtAddress >> 24));
}


/*!
 * @brief Enable the specific Interrupt
 *
 * @param baseAddr  USB baseAddr id
 * @param intrType  specific  interrupt type
 */
static inline void usb_hal_khci_enable_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_INTEN(_baseAddr,(uint8_t)intrType);
}

/*!
 * @brief Disable the specific Interrupt
 *
 * @param baseAddr USB baseAddr id
 * @param intrType  specific interrupt type 
 */
static inline void usb_hal_khci_disable_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_INTEN(_baseAddr, intrType);
}

/*!
 * @brief Get the interrupt status
 *
 * @param baseAddr USB baseAddr id
 * @return specific interrupt type 
 */
static inline uint8_t usb_hal_khci_get_interrupt_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return (USB_RD_ISTAT(_baseAddr));
}

#if defined (FSL_FEATURE_USB_KHCI_OTG_ENABLED) && (FSL_FEATURE_USB_KHCI_OTG_ENABLED == 1)
/*!
 * @brief Set the USB controller to host mode
 *
 * @param baseAddr USB baseAddr id.
 *
 */
static inline void usb_hal_khci_enable_otg(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_SET_OTGCTL(_baseAddr,USB_OTGCTL_OTGEN_MASK);
}

/*!
 * @brief Enable the specific OTG Interrupt
 *
 * @param baseAddr USB baseAddr id
 * @param intrType specific interrupt type
 * 
 */
static inline void usb_hal_khci_enable_otg_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;

    USB_SET_OTGICR(_baseAddr,(uint8_t)intrType);
}

/*!
 * @brief Get the otg interrupt status
 *
 * @param baseAddr USB baseAddr id
 * @return otg interrupt status
 * 
 */
static inline uint8_t usb_hal_khci_get_otg_interrupt_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    return (USB_RD_OTGISTAT(_baseAddr));
}

/*!
* @brief Clear the specific otg interrupt
*
* @param baseAddr usb baseAddr id
* @param intrType the interrupt type
*/
static inline void usb_hal_khci_clr_otg_interrupt(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_OTGISTAT(_baseAddr,(uint8_t)intrType);
}

/*!
* @brief Clear all the otg interrupts
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clr_all_otg_interrupts(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_OTGISTAT(_baseAddr,0xFF);
}

/*!
 * @brief Check if controller is busy on transferring.
 *
 * @param baseAddr USB baseAddr id.
 * @return the value of the TOKENBUSY bit from the control register
 */
static inline uint8_t usb_hal_khci_is_line_stable(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
  return ((USB_RD_OTGSTAT(_baseAddr) & USB_OTGSTAT_LINESTATESTABLE_MASK) ? 1:0);
}

/*!
* @brief Enable the pull up
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_pull_up(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_OTGCTL(_baseAddr, USB_OTGCTL_DPHIGH_MASK |  USB_OTGCTL_OTGEN_MASK |USB_OTGCTL_DMLOW_MASK |USB_OTGCTL_DPLOW_MASK);
}

/*!
* @brief Disable the pull up
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_pull_up(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_OTGCTL(_baseAddr, USB_OTGCTL_DPHIGH_MASK |  USB_OTGCTL_OTGEN_MASK |USB_OTGCTL_DMLOW_MASK |USB_OTGCTL_DPLOW_MASK);
}

/*!
* @brief Get OTG status
* @param baseAddr usb baseAddr id
* @return otg status
*/
static inline uint8_t usb_hal_khci_get_otg_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    return(USB_RD_OTGSTAT(_baseAddr));
}
#endif

/*!
 * @brief Get the interrupt enable status
 *
 * @param baseAddr USB baseAddr id
 * @return current enabled interrupt types
 */
static inline uint8_t usb_hal_khci_get_interrupt_enable_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return (USB_RD_INTEN(_baseAddr));
}

/*!
* @brief Clear the specific interrupt
*
* @param baseAddr usb baseAddr id
* @param intrType the interrupt type needs to be cleared
*/
static inline void usb_hal_khci_clr_interrupt(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;

    
    USB_WR_ISTAT(_baseAddr,(uint8_t)intrType);
}

/*!
* @brief Clear all the interrupts
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clr_all_interrupts(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ISTAT(_baseAddr,0xff);
}

/*!
* @brief Judge if an interrupt type happen
*
* @param baseAddr usb baseAddr id
* @param intrType the interrupt type
* @return the current interrupt type is happen or not
*/
static inline uint8_t usb_hal_khci_is_interrupt_issued(uint32_t baseAddr, uint32_t intrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    uint8_t temp = USB_RD_ISTAT(_baseAddr);
    return ( temp & USB_RD_INTEN(_baseAddr) & (intrType));
}


#if defined (FSL_FEATURE_USB_KHCI_DYNAMIC_SOF_THRESHOLD_COMPARE_ENABLED) && (FSL_FEATURE_USB_KHCI_DYNAMIC_SOF_THRESHOLD_COMPARE_ENABLED == 1)
/*!
* @brief Enable the dynamic  sof threshold
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_dynamic_sof_threshold(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_SOFDYNTHLD(_baseAddr, (uint8_t)1);
}

/*!
* @brief Diable the dynamic  sof threshold
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_dynamic_sof_threshold(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_SOFDYNTHLD(_baseAddr, (uint8_t)0);
}
#endif

#if defined (FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED) && (FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED == 1)
/*!
* @brief Judge if the device is attached 
*
* @param baseAddr usb baseAddr id
* @return the device is attached or not
*/
static inline uint8_t usb_hal_khci_is_attach_issued(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    uint8_t temp = USB_RD_USBTRC0_VREDG_DET(_baseAddr);
    return temp;
}

/*!
* @brief Disable the Vbus rising edge interrupt
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clear_vredg_en(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_VREDG_EN(_baseAddr, 0);
}

/*!
* @brief Enable the Vbus rising edge interrupt
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_vredg_en(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_VREDG_EN(_baseAddr, 1);
}

/*!
* @brief Judge if the device is detached 
*
* @param baseAddr usb baseAddr id
* @return the device is detached or not
*/
static inline uint8_t usb_hal_khci_is_detach_issued(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    uint8_t temp = USB_RD_USBTRC0_VFEDG_DET(_baseAddr);
    return temp;
}

/*!
* @brief Disable the Vbus falling edge interrupt
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clear_vfedg_en(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_VFEDG_EN(_baseAddr, 0);
}

/*!
* @brief Enable the Vbus falling edge interrupt
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_vfedg_en(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_WR_MISCCTRL_VFEDG_EN(_baseAddr, 1);
}
#endif


/*!
* @brief Enable all the error interrupt
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_all_error_interrupts(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ERREN(_baseAddr,0xFF);
}

/*!
* @brief Disable all the error interrupts
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_all_error_interrupts(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ERREN(_baseAddr,0);
}

/*!
* @brief Enable the specific error interrupts
* @param baseAddr usb baseAddr id
* @param errIntrType the error interrupt type
*/
static inline void usb_hal_khci_enable_error_interrupts(uint32_t baseAddr, uint32_t errIntrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_ERREN(_baseAddr,(uint8_t)errIntrType);
}

/*!
* @brief Disable  the specific error interrupt
* @param baseAddr usb baseAddr id
* @param errorIntrType the error interrupt type
*/
static inline void usb_hal_khci_disable_error_interrupts(uint32_t baseAddr, uint32_t errorIntrType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_ERREN(_baseAddr, errorIntrType);
}

/*!
* @brief Get the error interrupt status
*
* @param baseAddr usb baseAddr id
* @return  the error interrupt status
*/
static inline uint8_t usb_hal_khci_get_error_interrupt_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return (USB_RD_ERRSTAT(_baseAddr));
}

/*!
* @brief Get the error interrupt enable status
*
* @param baseAddr usb baseAddr id
* @return  the error interrupt enable status
*/
static inline uint8_t usb_hal_khci_get_error_interrupt_enable_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return (USB_RD_ERREN(_baseAddr));
}

/*!
* @brief Clear all the error interrupt happened
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clr_all_error_interrupts(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ERRSTAT(_baseAddr,0xff);
}

/*!
* @brief Check if the specific error happened
*
* @param baseAddr usb baseAddr id
* @param errorType the error happened type
* @return the ERRSTAT register together with the error interrupt
*/
static inline uint8_t usb_hal_khci_is_error_happend(uint32_t baseAddr, uint32_t errorType)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return ( USB_RD_ERRSTAT(_baseAddr) & (uint8_t)errorType);
}

/*!
 * @brief Clear the TOKENBUSY flag.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_clr_token_busy(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_CLR_CTL(_baseAddr,USB_CTL_TXSUSPENDTOKENBUSY_MASK);
}

/*!
 * @brief Check if controller is busy on transferring.
 *
 * @param baseAddr USB baseAddr id.
 * @return the value of the TOKENBUSY bit from the control register
 */
static inline uint8_t usb_hal_khci_is_token_busy(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    return (uint8_t)(USB_RD_CTL(_baseAddr) & USB_CTL_TXSUSPENDTOKENBUSY_MASK);
}

/*!
 * @brief reset all the BDT odd ping/pong fields.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_set_oddrst(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_SET_CTL(_baseAddr,USB_CTL_ODDRST_MASK);
}

/*!
 * @brief clear the ODDRST flag.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_clr_oddrst(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_CLR_CTL(_baseAddr,USB_CTL_ODDRST_MASK);
}

#if defined (FSL_FEATURE_USB_KHCI_HOST_ENABLED) && (FSL_FEATURE_USB_KHCI_HOST_ENABLED == 1)
/*!
 * @brief Begin to issue RESET signal.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_start_bus_reset(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_CTL(_baseAddr,USB_CTL_RESET_MASK);
}

/*!
 * @brief Stop issuing RESET signal
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_stop_bus_reset(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_CTL(_baseAddr,USB_CTL_RESET_MASK);
}

/*!
 * @brief Start to issue resume signal.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_start_resume(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_CTL(_baseAddr,USB_CTL_RESUME_MASK);
}

/*!
 * @brief Stop issuing resume signal.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_stop_resume(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_CTL(_baseAddr,USB_CTL_RESUME_MASK);
}

/*!
 * @brief Set the USB controller to host mode
 *
 * @param baseAddr USB baseAddr id.
 *
 */
static inline void usb_hal_khci_set_host_mode(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_CTL(_baseAddr,USB_CTL_HOSTMODEEN_MASK);
}

/*!
 * @brief Set the USB controller to device mode
 *
 * @param baseAddr USB baseAddr id.
 *
 */
static inline void usb_hal_khci_set_device_mode(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
        
    USB_CLR_CTL(_baseAddr,USB_CTL_HOSTMODEEN_MASK);
}

/*!
* @brief Set the transfer target 
*
* @param baseAddr usb baseAddr id
* @param address the address used to set
* @param speed the speed used to set
*/
static inline void usb_hal_khci_set_transfer_target(uint32_t baseAddr, uint32_t address, uint32_t speed)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ADDR(_baseAddr,(uint8_t)((speed == 0) ? (uint8_t)address : USB_ADDR_LSEN_MASK | (uint8_t)address));
}

/*!
* @brief Set the flag to indicate if the endpoint is communicating with controller through the hub  
* 
* @param baseAddr usb baseAddr id
* @param epNumber endpoint number
*/
static inline void usb_hal_khci_endpoint_on_hub(uint32_t baseAddr, uint32_t epNumber)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    
    USB_SET_ENDPT(_baseAddr, epNumber,USB_ENDPT_HOSTWOHUB_MASK);
}

/*!
* @brief Enable the support for low speed
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_low_speed_support(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_ADDR(_baseAddr, USB_ADDR_LSEN_MASK);
}


/*!
* @brief Disable the support for low speed
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_low_speed_support(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_ADDR(_baseAddr, USB_ADDR_LSEN_MASK);
}

/*!
* @brief Enable the host communicate to low speed device directly
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_communicate_low_speed_device(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ENDPT(_baseAddr, 0, USB_ENDPT_HOSTWOHUB_MASK);
}
 
/*!
* @brief Disable  the host communicate to low speed device directly
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_communicate_low_speed_device(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_ENDPT(_baseAddr, 0, USB_ENDPT_HOSTWOHUB_MASK);
}

/*!
* @brief Set pull downs for USB otg control pin
* 
* @param baseAddr usb baseAddr id
* @param bitfield the specific bitfield
*/
static inline uint8_t  usb_hal_khci_set_pull_downs(uint32_t baseAddr, uint8_t bitfield )
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_CLR_OTGCTL(_baseAddr,USB_OTGCTL_DMLOW_MASK | USB_OTGCTL_DPLOW_MASK);
    if(bitfield & 0x01)
    {
        USB_SET_OTGCTL(_baseAddr,USB_OTGCTL_DPLOW_MASK);
    }      

    if(bitfield & 0x02)
    {
        USB_SET_OTGCTL(_baseAddr,USB_OTGCTL_DMLOW_MASK);     
    }
    return USB_OK;
}

/*!
* @brief Set the target token for specific endpoint
* 
* @param baseAddr usb baseAddr id
* @param token the specific token
* @param endpoint_number the specific endpoint_number
*/
static inline void usb_hal_khci_set_target_token(uint32_t baseAddr, uint8_t token, uint8_t endpoint_number)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_TOKEN(_baseAddr, (uint8_t)(USB_TOKEN_TOKENENDPT(endpoint_number) | token));
}

/*!
* @brief Set the sof threshold
*
* @param baseAddr usb baseAddr id
* @param value value used to set
*/
static inline void usb_hal_khci_set_sof_theshold(uint32_t baseAddr, uint32_t value)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_SOFTHLD(_baseAddr, (uint8_t)value);
}

#endif


/*!
 * @brief Enable SOF.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_enable_sof(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_SET_CTL(_baseAddr,USB_CTL_USBENSOFEN_MASK);
}

/*!
 * @brief Disable the USB module.
 *
 * @param baseAddr USB baseAddr id.
 */
static inline void usb_hal_khci_disable_sof(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_CLR_CTL(_baseAddr,USB_CTL_USBENSOFEN_MASK);
}

/*!
 * @brief Clear the USB controller register
 *
 * @param baseAddr USB baseAddr id.
 *
 */
static inline void usb_hal_khci_clear_control_register(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_CLR_CTL(_baseAddr, 0xFFu);
}

/*!
 * @brief Get the speed  of the USB module.
 *
 * @param baseAddr USB baseAddr id.
 * @return the current line status of the USB module
 */
static inline uint8_t usb_hal_khci_get_line_status(uint32_t  baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    return ((USB_RD_CTL(_baseAddr) & USB_CTL_JSTATE_MASK) ? 0 : 1);
}

/*!
 * @brief 
 *
 * @param baseAddr USB baseAddr id.
 * @return the SE0 status
 */
static inline uint8_t usb_hal_khci_get_se0_status(uint32_t  baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    return ((USB_RD_CTL(_baseAddr) & USB_CTL_SE0_MASK) ? 1 : 0);
}


/*!
* @brief Get current  status
*
* @param baseAddr usb baseAddr id
* @return current status
*/
static inline uint8_t usb_hal_khci_get_transfer_status(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
        return USB_RD_STAT(_baseAddr);
}

/*!
* @brief Get the endpoint number from STAT register
*
* @param baseAddr usb baseAddr id
* @return endpoint number
*/
static inline uint8_t usb_hal_khci_get_transfer_done_ep_number(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return ((uint8_t)(USB_RD_STAT(_baseAddr) & 0xf0) >> 4);
}

/*!
* @brief Return the transmit dir from STAT register
*
* @param baseAddr usb baseAddr id
* @return transmit direction
*/
static inline uint8_t usb_hal_khci_get_transfer_done_direction(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return ((USB_RD_STAT(_baseAddr) & USB_STAT_TX_MASK) >>USB_STAT_TX_SHIFT);
}

/*!
* @brief Return the even or odd bank from STAT register
*
* @param baseAddr usb baseAddr id
* @return the even or odd bank
*/
static inline uint8_t usb_hal_khci_get_transfer_done_odd(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    return ((USB_RD_STAT(_baseAddr) & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT);
}

/*!
* @brief Returned the computed BDT address
*
* @param baseAddr usb baseAddr id
* @return the computed BDT address
*/
static inline uint16_t usb_hal_khci_get_frame_number(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    uint16_t temp = (uint16_t)((uint16_t)(USB_RD_FRMNUMH(_baseAddr)) << 8);
    return ( temp | USB_RD_FRMNUML(_baseAddr) );
}

/*!
* @brief Set the device address 
*
* @param baseAddr usb baseAddr id
* @param addr the address used to set
*/
static inline void usb_hal_khci_set_device_addr(uint32_t baseAddr, uint32_t addr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ADDR_ADDR(_baseAddr,addr);
}

/*!
* @brief Init the endpoint0
* 
* @param baseAddr usb baseAddr id
* @param isThoughHub endpoint0 is though hub or not
* @param isIsochPipe  current pipe is iso or not
*/
static inline void usb_hal_khci_endpoint0_init(uint32_t baseAddr, uint32_t isThoughHub, uint32_t isIsochPipe)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    uint8_t ep_ctl_val;
#if (FSL_FEATURE_USB_KHCI_HOST_ENABLED) 
    ep_ctl_val = (isThoughHub == 1 ? USB_ENDPT_HOSTWOHUB_MASK : 0)| USB_ENDPT_RETRYDIS_MASK |
            USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK | (isIsochPipe == 1 ? 0 : USB_ENDPT_EPHSHK_MASK);
#else
    ep_ctl_val = USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK |
            (isIsochPipe == 1 ? 0 : USB_ENDPT_EPHSHK_MASK);
#endif
    USB_WR_ENDPT(_baseAddr, 0, ep_ctl_val);
}

/*!
* @brief Stop the endpoint
* 
* @param baseAddr usb baseAddr id
* @param epNumber endpoint number
*/
static inline void usb_hal_khci_endpoint_shut_down(uint32_t baseAddr, uint32_t epNumber)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_ENDPT(_baseAddr,epNumber,0);
}

/*!
* @brief Set the endpoint in host mode which need handshake or not
* 
* @param baseAddr usb baseAddr id
* @param epNumber endpoint number
* @param isEphshkSet needs handshake or not
*/
static inline void usb_hal_khci_endpoint_enable_handshake(uint32_t baseAddr, uint32_t epNumber, uint32_t isEphshkSet)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_ENDPT(_baseAddr, epNumber,((isEphshkSet == 1) ? USB_ENDPT_EPHSHK_MASK : 0));
}

/*!
* @brief Set the endpoint in host mode which in TX or RX
* 
* @param baseAddr usb baseAddr id
* @param epNumber endpoint number
* @param isEptxenSet in TX or RX
*/
static inline void usb_hal_khci_endpoint_set_direction(uint32_t baseAddr, uint32_t epNumber, uint8_t isEptxenSet)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_ENDPT(_baseAddr, epNumber,((isEptxenSet == 1) ? USB_ENDPT_EPTXEN_MASK : USB_ENDPT_EPRXEN_MASK));
}

/*!
* @brief Clear the stall status of the endpoint
* 
* @param baseAddr usb baseAddr id
* @param epNumber endpoint number
*/
static inline void usb_hal_khci_endpoint_clr_stall(uint32_t baseAddr, uint32_t epNumber)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_ENDPT(_baseAddr, epNumber, USB_ENDPT_EPSTALL_MASK);
    
}

/*!
* @brief Enable the pull down
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_pull_down(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_USBCTRL(_baseAddr, USB_USBCTRL_PDE_MASK);
}


/*!
* @brief Disable the pull down
* 
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_pull_down(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_USBCTRL(_baseAddr, USB_USBCTRL_PDE_MASK);
}

/*!
* @brief Enable the DP pull up
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_dp_pull_up(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
#if (FSL_FEATURE_USB_KHCI_OTG_ENABLED)
    if (USB_RD_OTGCTL_OTGEN(_baseAddr))
    {
        USB_SET_OTGCTL(_baseAddr, USB_OTGCTL_DPHIGH_MASK);
    }
    else
    {
        USB_SET_CONTROL(_baseAddr, USB_CONTROL_DPPULLUPNONOTG_MASK);
    }
#else
    USB_SET_CONTROL(_baseAddr, USB_CONTROL_DPPULLUPNONOTG_MASK);
#endif
}

/*!
* @brief Disable the DP pull up
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_disable_dp_pull_up(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
#if (FSL_FEATURE_USB_KHCI_OTG_ENABLED)
    if (USB_RD_OTGCTL_OTGEN(_baseAddr))
    {
        USB_CLR_OTGCTL(_baseAddr, USB_OTGCTL_DPHIGH_MASK);
    }
    else
    {
        USB_CLR_CONTROL(_baseAddr, USB_CONTROL_DPPULLUPNONOTG_MASK);
    }
#else
    USB_CLR_CONTROL(_baseAddr, USB_CONTROL_DPPULLUPNONOTG_MASK);
#endif
}


/*!
* @brief Clear the USB TRC0 status
*
* @param baseAddr usb baseAddr id
*/
static inline void  usb_hal_khci_clr_usbtrc0(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;

    USB_WR_USBTRC0(_baseAddr, 0);
}

/*!
* @brief Set the controller to the suspend state
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_suspend(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_USBCTRL(_baseAddr, USB_USBCTRL_SUSP_MASK);
}


/*!
* @brief Clear the suspend state of the controller
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clr_suspend(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_CLR_USBCTRL(_baseAddr, USB_USBCTRL_SUSP_MASK);
}

/*!
* @brief Set weak pull down
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_weak_pulldown(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_CTL(_baseAddr, USB_CTL_SE0_MASK);
}

/*!
* @brief Reset control register
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_reset_control_register(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_CTL(_baseAddr, 0UL);
}

/*!
* @brief Set internal pull up
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_internal_pullup(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_WR_CONTROL(_baseAddr, USB_CONTROL_DPPULLUPNONOTG_MASK);
}

/*!
* @brief Set trc0
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_set_trc0(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    
    USB_SET_USBTRC0(_baseAddr, 0x40);
}
#endif
#if (FSL_FEATURE_USB_KHCI_USB_RAM )
#define USB_USBRAM_ADDRESS                       (0x400FE000)

/*!
* @brief Get the USB Ram address of the ctrol
*
* @param baseAddr usb baseAddr id
* @return the USBRAM address
*/
static inline uint32_t usb_hal_khci_get_usbram_add(uint32_t baseAddr)
{
    return USB_USBRAM_ADDRESS;
}
#endif

#if defined (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED == 1)
/*!
* @brief Clear the keep alive wake interrupt state of the controller
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_clr_keepalive_wake_int_sts(uint32_t baseAddr)
{
    USB0_KEEP_ALIVE_CTRL |= USB_KEEP_ALIVE_CTRL_WAKE_INT_STS_MASK;
}

/*!
* @brief Getkeep alive wake interrupt status
*
* @param baseAddr usb baseAddr id
* @return the keep alive wake status
*/
static inline uint8_t usb_hal_khci_get_keepalive_wake_int_sts(uint32_t baseAddr)
{
    return(USB0_KEEP_ALIVE_CTRL & USB_KEEP_ALIVE_CTRL_WAKE_INT_STS_MASK);
}
#endif

#if defined (FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED) && (FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED == 1)
/*!
* @brief set USB khci irc48m module enablement
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_ungate_irc48m(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_SET_CLK_RECOVER_IRC_EN(_baseAddr, USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK);
}

/*!
* @brief Set USB khci irc48m recovery block enablement
*
* @param baseAddr usb baseAddr id
*/
static inline void usb_hal_khci_enable_irc48m_recovery_block(uint32_t baseAddr)
{
    USB_MemMapPtr _baseAddr = (USB_MemMapPtr)baseAddr;
    USB_SET_CLK_RECOVER_CTRL(_baseAddr, USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN_MASK);
}
#endif

#if defined(__cplusplus)
        }
#endif
