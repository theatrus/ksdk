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
 * ANY THEORY OF LIABILITY, WHETHER IN -        CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_flexio_i2c_master_driver.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_i2c_shared_function.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////


#if defined FRDM_K82F
    #define I2C_SLAVE (3U) /*!< Instance number for I2C3. */
#else
    #define I2C_SLAVE (1U) /*!< Instance number for I2C1. */
#endif

#define I2C_SLAVE_ADDR              0x7FU
#define I2C_RESET_BUFF(x,size) \
   do \
   { \
        int32_t i; \
        for(i = 0; i < size; i++) \
        { \
            x[i] = 0x00; \
        } \
   } while (0)

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
static uint32_t s_flexioInstance;
flexio_i2c_state_t masterState;
i2c_slave_state_t slaveState;
static uint8_t s_masterReadBuffer[13] = {0};
static uint8_t s_slaveReadBuffer[13] = {0};
static uint8_t s_masterWriteBuffer[13] =
{
    "flexio i2c"
};
static uint8_t s_slaveWriteBuffer[13] =
{
    "standard i2c"
};

static i2c_slave_user_config_t slaveConfig =
{
    .address        = I2C_SLAVE_ADDR,
    .slaveListening = false,
    .slaveCallback  = NULL,
    .callbackParam  = NULL,
#if FSL_FEATURE_I2C_HAS_START_STOP_DETECT
    .startStopDetect  = false,
#endif
#if FSL_FEATURE_I2C_HAS_STOP_DETECT
    .stopDetect       = false,
#endif
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static int32_t i2c_compare_sink_source(uint8_t *source, uint8_t *sink, uint16_t count)
{
    uint16_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        if (source[i] != sink[i])
        {
            return false;
        }
    }
    return true;
}

static void master_send_slave_receive_int(void)
{
    FLEXIO_I2C_DRV_MasterSendData(&masterState,I2C_SLAVE_ADDR,NULL,s_masterWriteBuffer,13);
    I2C_DRV_SlaveReceiveDataBlocking(I2C_SLAVE,s_slaveReadBuffer,13, 1000);
    if(i2c_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 13) != true)
    {
       PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave write transfer failed!!\r\n");
       return;
    } 
    PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave write transfer succeed!!\r\n");
}
static void master_receive_slave_send_int(void)
{
    FLEXIO_I2C_DRV_MasterReceiveData(&masterState,I2C_SLAVE_ADDR,NULL,s_masterReadBuffer,13);
    I2C_DRV_SlaveSendDataBlocking(I2C_SLAVE,s_slaveWriteBuffer,13, 1000);
    if(i2c_compare_sink_source(s_masterReadBuffer, s_slaveWriteBuffer, 13) != true)
    {
       PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave read transfer failed!!\r\n");
       return;
    } 
    PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave read transfer succeed!!\r\n");
}
static void master_send_slave_receive_int_blocking(void)
{
    I2C_RESET_BUFF(s_slaveReadBuffer,13);
    I2C_DRV_SlaveReceiveData(I2C_SLAVE,s_slaveReadBuffer,13);
    FLEXIO_I2C_DRV_MasterSendDataBlocking(&masterState,I2C_SLAVE_ADDR,NULL,s_masterWriteBuffer,13, 1000);
    if(i2c_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 13) != true)
    {
       PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave write transfer failed!!\r\n");
       return;
    } 
    PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave write transfer succeed!!\r\n");
}
static void master_receive_slave_send_int_blocking(void)
{
    I2C_RESET_BUFF(s_masterReadBuffer,13);
    I2C_DRV_SlaveSendData(I2C_SLAVE,s_slaveWriteBuffer,13);
    FLEXIO_I2C_DRV_MasterReceiveDataBlocking(&masterState,I2C_SLAVE_ADDR,NULL,s_masterReadBuffer,13,1000);
    if(i2c_compare_sink_source(s_masterReadBuffer, s_slaveWriteBuffer, 13) != true)
    {
       PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave read transfer failed!!\r\n");
       return;
    } 
    PRINTF("\r\nFlexIO simulated I2C master to I2C standard slave read transfer succeed!!\r\n");
}

int main(void)
{
    s_flexioInstance = 0;
    /* Fill in FlexIO config data */    
    flexio_user_config_t userConfig = 
    {
        .useInt = true,
        .onDozeEnable = false,
        .onDebugEnable = true,
        .fastAccessEnable = false
    };
    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Call OSA_Init to setup LP Timer for timeout
    OSA_Init();
    FLEXIO_DRV_Init(s_flexioInstance,&userConfig);	
        
    /* Fill in i2c master config data */
    flexio_i2c_userconfig_t masterConfig;
    masterConfig.baudRate = 200000;
    masterConfig.i2cHwConfig.sdaPinIdx = 5;
    masterConfig.i2cHwConfig.sclkPinIdx = 4;
    masterConfig.i2cHwConfig.shifterIdx[0] = 0;
    masterConfig.i2cHwConfig.shifterIdx[1] = 1;
    masterConfig.i2cHwConfig.timerIdx[0] = 0;
    masterConfig.i2cHwConfig.timerIdx[1] = 1;
    
    // Init FlexIO simulated I2C master
    FLEXIO_I2C_DRV_MasterInit(s_flexioInstance,&masterState,&masterConfig);
    FLEXIO_DRV_Start(s_flexioInstance);
    /* Init standard I2C slave */
    I2C_DRV_SlaveInit(I2C_SLAVE, &slaveConfig, &slaveState);
	
    // Inform to start example
    PRINTF("\r\n++++++++++++++++ FLEXIO I2C Master Send/Receive Example Start +++++++++++++++++\r\n");
    PRINTF("\r\n-------------------Non-blocking&Blocking example--------------------------\
            \r\n1. FlexIO simulated I2C master write a buffer to I2C1 \
            \r\n2. I2C1 acts as slave and receives data from FlexIO simulated I2C Master.\
            \r\n3. Compare rxBuff and txBuff to see result.\
            \r\n4. FlexIO simulated I2C master read a buffer from I2C1\
            \r\n5. I2C1 send the buffer and FlexIO simulated I2C master receives the buffer.\
            \r\n6. Compare rxBuff and txBuff to see result.\r\n");
    PRINTF("\r\n============================================================\r\n");
    PRINTF("\r\nPress any key to start transfer:\r\n\r\n");
    while (1)
    {
        GETCHAR();
        /*non-blocking example*/
        PRINTF("\r\n----------Non-blocking example----------\r\n");
        /*For high baudrate, when flexio simulated I2C write to standard I2C, need to give standard 
        I2C higher priority to insure standard I2C could handle receive interrupt on time*/
        NVIC_SetPriority(g_flexioIrqId[s_flexioInstance],1);
        NVIC_SetPriority(g_i2cIrqId[I2C_SLAVE],0);  
        master_send_slave_receive_int();
        /*For high baudrate, when flexio simulated I2C read from standard I2C, need to give flexio
        simulated I2C higher priority to insure flexio simulated I2C could handle receive interrupt on time*/
        NVIC_SetPriority(g_flexioIrqId[s_flexioInstance],0);
        NVIC_SetPriority(g_i2cIrqId[I2C_SLAVE],1); 
        master_receive_slave_send_int();
        /*blocking example*/
        PRINTF("\r\n------------Blocking example------------\r\n");
        NVIC_SetPriority(g_flexioIrqId[s_flexioInstance],1);
        NVIC_SetPriority(g_i2cIrqId[I2C_SLAVE],0);  
        master_send_slave_receive_int_blocking();
        NVIC_SetPriority(g_flexioIrqId[s_flexioInstance],0);
        NVIC_SetPriority(g_i2cIrqId[I2C_SLAVE],1);        
        master_receive_slave_send_int_blocking();
    }
}
