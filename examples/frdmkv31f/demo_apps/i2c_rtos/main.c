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

#include <assert.h>
#include "i2c_rtos.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// task prio
#define TASK_MASTER_PRIO         6U
#if USE_RTOS
#define TASK_SLAVE_PRIO          4U
#define TASK_SAMPLE_PRIO         5U
#endif

// task size
#define TASK_MASTER_STACK_SIZE   1536U
#if USE_RTOS
#define TASK_SLAVE_STACK_SIZE    1024U
#define TASK_SAMPLE_STACK_SIZE   512U
#endif

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern uint32_t gSlaveId;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

// task declare
extern void task_master(task_param_t param);
#if USE_RTOS
extern void task_slave(task_param_t param);
extern void task_sample(task_param_t param);
#if defined(FSL_RTOS_MQX)
extern void MQX_I2C0_IRQHandler(void);
extern void MQX_I2C1_IRQHandler(void);
#endif
#endif

// task define
OSA_TASK_DEFINE(task_master, TASK_MASTER_STACK_SIZE);
#if USE_RTOS
OSA_TASK_DEFINE(task_slave, TASK_SLAVE_STACK_SIZE);
OSA_TASK_DEFINE(task_sample, TASK_SAMPLE_STACK_SIZE);
#endif

#if (FSL_RTOS_MQX)&&(MQX_COMMON_CONFIG == MQX_LITE_CONFIG)
#if MQX_STDIO
#error "MQX Lite configuration is designed to work with tool provided STD Library.\
Remove reference to MQX STD library from your build tool project options:\
IAR:\
    Linker->Library->aditional_libraries                             - remove lib_mqx_stdlib.a path \
    C/C++ Compiler->Preprocessor->Additional include directories:    - remove mqx_stdlib \
\
KEIL: \
    Linker->Misc controls   - remove lib_mqx_stdlib.lib path \
    C/C++->Include Paths    - remove mqx_stdlib \
\
KDS: \
    C/C++ Build\Settings->Cross ARM C Linker\Miscellaneous    - remove lib_mqx_stdlib.a path\
    C/C++ Build\Settings->Cross ARM C Compiler\Includes       - remove mqx_stdlib (on 4th line)\
\
Atollic: \
    C/C++ Build\Settings->C Linker/Libraries->Librarie search path    - remove lib_mqx_stdlib \
    C/C++ Build\Settings->C Compiler/Directories->Include Paths       - remove mqx_stdlib \
CMAKE : \
    Remove following lines from CMakeList.txt: \
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../rtos/mqx/lib/twrk22f120m.armgcc/debug/mqx_stdlib) \
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../rtos/mqx/lib/twrk22f120m.armgcc/release/mqx_stdlib) \
    \
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../rtos/mqx/lib/twrk22f120m.armgcc/debug/mqx_stdlib/lib_mqx_stdlib.a) \
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../rtos/mqx/lib/twrk22f120m.armgcc/release/mqx_stdlib/lib_mqx_stdlib.a) \
."
#endif /* MQX_STDIO */
#elif (FSL_RTOS_MQX)&&(MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
#define MAIN_TASK        8
void main_task(uint32_t param);
const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, main_task, 0xC00, 20, "main_task", MQX_AUTO_START_TASK},
   { 0L,        0L,        0L,    0L,  0L,         0L }
};
#endif /* (FSL_RTOS_MQX)&&(MQX_COMMON_CONFIG != MQX_LITE_CONFIG) */

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

#if (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
    void main_task(uint32_t param)
#else /* (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG) */
    int main(void)
#endif /* (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG) */
{
    osa_status_t result = kStatus_OSA_Error;

    hardware_init();
    GPIO_DRV_Init(NULL, ledPins);

    // Configure the power mode protection
    SMC_HAL_SetProtection(SMC_BASE_PTR, kAllowPowerModeVlp);

    // get cpu uid low value for slave
#if USE_RTOS
    gSlaveId = SIM_UIDL_UID(SIM_BASE_PTR);
#endif

    OSA_Init();

    NVIC_SetPriority(I2C0_IRQn, 6U);
#if (I2C_INSTANCE_COUNT >= 2)
    NVIC_SetPriority(I2C1_IRQn, 6U);
#endif
#if (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
    OSA_InstallIntHandler(I2C0_IRQn, MQX_I2C0_IRQHandler);
#if (I2C_INSTANCE_COUNT >= 2)
    OSA_InstallIntHandler(I2C1_IRQn, MQX_I2C1_IRQHandler);
#endif
#endif

#if USE_RTOS
    result = OSA_TaskCreate(task_slave,
                    (uint8_t *)"slave",
                    TASK_SLAVE_STACK_SIZE,
                    task_slave_stack,
                    TASK_SLAVE_PRIO,
                    (task_param_t)0,
                    false,
                    &task_slave_task_handler);
    if(result != kStatus_OSA_Success)
    {
        PRINTF("Failed to create slave task\r\n\r\n");
#if (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
        return;
#else
        return -1;
#endif
    }

    result = OSA_TaskCreate(task_sample,
                    (uint8_t *)"sample",
                    TASK_SAMPLE_STACK_SIZE,
                    task_sample_stack,
                    TASK_SAMPLE_PRIO,
                    (task_param_t)0,
                    false,
                    &task_sample_task_handler);
    if (result != kStatus_OSA_Success)
    {
        PRINTF("Failed to create sample task\r\n\r\n");
#if (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
        return;
#else
        return -1;
#endif
    }
#endif /* USE_RTOS */

    // create app tasks
    result = OSA_TaskCreate(task_master,
                    (uint8_t *)"master",
                    TASK_MASTER_STACK_SIZE,
                    task_master_stack,
                    TASK_MASTER_PRIO,
                    (task_param_t)0,
                    false,
                    &task_master_task_handler);
    if (result != kStatus_OSA_Success)
    {
        PRINTF("Failed to create master task\r\n\r\n");
#if (FSL_RTOS_MQX) && (MQX_COMMON_CONFIG != MQX_LITE_CONFIG)
        return;
#else
        return -1;
#endif
    }

    OSA_Start();

    for(;;) {}                    // Should not achieve here
}
