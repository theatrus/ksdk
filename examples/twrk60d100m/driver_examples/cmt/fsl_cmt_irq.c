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

#include <stdlib.h>
#include "fsl_cmt_driver.h"
#include "fsl_os_abstraction.h"
 
/*******************************************************************************
 * Variables
 ******************************************************************************/
extern semaphore_t sema;
extern cmt_modulate_data_t  modData;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief CMT ISR callback
 */
void CMT_Callback(uint32_t instance)
{
    if(CMT_DRV_ModulateDataByBit(instance, &modData, false) == kStatus_CMT_Done)
    {
        /* Stop carrier generator and modulation*/
        CMT_DRV_StopCarrierModulate(instance);
        /* Clear end of cycle flag */
        CMT_DRV_ClearEndofCycleFlag(instance, 0);
        /* Post the semaphore*/
        OSA_SemaPost(&sema);
    }
}

/*!
 * @brief System default IRQ handler defined in startup code.
 *
 */
void CMT_IRQHandler(void)
{
    if (CMT_DRV_IsEndofCycle(0))
    {
        /* Clear interrupt flag.*/
        CMT_Callback(0);
    }
}


/*! @} */

/*******************************************************************************
 * EOF
 ******************************************************************************/

