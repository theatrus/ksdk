/*
* Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "fsl_uart_driver.h"
#include "fsl_flexcan_driver.h"
#include "fsl_pit_driver.h"
#include "fsl_interrupt_manager.h"
#include "fsl_clock_manager.h"
#include "board.h"

/* The following tables are the CAN bit timing parameters that are calculated by using the method
 * outlined in AN1798, section 4.1.
 */
/*
 * The table contains propseg, pseg1, pseg2, pre_divider, and rjw. The values are calculated for
 * a protocol engine clock of 60MHz
 */
flexcan_time_segment_t bit_rate_table_60Mhz[] = {
    { 6, 7, 7, 19, 3},  /* 125 kHz */
    { 6, 7, 7,  9, 3},  /* 250 kHz */
    { 6, 7, 7,  4, 3},  /* 500 kHz */
    { 6, 5, 5,  3, 3},  /* 750 kHz */
    { 6, 5, 5,  2, 3},  /* 1   MHz */
};

/*
 * The table contains propseg, pseg1, pseg2, pre_divider, and rjw. The values are calculated for
 * a protocol engine clock of 48MHz
 */
flexcan_time_segment_t bit_rate_table_48Mhz[] = {
    { 6, 7, 7, 15, 3},  /* 125 kHz */
    { 6, 7, 7,  7, 3},  /* 250 kHz */
    { 6, 7, 7,  3, 3},  /* 500 kHz */
    { 6, 3, 3,  3, 3},  /* 750 kHz */
    { 6, 3, 3,  2, 3},  /* 1   MHz */
};

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static void pit0_isr_callback(uint32_t channel);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

volatile uint8_t timeout_flag = false;  //disable timeout event

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Init PIT timer
 */
static void init_pit_timer(uint8_t pitInstance)
{
    // Structure of initialize PIT, this will enable PIT module as default
    pit_user_config_t pitConfig = {
        .isInterruptEnabled = true,
        .isTimerChained = false,
        // for 115200bps 8N1, 1 charactor is around 87us, here set 4ms timeout
        .periodUs = 4000
    };

    // clock enable in the pit driver, and disable timer run in debug mode
    PIT_DRV_Init(pitInstance, false);

    // Init pit timer.
    PIT_DRV_InitChannel(pitInstance, 0, &pitConfig);

    // Register a callback from the pit0 ISR
    PIT_DRV_InstallCallback(pitInstance, 0, pit0_isr_callback);

    // start pit timer 0
    PIT_DRV_StartTimer(pitInstance, 0);
}

//! @brief main function.
int main(void)
{
    uint8_t ch0[8], ch1[8], ch2[8], *ch; // ch0 and ch1 for uart receiving buffer, ch2 for uart transfer buffer
    uint8_t node_type; // var used to store the node type input from the console
    uint32_t remainingReceiveByteCount, receivedByteCount;
    uart_status_t status;
    uint32_t result;
    uint32_t rx_fifo_id[8]; // though rx fifo is not used in this demo, keep it for future useage
    uint32_t temp, i, j;
    flexcan_state_t can_state;
    flexcan_user_config_t flexcan1_data;
    uint32_t TX_identifier;
    uint32_t RX_identifier;
    uint32_t TX_mailbox_num;
    uint32_t RX_mailbox_num;
    uint32_t canPeClk;
    flexcan_id_table_t id_table;
    flexcan_mb_t rx_fifo;
    flexcan_mb_t rx_mb;

    flexcan_data_info_t tx_info;
    flexcan_data_info_t rx_info;

    uint8_t flexcanInstance = BOARD_CAN_INSTANCE;
    uint8_t uartInstance = BOARD_DEBUG_UART_INSTANCE;
    uint32_t uartBaseAddr = BOARD_DEBUG_UART_BASEADDR;
    uint8_t pitInstance = 0; // PIT0 is used
    // instantiate variable uartState of type uart_state_t
    uart_state_t uartState;
    uart_user_config_t uartConfig;
    uartConfig.baudRate = 115200u;
    uartConfig.bitCountPerChar = kUart8BitsPerChar;
    uartConfig.parityMode = kUartParityDisabled;
    uartConfig.stopBitCount = kUartOneStopBit;

    // init the uart module with base address and config structure
    UART_DRV_Init(uartInstance, &uartState, &uartConfig);

    OSA_Init();

    hardware_init();
    configure_can_pins(BOARD_CAN_INSTANCE);
    dbg_uart_init();

    init_pit_timer(pitInstance);

    flexcan1_data.max_num_mb = 16u;
    flexcan1_data.num_id_filters = kFlexCanRxFifoIDFilters_8;
    flexcan1_data.is_rx_fifo_needed = false; //disable fifo here

    /* Select mailbox number */
    RX_mailbox_num = 8u;
    TX_mailbox_num = 9u;

    printf("\r\n*********FlexCAN : SCI2CAN demo *********");
    printf("\r\n   Message format: Standard (11 bit id)");
    printf("\r\n   Message buffer 8 used for Rx.");
    printf("\r\n   Message buffer 9 used for Tx.");
    printf("\r\n   OSJTAG Port used for Serial Console.");
    printf("\r\n   Interrupt Mode: Enabled");
    printf("\r\n   Operation Mode: TX and RX --> Normal");
    printf("\r\n*****************************************\r\n");

    do {
        printf("Please select local node as A or B:\r\n");
        printf("Node:");
        node_type=getchar();
        printf("%c",node_type);
        printf("\r\n");
    }while((node_type!='A') && (node_type!='B') && (node_type!='a') && (node_type!='b'));
    if((node_type=='A') || (node_type=='a'))
    {
        RX_identifier = 0x123;
        TX_identifier = 0x321;
        rx_fifo_id[0] = 0x123; // set rx fifo id
    }
    else
    {
        RX_identifier = 0x321;
        TX_identifier = 0x123;
        rx_fifo_id[0] = 0x321; // set rx fifo id
    }

    result = FLEXCAN_DRV_Init(flexcanInstance, &flexcan1_data, true, &can_state);
    if (result)
    {
        printf("\r\nFLEXCAN initilization failed. result: 0x%lx", result);
        return (-1);
    }

    /* Get the CAN clock used to feed the CAN protocol engine */
    if (FLEXCAN_HAL_GetClock(g_flexcanBaseAddr[flexcanInstance]))
    {
        canPeClk = CLOCK_SYS_GetFlexcanFreq(0, kClockFlexcanSrcBusClk);
    }
    else
    {
        canPeClk = CLOCK_SYS_GetFlexcanFreq(0, kClockFlexcanSrcOsc0erClk);
    }

    /* Decide which table to use */
    switch (canPeClk)
    {
        case 60000000:
            result = FLEXCAN_DRV_SetBitrate(flexcanInstance, &bit_rate_table_60Mhz[4]); /* 1   MHz */
            break;
        case 48000000:
            result = FLEXCAN_DRV_SetBitrate(flexcanInstance, &bit_rate_table_48Mhz[4]); /* 1   MHz */
            break;
        default:
            printf("\r\nFLEXCAN bitrate table not available for PE clock: %lu", canPeClk);
            return kStatus_FLEXCAN_Fail;
    }
    if (result)
    {
        printf("\r\nFLEXCAN set bitrate failed. result: 0x%lx", result);
        return (-1);
    }

    FLEXCAN_DRV_SetMaskType(flexcanInstance, kFlexCanRxMask_Global);

    if (flexcan1_data.is_rx_fifo_needed)
    {
        result = FLEXCAN_DRV_SetRxFifoGlobalMask(flexcanInstance, kFlexCanMbId_Std, 0x7FF);
        if (result)
        {
            printf("\r\nFLEXCAN set rx fifo global mask failed. result: 0x%lx", result);
            return (-1);
        }
    }

    // rx_mb
    result = FLEXCAN_DRV_SetRxMbGlobalMask(flexcanInstance, kFlexCanMbId_Std, 0x123);
    if (result)
    {
        printf("\r\nFLEXCAN set rx MB global mask failed. result: 0x%lx", result);
        return (-1);
    }

    // FlexCAN reveive config
    rx_info.msg_id_type = kFlexCanMbId_Std;
    rx_info.data_length = 1;

    if (flexcan1_data.is_rx_fifo_needed)
    {
        id_table.is_extended_mb = false;
        id_table.is_remote_mb = false;
        rx_fifo_id[0] = RX_identifier;
        rx_fifo_id[1] = 0x667;
        rx_fifo_id[2] = 0x676;
        rx_fifo_id[3] = 0x66E;
        rx_fifo_id[4] = 0x66F;
        for (i = 5; i < 8; i++)
        {
            rx_fifo_id[i] = 0x6E6;
        }
        id_table.id_filter = rx_fifo_id;
        // Configure RX FIFO fields
        result = FLEXCAN_DRV_ConfigRxFifo(flexcanInstance, kFlexCanRxFifoIdElementFormat_A, &id_table);
        if (result)
        {
            printf("\r\nFlexCAN RX FIFO configuration failed. result: 0x%lx", result);
            return (-1);
        }
    }

    // Configure RX MB fields
    result = FLEXCAN_DRV_ConfigRxMb(flexcanInstance, RX_mailbox_num, &rx_info, RX_identifier);
    if (result)
    {
        printf("\r\nFlexCAN RX MB configuration failed. result: 0x%lx", result);
        return (-1);
    }

    // FlexCAN transfer config
    tx_info.msg_id_type = kFlexCanMbId_Std;
    tx_info.data_length = 8;

    result = FLEXCAN_DRV_ConfigTxMb(flexcanInstance, TX_mailbox_num, &tx_info, TX_identifier);
    if (result)
    {
        printf("\nTransmit MB config error. Error Code: 0x%lx", result);
    }

    ch = ch0;
    UART_DRV_ReceiveData(uartInstance,ch,8);
    while(1)
    {
        if(UART_HAL_GetStatusFlag(uartBaseAddr, kUartRxActiveEdgeDetect)) //check if start bit detected
        {
            UART_HAL_ClearStatusFlag(uartBaseAddr, kUartRxActiveEdgeDetect);
            PIT_DRV_StopTimer(pitInstance,0);
            PIT_DRV_StartTimer(pitInstance,0);

            if(!(timeout_flag)) // if timeout event is disabled
            {
                timeout_flag = 0xff; // enable timeout event
            }
        }

        status = UART_DRV_GetReceiveStatus(uartInstance,&remainingReceiveByteCount); //get receive status

        receivedByteCount =  8 - remainingReceiveByteCount;

        // if receiving complete or still in receiving but timeout event happens
        if((status == kStatus_UART_Success) || ((status == kStatus_UART_RxBusy) && (timeout_flag == true)))
        {
            if(receivedByteCount) // if has received something
            {
                if(timeout_flag == true)
                {
                    UART_DRV_AbortReceivingData(uartInstance); // cancel the receiving

                    //Re-calculate receivedByteCount
                    status = UART_DRV_GetReceiveStatus(uartInstance,&remainingReceiveByteCount);
                    receivedByteCount =  8 - remainingReceiveByteCount;

                    timeout_flag = 0xff; // re-enable timeout event
                }

                ch = (ch == ch0) ? ch1 : ch0; // switch receiving buffer
                UART_DRV_ReceiveData(uartInstance, ch, 8);// restart the receiving process

                tx_info.data_length = receivedByteCount; // number of bytes to be sent
                result = FLEXCAN_DRV_Send(flexcanInstance, TX_mailbox_num, &tx_info, TX_identifier,
                                          (ch == ch1) ? ch0 : ch1, 1000);

                if (result)
                {
                    printf("\r\nTransmit send configuration failed. result: 0x%lx", result);
                    return (-1);
                }
            }
        }

        if (flexcan1_data.is_rx_fifo_needed)
        {
            result = FLEXCAN_DRV_RxFifo(flexcanInstance, &rx_fifo, 0);

            if (result == kStatus_FLEXCAN_TimeOut)
            {
                continue;
            }
            if (result != kStatus_FLEXCAN_Success)
            {
                printf("\r\nFlexCAN RX receive failed. result: 0x%lx", result);
                return (-1);
            }

            temp = ((rx_fifo.cs) >> 16) & 0xF;
            for(j = 0; j < temp; j++)
            {
                ch2[j] = rx_fifo.data[j];
            }

            // Wait until the previous uart transaction is finished
            while (UART_DRV_GetTransmitStatus(uartInstance, NULL) == kStatus_UART_TxBusy);

            UART_DRV_SendData(uartInstance,ch2,temp);
        }
        else
        {
            result = FLEXCAN_DRV_RxMessageBuffer(flexcanInstance, RX_mailbox_num,
                                                 &rx_mb, 0);

            if (result == kStatus_FLEXCAN_TimeOut)
            {
                continue;
            }
            if (result != kStatus_FLEXCAN_Success)
            {
                printf("\r\nFlexCAN receive failed. result: 0x%lx", result);
                return (-1);
            }

            temp = ((rx_mb.cs) >> 16) & 0xF;
            for(j = 0; j < temp; j++)
            {
                ch2[j] = rx_mb.data[j];
            }

            // Wait until the previous uart transaction is finished
            while (UART_DRV_GetTransmitStatus(uartInstance, NULL) == kStatus_UART_TxBusy);

            UART_DRV_SendData(uartInstance,ch2,temp);
        }
    }
}

static void pit0_isr_callback(uint32_t channel)
{
    // Set timeout_flag
    if(timeout_flag==0xff)
    {
        timeout_flag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
