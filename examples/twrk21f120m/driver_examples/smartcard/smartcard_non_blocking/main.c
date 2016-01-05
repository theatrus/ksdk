/*
* Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "board.h"
#include "fsl_debug_console.h"

#if USING_EDMA
#include "fsl_edma_driver.h"
#elif USING_DMA
#include "fsl_dma_driver.h"
#endif

#if USING_NCN8025_INTERFACE
#include "fsl_smartcard_ncn8025_driver.h"
#elif USING_DIRECT_INTERFACE
#include "fsl_smartcard_direct_driver.h"
#endif

#include "fsl_smartcard_driver.h"


/*******************************************************************************
* Definitions
******************************************************************************/
#define MAX_TRANSFER_SIZE       (258)
/*******************************************************************************
* Private Functions
******************************************************************************/
static int16_t receive_atr(uint32_t instance, smartcard_state_t *smartcardState, uint8_t* buf, uint16_t length);
static void send_pps(uint32_t instance, uint8_t *ppsBuff, uint8_t ppsLenght);
static void wait_for_card_presense(void);
void smartcard_interface_callback_function(uint32_t instance, void * smartcardState);
/*******************************************************************************
* Prototypes
******************************************************************************/
smartcard_state_t g_smartcardState;
uint32_t g_smartcardInstance;
uint8_t txBuff[MAX_TRANSFER_SIZE];
uint8_t rxBuff[MAX_TRANSFER_SIZE];
semaphore_t cardPresenceIrqSync;

/*******************************************************************************
* Code
******************************************************************************/
/*
 * This example will example the efficiency of the transmit/receive drivers with
 * using non-blocking/async methods. Transfer data between board and smartcard
 * (Zeitcontrol ZC7.5 RevD). The example will do transfers both in T=0 and T=1
 * modes.
 */


/*!
 * @brief This function initialize smartcard_user_config_t structure. Enables configuration
 * of most common settings of the UART peripheral, smartcard clock, smartcard interface configuration,
 * interface instance number, slot number, operating voltage ...
 */
void smartcard_init_user_config(smartcard_user_config_t *smartcardConfig)
{
    smartcardConfig->interfaceConfig.sCClock = HW_SMARTCARD_CLOCK_VALUE;
    smartcardConfig->interfaceConfig.interfaceInstance = 0;
    smartcardConfig->interfaceConfig.cardSoltNo = 0;
    smartcardConfig->vcc = kSmartcardVoltageClassB3_3V;
    smartcardConfig->xferMode = kSmartcardCPUXferMode;
    smartcardConfig->interfaceConfig.clockModuleInstance = HW_SMARTCARD_CLOCK_MODULE_INSTANCE;
    smartcardConfig->interfaceConfig.clockModuleChannel = HW_SMARTCARD_CLOCK_MODULE_CHANNEL;
    smartcardConfig->interfaceConfig.controlPort = HW_SMARTCARD_CONTROL_PORT_INSTANCE;
    smartcardConfig->interfaceConfig.controlPin = HW_SMARTCARD_CONTROL_PIN;
    smartcardConfig->interfaceConfig.irqPort = HW_SMARTCARD_IRQ_PORT_INSTANCE;
    smartcardConfig->interfaceConfig.irqPin = HW_SMARTCARD_IRQ_PIN;
    smartcardConfig->interfaceConfig.resetPort = HW_SMARTCARD_RST_PORT_INSTANCE;
    smartcardConfig->interfaceConfig.resetPin = HW_SMARTCARD_RST_PIN;
    smartcardConfig->interfaceConfig.vsel0Port = HW_SMARTCARD_VSEL0_PORT_INSTANCE;
    smartcardConfig->interfaceConfig.vsel0Pin = HW_SMARTCARD_VSEL0_PIN;
    smartcardConfig->interfaceConfig.vsel1Port = HW_SMARTCARD_VSEL1_PORT_INSTANCE;
    smartcardConfig->interfaceConfig.vsel1Pin = HW_SMARTCARD_VSEL1_PIN;

    /* Set-up the numerical id for TS detection timer */
    smartcardConfig->extTimerConfig.tsTimerId = 0;
}


/*!
 * @brief This function waits for card insertion/detection
 */
void wait_for_card_presense(void)
{
    osa_status_t syncStatus;
    smartcard_interface_control_t interfaceControl = kSmartcardInterfaceReadStatus;
    smartcard_interface_slot_t slotParams;


    /* Putting delay as few boards has de-bouncing cap in card slot presence detect pin */
    OSA_TimeDelay(1000);
    slotParams.present = 0;

    /* Read card presence status */
    SMARTCARD_DRV_InterfaceControl(g_smartcardInstance, &interfaceControl, &slotParams);

    /* Check if a card is already inserted */
    if(!slotParams.present)
    {
        PRINTF("Please insert a smart card to test\r\n");
    }

    while(true)
    {
        /* Read card presence status */
        SMARTCARD_DRV_InterfaceControl(g_smartcardInstance, &interfaceControl, &slotParams);

        /* Return if card is present/inserted */
        if(slotParams.present)
        {
            break;
        }

        /* Wait until a interface/PHY interrupt event occurs */
        do
        {
            syncStatus = OSA_SemaWait(&cardPresenceIrqSync, OSA_WAIT_FOREVER);
        }while(syncStatus == kStatus_OSA_Idle);
    }
}

/*!
 * @brief Callback function for the Smartcard interface/PHY IC ISR
 */
void smartcard_interface_callback_function(uint32_t instance, void * smartcardState)
{
    /* Post sync object */
    OSA_SemaPost(&cardPresenceIrqSync);
}

/*
* This function receives the ATR bytes.
*/
static int16_t receive_atr(uint32_t instance, smartcard_state_t *smartcardState, uint8_t* buf, uint16_t length)
{
    uint32_t bytesRemaining = 0;

    /* Set the correct state */
    smartcardState->transState = kSmartcardReceivingState;

    /* Receive ATR bytes using non-blocking API */
    SMARTCARD_DRV_ReceiveData(instance, buf, length);

    /* Wait or do something */
    while(SMARTCARD_DRV_GetReceiveStatus(instance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    return (length - bytesRemaining);
}

/*
* This function sends a basic PPS to the card
*/
static void send_pps(uint32_t instance, uint8_t *ppsBuff, uint8_t ppsLenght)
{
    uint32_t bytesRemaining = 0;

    /* Initialize timer states in driver */
    g_smartcardState.timersState.wwtExpired = false;
    g_smartcardState.timersState.cwtExpired = false;
    g_smartcardState.timersState.bwtExpired = false;

    /* Reset WWT timer */
    SMARTCARD_DRV_Control(instance, kSmartcardResetWWT, 0);

    /* Enable WWT timer before starting T=0 transport */
    SMARTCARD_DRV_Control(instance, kSmartcardEnableWWT, 0);

    /* Send PPS bytes to card using non-blocking API */
    SMARTCARD_DRV_SendData(instance, ppsBuff, ppsLenght);

    /* Wait or do something */
    while(SMARTCARD_DRV_GetTransmitStatus(instance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Receive PPS bytes from card using non-blocking API */
    SMARTCARD_DRV_ReceiveData(instance, rxBuff, ppsLenght);

    /* Wait or do something */
    while(SMARTCARD_DRV_GetReceiveStatus(instance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Disable WWT timer */
    SMARTCARD_DRV_Control(instance, kSmartcardDisableWWT, 0);
}

/*!
 * @brief Check send/receive blocking functionality
 *
 */
int main(void)
{
    uint8_t atrBuff[100];
    uint16_t i,j;
    uint32_t rcvLength = 0;
    uint8_t length;
    uint8_t T1_LRC=0;
    uint8_t increment = 1;
    uint32_t bytesRemaining = 0;
    smartcard_user_config_t smartcardConfig;

#if USING_EDMA
    edma_state_t state;
    edma_user_config_t edmaUserConfig;
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
#elif USING_DMA
    dma_state_t state;
#endif
    osa_status_t syncStatus;

    /* Initialize board hardware */
    hardware_init();
    OSA_Init();

    g_smartcardInstance = BOARD_SMARTCARD_MODULE_INSTANCE;

    /* Configure smart-card interface pins as per board requirements */
    configure_smartcard_pins(g_smartcardInstance);
    /* Fill in smart-card driver user configuration data */
    smartcard_init_user_config(&smartcardConfig);

    PRINTF("\r\n***** SMARTCARD Driver Send Receive non-blocking functionality example *****\r\n\r\n");

#if USING_EDMA
    EDMA_DRV_Init(&state, &edmaUserConfig);
#elif USING_DMA
    DMA_DRV_Init(&state);
#endif

    /* Initialize the smart-card module with base address and config structure*/
    SMARTCARD_DRV_Init(g_smartcardInstance, &g_smartcardState, &smartcardConfig);
    /* Install test/application callback function */
    SMARTCARD_DRV_InstallInterfaceCallback(g_smartcardInstance, smartcard_interface_callback_function, NULL);

    /* Wait for a card inserted */
    wait_for_card_presense();
    PRINTF("Card inserted.\r\n");

    /* Deactivate the card */
    PRINTF("Deactivating card...");
    SMARTCARD_DRV_Deactivate(g_smartcardInstance);
    PRINTF("Done!\r\n");

    /******************* Setup for ATR mode(T=0) of transfer *******************/
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardSetupATRMode, 0);

    /* Invalidate ATR buffer first */
    memset(atrBuff, 0, sizeof(atrBuff));

    /* Deactivate the card */
    PRINTF("Resetting/Activating card...");

    /* Enable initial character detection */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableInitDetect, 0);

    /* Reset and activate the card */
    SMARTCARD_DRV_Reset(g_smartcardInstance, kSmartcardColdReset);

    g_smartcardState.timersState.adtExpired = false;
    g_smartcardState.timersState.wwtExpired = false;

    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableADT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableWWT, 0);

    g_smartcardState.transState = kSmartcardWaitingForTSState;

    /* Wait for Initial character. */
    while(g_smartcardState.transState!= kSmartcardReceivingState)
    {
        g_smartcardState.isRxBusy = true;
        g_smartcardState.isRxBlocking = true;
        /* Wait until all the data is received or for timeout.*/
        do
        {
            syncStatus = OSA_SemaWait(&g_smartcardState.rxIrqSync, OSA_WAIT_FOREVER);
        }while(syncStatus == kStatus_OSA_Idle);
    }

    /* Start WWT,CWT and ADT timer */
    g_smartcardState.timersState.wwtExpired = false;
    g_smartcardState.timersState.cwtExpired = false;

    /* Enable WWT and ADT timer before starting T=0 transport */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardResetWWT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableWWT, 0);

    /* Receiving cold ATR */
    /* Disable nack while receiving ATR */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableAnack, 0);
    receive_atr(g_smartcardInstance, &g_smartcardState, atrBuff, sizeof(atrBuff));

    /* Enable the nack after receiving ATR */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableAnack, 0);

    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableADT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableWWT, 0);

    /* Initialize with Zeitcontrol ZC7.5 RevD card known ATR parameters */
    g_smartcardState.cardParams.Fi = 372;
    g_smartcardState.cardParams.WI = 0x80;
    g_smartcardState.cardParams.currentD = 1;
    g_smartcardState.cardParams.CWI = 0x05;
    g_smartcardState.cardParams.BWI = 0x07;
    PRINTF("Done!\r\n");

    PRINTF("======================================================\r\n");
    PRINTF("Send receive non-blocking functionality example in T=0 mode started!\r\n");

    /******************* Setup for T=0 transfer *******************/
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardSetupT0Mode, 0);
    /* Send a PPS for T=0 mode */
    /* send a PPS indicating T=1 protocol */
    txBuff[0] = 0xFF;
    txBuff[1] = 0x00;
    txBuff[2] = 0x00;

    for(i=0;i<2;i++)
    {
        txBuff[2]^= txBuff[i];
    }
    send_pps(g_smartcardInstance, txBuff, 3);

    /******************* Prepare and transfer a T-APDU *******************/
    /* APDU */
    txBuff[0]=0xC0;     /* CLA command */
    txBuff[1]=0x02;     /* Instruction code (0x14 for EEPROM SIZE) */
    txBuff[2]=0x00;     /* P1 */
    txBuff[3]=0x00;     /* P2 */
    txBuff[4]=0x06;     /* P3 = Le */

    /* Start WWT timer */
    g_smartcardState.timersState.wwtExpired = false;
    g_smartcardState.timersState.cwtExpired = false;
    g_smartcardState.timersState.bwtExpired = false;

    /* Reset WWT timer */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardResetWWT, 0);

    /* Enable WWT timer before starting T=0 transport */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableWWT, 0);

    /* Send command using non-blocking API */
    SMARTCARD_DRV_SendData(g_smartcardInstance, txBuff, 5);

    /* Wait or do something */
    while(SMARTCARD_DRV_GetTransmitStatus(g_smartcardInstance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Wait for INS byte using non-blocking API */
    SMARTCARD_DRV_ReceiveData(g_smartcardInstance, rxBuff, 1);
    while(SMARTCARD_DRV_GetReceiveStatus(g_smartcardInstance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /*
     * Now receive the remaining bytes using non-blocking API.
     * 6 bytes for Start address and size of EEPROM for Zeitcontrol ZC7.5 RevD
     * 2 bytes for status words.
     */
    SMARTCARD_DRV_ReceiveData(g_smartcardInstance, (rxBuff + 1), (6 + 2));
    while(SMARTCARD_DRV_GetReceiveStatus(g_smartcardInstance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Disable WWT timer */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableWWT, 0);

    PRINTF("\r\nSmartcard EEPROM Start Address = 0x%02X%02X%02X\r\n", rxBuff[1], rxBuff[2], rxBuff[3]);
    if((rxBuff[4] != 0x00) || (rxBuff[5] != 0x80) || (rxBuff[6] != 0x00))
    {
        PRINTF("Incorrect smartcard EEPROM Size!\r\n");
    }
    else
    {
        PRINTF("Smartcard EEPROM Size = 32 KBytes\r\n");
    }

    /* Deactivate the card */
    PRINTF("\r\nDeactivating card...");
    SMARTCARD_DRV_Deactivate(g_smartcardInstance);
    PRINTF("Done!\r\n");

    /******************* Setup for T=0 transfer *******************/
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardSetupATRMode, 0);

    /* Invalidate ATR buffer first */
    memset(atrBuff, 0, sizeof(atrBuff));

    /* Deactivate the card */
    PRINTF("Resetting/Activating card...");

    /* Enable initial character detection */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableInitDetect, 0);

    /* Reset and activate the card */
    SMARTCARD_DRV_Reset(g_smartcardInstance, kSmartcardColdReset);

    g_smartcardState.timersState.adtExpired = false;
    g_smartcardState.timersState.wwtExpired = false;

    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableADT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableWWT, 0);

    g_smartcardState.transState = kSmartcardWaitingForTSState;

    /* Wait for Initial character. */
    while(g_smartcardState.transState!= kSmartcardReceivingState)
    {
        g_smartcardState.isRxBusy = true;
        g_smartcardState.isRxBlocking = true;
        /* Wait until all the data is received or for timeout.*/
        do
        {
            syncStatus = OSA_SemaWait(&g_smartcardState.rxIrqSync, 0);
        }while(syncStatus == kStatus_OSA_Idle);
    }

    /* Start WWT,CWT and ADT  timer */
    g_smartcardState.timersState.wwtExpired = false;
    g_smartcardState.timersState.cwtExpired = false;

    /* Enable WWT and ADT timer before starting T=0 transport */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardResetWWT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableWWT, 0);

    /* Receiving cold ATR */
    /* Disable nack while receiving ATR */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableAnack, 0);
    receive_atr(g_smartcardInstance, &g_smartcardState, atrBuff, sizeof(atrBuff));

    /* Enable the nack after receiving ATR */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableAnack, 0);

    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableADT, 0);
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableWWT, 0);

    /* Initialize with Zeitcontrol ZC7.5 RevD card known ATR parameters */
    g_smartcardState.cardParams.Fi = 372;
    g_smartcardState.cardParams.WI = 0x80;
    g_smartcardState.cardParams.currentD = 1;
    g_smartcardState.cardParams.CWI = 0x05;
    g_smartcardState.cardParams.BWI = 0x07;
    PRINTF("Done!\r\n");

    PRINTF("\r\nSend receive non-blocking functionality example in T=1 mode started!\r\n");
    /******************* Setup for T=0 transfer *******************/
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardSetupT0Mode, 0);
    /* Send a PPS indicating T=1 protocol */
    txBuff[0] = 0xFF;
    txBuff[1] = 0x01;
    txBuff[2] = 0x00;

    for(i=0;i<2;i++)
    {
        txBuff[2]^= txBuff[i];
    }
    send_pps(g_smartcardInstance, txBuff, 3);

    /* Set ECHO command data length */
    length = 16;                /* payload */

    /* Prolog */
    txBuff[0] = 0x00;           /* NAD */
    txBuff[1] = 0;              /* I-block, PCB */
    txBuff[2] = length + 6;     /* LEN */

    /* APDU */
    txBuff[3]=0xC0;             /* CLA command */
    txBuff[4]=0x14;             /* Instruction code (0x14 for ECHO) */
    txBuff[5]=increment;        /* increment value - P1 */
    txBuff[6]=0;                /* P2 */
    txBuff[7]=length;           /* P3 = Lc */

    /* Fill the data bytes */
    for(i=0;i<length;i++)
    {
        txBuff[i+8]=i;
    }

    /* Last byte is the response data length */
    txBuff[length+8]= length;

    /* Calculate LRC */
    for(j=0;j<=length+8;j++)
    {
        T1_LRC ^= txBuff[j];
    }
    txBuff[length+9]=T1_LRC;

    /******************* Setup for T=1 transfer *******************/
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardSetupT1Mode, 0);

    /* Start CWT timer */
    g_smartcardState.timersState.wwtExpired = false;
    g_smartcardState.timersState.cwtExpired = false;
    g_smartcardState.timersState.bwtExpired = false;

    /* Enable BWT timer */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardEnableBWT, 0);

    /* Send T-APDU command using non-blocking API */
    SMARTCARD_DRV_SendData(g_smartcardInstance, txBuff, length+10);
    while(SMARTCARD_DRV_GetTransmitStatus(g_smartcardInstance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Receive T-APDU response using non-blocking API */
    SMARTCARD_DRV_ReceiveData(g_smartcardInstance, rxBuff, length+6);
    while(SMARTCARD_DRV_GetReceiveStatus(g_smartcardInstance, &bytesRemaining) != kStatus_SMARTCARD_Success) {}

    /* Disable CWT timer */
    SMARTCARD_DRV_Control(g_smartcardInstance, kSmartcardDisableCWT, 0);

    /* Payload length + NAD + PCB + LEN + SW1 + SW2 + LRC, it should be length + 6 */
    rcvLength = (length+6) - bytesRemaining;
    if(rcvLength!=(length+6))
    {
        PRINTF("Received length did not match!\r\n");
    }

    T1_LRC = 0;
    for(i=0;i<rcvLength-1;i++)
    {
        T1_LRC^= rxBuff[i];
    }
    PRINTF("\r\n");

    /* Check if received LRC is correct */
    if(T1_LRC == rxBuff[i])
    {
        PRINTF("LRC received correctly\r\n");
    }
    else
    {
        PRINTF("LRC Error\r\n");
    }

    /* Check if receive data is correct */
    for(i=0;i<length;i++)
    {
        if((txBuff[i+8]+increment)!= rxBuff[i+3])
        {
            PRINTF("Incorrect Echo data.\r\n");
            break;
        }
    }

    if(i == length)
    {
        PRINTF("Echo data received correctly!\r\n");
    }

    /* Check if Command response status bytes are OK */
    if((rxBuff[i+3]==0x90)&&(rxBuff[i+4]==0x0))
    {
        PRINTF("Command status OK\r\n");
    }
    else
    {
        PRINTF("Command status not OK\r\n");
    }

    PRINTF("\r\nSend receive non-blocking functionality example finished!\r\n");
    PRINTF("======================================================\r\n");

    return 0;
}

/*******************************************************************************
* EOF
******************************************************************************/

