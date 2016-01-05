/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_mpu_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define MPU_INSTANCE        0
#define ARRAY_SIZE          100

#define CPU_REG_NVIC_SHCSR                      (*((uint32_t *)(0xE000ED24)))
#define CPU_REG_NVIC_SHCSR_BUSFAULTENA          0x00020000
#define CPU_REG_SCnSCB_ACTLR                    (*((uint32_t *)(0xE000E008)))
#define CPU_REG_SCnSCB_ACTLR_DISDEFWBUF         0x00000002
// Region actual end/start address is computed by the following equation:
#define ACTUAL_END_ADDR(x)                      ((uint32_t)x | (0x1FU))
#define ACTUAL_START_ADDR(x)                    ((uint32_t)x & (~0x1FU))

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
mpu_user_config_t  userConfig1;
mpu_user_config_t  userConfig2;
mpu_user_config_t  userConfig3;
volatile bool busFaultFlag = false;
volatile uint32_t regionArray[ARRAY_SIZE];
uint32_t cnt = 0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief The function is used to enable bus fault
 */
void enable_busfault(void)
{
    CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_BUSFAULTENA;
}

/*!
 * @brief The function is used to disable write buffer
 */
void disable_writebuffer(void)
{
    CPU_REG_SCnSCB_ACTLR |= CPU_REG_SCnSCB_ACTLR_DISDEFWBUF;
}

/*!
 * @brief BusFault IRQ Handler
 */
#if defined (KM34Z7_SERIES)
void HardFault_Handler(void)
#else
void BusFault_Handler(void)
#endif
{
    mpu_low_masters_access_rights_t accessRights =
    {
       .superAccessRights = kMPUSupervisorEqualToUsermode,
       .userAccessRights  = kMPUUserReadWriteExecute,
    };

    // RegionArray is not updated
    PRINTF("regionArray[%d] = %d \r\n", cnt, regionArray[cnt]);

    PRINTF("Core access violation and generate busfault!\r\n");

    MPU_DRV_SetLowMasterAccessRights(MPU_INSTANCE, (mpu_region_num_t)0, (mpu_master_t)0, &accessRights);

    PRINTF("Core is granted write access permission!\r\n");  // now core can write regionArray[]

    busFaultFlag = true;
}

/*!
 * @brief MPU example defines protected/unprotected memory region from core
 *        The first and third memory region are configured to read/write/execute able.
 *        A second memory region is configured to be un-writeable, writing to it
 *        is to generate bus fault. In bus fault handler, the region is configured
 *        to writeable to allow writting.
 */
int main(void)
{
    // Access permisson R W E are given to Core/DMA/debugger.
    mpu_low_masters_access_rights_t accessRights =
    {
        .superAccessRights       = kMPUSupervisorReadWriteExecute,
        .userAccessRights        = kMPUUserNoAccessRights,
#if FSL_FEATURE_MPU_HAS_PROCESS_IDENTIFIER
        .processIdentifierEnable = false,
#endif
    };

    // Access permission R W E are given to DMA/debugger/ and core can not Write.
    mpu_low_masters_access_rights_t accessRights1 =
    {
        .superAccessRights = kMPUSupervisorReadExecute,
        .userAccessRights  = kMPUUserNoAccessRights,
    };

    // Init hardware.
    hardware_init();

    // Disable write buffer
    disable_writebuffer();

    // Enable bus fault
    enable_busfault();

    // MPU region1 is used to configure access permission of 0 ~ start address region 1
    userConfig1.regionConfig.regionNum          = (mpu_region_num_t)1U;
    userConfig1.regionConfig.startAddr          = 0;
    userConfig1.regionConfig.endAddr            = ACTUAL_START_ADDR(&regionArray[0]) - 0x01;
    userConfig1.regionConfig.accessRights1[0]   = accessRights;
    userConfig1.regionConfig.regionEnable       = true;
    userConfig1.next = &userConfig2;

    // MPU region2 is used to configure access permission of startAddr ~ startAddr + 100
    userConfig2.regionConfig.regionNum          = (mpu_region_num_t)2U;
    userConfig2.regionConfig.startAddr          = ACTUAL_START_ADDR(&regionArray[0]);
    userConfig2.regionConfig.endAddr            = ACTUAL_END_ADDR(&regionArray[ARRAY_SIZE - 1]);
    userConfig2.regionConfig.accessRights1[0]   = accessRights1;
    userConfig2.regionConfig.regionEnable       = true;
    userConfig2.next = &userConfig3;

    // MPU region3 is used to configure access permission of end address region 1 ~ 0xffffffff
    userConfig3.regionConfig.regionNum          = (mpu_region_num_t)3;
    userConfig3.regionConfig.startAddr          = ACTUAL_END_ADDR(&regionArray[ARRAY_SIZE - 1]) + 0x01;
    userConfig3.regionConfig.endAddr            = 0xFFFFFFFFU;
    userConfig3.regionConfig.accessRights1[0]   = accessRights;
    userConfig3.regionConfig.regionEnable       = true;
    userConfig3.next = NULL;

    // Init MPU
    MPU_DRV_Init(MPU_INSTANCE, &userConfig1);

    PRINTF("MPU example begin.\r\n\r\n");

    // Initialize array
    for(uint32_t i = 0; i < ARRAY_SIZE; i++)
    {
        regionArray[i] = 0;
    }

    while (1)
    {
        PRINTF("Set regionArray to un-writeable.\r\n");

        // Remove Core write permission in region0
        MPU_DRV_SetLowMasterAccessRights(MPU_INSTANCE, (mpu_region_num_t)0, (mpu_master_t)0, &accessRights1);

        PRINTF("Write %d to regionArray at No.%d\r\n", cnt, cnt);

        // Cannot write here, bus fault occur
        regionArray[cnt] = cnt;
        
        // ISB to make sure the instruction execute sequence
        __ISB();

        // Add delay to avoid the check instruction to be done during the bus fault handler response period
        for(volatile uint32_t i = 0; i < 10; i++); 

        if (false == busFaultFlag)
        {
            // Error occur
            PRINTF("ERROR: Cannot protect regionArray !\r\n");
        }
        else
        {
            // Bus fault occurs, regionArray is updated
            PRINTF("regionArray[%d] = %d \r\n", cnt, regionArray[cnt]);
            PRINTF("Protected regionArray successfully !\r\n");
        }

        busFaultFlag = false;

        // Clear regionArray
        regionArray[cnt] = 0;

        PRINTF("Press any key to continue\r\n\r\n");
        GETCHAR();

        if (++cnt == ARRAY_SIZE)
        {
            cnt = 0;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
