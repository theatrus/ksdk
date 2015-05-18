/*******************************************************************************
*
* Copyright [2014-]2014 Freescale Semiconductor, Inc.

*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale License
* distributed with this Material.
* See the LICENSE file distributed for more details.
*
*
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "board.h"
#include "fsl_cmp_driver.h"
#include "fsl_device_registers.h"
#include "fsl_aoi_driver.h"
#include "fsl_pit_driver.h"
#include "fsl_xbar_driver.h"
#include "fsl_debug_console.h"

#define DEMO_CMP0_INSTANCE    (0U)
#define DEMO_PIT0_INSTANCE    (0U)

/******************************************************************************
*Definition
******************************************************************************/
cmp_state_t demoCmpStateStruct;

void XBAR_PrintMsg(void);

int main(void)
{
    cmp_comparator_config_t demoCmpUserConfigStruct;
    cmp_sample_filter_config_t demoCmpSampleFilterConfigStruct;
    cmp_dac_config_t demoCmpDacConfigStruct;

    pit_user_config_t demoPitInitStruct;

    aoi_event_config_t demoEventLogicStruct;

    xbar_control_config_t demoControlConfigStruct;
    xbar_state_t xbarStatePtr;

    hardware_init();
    dbg_uart_init();

    PRINTF("XBAR and AOI Demo: Start...\r\n");

    /* Init the CMP0 comparator. */
    CMP_DRV_StructInitUserConfigDefault(&demoCmpUserConfigStruct, kCmpInputChn1, kCmpInputChnDac);
    demoCmpUserConfigStruct.risingIntEnable = false;
    demoCmpUserConfigStruct.fallingIntEnable = false;
    CMP_DRV_Init(DEMO_CMP0_INSTANCE, &demoCmpStateStruct, &demoCmpUserConfigStruct);

    /* Configure the internal DAC when in used. */
    demoCmpDacConfigStruct.dacEnable = true;
    demoCmpDacConfigStruct.dacValue = 32U; /* 0U - 63U */
    demoCmpDacConfigStruct.refVoltSrcMode = kCmpDacRefVoltSrcOf2;
    CMP_DRV_ConfigDacChn(DEMO_CMP0_INSTANCE, &demoCmpDacConfigStruct);

    /* Configure the CMP Sample/Filter Mode. */
    demoCmpSampleFilterConfigStruct.workMode = kCmpContinuousMode;
    CMP_DRV_ConfigSampleFilter(DEMO_CMP0_INSTANCE, &demoCmpSampleFilterConfigStruct);

    /* Init pit module and disable run in debug */
    PIT_DRV_Init(DEMO_PIT0_INSTANCE, false);

    /* Initialize PIT timer 0 */
    demoPitInitStruct.isInterruptEnabled = true,
    demoPitInitStruct.periodUs = 0;
    PIT_DRV_InitChannel(DEMO_PIT0_INSTANCE, 0, &demoPitInitStruct);

    /* Set timer0 period and start it.*/
    PIT_DRV_SetTimerPeriodByUs(DEMO_PIT0_INSTANCE, 0, 500000);

    /* Init the AOI module */
    AOI_DRV_Init(AOI_IDX);

    /* Configure the AOI event */
    demoEventLogicStruct.PT0AC = kAoiConfigInvInputSignal;  /* CMP0 output*/
    demoEventLogicStruct.PT0BC = kAoiConfigInputSignal;     /* PIT0 output*/
    demoEventLogicStruct.PT0CC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT0DC = kAoiConfigLogicOne;

    demoEventLogicStruct.PT1AC = kAoiConfigLogicZero;
    demoEventLogicStruct.PT1BC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT1CC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT1DC = kAoiConfigLogicOne;

    demoEventLogicStruct.PT2AC = kAoiConfigLogicZero;
    demoEventLogicStruct.PT2BC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT2CC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT2DC = kAoiConfigLogicOne;

    demoEventLogicStruct.PT3AC = kAoiConfigLogicZero;
    demoEventLogicStruct.PT3BC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT3CC = kAoiConfigLogicOne;
    demoEventLogicStruct.PT3DC = kAoiConfigLogicOne;
    AOI_DRV_ConfigEventLogic(AOI_IDX, kAoiEvent0, &demoEventLogicStruct);

    /* Init the XBAR module */
    XBAR_DRV_Init(&xbarStatePtr);

    /* Configure the XBAR interrupt */
    demoControlConfigStruct.activeEdge = kXbarEdgeRising;
    demoControlConfigStruct.intDmaReq = kXbarReqIen;
    XBAR_DRV_ConfigOutControl(0, &demoControlConfigStruct);

    /* Configure the XBAR signal connections */
    XBAR_DRV_ConfigSignalConnection(kXbarbInputCMP0_output, kXbarbOutputAOI_IN0);
    XBAR_DRV_ConfigSignalConnection(kXbarbInputPIT_trigger_0, kXbarbOutputAOI_IN1);
    XBAR_DRV_ConfigSignalConnection(kXbaraInputAND_OR_INVERT_0, kXbaraOutputDMAMUX18);

    /* Start the CMP0 function. */
    CMP_DRV_Start(DEMO_CMP0_INSTANCE);

    /* Start the PIT. */
    PIT_DRV_StartTimer(DEMO_PIT0_INSTANCE, 0);

    while (1)
    {

    }
}

void XBAR_PrintMsg(void)
{
    PRINTF("\r\nThe button is pressed...\n\r");
}


