/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: ieee11073_nom_strings.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* This file contains the nomenclature strings equivalents of the IEEE 11073 nomenclature
* codes. It is used by the manager to print the OID-Type attributes and partition codes.
*
*END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "ieee11073_phd_types.h"
#include "ieee11073_nom_codes.h"

/* Table definition */
const nomenclature_ascii_struct_t  g_nomenclature_ascii_table[] =
{
 /* From Object Infrastructure (MDC_PART_OBJ) */
 {MDC_ATTR_ID_TYPE,                     (int8_t*)"ID type"},
 {MDC_ATTR_METRIC_SPEC_SMALL,           (int8_t*)"Small metric specification "},
 {MDC_ATTR_UNIT_CODE,                   (int8_t*)"Unit code"},
 {MDC_ATTR_ATTRIBUTE_VAL_MAP,           (int8_t*)"Value map"},
 /* From Medical supervisory control and data acquisition (MDC_PART_SCADA) */
 {MDC_TEMP_BODY,                        (int8_t*)"Temp Body"},
 {MDC_PULS_RATE_NON_INV,                (int8_t*)"Pulse Rate"},
 {MDC_PRESS_BLD_NONINV,                 (int8_t*)"Blood Press"},
 {MDC_PRESS_BLD_NONINV_SYS,             (int8_t*)"Systolic BP"},
 {MDC_PRESS_BLD_NONINV_DIA,             (int8_t*)"Diastolic BP"},
 {MDC_PRESS_BLD_NONINV_MEAN,            (int8_t*)"Mean BP "},
 {MDC_MASS_BODY_ACTUAL,                 (int8_t*)"Body Weight"},
 {MDC_LEN_BODY_ACTUAL,                  (int8_t*)"Body Length"},
 {MDC_RATIO_MASS_BODY_LEN_SQ,           (int8_t*)"BMI"},
 {MDC_CTXT_GLU_EXERCISE,                (int8_t*)"Glu Exercise"},
 {MDC_CTXT_GLU_MEAL,                    (int8_t*)"Glu Meal"},
 {MDC_CTXT_GLU_MEAL_PREPRANDIAL,        (int8_t*)"29260"},
 {MDC_CTXT_GLU_MEAL_POSTPRANDIAL,       (int8_t*)"29264"},
 {MDC_CONC_GLU_CAPILLARY_WHOLEBLOOD,    (int8_t*)"Glu Whole"},
 /* From Dimensions (MDC_PART_DIM) */
 {MDC_DIM_PERCENT,                      (int8_t*)"%"},
 {MDC_DIM_MILLI_L,                      (int8_t*)"ml"},
 {MDC_DIM_MILLI_G,                      (int8_t*)"mg"},
 {MDC_DIM_MILLI_G_PER_DL,               (int8_t*)"mg/dl"},
 {MDC_DIM_MILLI_MOLE_PER_L,             (int8_t*)"mmol/l"},
 {MDC_DIM_X_G,                          (int8_t*)"g"},
 {MDC_DIM_KILO_G,                       (int8_t*)"kg"},
 {MDC_DIM_MIN,                          (int8_t*)"min"},
 {MDC_DIM_HR,                           (int8_t*)"h"},
 {MDC_DIM_DAY,                          (int8_t*)"d"},
 {MDC_DIM_DEGC,                         (int8_t*)"degrC"},
 {MDC_DIM_MMHG,                         (int8_t*)"mmHg"},
 {MDC_DIM_CENTI_M,                      (int8_t*)"cm"},
 {MDC_DIM_KG_PER_M_SQ,                  (int8_t*)"kg/m2"},
 {MDC_DIM_BEAT_PER_MIN,                 (int8_t*)"bpm"},
 /* From Communication Infrastructure (MDC_PART_INFRA) */
 {MDC_DEV_SPEC_PROFILE_PULS_OXIM,       (int8_t*)"MDC_DEV_SPEC_PROFILE_PULS_OXIM"},
 {MDC_DEV_SPEC_PROFILE_BP,              (int8_t*)"MDC_DEV_SPEC_PROFILE_BP"},
 {MDC_DEV_SPEC_PROFILE_TEMP,            (int8_t*)"MDC_DEV_SPEC_PROFILE_TEMP"},
 {MDC_DEV_SPEC_PROFILE_SCALE,           (int8_t*)"MDC_DEV_SPEC_PROFILE_SCALE"},
 {MDC_DEV_SPEC_PROFILE_GLUCOSE,         (int8_t*)"MDC_DEV_SPEC_PROFILE_GLUCOSE"},
 {MDC_DEV_SPEC_PROFILE_HF_CARDIO,       (int8_t*)"MDC_DEV_SPEC_PROFILE_HF_CARDIO"},
 {MDC_DEV_SPEC_PROFILE_HF_STRENGTH,     (int8_t*)"MDC_DEV_SPEC_PROFILE_HF_STRENGTH"}  
};

/* Table definition */
const partition_ascii_struct_t  g_partition_ascii_table[] =
{
 {MDC_PART_OBJ,                         (int8_t*)"Object Infrast"},
 {MDC_PART_SCADA,                       (int8_t*)"SCADA"},
 {MDC_PART_DIM,                         (int8_t*)"Dimension"},
 {MDC_PART_INFRA,                       (int8_t*)"Infrastructure"},
 {MDC_PART_PHD_DM,                      (int8_t*)"Disease Mgmt"},
 {MDC_PART_PHD_HF,                      (int8_t*)"H&F Set"},
 {MDC_PART_PHD_AI,                      (int8_t*)"Aging Independently"},
 {MDC_PART_RET_CODE,                    (int8_t*)"Return Codes"},
 {MDC_PART_EXT_NOM,                     (int8_t*)"Ext. Nomenclature"}
};

const uint16_t g_nom_ascii_count        = sizeof(g_nomenclature_ascii_table)/sizeof(g_nomenclature_ascii_table[0]);
const uint16_t g_partition_ascii_count  = sizeof(g_partition_ascii_table)/sizeof(g_partition_ascii_table[0]);
