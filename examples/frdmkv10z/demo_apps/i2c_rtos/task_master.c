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

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// SDK Included Files
#include "fsl_i2c_master_driver.h"
#include "i2c_rtos.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define I2C_RTOS_LED_R   'R'
#define I2C_RTOS_LED_G   'G'
#define I2C_RTOS_LED_B   'B'

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern uint32_t gSlaveId;
// i2c master driver control block
static i2c_master_state_t master;
// i2c slave info
static i2c_device_t slave =
{
    .address = I2C_RTOS_SLAVE_ADDRESS,
    .baudRate_kbps = 50
};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

static int cmd_read_temperature(void)
{
    i2c_status_t i2c_ret;

    // Buffers for send and recv data to/from I2C bus
    uint32_t bufRecv[1] = {0};
    uint8_t bufSend[2] = {0};

    bufSend[0] = I2C_RTOS_MAGIC;
    bufSend[1] = I2C_RTOS_TEMP_CMD;

    PRINTF("Fetching temperature from client\r\n");
    // Send read temperature request to slave
    i2c_ret = I2C_DRV_MasterSendDataBlocking(I2C_RTOS_MASTER_INSTANCE,
                                             &slave, NULL, 0, bufSend, 2, 20);
    if (i2c_ret != kStatus_I2C_Success)
    {
        PRINTF("Error while sending request to slave:%d\r\n", i2c_ret);
        return 1;
    }

    // wait for slave prepare the data
    OSA_TimeDelay(200);

    // read from slave
    I2C_DRV_MasterReceiveDataBlocking(I2C_RTOS_MASTER_INSTANCE,
                                      &slave, NULL, 0, (uint8_t*)bufRecv, 4, 20);

    // get data, and convert to C degree
    PRINTF("Client Chip Temperature:%hdC\r\n", (int)bufRecv[0]);
    return 0;
}

static int cmd_light_led(int32_t led_type, int32_t on)
{
    i2c_status_t i2c_ret;
    // Buffer for transmitted data
    uint8_t bufSend[4] = {0};

    // Set the send buffer
    bufSend[0] = I2C_RTOS_MAGIC;
    bufSend[1] = I2C_RTOS_LIGHT_CMD;
    bufSend[2] = led_type;
    bufSend[3] = on;

    // Send RGB value to slave
    i2c_ret = I2C_DRV_MasterSendDataBlocking(I2C_RTOS_MASTER_INSTANCE, &slave,
                                  NULL, 0, bufSend, 4, 200);
    // print error message if transfer was not successful
    if (i2c_ret != kStatus_I2C_Success)
    {
        PRINTF("Error while sending RGB values to slave:%d\r\n", i2c_ret);
        return 1;
    }
    return 0;
}

static int cmd_read_id(void)
{
    i2c_status_t i2c_ret;

    // Buffers for send and recv data to/from I2C bus
    uint32_t bufRecv[1] = {0};
    uint8_t bufSend[2] = {0};

    bufSend[0] = I2C_RTOS_MAGIC;
    bufSend[1] = I2C_RTOS_READID_CMD;

    PRINTF("Fetching chip ID from client\r\n");
    // Send read temperature request to slave
    i2c_ret = I2C_DRV_MasterSendDataBlocking(I2C_RTOS_MASTER_INSTANCE, &slave, NULL,
                                  0, bufSend, 2, 200);
    if (i2c_ret != kStatus_I2C_Success)
    {
        PRINTF("Error while sending request to slave:%d\r\n", i2c_ret);
        return 1;
    }

    // wait for slave prepare the data
    OSA_TimeDelay(200);

    // read from slave
    I2C_DRV_MasterReceiveDataBlocking(I2C_RTOS_MASTER_INSTANCE, &slave, NULL, 0,
                        (uint8_t *)bufRecv, 4, 200);

    // get data, and convert to C degree
    PRINTF("Client ID:0x%x\r\n", (unsigned int)bufRecv[0]);
    return 0;
}

static void print_menu(void)
{
#ifdef MRB_KW01
    PRINTF("4 - Read Temperature -- Get temperature of client\r\n");
    PRINTF("5 - Read Id          -- Read client unique id\r\n");
    PRINTF("Please enter your choice (4 - 5): ");
#else  
    PRINTF("1 - LED Red Toggle   -- Red Light toggles on/off\r\n");
    PRINTF("2 - LED Green Toggle -- Green Light toggles on/off\r\n");
#if defined(BOARD_GPIO_LED_BLUE)
    PRINTF("3 - LED Blue Toggle  -- Blue Light toggles on/off\r\n");
#endif
    PRINTF("4 - Read Temperature -- Get temperature of client\r\n");
    PRINTF("5 - Read Id          -- Read client unique id\r\n");
    PRINTF("Please enter your choice (1 - 5): ");
#endif
}

static uint32_t get_user_choice(void)
{
    uint32_t in_val = 0;

    in_val = GETCHAR();
#ifndef FSL_RTOS_MQX
    PUTCHAR(in_val);
#endif
    PUTCHAR('\r');
    PUTCHAR('\n');

    return in_val;
}

void task_master(task_param_t param)

{
    // init i2c master driver
    I2C_DRV_MasterInit(I2C_RTOS_MASTER_INSTANCE, &master);

    // main loop
    while (1)
    {
        static int32_t ledr_sts = 0;
        static int32_t ledg_sts = 0;
#if defined(BOARD_GPIO_LED_BLUE)
        static int32_t ledb_sts = 0;
#endif

        PRINTF("\r\n");
        print_menu();

        switch (get_user_choice())
        {
        case '1':
            // Led Red toggle
            ledr_sts ^= 1;
            cmd_light_led(I2C_RTOS_LED_R, ledr_sts);
            PRINTF("Red LED %s\r\n", ledr_sts ? "on" : "off");
            break;
        case '2':
            // Led Green toggle
            ledg_sts ^= 1;
            cmd_light_led(I2C_RTOS_LED_G, ledg_sts);
            PRINTF("Green LED %s\r\n", ledg_sts ? "on" : "off");
            break;
#if defined(BOARD_GPIO_LED_BLUE)
        case '3':
            // Led Blue toggle
            ledb_sts ^= 1;
            cmd_light_led(I2C_RTOS_LED_B, ledb_sts);
            PRINTF("Blue LED %s\r\n", ledb_sts ? "on" : "off");
            break;
#endif
        case '4':
            // Read temp
            cmd_read_temperature();
            break;
        case '5':
            // Read id
            cmd_read_id();
            break;
        default:
            PRINTF("Invalid choice!\r\n");
        }
        PRINTF("\r\n");
    }
}
