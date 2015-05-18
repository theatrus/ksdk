/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: usb_uart_drv.c$
 * $Version : 3.8.28.0$
 * $Date    : Sep-13-2012$
 *
 * Comments:
 *
 *   This file contains the UART drivers for USB demo/example reference.
 *
 *END************************************************************************/
#include "board.h"
#include "usb_uart_drv.h"
extern void UART_DRV_IRQHandler(uint32_t instance);
/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @addtogroup uart_driver
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Convert the return value of UART-like functions.
 *
 * @param instance The UART instance number.
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t _USB_UART_DRV_Convert_RetVal(uint32_t retval)
{
    switch(retval)
    {
    case kStatus_UART_Success:
        return kStatus_USB_UART_Success;

    case kStatus_UART_BaudRateCalculationError:
        return kStatus_USB_UART_BaudRateCalculationError;

    case kStatus_UART_RxStandbyModeError:
        return kStatus_USB_UART_RxStandbyModeError;

    case kStatus_UART_ClearStatusFlagError:
        return kStatus_USB_UART_ClearStatusFlagError;

    case kStatus_UART_TxNotDisabled:
        return kStatus_USB_UART_TxNotDisabled;

    case kStatus_UART_RxNotDisabled:
        return kStatus_USB_UART_RxNotDisabled;

    case kStatus_UART_TxOrRxNotDisabled:
        return kStatus_USB_UART_TxOrRxNotDisabled;

    case kStatus_UART_TxBusy:
        return kStatus_USB_UART_TxBusy;

    case kStatus_UART_RxBusy:
        return kStatus_USB_UART_RxBusy;

    case kStatus_UART_NoTransmitInProgress:
        return kStatus_USB_UART_NoTransmitInProgress;

    case kStatus_UART_NoReceiveInProgress:
        return kStatus_USB_UART_NoReceiveInProgress;

    case kStatus_UART_Timeout:
        return kStatus_USB_UART_Timeout;

    case kStatus_UART_Initialized:
        return kStatus_USB_UART_Initialized;

    case kStatus_UART_NoDataToDeal:
        return kStatus_USB_UART_NoDataToDeal;
    default:
        return kStatus_USB_UART_Success;
    }
}

/*!
 * @brief Convert the return value of UART-like functions.
 *
 * @param instance The UART instance number.
 * @return An error code or kStatus_UART_Success.
 */
IRQn_Type USB_UART_Get_USB_iRQ_Num(uint32_t instance)
{
    return g_uartRxTxIrqId[instance];
}

/*!
 * @name UART Interrupt Driver
 * @{
 */

/*!
 * @brief Initializes an UART instance for operation.
 *
 * This function initializes the run-time state structure to keep track of the on-going
 * transfers, ungates the clock to the UART module, initializes the module
 * to user-defined settings and default settings, configures the IRQ state structure and enables
 * the module-level interrupt to the core, and enables the UART module transmitter and receiver.
 * This example shows how to set up the usb_uart_state_t and the
 * usb_uart_user_config_t parameters and how to call the USB_UART_DRV_Init function by passing
 * in these parameters:
 @code
 usb_uart_user_config_t uartConfig;
 uartConfig.baudRate = 9600;
 uartConfig.bitCountPerChar = kUart8BitsPerChar;
 uartConfig.parityMode = kUartParityDisabled;
 uartConfig.stopBitCount = kUartOneStopBit;
 usb_uart_state_t uartState;
 USB_UART_DRV_Init(instance, &uartState, &uartConfig);
 @endcode
 *
 * @param instance The UART instance number.
 * @param uartStatePtr A pointer to the UART driver state structure memory. The user 
 * passes in the memory for this run-time state structure. The UART driver
 *  populates the members. The run-time state structure keeps track of the
 *  current transfer in progress.
 * @param uartUserConfig The user configuration structure of type uart_user_config_t. The user
 *  populates the members of this structure and passes the pointer of this structure
 *  to this function.
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t USB_UART_DRV_Init(uint32_t instance, usb_uart_state_t * uartStatePtr,
    const usb_uart_user_config_t * uartUserConfig)
{
    uart_status_t uart_sts;
    uart_user_config_t uartUserCfg;
    uartUserCfg.baudRate = uartUserConfig->baudRate;
    uartUserCfg.parityMode = (uart_parity_mode_t) uartUserConfig->parityMode;
    uartUserCfg.stopBitCount = (uart_stop_bit_count_t) uartUserConfig->stopBitCount;
    uartUserCfg.bitCountPerChar = (uart_bit_count_per_char_t) uartUserConfig->bitCountPerChar;
    uart_sts = UART_DRV_Init(instance, (uart_state_t *) uartStatePtr, &uartUserCfg);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Shuts down the UART by disabling interrupts and the transmitter/receiver.
 *
 * This function disables the UART interrupts, disables the transmitter and receiver, and
 * flushes the FIFOs (for modules that support FIFOs).
 *
 * @param instance The UART instance number.
 */
void USB_UART_DRV_Deinit(uint32_t instance)
{
    UART_DRV_Deinit(instance);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_IRQHandler
 * Description   : Interrupt handler for UART.
 * This handler uses the buffers stored in the uart_state_t structs to transfer
 * data. This is not a public API as it is called whenever an interrupt occurs.
 *
 *END**************************************************************************/
void USB_UART_DRV_IRQHandler(uint32_t instance)
{
    UART_DRV_IRQHandler(instance);
}

/*!
 * @brief Installs callback function for the UART receive.
 *
 * @note Once a callback is installed, it bypasses the UART driver logic.
 * So, the callback needs to handle the indexes of rxBuff, rxSize.
 *
 * @param instance The UART instance number.
 * @param function The UART receive callback function.
 * @param rxBuff The receive buffer used inside IRQHandler. This buffer must be kept as long as the callback is alive.
 * @param callbackParam The UART receive callback parameter pointer.
 * @param alwaysEnableRxIrq Whether always enable Rx IRQ or not.
 * @return Former UART receive callback function pointer.
 */
usb_uart_rx_callback_t USB_UART_DRV_InstallRxCallback(uint32_t instance,
    usb_uart_rx_callback_t function,
    uint8_t * rxBuff,
    void * callbackParam,
    bool alwaysEnableRxIrq)
{
    return (usb_uart_rx_callback_t) UART_DRV_InstallRxCallback(instance, (uart_rx_callback_t) function, rxBuff, callbackParam, alwaysEnableRxIrq);
}

/*!
 * @brief Sends (transmits) data out through the UART module using a blocking method.
 *
 * A blocking (also known as synchronous) function means that the function does not return until
 * the transmit is complete. This blocking function is used to send data through the UART port.
 *
 * @param instance The UART instance number.
 * @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 * @param txSize The number of bytes to send.
 * @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t USB_UART_DRV_SendDataBlocking(uint32_t instance,
    const uint8_t * txBuff,
    uint32_t txSize,
    uint32_t timeout)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_SendDataBlocking(instance, txBuff, txSize, timeout);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Sends (transmits) data through the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the transmit function. The application has to get the
 * transmit status to see when the transmit is complete. In other words, after calling non-blocking
 * (asynchronous) send function, the application must get the transmit status to check if transmit
 * is complete.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 * @param txSize The number of bytes to send.
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t USB_UART_DRV_SendData(uint32_t instance, const uint8_t * txBuff, uint32_t txSize)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_SendData(instance, txBuff, txSize);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Returns whether the previous UART transmit has finished.
 *
 * When performing an async transmit, call this function to ascertain the state of the
 * current transmission: in progress (or busy) or complete (success). If the
 * transmission is still in progress, the user can obtain the number of words that have been
 * transferred.
 *
 * @param instance The UART module base address.
 * @param bytesRemaining A pointer to a value that is filled in with the number of bytes that
 *                       are remaining in the active transfer.
 * @return The transmit status.
 * @retval kStatus_UART_Success The transmit has completed successfully.
 * @retval kStatus_UART_TxBusy The transmit is still in progress. @a bytesTransmitted is
 *     filled with the number of bytes which are transmitted up to that point.
 */
usb_uart_status_t USB_UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t * bytesRemaining)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_GetTransmitStatus(instance, bytesRemaining);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Terminates an asynchronous UART transmission early.
 *
 * During an async UART transmission, the user can terminate the transmission early
 * if the transmission is still in progress.
 *
 * @param instance The UART module base address.
 * @return Whether the aborting success or not.
 * @retval kStatus_UART_Success The transmit was successful.
 * @retval kStatus_UART_NoTransmitInProgress No transmission is currently in progress.
 */
usb_uart_status_t USB_UART_DRV_AbortSendingData(uint32_t instance)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_AbortSendingData(instance);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Gets (receives) data from the UART module using a blocking method.
 *
 * A blocking (also known as synchronous) function means that the function does not return until
 * the receive is complete. This blocking function sends data through the UART port.
 *
 * @param instance The UART module base address.
 * @param rxBuff A pointer to the buffer containing 8-bit read data chars received.
 * @param rxSize The number of bytes to receive.
 * @param timeout A timeout value for RTOS abstraction sync control in milliseconds (ms).
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t USB_UART_DRV_ReceiveDataBlocking(uint32_t instance,
    uint8_t * rxBuff,
    uint32_t rxSize,
    uint32_t timeout)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_ReceiveDataBlocking(instance, rxBuff, rxSize, timeout);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}
/*!
 * @brief Gets (receives) data from the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the receive function. The application has to get the
 * receive status to see when the receive is complete. In other words, after calling non-blocking
 * (asynchronous) get function, the application must get the receive status to check if receive
 * is completed or not.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @param rxBuff  A pointer to the buffer containing 8-bit read data chars received.
 * @param rxSize The number of bytes to receive.
 * @return An error code or kStatus_UART_Success.
 */
usb_uart_status_t USB_UART_DRV_ReceiveData(uint32_t instance, uint8_t * rxBuff, uint32_t rxSize)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_ReceiveData(instance, rxBuff, rxSize);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Returns whether the previous UART receive is complete.
 *
 * When performing an async receive, call this function to find out the state of the
 * current receive progress: in progress (or busy) or complete (success). In addition, if the
 * receive is still in progress, the user can obtain the number of words that have been
 * currently received.
 *
 * @param instance The UART module base address.
 * @param bytesRemaining A pointer to a value that is filled in with the number of bytes which
 *                       still need to be received in the active transfer.
 * @return The receive status.
 * @retval kStatus_UART_Success The receive has completed successfully.
 * @retval kStatus_UART_RxBusy The receive is still in progress. @a bytesReceived is
 *     filled with the number of bytes which are received up to that point.
 */
usb_uart_status_t USB_UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t * bytesRemaining)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_GetReceiveStatus(instance, bytesRemaining);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*!
 * @brief Terminates an asynchronous UART receive early.
 *
 * During an async UART receive, the user can terminate the receive early
 * if the receive is still in progress.
 *
 * @param instance The UART module base address.
 * @return Whether the aborting success or not.
 * @retval kStatus_UART_Success The receive was successful.
 * @retval kStatus_UART_NoTransmitInProgress No receive is currently in progress.
 */
usb_uart_status_t USB_UART_DRV_AbortReceivingData(uint32_t instance)
{
    uart_status_t uart_sts;
    uart_sts = UART_DRV_AbortReceivingData(instance);
    return _USB_UART_DRV_Convert_RetVal(uart_sts);
}

/*@}*/

/*! @}*/

/*******************************************************************************
 * EOF
 ******************************************************************************/
