/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       GMCLIB_SvmStd.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for GMCLIB_SvmStd function
*
******************************************************************************/
#ifndef GMCLIB_SVMSTD_H
#define GMCLIB_SVMSTD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section GMCLIB_SvmStd_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section GMCLIB_SvmStd_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section GMCLIB_SvmStd_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section GMCLIB_SvmStd_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
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
  * @violates @ref GMCLIB_SvmStd_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref GMCLIB_SvmStd_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref GMCLIB_SvmStd_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define GMCLIB_SvmStd(...)     macro_dispatcher(GMCLIB_SvmStd, __VA_ARGS__)(__VA_ARGS__)     /*!< This function calculates the duty-cycle ratios using the Standard Space Vector Modulation technique. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref GMCLIB_SvmStd_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref GMCLIB_SvmStd_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
    * defined.
    * @violates @ref GMCLIB_SvmStd_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DGMCLIB00074 */
    #define GMCLIB_SvmStd_Dsptchr_2(pOut,pIn)     GMCLIB_SvmStd_Dsptchr_3(pOut,pIn,F32)     /*!< Function dispatcher for GMCLIB_SvmStd_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref GMCLIB_SvmStd_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref GMCLIB_SvmStd_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
    * defined.
    * @violates @ref GMCLIB_SvmStd_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DGMCLIB00074 */
    #define GMCLIB_SvmStd_Dsptchr_2(pOut,pIn)     GMCLIB_SvmStd_Dsptchr_3(pOut,pIn,F16)     /*!< Function dispatcher for GMCLIB_SvmStd_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref GMCLIB_SvmStd_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref GMCLIB_SvmStd_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref GMCLIB_SvmStd_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define GMCLIB_SvmStd_Dsptchr_3(pOut,pIn,Impl)    GMCLIB_SvmStd_Dsptchr_(pOut,pIn,Impl)     /*!< Function dispatcher for GMCLIB_SvmStd_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref GMCLIB_SvmStd_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref GMCLIB_SvmStd_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref GMCLIB_SvmStd_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref GMCLIB_SvmStd_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in
  * macro.
  */
  /** @remarks Implements DGMCLIB00078 */
  #define GMCLIB_SvmStd_Dsptchr_(pOut,pIn,Impl)     GMCLIB_SvmStd_##Impl(pOut,pIn)            /*!< Function dispatcher for GMCLIB_SvmStd_Dsptchr_, do not modify!!! */
#endif

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/




/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern UWord32 GMCLIB_SvmStd_F32(MCLIB_3_COOR_SYST_T_F32 * pOut,const MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32 *const pIn);
#endif /* SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON */




/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern UWord16 GMCLIB_SvmStd_F16(MCLIB_3_COOR_SYST_T_F16 * pOut,const MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16 *const pIn);
#endif /* SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON */




#ifdef __cplusplus
}
#endif

#endif /* GMCLIB_SVMSTD_H */
