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

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
// SDK Included Files
#include "board.h"
#include "fsl_tpm_driver.h"
#include "fsl_debug_console.h"
#include "accel.h"


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Main demo function.
 */
int main (void)
{

    tpm_general_config_t driverInfo;
    accel_dev_t accDev;
    accel_dev_interface_t accDevice;
    accel_sensor_data_t accelData;
    accel_i2c_interface_t i2cInterface;
    tpm_pwm_param_t yAxisParams;
    tpm_pwm_param_t xAxisParams;
    int16_t xData, yData;
    int16_t xAngle, yAngle;

    xAxisParams.mode              = kTpmEdgeAlignedPWM;
    xAxisParams.edgeMode          = kTpmHighTrue;
    xAxisParams.uFrequencyHZ      = 100000u;
    xAxisParams.uDutyCyclePercent = 0u;

    yAxisParams.mode              = kTpmEdgeAlignedPWM;
    yAxisParams.edgeMode          = kTpmHighTrue;
    yAxisParams.uFrequencyHZ      = 100000u;
    yAxisParams.uDutyCyclePercent = 0u;

    // Register callback func for I2C
    i2cInterface.i2c_init       =  I2C_DRV_MasterInit;
    i2cInterface.i2c_read       =  I2C_DRV_MasterReceiveDataBlocking;
    i2cInterface.i2c_write      =  I2C_DRV_MasterSendDataBlocking;

    accDev.i2c      = &i2cInterface;
    accDev.accel    = &accDevice;

    accDev.slave.baudRate_kbps  = BOARD_ACCEL_BAUDRATE;
    accDev.slave.address        = BOARD_ACCEL_ADDR;
    accDev.bus                  = BOARD_ACCEL_I2C_INSTANCE;

    // Initialize standard SDK demo application pins.
    hardware_init();

    // Accel device driver utilizes the OSA, so initialize it.
    OSA_Init();

    // Print the initial banner.
    PRINTF("Bubble Level Demo!\r\n\r\n");

    // Initialize the Accel.
    accel_init(&accDev);

    // Prepare memory for initialization.
    memset(&driverInfo, 0, sizeof(driverInfo));

    // Init TPM.
    TPM_DRV_Init(BOARD_BUBBLE_TPM_INSTANCE, &driverInfo);

    // Set clock for TPM.
    TPM_DRV_SetClock(BOARD_BUBBLE_TPM_INSTANCE, kTpmClockSourceModuleClk, kTpmDividedBy2);

    // Main loop.  Get sensor data and update duty cycle for the TPM timer.
    while(1)
    {
        // Wait 5 ms in between samples (accelerometer updates at 200Hz).
        OSA_TimeDelay(5);

        // Get new accelerometer data.
          accDev.accel->accel_read_sensor_data(&accDev,&accelData);

        // Init PWM module with updated configuration.
        TPM_DRV_PwmStart(BOARD_BUBBLE_TPM_INSTANCE, &xAxisParams, BOARD_TPM_X_CHANNEL);
        TPM_DRV_PwmStart(BOARD_BUBBLE_TPM_INSTANCE, &yAxisParams, BOARD_TPM_Y_CHANNEL);

        // Get the X and Y data from the sensor data structure.fxos_data
        xData = (int16_t)((accelData.data.accelXMSB << 8) | accelData.data.accelXLSB);
        yData = (int16_t)((accelData.data.accelYMSB << 8) | accelData.data.accelYLSB);

        // Convert raw data to angle (normalize to 0-90 degrees).  No negative angles.
        xAngle = abs((int16_t)(xData * 0.011));
        yAngle = abs((int16_t)(yData * 0.011));

        // Update angles to turn on LEDs when angles ~ 90
        if(xAngle > 85) xAngle = 100;
        if(yAngle > 85) yAngle = 100;
        // Update angles to turn off LEDs when angles ~ 0
        if(xAngle < 5) xAngle = 0;
        if(yAngle < 5) yAngle = 0;

        // Update pwm duty cycle
        xAxisParams.uDutyCyclePercent = 100 - xAngle ;
        yAxisParams.uDutyCyclePercent = 100 - yAngle ;

        // Print out the raw accelerometer data.
        PRINTF("x= %d y = %d\r\n", xData, yData);
    }
}
