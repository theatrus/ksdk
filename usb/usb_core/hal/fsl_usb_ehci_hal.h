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

#ifndef __FSL_USB_EHCI_HAL_H__
#define __FSL_USB_EHCI_HAL_H__

#define  HW_USB_INSTANCE_COUNT              (1U)
#include<stdint.h>
#include<stdbool.h>
#include<assert.h>
#include"fsl_device_registers.h"

/*!
 * @addtogroup usb_ehci_hal
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
 * @brief Get the ehci base addr.
 *
 * This function enables get the base addr of ehci.
 *
 * @param instance the instance number of this USB module.
 * @return Current USB base address.
 */
#if (HW_USB_INSTANCE_COUNT > 1)
static USB_MemMapPtr _usb_hal_ehci_get_base_addr(uint32_t instance)
{
    USB_MemMapPtr base;
    switch(instance)
    {
        case 0:
            base = USB0_BASE_PTR;
            break;
        case 1:
            base = USB1_BASE_PTR;
            break;
        default:
            break;
        }
        return base;
}
#endif

/*!
 * @brief Set the controller mode to host.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_controller_host_mode(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_CM_MASK) | USBHS_USBMODE_CM(0x3); 
}

/*!
 * @brief Set the controller mode to device.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_controller_device_mode(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_CM_MASK) | USBHS_USBMODE_CM(0x2); 
}

/*!
 * @brief Set the controller mode to big endian.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_big_endian(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_ES_MASK)|(1 << USBHS_USBMODE_ES_SHIFT);
}

/*!
 * @brief Set the controller mode to little endian.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_little_endian(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_ES_MASK)|(0 << USBHS_USBMODE_ES_SHIFT);
}

/*!
 * @brief Set the controller mode to disable lock clock.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_disable_setup_lock(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_SLOM_MASK)|(1 << USBHS_USBMODE_SLOM_SHIFT);
}

/*!
 * @brief Set the controller mode to enable lock clock.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_enable_setup_lock(uint32_t baseAddr)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = (USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) & ~USBHS_USBMODE_SLOM_MASK)|(0 << USBHS_USBMODE_SLOM_SHIFT);
}

/*!
 * @brief Set the controller mode to specific mode.
 *
 * This function enables get the base addr of ehci.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param mode the mode needs to set to USBMODE.
 */
static inline void usb_hal_ehci_set_usb_mode(uint32_t baseAddr, uint32_t mode)
{
    USBHS_USBMODE_REG((USBHS_MemMapPtr)baseAddr) = mode;
}

/*!
 * @brief Get the dcc params.
 *
 * This function gets the dcc params
 *
 * @param baseAddr the baseAddr of USB module.
 * @return Current dcc params.
 */
static inline uint32_t usb_hal_ehci_get_dcc_params(uint32_t baseAddr)
{
    return USBHS_DCCPARAMS_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Clear epsetup status.
 *
 * This function clears the epsetup status
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epSetupStatus the epSetupStatus needs to clear from endpoint.
 */
static inline void usb_hal_ehci_clear_ep_setup_status(uint32_t baseAddr, uint32_t epSetupStatus)
{   
    USBHS_EPSETUPSR_REG((USBHS_MemMapPtr)baseAddr) |= (epSetupStatus << USBHS_EPSETUPSR_EPSETUPSTAT_SHIFT) & USBHS_EPSETUPSR_EPSETUPSTAT_MASK;
}

/*!
 * @brief Clear epsetup epnum.
 *
 * This function clears the epsetup epnum.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param ep_num the ep_num needs to clear.
 */
static inline void usb_hal_ehci_clear_setup_stat_ep_num(uint32_t baseAddr, uint32_t ep_num)
{
    USBHS_EPSETUPSR_REG((USBHS_MemMapPtr)baseAddr) = ep_num;
}

/*!
 * @brief Get epsetup status.
 *
 * This function gets the epsetup status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the epSetupStatus of endpoint.
 */
static inline uint32_t usb_hal_ehci_get_ep_setup_status(uint32_t baseAddr)
{   
    return USBHS_EPSETUPSR_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Get ep complete status.
 *
 * This function gets the complete status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the complete status of endpoint.
 */
static inline uint32_t usb_hal_ehci_get_ep_complete_status(uint32_t baseAddr)
{
    return USBHS_EPCOMPLETE_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Clear ep complete status.
 *
 * This function clears the complete status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epCompleteStatus the epCompleteStatus of endpoint.
 */
static inline void usb_hal_ehci_clear_ep_complete_status(uint32_t baseAddr, uint32_t epCompleteStatus)
{
    USBHS_EPCOMPLETE_REG((USBHS_MemMapPtr)baseAddr) = epCompleteStatus;
}

/*!
 * @brief Set eplist address.
 *
 * This function Set eplist address.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epQHAddress epQHAddress the epQHAddress of endpoint.
 */
static inline void usb_hal_ehci_set_eplist_address(uint32_t baseAddr, uint32_t epQHAddress)
{
    USBHS_EPLISTADDR_REG((USBHS_MemMapPtr)baseAddr) = epQHAddress;
}

/*!
 * @brief Get eplist address.
 *
 * This function Get eplist address.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return epQHAddress the epQHAddress needs to get.
 */
static inline uint32_t usb_hal_ehci_get_eplist_address(uint32_t baseAddr)
{
    return USBHS_EPLISTADDR_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Get the interrupt enable register status.
 *
 * This function Get the interrupt enable register status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the interrupt enable register status.
 */
static inline uint32_t usb_hal_ehci_get_interrupt_enable_status(uint32_t baseAddr)
{
    return USBHS_USBINTR_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Enable the specific interrupt.
 *
 * This function Enable the specific interrupt.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the specific interrupt type.
 */
static inline void usb_hal_ehci_enable_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USBHS_USBINTR_REG((USBHS_MemMapPtr)baseAddr) |= intrType;
}

/*!
 * @brief Set the interrupt register to a specific value.
 *
 * This function Set the interrupt register to a specific value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the specific interrupt type.
 */
static inline void usb_hal_ehci_interrupts_set(uint32_t baseAddr, uint32_t intrType)
{
   USBHS_USBINTR_REG((USBHS_MemMapPtr)baseAddr) = intrType;
}

/*!
 * @brief Disable the specific interrupts.
 *
 * This function Disable the specific interrupts.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the specific interrupt type.
 */
static inline void usb_hal_ehci_disable_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USBHS_USBINTR_REG((USBHS_MemMapPtr)baseAddr) &= ~intrType;
}

/*!
 * @brief Initiate attach event.
 *
 * This function Initiate attach event.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_initiate_attach_event(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) |= EHCI_CMD_RUN_STOP;
}

/*!
 * @brief Initiate deattach event.
 *
 * This function Initiate deattach event.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_initiate_detach_event(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= (uint32_t)(~(uint32_t)EHCI_CMD_RUN_STOP);
}

/*!
 * @brief Reset the controller.
 *
 * This function Reset the controller.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_reset_controller(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) = EHCI_CMD_CTRL_RESET;
}

/*!
 * @brief Set the dtd tripwire.
 *
 * This function set the dtd tripwire.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_dtd_tripwire(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) |= EHCI_CMD_ATDTW_TRIPWIRE_SET;
}

/*!
 * @brief Clear the dtd tripwire.
 *
 * This function Clear the dtd tripwire.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_clear_dtd_tripwire(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= (uint32_t)(~(uint32_t)EHCI_CMD_ATDTW_TRIPWIRE_SET);
}

/*!
 * @brief Disable the usb cmd async sched.
 *
 * This function Disable the usb cmd async sched.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_echi_disable_usb_cmd_async_sched(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= (uint32_t)(~(uint32_t)EHCI_USBCMD_ASYNC_SCHED_ENABLE);
}

/*!
 * @brief Enable the usb cmd async sched.
 *
 * This function Enable the usb cmd async sched.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_enable_usb_cmd_async_sched(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) |= EHCI_USBCMD_ASYNC_SCHED_ENABLE;
}

/*!
 * @brief Disable the usb cmd periodic sched.
 *
 * This function Disable the usb periodic async sched.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_disable_usb_cmd_periodic_sched(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= (uint32_t)(~(uint32_t)EHCI_USBCMD_PERIODIC_SCHED_ENABLE);
}

/*!
 * @brief Get the dtd tripwire status.
 *
 * This function Get the dtd tripwire status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the dtd tripwire status.
 */
static inline uint32_t usb_hal_ehci_is_tripwire_set(uint32_t baseAddr)
{
    return (USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) & EHCI_CMD_ATDTW_TRIPWIRE_SET);
}

/*!
 * @brief Clear the dtd tripwire status.
 *
 * This function Clear the dtd tripwire status.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_clear_usb_cmd_setup_trip_wire(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= (uint32_t)(~(uint32_t)EHCI_CMD_SETUP_TRIPWIRE_SET);
}

/*!
 * @brief Set the dtd tripwire status.
 *
 * This function Set the dtd tripwire status
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_set_usb_cmd_setup_trip_wire(uint32_t baseAddr)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) |= EHCI_CMD_SETUP_TRIPWIRE_SET;
}

/*!
 * @brief Set the usb cmd register to a specific value.
 *
 * This function Set the usb cmd register to a specific value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param value the specific value.
 */
static inline void usb_hal_ehci_set_usb_cmd(uint32_t baseAddr, uint32_t value)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) = value;
}

/*!
 * @brief Clear the usb cmd register.
 *
 * This function Clear the usb cmd register.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param value the specific value.
 */
static inline void usb_hal_ehci_clear_usb_cmd(uint32_t baseAddr, uint32_t value)
{
    USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr) &= ~(value);
}


/*!
 * @brief Get the usb cmd value.
 *
 * This function Get the usb cmd register value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the specific value.
 */
static inline uint32_t  usb_hal_ehci_get_usb_cmd(uint32_t baseAddr)
{
    return USBHS_USBCMD_REG((USBHS_MemMapPtr)baseAddr);
}


/*!
 * @brief Get the usb cap_length.
 *
 * This function Get the usb cap_length.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the cap length.
 */
static inline uint8_t usb_hal_ehci_get_cap_length(uint32_t baseAddr)
{
    return 0;
}

/*!
 * @brief Set the usb config register to a specific value.
 *
 * This function Set the usb config register to a specific value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param value the value needs to be set.
 */
static inline void usb_hal_ehci_set_usb_config(uint32_t baseAddr, uint32_t value)
{
    //USBHS_CONFIGFLAG = value;
}

/*!
 * @brief Set the usb endpoint prime to a specific value.
 *
 * This function Set the usb endpoint prime to a specific value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param value the value needs to be set.
 */
static inline void usb_hal_ehci_set_endpoint_prime(uint32_t baseAddr, uint32_t value)
{   
    USBHS_EPPRIME_REG((USBHS_MemMapPtr)baseAddr) = value;
}

/*!
 * @brief Get the usb endpoint prime  value.
 *
 * This function Get the usb endpoint prime  value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the value of endpoint prime.
 */
static inline uint32_t usb_hal_ehci_get_endpoint_prime(uint32_t baseAddr)
{
    return USBHS_EPPRIME_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Get the usb endpoint status.
 *
 * This function Get the usb endpoint status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the endpoint status.
 */
static inline uint32_t usb_hal_echi_get_endpoint_status(uint32_t baseAddr)
{
    return USBHS_EPSR_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Flush the endpoint buffer.
 *
 * This function Flush the endpoint buffer.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be flushed.
 */
static inline void usb_hal_ehci_flush_endpoint_buffer(uint32_t baseAddr, uint32_t epNumber)
{
    USBHS_EPFLUSH_REG((USBHS_MemMapPtr)baseAddr) = epNumber;
}

/*!
 * @brief Get the endpoint transfer buffer status.
 *
 * This function Get the endpoint transfer buffer status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber of the buffer status.
 * @return the transfer buffer status.
 */
static inline uint32_t usb_hal_ehci_is_endpoint_transfer_flushed(uint32_t baseAddr, uint32_t epNumber)
{
    return (USBHS_EPFLUSH_REG((USBHS_MemMapPtr)baseAddr) & epNumber);
}

/*!
 * @brief Get the frame index.
 *
 * This function Get the frame index.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the frame index.
 */
static inline uint32_t usb_hal_ehci_get_frame_index(uint32_t baseAddr)
{
    return USBHS_FRINDEX_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Get the port status.
 *
 * This function Get the port status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the port status.
 */
static inline uint32_t usb_hal_ehci_get_port_status(uint32_t baseAddr)
{
    return USBHS_PORTSC1_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Set the port status.
 *
 * This function Set the port status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param status the port status.
 */
static inline void usb_hal_ehci_set_port_status(uint32_t baseAddr, uint32_t status)
{
    USBHS_PORTSC1_REG((USBHS_MemMapPtr)baseAddr) = status;
}

/*!
 * @brief Get the usb interrupt status register value
 *
 * This function Get the usb interrupt status register value
 *
 * @param baseAddr the baseAddr of USB module.
 * @return  the usb interrupt status register value.
 */
static inline uint32_t usb_hal_ehci_get_usb_interrupt_status(uint32_t baseAddr)
{
    return USBHS_USBSTS_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Clear the usb interrupt status register value.
 *
 * This function Clear the usb interrupt status register value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param status the usb interrupt status register value.
 */
static inline void usb_hal_ehci_clear_usb_interrupt_status(uint32_t baseAddr, uint32_t status)
{
    USBHS_USBSTS_REG((USBHS_MemMapPtr)baseAddr) = status;
}

/*!
 * @brief Get the hcsparams value.
 *
 * This function Get the hcsparams value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the hcsparams value.
 */
static inline uint32_t usb_hal_ehci_get_hcsparams(uint32_t baseAddr)
{
    return USBHS_HCSPARAMS_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Get the hccparams value.
 *
 * This function Get the hccparams value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the hccparams value.
 */
static inline uint32_t usb_hal_ehci_get_hccparams(uint32_t baseAddr)
{
    return USBHS_HCCPARAMS_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Clear the device address.
 *
 * This function Clear the device address.
 *
 * @param baseAddr the baseAddr of USB module.
 */
static inline void usb_hal_ehci_clear_device_address(uint32_t baseAddr)
{
    USBHS_DEVICEADDR_REG((USBHS_MemMapPtr)baseAddr) &= ~0xFE000000;
}

/*!
 * @brief Set qh to curr async list.
 *
 * This function Set qh to curr async list.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param qh_addr the qh address.
 */
static inline void usb_hal_ehci_set_qh_to_curr_async_list(uint32_t baseAddr, uint32_t qh_addr)
{
    USBHS_ASYNCLISTADDR_REG((USBHS_MemMapPtr)baseAddr) = qh_addr;
}

/*!
 * @brief Get qh from curr async list.
 *
 * This function Get qh from curr async list.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the qh address.
 */
static inline uint32_t usb_hal_ehci_get_curr_async_list(uint32_t baseAddr)
{
   return USBHS_ASYNCLISTADDR_REG((USBHS_MemMapPtr)baseAddr);
}

/*!
 * @brief Set the periodic list base address.
 *
 * This function Set the periodic list base address.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param base_addr the periodic list base address.
 */
static inline void usb_hal_ehci_set_periodic_list_base_addr(uint32_t baseAddr, uint32_t base_addr)
{
    USBHS_PERIODICLISTBASE_REG((USBHS_MemMapPtr)baseAddr) = base_addr;
}

/*!
 * @brief Enable the specific endpoint.
 *
 * This function Enable the specific endpoint.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be enabled.
 * @param direction  the direction of the endpoint.
 * @param type the transfer type.
 */
static inline void usb_hal_ehci_enable_endpoint(uint32_t baseAddr, uint32_t epNumber, uint32_t direction, uint32_t type)
{
    if (epNumber == 0)
    {
        USBHS_EPCR0_REG((USBHS_MemMapPtr)baseAddr) |= ((direction ? (uint32_t)(EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (uint32_t)(EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
    }
    else
    {
        USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1) |= ((direction ? (uint32_t)(EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (uint32_t)(EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
    }
}

/*!
 * @brief Disable the specific endpoint.
 *
 * This function Disable the specific endpoint.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be disabled.
 * @param direction the directionof the endpoint.
 */
static inline void usb_hal_ehci_disable_endpoint(uint32_t baseAddr, uint32_t epNumber, uint32_t direction)
{
    USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr, epNumber) &= ~(direction ? ((uint32_t)EHCI_EPCTRL_TX_ENABLE|EHCI_EPCTRL_TX_TYPE):((uint32_t)EHCI_EPCTRL_RX_ENABLE|EHCI_EPCTRL_RX_TYPE));
}

/*!
 * @brief Get the endpoint ep control value.
 *
 * This function Get the endpoint ep control value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be get.
 * @return the control value.
 */
static inline uint32_t usb_hal_ehci_get_endpoint_control(uint32_t baseAddr, uint32_t epNumber)
{
    return (epNumber == 0 ? USBHS_EPCR0_REG((USBHS_MemMapPtr)baseAddr) : USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1));
}   

/*!
 * @brief Clear the endpoint  stall status.
 *
 * This function Clear the endpoint  stall status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be stalled.
 * @param direction the direction of endpoint.
 */
static inline void usb_hal_ehci_clear_endpoint_stall(uint32_t baseAddr, uint32_t epNumber, uint32_t direction)
{
    if (epNumber == 0)
    {
        USBHS_EPCR0_REG((USBHS_MemMapPtr)baseAddr) &= ~(direction ? (uint32_t)EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
    }
    else
    {
        USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1) &= ~(direction ? (uint32_t)EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
    }
}

/*!
 * @brief Reset the endpoint data toggle.
 *
 * This function Reset the endpoint data toggle.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be reset toggle.
 * @param direction the direction of the endpoint.
 */
static inline void usb_hal_ehci_reset_endpoint_data_toggle(uint32_t baseAddr, uint32_t epNumber, uint32_t direction)
{
    if (epNumber != 0)
    {
        USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1) |= (direction ? (uint32_t)EHCI_EPCTRL_TX_DATA_TOGGLE_RST : EHCI_EPCTRL_RX_DATA_TOGGLE_RST);
    }
}

/*!
 * @brief Stall the endpoint both directions.
 *
 * This function Stall the endpoint both directions.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be stalled.
 */
static inline void usb_hal_ehci_stall_both_directions(uint32_t baseAddr, uint32_t epNumber)
{
    if (epNumber == 0)
    {
        USBHS_EPCR0_REG((USBHS_MemMapPtr)baseAddr) |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
    }
    else
    {
        USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1) |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
    }
}

/*!
 * @brief Stall the endpoint specific directions.
 *
 * This function Stall the endpoint specific directions.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param epNumber the epNumber needs to be stalled.
 * @param direction the direction of endpoint.
 */
static inline void usb_hal_ehci_stall_specific_direction(uint32_t baseAddr, uint32_t epNumber, uint32_t direction)
{
    if (epNumber == 0)
    {
        USBHS_EPCR0_REG((USBHS_MemMapPtr)baseAddr) |= (uint32_t)(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
    }
    else
    {
        USBHS_EPCR_REG((USBHS_MemMapPtr)baseAddr,epNumber - 1) |= (uint32_t)(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
    }
}


/*!
 * @brief Set the device address register to a specific value.
 *
 * This function Set the device address register to a specific value.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param address the address needs to be set.
 */
static inline void usb_hal_ehci_set_device_address(uint32_t baseAddr, uint32_t address)
{
    USBHS_DEVICEADDR_REG((USBHS_MemMapPtr)baseAddr) = (uint32_t)(address << USBHS_ADDRESS_BIT_SHIFT);
}

/*!
 * @brief Clear the ep QH max packet length.
 *
 * This function Clear the ep QH max packet length.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr needs to be set.
 */
static inline void usb_hal_ehci_clear_max_packet_length(usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
    epQueueHeadAddr->MAX_PKT_LENGTH = 0;
}

/*!
 * @brief Set the ep QH max packet length.
 *
 * This function Set the ep QH max packet length.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr needs to be set.
 * @param maxPacketSize the maxPacketSize needs to be set.
 */
static inline void usb_hal_ehci_set_max_packet_length(usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t maxPacketSize)
{
    epQueueHeadAddr->MAX_PKT_LENGTH = ((uint32_t)maxPacketSize) ;
}

/*!
 * @brief Set the ep QH max packet length for non iso.
 *
 * This function Set the ep QH max packet length for non iso.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr's max length needs to be set.
 * @param value the value needs to be set.
 */
static inline void usb_hal_ehci_set_max_packet_length_for_non_iso(usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t value)
{
    epQueueHeadAddr->MAX_PKT_LENGTH = value;
}

/*!
 * @brief Get the ehci iso set value.
 *
 * This function Get the ehci iso set value.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr's IOS needs to be get.
 * @return the value
 */
static inline uint32_t usb_hal_ehci_is_ios_set( usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
    return (epQueueHeadAddr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS);
}

/*!
 * @brief Set the dtd terminate value.
 *
 * This function Set the dtd terminate value.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr needs to be set.
 */
static inline void usb_hal_ehci_set_next_dtd_terminate(usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
    epQueueHeadAddr->NEXT_DTD_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
}

/*!
 * @brief Set the next tr elem dtd terminate value.
 *
 * This function Set the next tr elem dtd terminate value.
 *
 * @param dTD_ptr the dTD_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_dtd_next_tr_elem_terminate(volatile usb_ehci_dev_dtd_struct_t * dTD_ptr)
{
   dTD_ptr->NEXT_TR_ELEM_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
}

/*!
 * @brief Set the next tr elem dtd ptr.
 *
 * This function Set the next tr elem dtd ptr.
 *
 * @param temp_dTD_ptr the temp_dTD_ptr needs to be set.
 * @param dTD_ptr the dTD_ptr needs to be set to next tr elem.
 */
static inline void usb_hal_ehci_set_dtd_next_tr_elem_ptr(volatile usb_ehci_dev_dtd_struct_t * temp_dTD_ptr, uint32_t dTD_ptr)
{
   temp_dTD_ptr->NEXT_TR_ELEM_PTR = dTD_ptr;
}

/*!
 * @brief Set the epQueueHeadAddr next dtd ptr.
 *
 * This function Set the epQueueHeadAddr next dtd ptr.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr needs to be set.
 * @param dtdAddr the dtdAddr needs to set to next dtd ptr.
 */
static inline void usb_hal_ehci_set_qh_next_dtd(usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t dtdAddr)
{
    epQueueHeadAddr->NEXT_DTD_PTR = dtdAddr;
}

/*!
 * @brief Set the next dtd invalid.
 *
 * This function Set the next dtd invalid.
 *
 * @param dtdAddr the dtdAddr needs to be invalid.
 */
static inline void usb_hal_ehci_set_next_dtd_invalid(usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    dtdAddr->NEXT_TR_ELEM_PTR = USBHS_TD_NEXT_TERMINATE;
}

/*!
 * @brief Clear the dtd size ioc status.
 *
 * This function Clear the dtd size ioc status.
 *
 * @param dtdAddr the dtdAddr's ios status needs to be cleared.
 */
static inline void usb_hal_ehci_clear_size_ioc_sts( usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    dtdAddr->SIZE_IOC_STS = 0;
}

/*!
 * @brief Clear the dtd size ioc status reserved fields.
 *
 * This function Clear the dtd size ioc status reserved fields.
 *
 * @param dtdAddr the dtdAddr needs to be cleared.
 */
static inline void usb_hal_ehci_clear_ioc_reserved_fields(volatile usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    dtdAddr->SIZE_IOC_STS &= ~((uint32_t)USBHS_TD_RESERVED_FIELDS);
}

/*!
 * @brief Clear the dtd.
 *
 * This function Clear the dtd.
 *
 * @param dtdAddr the dtdAddr needs to be cleared.
 */
static inline void usb_hal_ehci_clear_dtd(volatile usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    dtdAddr->NEXT_TR_ELEM_PTR = 0;
    dtdAddr->SIZE_IOC_STS = 0;
    dtdAddr->BUFF_PTR0 = 0;
    dtdAddr->BUFF_PTR1 = 0;
    dtdAddr->BUFF_PTR2 = 0;
    dtdAddr->BUFF_PTR3 = 0;
    dtdAddr->BUFF_PTR4 = 0;
}

/*!
 * @brief Set the dtd buffer address.
 *
 * This function Set the dtd buffer address.
 *
 * @param dtdAddr the dtdAddr needs to be set.
 * @param addr the addr needs to set to dtd buffer.
 */
static inline void usb_hal_ehci_set_dtd_buffer_address(volatile usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t addr)
{
    dtdAddr->BUFF_PTR0 = addr;
    dtdAddr->BUFF_PTR1 = addr + 4096;
    dtdAddr->BUFF_PTR2 = addr + (4096*2);
    dtdAddr->BUFF_PTR3 = addr + (4096*3);
    dtdAddr->BUFF_PTR4 = addr + (4096*4);
}

/*!
 * @brief Set the dtd size ioc status value.
 *
 * This function Set the dtd size ioc status value.
 *
 * @param dtdAddr the dtdAddr ioc status needs to be set.
 * @param value the value needs to be set.
 */
static inline void usb_hal_ehci_set_dtd_size_ioc_status(volatile usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t value)
{
    dtdAddr->SIZE_IOC_STS = value;
}

/*!
 * @brief Get the dtd size ioc status value.
 *
 * This function Get the dtd size ioc status value.
 *
 * @param dtdAddr the dtdAddr ioc status value needs to be get.
 * @return the dtd ioc status.
 */
static inline uint32_t usb_hal_ehci_get_dtd_size_ioc_status(volatile usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    return dtdAddr->SIZE_IOC_STS;
}

/*!
 * @brief Get the next dtd address.
 *
 * This function Get the next dtd address.
 *
 * @param dtdAddr the dtdAddr's next td address needs to be get.
 * @return the next dtd address.
 */
static inline uint32_t usb_hal_ehci_get_next_dtd_address(volatile usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    return (dtdAddr->NEXT_TR_ELEM_PTR & USBHS_TD_ADDR_MASK);
}

/*!
 * @brief Get the dtd error status.
 *
 * This function Get the dtd error status.
 *
 * @param dtdAddr the dtdAddr's error status needs to be get.
 * @return the dtd error status.
 */
static inline uint32_t usb_hal_ehci_get_dtd_error_status( usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    return (dtdAddr->SIZE_IOC_STS & USBHS_TD_ERROR_MASK);
}

/*!
 * @brief Clear the qh error status.
 *
 * This function Clear the qh error status.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr needs to be cleared.
 * @param errors the errors needs to clear.
 */
static inline void usb_hal_ehci_clear_qh_error_status( usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t errors)
{
    epQueueHeadAddr->SIZE_IOC_INT_STS &= ~errors;
}

/*!
 * @brief Get the tr packet size.
 *
 * This function Get the tr packet size.
 *
 * @param dtdAddr the dtdAddr's packet size needs to get.
 * @return the dtd tr packet size.
 */
static inline uint32_t usb_hal_ehci_get_tr_packet_size(usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    return ((dtdAddr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
}

/*!
 * @brief Get the xd for this dtd.
 *
 * This function Get the xd for this dtd.
 *
 * @param dtdAddr the dtdAddr's xd needs to be get.
 * @return the xd for this dtd.
 */
static inline uint32_t usb_hal_ehci_get_xd_for_this_dtd( usb_ehci_dev_dtd_struct_t * dtdAddr)
{
    return (uint32_t)dtdAddr->xd_for_this_dtd;
}

/*!
 * @brief Clear the ioc int status.
 *
 * This function Clear the ioc int status.
 *
 * @param epQueueHeadAddr the epQueueHeadAddr's ioc interrupt status needs to be cleared.
 */
static inline void usb_hal_ehci_clear_size_ioc_int_sts( usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
    epQueueHeadAddr->SIZE_IOC_INT_STS = 0;
}


/*!
 * @brief Get the fstn normal path link ptr.
 *
 * This function Get the fstn normal path link ptr.
 *
 * @param FSTN_ptr the FSTN_ptr's link ptr needs to be get.
 * @return the normal path link ptr.
 */
static inline uint32_t usb_hal_ehci_get_fstn_normal_path_link_ptr(ehci_fstn_struct_t* FSTN_ptr)
{
   return FSTN_ptr->normal_path_link_ptr;
}

/*!
 * @brief Set the fstn normal path link ptr.
 *
 * This function Set the fstn normal path link ptr.
 *
 * @param FSTN_ptr the FSTN_ptr's link ptr needs to be set.
 * @param next the normal path link ptr value.
 */
static inline void  usb_hal_ehci_set_fstn_normal_path_link_ptr(ehci_fstn_struct_t* FSTN_ptr, uint32_t next)
{
   FSTN_ptr->normal_path_link_ptr = next;
}

/*!
 * @brief Add the frame list pointer itd.
 *
 * This function Add the frame list pointer itd.
 *
 * @param transaction_descriptor the transaction_descriptor needs to be added.
 * @param itd_ptr the itd_ptr needs to be set to frame pointer itd.
 */
static inline void usb_hal_ehci_add_frame_list_pointer_itd( uint32_t *transaction_descriptor, ehci_itd_struct_t* itd_ptr)
{
   *transaction_descriptor = (uint32_t) itd_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_ITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS);
}

/*!
 * @brief Add the frame list pointer sitd.
 *
 * This function Add the frame list pointer sitd.
 *
 * @param transaction_descriptor the transaction_descriptor needs to be added.
 * @param sitd_ptr the sitd_ptr needs to be added to frame list pointer itd.
 */
static inline void usb_hal_ehci_add_frame_list_pointer_sitd( uint32_t *transaction_descriptor, ehci_sitd_struct_t* sitd_ptr)
{
   *transaction_descriptor = (uint32_t) sitd_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_SITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS);
}

/*!
 * @brief Get the periodic list address.
 *
 * This function Get the periodic list address.
 *
 * @param transfer_data_struct_ptr the transfer_data_struct_ptr needs to be get.
 * @return  the transfer_data_struct_ptr pointer.
 */
static inline uint32_t usb_hal_ehci_get_periodic_list_addr( volatile uint32_t*  transfer_data_struct_ptr)
{
   return (*transfer_data_struct_ptr);
}

/*!
 * @brief Get the periodic list address.
 *
 * This function Get the periodic list address.
 *
 * @param transfer_data_struct_ptr the transfer_data_struct_ptr needs to be set.
 * @param qh_ptr the qh_ptr needs to set to transfer_data_struct_ptr
 */
static inline void usb_hal_ehci_set_periodic_list_addr( volatile uint32_t*  transfer_data_struct_ptr, ehci_qh_struct_t* qh_ptr)
{
   *transfer_data_struct_ptr = ((uint32_t)qh_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_QH << EHCI_QH_ELEMENT_TYPE_BIT_POS));
}

/*!
 * @brief Set the transfer data struct.
 *
 * This function Set the transfer data struct.
 *
 * @param transfer_data_struct_ptr the transfer_data_struct_ptr needs to be set.
 * @param temp_transfer_struct_ptr  the temp_transfer_struct_ptr pointer set to transfer_data_struct_ptr.
 */
static inline void usb_hal_ehci_set_transfer_data_struct( volatile uint32_t*  transfer_data_struct_ptr, uint32_t temp_transfer_struct_ptr)
{
  *transfer_data_struct_ptr = temp_transfer_struct_ptr;
}


/*!
 * @brief Set the itd next link pointer.
 *
 * This function Set the itd next link pointer.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 * @param transaction_descriptor the transaction_descriptor pointer set to next linker pointer.
 */
static inline void usb_hal_ehci_set_itd_next_link_pointer(ehci_itd_struct_t* itd_ptr, uint32_t transaction_descriptor )
{
    itd_ptr->next_link_ptr = (uint32_t)transaction_descriptor;
}

/*!
 * @brief Get the itd next link pointer.
 *
 * This function Get the itd next link pointer.
 *
 * @param itd_ptr the itd_ptr needs to be get.
 * @return  the next link pointer.
 */
static inline uint32_t usb_hal_ehci_get_itd_next_link_pointer(ehci_itd_struct_t* itd_ptr)
{
   return itd_ptr->next_link_ptr;
}

/*!
 * @brief Set the itd next link pointer terminate bit.
 *
 * This function Set the itd next link pointer terminate bit.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_ITD_terminate_bit(ehci_itd_struct_t* itd_ptr)
{
   itd_ptr->next_link_ptr = EHCI_ITD_T_BIT;
}

/*!
 * @brief Store the endpoint number and device address.
 *
 * This function Store the endpoint number and device address.
 *
 * @param itd_ptr the itd_ptr needs to be stored.
 * @param value the value needs to be set to buffer page ptr.
 */
static inline void usb_hal_ehci_store_endpoint_number_and_device_addr( ehci_itd_struct_t* itd_ptr, uint32_t value)
{
    itd_ptr->buffer_page_ptr_list[0] = value;
}

/*!
 * @brief Store the max packet size and direction.
 *
 * This function Store the max packet size and direction.
 *
 * @param itd_ptr the itd_ptr needs to be stored.
 * @param value the value needs to be set to buffer page ptr.
 */
static inline void usb_hal_ehci_store_max_packet_size_and_direction( ehci_itd_struct_t* itd_ptr, uint32_t value)
{
    itd_ptr->buffer_page_ptr_list[1] = value;
}

/*!
 * @brief Store the  direction.
 *
 * This function Store the direction.
 *
 * @param itd_ptr the itd_ptr needs to be stored.
 * @param direction the direction needs to set.
 */
static inline void usb_hal_ehci_store_direction( ehci_itd_struct_t* itd_ptr, uint32_t direction)
{
    itd_ptr->buffer_page_ptr_list[1] |= (uint32_t)(direction << EHCI_ITD_DIRECTION_BIT_POS);
}

/*!
 * @brief Store the transaction number per micro frame.
 *
 * This function Store the transaction number per micro frame.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 * @param number the number of micro frame.
 */
static inline void usb_hal_ehci_set_transaction_number_per_micro_frame( ehci_itd_struct_t* itd_ptr, uint32_t number)
{
   itd_ptr->buffer_page_ptr_list[2] = number;
}

/*!
 * @brief Set buffer page pointer.
 *
 * This function Set buffer page pointer.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 * @param page_number the page_number of the itd.
 * @param buffer_ptr the buffer ptr pointer of the itd buffer page ptr.
 */
static inline void usb_hal_ehci_set_buffer_page_pointer( ehci_itd_struct_t* itd_ptr, uint32_t page_number, uint32_t buffer_ptr)
{
   itd_ptr->buffer_page_ptr_list[page_number] = buffer_ptr;
}

/*!
 * @brief Get buffer page pointer.
 *
 * This function Get buffer page pointer.
 *
 * @param itd_ptr the itd_ptr needs to be get.
 * @param page_number the page_number of the itd.
 * @return the buffer ptr pointer
 */
static inline uint32_t usb_hal_ehci_get_buffer_page_pointer( ehci_itd_struct_t* itd_ptr, uint32_t page_number)
{
   return itd_ptr->buffer_page_ptr_list[page_number];
}

/*!
 * @brief Set the transcation and control bit.
 *
 * This function Set the transcation and control bit.
 *
 * @param itd_ptr the itd_ptr needs to be get.
 * @param number the number of the tr_status ctrl.
 * @param value the value needs to be set.
 */
static inline void usb_hal_ehci_set_transcation_status_and_control_bit( ehci_itd_struct_t* itd_ptr, uint32_t number, uint32_t value)
{
  itd_ptr->tr_status_ctl_list[number] = value;
}

/*!
 * @brief Set the transcation status active bit.
 *
 * This function Set the transcation status active bit.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 * @param number the number of the ctrl list.
 */
static inline void usb_hal_ehci_set_transcation_status_active_bit( ehci_itd_struct_t* itd_ptr, uint32_t number)
{
   itd_ptr->tr_status_ctl_list[number] = itd_ptr->tr_status_ctl_list[number] | EHCI_ITD_ACTIVE_BIT;
}

/*!
 * @brief Get the transcation status control bit.
 *
 * This function Get the transcation status control bit.
 *
 * @param itd_ptr the itd_ptr needs to be get.
 * @param transaction_number the transaction_number of the ctrl list.
 * @return the transcation status control bit.
 */
static inline uint32_t usb_hal_ehci_get_transcation_status_ctl_list( ehci_itd_struct_t* itd_ptr, uint32_t transaction_number)
{
   return itd_ptr->tr_status_ctl_list[transaction_number];
}

/*!
 * @brief Set the itd length scheduled.
 *
 * This function Set the itd length scheduled.
 *
 * @param itd_ptr the itd_ptr needs to be set.
 * @param length_scheduled the length_scheduled set to itd_ptr.
 */
static inline void usb_hal_ehci_set_itd_length_scheduled( ehci_itd_struct_t* itd_ptr, uint32_t length_scheduled)
{
  itd_ptr->reserved[MEM_SIZE_INDEX] = length_scheduled;
}

/*!
 * @brief Set the sitd next link pointer.
 *
 * This function Set the sitd next link pointer.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param transaction_descriptor the transaction_descriptor set to next linker pointer.
 */
static inline void usb_hal_ehci_set_sitd_next_link_pointer(ehci_sitd_struct_t* sitd_ptr, uint32_t transaction_descriptor )
{
    sitd_ptr->next_link_ptr =  (uint32_t)transaction_descriptor;
}

/*!
 * @brief Get the sitd next link pointer.
 *
 * This function Get the sitd next link pointer.
 *
 * @param sitd_ptr the sitd_ptr needs to be get.
 * @return the transaction_descriptor.
 */
static inline uint32_t usb_hal_ehci_get_sitd_next_link_pointer(ehci_sitd_struct_t* sitd_ptr)
{
   return sitd_ptr->next_link_ptr;
}

/*!
 * @brief Get the sitd transfer status.
 *
 * This function Get the sitd transfer status.
 *
 * @param sitd_ptr the sitd_ptr status needs to be get.
 * @return the sitd transfer status.
 */
static inline uint32_t usb_hal_ehci_get_sitd_transfer_state(ehci_sitd_struct_t* sitd_ptr)
{
    return sitd_ptr->transfer_state;
}

/*!
 * @brief Set the sitd next link terminate bit.
 *
 * This function Set the sitd next link terminate bit.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_sitd_next_link_terminate_bit( ehci_sitd_struct_t* sitd_ptr)
{
    sitd_ptr->next_link_ptr = EHCI_SITD_T_BIT;
}

/*!
 * @brief Set the sitd back link terminate bit.
 *
 * This function Set the back next link terminate bit.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_sitd_back_pointer_terminate_bit( ehci_sitd_struct_t* sitd_ptr)
{
    sitd_ptr->back_link_ptr = EHCI_SITD_T_BIT;
}

/*!
 * @brief Set the sitd ep capab charac register.
 *
 * This function Set the sitd ep capab charac register.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param value the value set to ep_capab_charac.
 */
static inline void usb_hal_ehci_set_sitd_ep_capab_charac( ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->ep_capab_charac = value;
}

/*!
 * @brief Set the sitd uframe sched ctl register.
 *
 * This function Set the sitd uframe sched ctl register.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param value the value set to uframe_sched_ctl.
 */
static inline void usb_hal_ehci_set_sitd_uframe_sched_ctl( ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->uframe_sched_ctl= value;
}

/*!
 * @brief Set the sitd transfer state.
 *
 * This function Set the sitd transfer state.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param value the value set to transfer state.
 */
static inline void usb_hal_ehci_set_sitd_transfer_state( ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->transfer_state = value;
}

/*!
 * @brief Set the sitd length scheduled.
 *
 * This function Set the sitd length scheduled.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param length_scheduled the length_scheduled set to sitd.
 */
static inline void usb_hal_ehci_set_sitd_length_scheduled( ehci_sitd_struct_t* sitd_ptr, uint32_t length_scheduled)
{
   sitd_ptr->reserved[MEM_SIZE_INDEX] = length_scheduled;
}

/*!
 * @brief Get the sitd length scheduled.
 *
 * This function Get the sitd length scheduled.
 *
 * @param sitd_ptr the sitd_ptr needs to be get.
 * @return the length_scheduled.
 */
static inline uint32_t usb_hal_ehci_get_sitd_length_scheduled( ehci_sitd_struct_t* sitd_ptr)
{
  return sitd_ptr->reserved[MEM_SIZE_INDEX];
}

/*!
 * @brief Get the sitd buffer ptr0.
 *
 * This function Get the sitd buffer ptr0.
 *
 * @param sitd_ptr the sitd_ptr needs to be get.
 * @return the buffer ptr0.
 */
static inline uint32_t usb_hal_ehci_get_sitd_buffer_ptr_0( ehci_sitd_struct_t* sitd_ptr)
{
  return sitd_ptr->buffer_ptr_0;
}

/*!
 * @brief Set the sitd buffer ptr0.
 *
 * This function Set the sitd buffer ptr0.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param buffer_ptr the buffer ptr0 set to sitd.
 */
static inline void usb_hal_ehci_set_sitd_buffer_ptr_0( ehci_sitd_struct_t* sitd_ptr, uint32_t buffer_ptr)
{
   sitd_ptr->buffer_ptr_0 = buffer_ptr;
}

/*!
 * @brief Set the sitd buffer ptr1.
 *
 * This function Set the sitd buffer ptr1.
 *
 * @param sitd_ptr the sitd_ptr needs to be set.
 * @param buffer_ptr the buffer ptr1 set to sitd.
 */
static inline void usb_hal_ehci_set_sitd_buffer_ptr_1( ehci_sitd_struct_t* sitd_ptr, uint32_t buffer_ptr)
{
   sitd_ptr->buffer_ptr_1 = buffer_ptr;
}

/*!
 * @brief Set the sitd alt qtd link pointer terminate bit.
 *
 * This function Set the sitd alt qtd link pointer terminate bit.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_qh_next_alt_qtd_link_terminate( ehci_qh_struct_t* qh_ptr)
{
   qh_ptr->alt_next_qtd_link_ptr = EHCI_QTD_T_BIT;
}

/*!
 * @brief Set the qh next qtd link pointer.
 *
 * This function Set the qh next qtd link pointer.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 * @param first_qtd_ptr the first_qtd_ptr set to next qtd link.
 */
static inline void usb_hal_ehci_set_qh_next_qtd_link_ptr( ehci_qh_struct_t* qh_ptr, uint32_t first_qtd_ptr)
{

   qh_ptr->next_qtd_link_ptr = (uint32_t)first_qtd_ptr;
}

/*!
 * @brief Set the qh next qtd link terminate bit.
 *
 * This function Set the qh next qtd link terminate bit.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_qh_next_qtd_link_terminate( ehci_qh_struct_t* qh_ptr)

{
   qh_ptr->next_qtd_link_ptr = EHCI_QTD_T_BIT;
}

/*!
 * @brief Get the qh next qtd link pointer.
 *
 * This function Get the qh next qtd link pointer.
 *
 * @param qh_ptr the qh_ptr needs to be get.
 * @return the qh_ptr next qtd link pointer.
 */
static inline uint32_t usb_hal_ehci_get_next_qtd_link_ptr( ehci_qh_struct_t* qh_ptr)
{
  return (uint32_t)qh_ptr->next_qtd_link_ptr;
}

/*!
 * @brief Clear the qh status.
 *
 * This function Clear the qh status.
 *
 * @param qh_ptr the qh_ptr needs to be cleared.
 */
static inline void usb_hal_ehci_clear_qh_status( ehci_qh_struct_t* qh_ptr)
{
  qh_ptr->status = 0;
}

/*!
 * @brief Return the qh status.
 *
 * This function Clear the qh status.
 *
 * @param qh_ptr the qh_ptr needs to be get.
 * @return the qh_ptr status.
 */
static inline uint32_t usb_hal_ehci_get_qh_status( ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->status;
}

/*!
 * @brief Set the qh status.
 *
 * This function Set the qh status.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 * @param status the qh_ptr status set to status.
 */
static inline void usb_hal_ehci_set_qh_status( ehci_qh_struct_t* qh_ptr, uint32_t status)
{
   qh_ptr->status = status;
}

/*!
 * @brief Get the qh horiz link ptr.
 *
 * This function Get the qh horiz link ptr.
 *
 * @param qh_ptr the qh_ptr needs to be get.
 * @return the qh horiz link ptr.
 */
static inline uint32_t usb_hal_ehci_get_qh_horiz_link_ptr( ehci_qh_struct_t* qh_ptr)
{
  return qh_ptr->horiz_link_ptr;
}

/*!
 * @brief Set the qh horiz link ptr.
 *
 * This function Set  the qh horiz link ptr.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 * @param link_ptr the link_ptr set to horiz.
 */
static inline void usb_hal_ehci_set_qh_horiz_link_ptr( ehci_qh_struct_t* qh_ptr, uint32_t link_ptr)
{

   qh_ptr->horiz_link_ptr = link_ptr;
}

/*!
 * @brief Init the qh.
 *
 * This function Init the qh.
 *
 * @param qh_ptr the qh_ptr needs to be init.
 */
static inline void usb_hal_ehci_init_qh( ehci_qh_struct_t* qh_ptr)
{
    qh_ptr->curr_qtd_link_ptr = EHCI_QTD_T_BIT;
    qh_ptr->alt_next_qtd_link_ptr = EHCI_QTD_T_BIT;
    qh_ptr->status = 0;
    qh_ptr->buffer_ptr_0 = 0;
    qh_ptr->buffer_ptr_1 = 0;
    qh_ptr->buffer_ptr_2 = 0;
    qh_ptr->buffer_ptr_3 = 0;
    qh_ptr->buffer_ptr_4 = 0;
}

/*!
 * @brief Set the ep capab charac1.
 *
 * This function Set the ep capab charac1.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 * @param temp_ep_capab the temp_ep_capab set to qh.
 */
static inline void usb_hal_ehci_set_ep_capab_charac1( ehci_qh_struct_t* qh_ptr, uint32_t temp_ep_capab)
{
   qh_ptr->ep_capab_charac1 = temp_ep_capab;
}

/*!
 * @brief Get the ep capab charac1.
 *
 * This function Get the ep capab charac1.
 *
 * @param qh_ptr the qh_ptr needs to be get.
 * @return the ep_capab1.
 */
static inline uint32_t usb_hal_ehci_get_ep_capab_charac1( ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->ep_capab_charac1;
}

/*!
 * @brief Set the ep capab charac2.
 *
 * This function Set the ep capab charac2.
 *
 * @param qh_ptr the qh_ptr needs to be set.
 * @param temp_ep_capab the temp_ep_capab set to qh.
 */
static inline void usb_hal_ehci_set_ep_capab_charac2( ehci_qh_struct_t* qh_ptr, uint32_t temp_ep_capab)
{
   qh_ptr->ep_capab_charac2 = temp_ep_capab;
}

/*!
 * @brief Get the ep capab charac2.
 *
 * This function Get the ep capab charac2.
 *
 * @param qh_ptr the qh_ptr needs to be get.
 * @return the ep_capab2.
 */
static inline uint32_t usb_hal_ehci_get_ep_capab_charac2( ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->ep_capab_charac2;
}

/*!
 * @brief Set the horiz link ptr terminate. 
 *
 * This function Set the horiz link ptr terminate. 
 *
 * @param qh_ptr the qh_ptr needs to be set.
 */
static inline void usb_hal_ehci_set_qh_horiz_link_ptr_head_pointer_terminate( ehci_qh_struct_t* qh_ptr)
{
   qh_ptr->horiz_link_ptr = EHCI_QUEUE_HEAD_POINTER_T_BIT;
}

/*!
 * @brief Set the buffer page pointer.
 *
 * This function Set the buffer page pointer.
 *
 * @param qtd_ptr the qtd_ptr needs to be set.
 * @param buffer_start_address the buffer_start_address set to qh.
 */
static inline void usb_hal_ehci_set_qtd_buffer_page_pointer( ehci_qtd_struct_t* qtd_ptr, uint32_t buffer_start_address)
{
   qtd_ptr->buffer_ptr_0 = buffer_start_address;
   qtd_ptr->buffer_ptr_1 = qtd_ptr->buffer_ptr_0 + 4096;
   qtd_ptr->buffer_ptr_2 = qtd_ptr->buffer_ptr_1 + 4096;
   qtd_ptr->buffer_ptr_3 = qtd_ptr->buffer_ptr_2 + 4096;
   qtd_ptr->buffer_ptr_4 = qtd_ptr->buffer_ptr_3 + 4096;
}

/*!
 * @brief Set the qtd token.
 *
 * This function Set the qtd token.
 *
 * @param qtd_ptr the qtd_ptr needs to be set.
 * @param token the token needs to be set to qtd_ptr.
 */
static inline void usb_hal_ehci_set_qtd_token( ehci_qtd_struct_t* qtd_ptr, uint32_t token)
{

   qtd_ptr->token = token;
}

/*!
 * @brief Clear the qtd token bits.
 *
 * This function Clear the qtd token bits.
 *
 * @param qtd_ptr the qtd_ptr needs to be cleared.
 * @param token_bits the token_bits needs to be cleared.
 */
static inline void usb_hal_ehci_clear_qtd_token_bits( ehci_qtd_struct_t* qtd_ptr, uint32_t token_bits)
{
   qtd_ptr->token &= ~token_bits;
}

/*!
 * @brief Get the qtd token.
 *
 * This function Get the qtd token.
 *
 * @param qtd_ptr the qtd_ptr's token needs to be get.
 * @return the qtd token.
 */
static inline uint32_t usb_hal_ehci_get_qtd_token( ehci_qtd_struct_t* qtd_ptr)
{

   return qtd_ptr->token;
}

/*!
 * @brief Set the qtd next ptr terminate.
 *
 * This function Set the qtd next ptr terminate.
 *
 * @param qtd_ptr the qtd_ptr's next ptr needs to be set.
 */
static inline void usb_hal_ehci_set_qtd_terminate_bit( ehci_qtd_struct_t* qtd_ptr)
{
    qtd_ptr->next_qtd_ptr = EHCI_QTD_T_BIT;
}

/*!
 * @brief Get the qtd next ptr. 
 *
 * This function Get the qtd next ptr.
 *
 * @param qtd_ptr the qtd_ptr's next ptr needs to be get.
 * @return the qtd next ptr.
 */
static inline uint32_t usb_hal_ehci_get_next_qtd_ptr(ehci_qtd_struct_t* qtd_ptr)
{
    return qtd_ptr->next_qtd_ptr;
}

/*!
 * @brief Set the alt next qtd terminate bit.
 *
 * This function Set the alt next qtd terminate bit.
 *
 * @param qtd_ptr the qtd_ptr's next ptr needs to be set.
 */
static inline void usb_hal_ehci_set_alt_next_qtd_terminate_bit( ehci_qtd_struct_t* qtd_ptr)
{
  qtd_ptr->alt_next_qtd_ptr = EHCI_QTD_T_BIT;
}

/*!
 * @brief Set the next qtd ptr.
 *
 * This function Set the next qtd ptr.
 *
 * @param prev_qtd_ptr the prev_qtd_ptr needs to be set.
 * @param qtd_ptr the qtd_ptr set to next_qtd_ptr.
 */
static inline void usb_hal_ehci_link_qtd( ehci_qtd_struct_t* prev_qtd_ptr,uint32_t qtd_ptr)
{
   prev_qtd_ptr->next_qtd_ptr = qtd_ptr;
}

/*!
 * @brief Enable the otg interrupts.
 *
 * This function Enable the otg interrupts.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the intrType needs to be enabled.
 */
static inline void usb_hal_ehci_enable_otg_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USBHS_OTGSC_REG((USBHS_MemMapPtr)baseAddr) |= intrType;
}

/*!
 * @brief Disable the otg interrupts.
 *
 * This function Disable the otg interrupts.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the intrType needs to be disabled.
 */
static inline void usb_hal_ehci_disable_otg_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USBHS_OTGSC_REG((USBHS_MemMapPtr)baseAddr) &= ~intrType;
}

/*!
 * @brief Clear the otg interrupts.
 *
 * This function Clear the otg interrupts.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param intrType the intrType needs to be cleared.
 */
static inline void usb_hal_ehci_clear_otg_interrupts(uint32_t baseAddr, uint32_t intrType)
{
    USBHS_OTGSC_REG((USBHS_MemMapPtr)baseAddr)|= intrType;
}

/*!
 * @brief Get the otg status.
 *
 * This function Get the otg status.
 *
 * @param baseAddr the baseAddr of USB module.
 * @return the otg status.
 */
static inline uint32_t usb_hal_ehci_get_otg_status(uint32_t baseAddr)
{
    return USBHS_OTGSC_REG((USBHS_MemMapPtr)baseAddr);
    
}

/*!
 * @brief Set the gpt timer.
 *
 * This function Set the gpt timer.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param gpt_index the gpt_index of the timer.
 * @param ms the ms needs to be set.
 */

static inline void usb_hal_ehci_set_gpt_timer(uint32_t baseAddr, uint32_t gpt_index, uint32_t ms)
{

    if( gpt_index == 0)
    {
        USBHS_GPTIMER0LD_REG((USBHS_MemMapPtr)baseAddr) = ms*0x3E7;
    }
    else if( gpt_index == 1)
    {
        USBHS_GPTIMER1LD_REG((USBHS_MemMapPtr)baseAddr) = ms*0x3E7;
    }
}

/*!
 * @brief Run the gpt timer.
 *
 * This function Run the gpt timer.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param gpt_index the gpt_index of the timer.
 */
static inline void usb_hal_ehcit_gpt_timer_run(uint32_t baseAddr, uint32_t gpt_index)
{
    if( gpt_index == 0)
    {
        USBHS_GPTIMER0CTL_REG((USBHS_MemMapPtr)baseAddr) |= USBHS_GPTIMER0CTL_MODE_MASK | USBHS_GPTIMER0CTL_RST_MASK |USBHS_GPTIMER0CTL_RUN_MASK;
    }
    else if( gpt_index == 1)
    {
        USBHS_GPTIMER1CTL_REG((USBHS_MemMapPtr)baseAddr)  |= USBHS_GPTIMER1CTL_MODE_MASK | USBHS_GPTIMER1CTL_RST_MASK |USBHS_GPTIMER1CTL_RUN_MASK;
    }
}

/*!
 * @brief Stop the gpt timer.
 *
 * This function Stop the gpt timer.
 *
 * @param baseAddr the baseAddr of USB module.
 * @param gpt_index the gpt_index of the timer.
 */
static inline void usb_hal_ehcit_gpt_timer_stop(uint32_t baseAddr, uint32_t gpt_index)
{
    if( gpt_index == 0)
    {
        USBHS_GPTIMER0CTL_REG((USBHS_MemMapPtr)baseAddr) &= ~USBHS_GPTIMER0CTL_RUN_MASK;
    }
    else if( gpt_index == 1)
    {
        USBHS_GPTIMER1CTL_REG((USBHS_MemMapPtr)baseAddr)  &= ~USBHS_GPTIMER1CTL_RUN_MASK;
    }
}
#if defined(__cplusplus)
}
#endif

#endif
