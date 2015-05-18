/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       GFLIB_ControllerPIrAW.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for GFLIB_ControllerPIrAW function
*
******************************************************************************/
#ifndef GFLIB_CONTROLLERPIRAW_H
#define GFLIB_CONTROLLERPIRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section GFLIB_ControllerPIrAW_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section GFLIB_ControllerPIrAW_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section GFLIB_ControllerPIrAW_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section GFLIB_ControllerPIrAW_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section GFLIB_ControllerPIrAW_h_REF_5
* Violates MISRA 2004 Required Rule 5.1, Identifier exceeds 31 characters.
* This is not a violation since all the used compilers interpret the identifiers correctly.
*
* @section GFLIB_ControllerPIrAW_h_REF_6
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
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  */
  #define GFLIB_ControllerPIrAW(...)     macro_dispatcher(GFLIB_ControllerPIrAW, __VA_ARGS__)(__VA_ARGS__)     /*!< This function calculates a standard recurrent form of the Proportional-Integral controller, with integral anti-windup. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_R_T                         GFLIB_CONTROLLER_PIAW_R_T_F32                         /*!< Definition of GFLIB_CONTROLLER_PIAW_R_T as alias for GFLIB_CONTROLLER_PIAW_R_T_F32 datatype in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_R_DEFAULT                   GFLIB_CONTROLLER_PIAW_R_DEFAULT_F32                   /*!< Definition of GFLIB_CONTROLLER_PIAW_R_DEFAULT as alias for GFLIB_CONTROLLER_PIAW_R_DEFAULT_F32 default value in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like
    * macro defined.
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGFLIB00287 */
    #define GFLIB_ControllerPIrAW_Dsptchr_2(InErr,pParam)     GFLIB_ControllerPIrAW_Dsptchr_3(InErr,pParam,F32)     /*!< Function dispatcher for GFLIB_ControllerPIrAW_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_R_T                         GFLIB_CONTROLLER_PIAW_R_T_F16                         /*!< Definition of GFLIB_CONTROLLER_PIAW_R_T as alias for GFLIB_CONTROLLER_PIAW_R_T_F16 datatype in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GFLIB_CONTROLLER_PIAW_R_DEFAULT                   GFLIB_CONTROLLER_PIAW_R_DEFAULT_F16                   /*!< Definition of GFLIB_CONTROLLER_PIAW_R_DEFAULT as alias for GFLIB_CONTROLLER_PIAW_R_DEFAULT_F16 default value in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like
    * macro defined.
    * @violates @ref GFLIB_ControllerPIrAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGFLIB00287 */
    #define GFLIB_ControllerPIrAW_Dsptchr_2(InErr,pParam)     GFLIB_ControllerPIrAW_Dsptchr_3(InErr,pParam,F16)     /*!< Function dispatcher for GFLIB_ControllerPIrAW_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  */
  #define GFLIB_ControllerPIrAW_Dsptchr_3(InErr,pParam,Impl)    GFLIB_ControllerPIrAW_Dsptchr_(InErr,pParam,Impl)     /*!< Function dispatcher for GFLIB_ControllerPIrAW_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
  * definition for macro.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
  * macro parameter in definition of macro.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used
  * in macro.
  */
  /** @remarks Implements DGFLIB00290 */
  #define GFLIB_ControllerPIrAW_Dsptchr_(InErr,pParam,Impl)     GFLIB_ControllerPIrAW_##Impl(InErr,pParam)            /*!< Function dispatcher for GFLIB_ControllerPIrAW_Dsptchr_, do not modify!!! */
#endif

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/




/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)
  /*
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_5 MISRA 2004 Required Rule 5.1, Identifier exceeds 31
  * characters.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  */
  #define GFLIB_CONTROLLER_PIAW_R_DEFAULT_F32 {0,0,0,0,INT32_MIN,INT32_MAX,0}     /*!< Default value for GFLIB_CONTROLLER_PIAW_R_T_F32. */

  /************************************************************************/
  /*!
  @struct GFLIB_CONTROLLER_PIAW_R_T_F32 "\GFLIB_ControllerPIrAW.h"

  @brief  Structure containing parameters and states of the recurrent form PI controller with anti-windup.
  *//**********************************************************************/
  /** @remarks Implements DGFLIB00289 */
  typedef struct{
    Frac32 f32CC1sc;      /*!< CC1 coefficient, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32 f32CC2sc;      /*!< CC2 coefficient, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32 f32Acc;        /*!< Internal controller accumulator. */
    Frac32 f32InErrK1;    /*!< Controller input from the previous calculation step. */
    Frac32 f32UpperLimit; /*!< Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    Frac32 f32LowerLimit; /*!< Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{31}\f$, 2\f$^{31}\f$-1). */
    UWord16    u16NShift;     /*!< Scaling factor for the controller coefficients, integer format [0, 31]. */
  }GFLIB_CONTROLLER_PIAW_R_T_F32;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern Frac32 GFLIB_ControllerPIrAW_F32(Frac32 f32InErr,GFLIB_CONTROLLER_PIAW_R_T_F32 * const pParam);
#endif /* SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON */




/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)
  /*
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_5 MISRA 2004 Required Rule 5.1, Identifier exceeds 31
  * characters.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  * @violates @ref GFLIB_ControllerPIrAW_h_REF_6 MISRA 2004 Required Rule 1.4, Identifier clash.
  */
  #define GFLIB_CONTROLLER_PIAW_R_DEFAULT_F16 {0,0,0,0,INT16_MIN,INT16_MAX,0}     /*!< Default value for GFLIB_CONTROLLER_PIAW_R_T_F16. */

  /************************************************************************/
  /*!
  @struct GFLIB_CONTROLLER_PIAW_R_T_F16 "\GFLIB_ControllerPIrAW.h"

  @brief  Structure containing parameters and states of the recurrent form PI controller with anti-windup.
  *//**********************************************************************/
  /** @remarks Implements DGFLIB00289 */
  typedef struct{
    Frac16 f16CC1sc;      /*!< CC1 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac16 f16CC2sc;      /*!< CC2 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac32 f32Acc;        /*!< Internal controller accumulator. */
    Frac16 f16InErrK1;    /*!< Controller input from the previous calculation step. */
    Frac16 f16UpperLimit; /*!< Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    Frac16 f16LowerLimit; /*!< Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1). */
    UWord16    u16NShift;     /*!< Scaling factor for the controller coefficients, integer format [0, 15]. */
  }GFLIB_CONTROLLER_PIAW_R_T_F16;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern Frac16 GFLIB_ControllerPIrAW_F16(Frac16 f16InErr,GFLIB_CONTROLLER_PIAW_R_T_F16 * const pParam);
#endif /* SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON */




#ifdef __cplusplus
}
#endif

#endif /* GFLIB_CONTROLLERPIRAW_H */
