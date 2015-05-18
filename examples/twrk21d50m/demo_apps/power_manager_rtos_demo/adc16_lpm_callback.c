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

#include "fsl_power_manager.h"
#include "fsl_clock_manager.h"
#include "adc16_temperature.h"

/*FUNCTION*********************************************************************
 *
 * Function Name : adc16_pm_callback
 * Description   : ADC16 callback for change event from power manager
 *
 *END*************************************************************************/
power_manager_error_code_t adc16_pm_callback(power_manager_notify_struct_t * notify,
    power_manager_callback_data_t * dataPtr)
{
    power_manager_error_code_t result = kPowerManagerSuccess;
    power_manager_modes_t crr_mode = POWER_SYS_GetCurrentMode();
    switch (notify->notifyType)
    {
        case kPowerManagerNotifyRecover:
        break;
        case kPowerManagerNotifyBefore:
        if(!(notify->targetPowerConfigPtr->mode == kPowerManagerRun
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
        || notify->targetPowerConfigPtr->mode == kPowerManagerHsrun
#endif
        ))
        {
            adc16DeinitPitTriggerSource(HWADC_INSTANCE);
        }
        break;
        case kPowerManagerNotifyAfter:
            if( crr_mode == kPowerManagerRun
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
             || crr_mode == kPowerManagerHsrun
#endif
             )
            {
                /* if in run mode, enable trigger to ADC. */
                adc16InitPitTriggerSource(HWADC_INSTANCE);
            }
        break;
        default:
            result = kPowerManagerError;
        break;
    }
    return result;
}
/******************************************************************************
 * EOF
 *****************************************************************************/
