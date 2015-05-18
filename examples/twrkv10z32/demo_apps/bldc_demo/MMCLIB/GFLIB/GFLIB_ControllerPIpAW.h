/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       GFLIB_ControllerPIpAW.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for GFLIB_ControllerPIpAW function
*
******************************************************************************/
#ifndef GFLIB_CONTROLLERPIPAW_H
#define GFLIB_CONTROLLERPIPAW_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section GFLIB_ControllerPIpAW_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section GFLIB_ControllerPIpAW_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section GFLIB_ControllerPIpAW_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section GFLIB_ControllerPIpAW_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section GFLIB_ControllerPIpAW_h_REF_5
* Violates MISRA 2004 Required Rule 5.1, Identifier exceeds 31 characters.
* This is not a violation since all the used compilers interpret the identifiers correctly.
*
* @section GFLIB_ControllerPIpAW_h_REF_6
* Violates MISRA 2004 Required Rule 1.4, Identifier clash.
* This is not a violation since all the used compilers interpret the identifiers correctly.
*/
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_MacroDisp.h"
#include "mlib.h"

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  */
  #define GFLIB_ControllerPIpAW(...)     macro_dispatcher(GFLIB_ControllerPIpAW, __VA_ARGS__)(__VA_ARGS__)     /*!< This function calculates a standard recurrent form of the Proportional- Integral controller, with integral anti-windup. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_P_T                         GFLIB_CONTROLLER_PIAW_P_T_F32                         /*!< Definition of GFLIB_CONTROLLER_PIAW_P_T as alias for GFLIB_CONTROLLER_PIAW_P_T_F32 datatype in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_P_DEFAULT                   GFLIB_CONTROLLER_PIAW_P_DEFAULT_F32                   /*!< Definition of GFLIB_CONTROLLER_PIAW_P_DEFAULT as alias for GFLIB_CONTROLLER_PIAW_P_DEFAULT_F32 default value in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like
    * macro defined.
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGFLIB00313 */
    #define GFLIB_ControllerPIpAW_Dsptchr_2(InErr,pParam)     GFLIB_ControllerPIpAW_Dsptchr_3(InErr,pParam,F32)     /*!< Function dispatcher for GFLIB_ControllerPIpAW_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_P_T                         GFLIB_CONTROLLER_PIAW_P_T_F16                         /*!< Definition of GFLIB_CONTROLLER_PIAW_P_T as alias for GFLIB_CONTROLLER_PIAW_P_T_F16 datatype in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_P_DEFAULT                   GFLIB_CONTROLLER_PIAW_P_DEFAULT_F16                   /*!< Definition of GFLIB_CONTROLLER_PIAW_P_DEFAULT as alias for GFLIB_CONTROLLER_PIAW_P_DEFAULT_F16 default value in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like
    * macro defined.
    * @violates @ref GFLIB_ControllerPIpAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGFLIB00313 */
    #define GFLIB_ControllerPIpAW_Dsptchr_2(InErr,pParam)     GFLIB_ControllerPIpAW_Dsptchr_3(InErr,pParam,F16)     /*!< Function dispatcher for GFLIB_ControllerPIpAW_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  */
  #define GFLIB_ControllerPIpAW_Dsptchr_3(InErr,pParam,Impl)    GFLIB_ControllerPIpAW_Dsptchr_(InErr,pParam,Impl)     /*!< Function dispatcher for GFLIB_ControllerPIpAW_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used
  * in macro.
  */
  /** @remarks Implements DGFLIB00315 */
  #define GFLIB_ControllerPIpAW_Dsptchr_(InErr,pParam,Impl)     GFLIB_ControllerPIpAW_##Impl(InErr,pParam)            /*!< Function dispatcher for GFLIB_ControllerPIpAW_Dsptchr_, do not modify!!! */
#endif

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/




/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)
  /*
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_5 MISRA 2004 Required Rule 5.1, Identifier exceeds 31
  * characters.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  */
  #define GFLIB_CONTROLLER_PIAW_P_DEFAULT_F32 {0,0,0,0,INT32_MIN,INT32_MAX,0,0,0}     /*!< Default value for GFLIB_CONTROLLER_PIAW_P_T_F32. */

  /************************************************************************/
  /*!
  @struct GFLIB_CONTROLLER_PIAW_P_T_F32 "\GFLIB_ControllerPIpAW.h"

  @brief  Structure containing parameters and states of the parallel form PI controller with anti-windup.
  *//**********************************************************************/
  /** @remarks Implements DGFLIB00316 */
  typedef struct{
    Frac32   f32PropGain;        /*!< Proportional Gain, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32   f32IntegGain;       /*!< Integral Gain, fractional format normalized to fit into  (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Word16      w16PropGainShift;   /*!< Proportional Gain Shift, integer format [-31, 31]. */
    Word16      w16IntegGainShift;  /*!< Integral Gain Shift, integer format [-31, 31]. */
    Frac32   f32LowerLimit;      /*!< Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32   f32UpperLimit;      /*!< Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32   f32IntegPartK_1;    /*!< State variable integral part at step k-1. */
    Frac32   f32InK_1;           /*!< State variable input error at step k-1. */
    UWord16      u16LimitFlag;       /*!< Limitation flag, if set to 1, the controller output has reached either the UpperLimit or LowerLimit. */
  }GFLIB_CONTROLLER_PIAW_P_T_F32;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern Frac32 GFLIB_ControllerPIpAW_F32(Frac32 f32InErr,GFLIB_CONTROLLER_PIAW_P_T_F32 * const pParam);
#endif /* SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON */




/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)
  /*
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_5 MISRA 2004 Required Rule 5.1, Identifier exceeds 31
  * characters.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIpAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  */
  #define GFLIB_CONTROLLER_PIAW_P_DEFAULT_F16 {0,0,0,0,INT16_MIN,INT16_MAX,0,0,0}     /*!< Default value for GFLIB_CONTROLLER_PIAW_P_T_F16. */

  /************************************************************************/
  /*!
  @struct GFLIB_CONTROLLER_PIAW_P_T_F16 "\GFLIB_ControllerPIpAW.h"

  @brief  Structure containing parameters and states of the parallel form PI controller with anti-windup.
  *//**********************************************************************/
  /** @remarks Implements DGFLIB00316 */
  typedef struct{
    Frac16   f16PropGain;        /*!< Proportional Gain, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac16   f16IntegGain;       /*!< Integral Gain, fractional format normalized to fit into  (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Word16      w16PropGainShift;   /*!< Proportional Gain Shift, integer format [-15, 15]. */
    Word16      w16IntegGainShift;  /*!< Integral Gain Shift, integer format [-15, 15]. */
    Frac16   f16LowerLimit;      /*!< Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac16   f16UpperLimit;      /*!< Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac32   f32IntegPartK_1;    /*!< State variable integral part at step k-1. */
    Frac16   f16InK_1;           /*!< State variable input error at step k-1. */
    UWord16      u16LimitFlag;       /*!< Limitation flag, if set to 1, the controller output has reached either the UpperLimit or LowerLimit. */
  }GFLIB_CONTROLLER_PIAW_P_T_F16;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern Frac16 GFLIB_ControllerPIpAW_F16(Frac16 f16InErr,GFLIB_CONTROLLER_PIAW_P_T_F16 * const pParam);
#endif /* SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON */




#ifdef __cplusplus
}
#endif

#endif /* GFLIB_CONTROLLERPIPAW_H */
