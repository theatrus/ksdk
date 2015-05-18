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

#include "fsl_xbar_driver.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void XBAR_PrintMsg();

/*******************************************************************************
 * Code
 ******************************************************************************/
/* XBAR IRQ handler that would cover the same name's APIs in startup code */
#if !defined(FSL_FEATURE_XBAR_HAS_SINGLE_MODULE)
void XBARA_IRQHandler(void)
#else
void XBAR_IRQHandler(void)
#endif
{
    XBAR_DRV_IRQHandler();
    XBAR_PrintMsg();
}

/******************************************************************************
 * EOF
 *****************************************************************************/