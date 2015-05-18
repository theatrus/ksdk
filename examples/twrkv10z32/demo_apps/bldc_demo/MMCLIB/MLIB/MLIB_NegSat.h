/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_NegSat.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_NegSat function
*
******************************************************************************/
#ifndef MLIB_NEGSAT_H
#define MLIB_NEGSAT_H
/*!
@if MLIB_GROUP
    @addtogroup MLIB_GROUP
@else
    @defgroup MLIB_GROUP   MLIB
@endif
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section MLIB_NegSat_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_NegSat_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_NegSat_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_NegSat_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_NegSat_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Neg.h"
#else
  /* Following include serves for Freescale internal testing purposes only.
  *  This header is not part of the release. */
  #include "CCOV_MLIB_Neg.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_NegSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_NegSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_NegSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_NegSat(...)     macro_dispatcher(MLIB_NegSat, __VA_ARGS__)(__VA_ARGS__)     /*!< This function returns negative value of input parameter and saturate if necessary. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_NegSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_NegSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_NegSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00036 */
    #define MLIB_NegSat_Dsptchr_1(In)     MLIB_NegSat_Dsptchr_2(In,F32)     /*!< Function dispatcher for MLIB_NegSat_Dsptchr_1, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_NegSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_NegSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_NegSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00036 */
    #define MLIB_NegSat_Dsptchr_1(In)     MLIB_NegSat_Dsptchr_2(In,F16)     /*!< Function dispatcher for MLIB_NegSat_Dsptchr_1, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_NegSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_NegSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_NegSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_NegSat_Dsptchr_2(In,Impl)    MLIB_NegSat_Dsptchr_(In,Impl)     /*!< Function dispatcher for MLIB_NegSat_Dsptchr_2, do not modify!!! */

  /*
  * @violates @ref MLIB_NegSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_NegSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_NegSat_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00038 */
  #define MLIB_NegSat_Dsptchr_(In,Impl)     MLIB_NegSat_##Impl(In)            /*!< Function dispatcher for MLIB_NegSat_Dsptchr_, do not modify!!! */
#endif

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/

/****************************************************************************
* Exported function prototypes
****************************************************************************/

/****************************************************************************
* Inline functions
****************************************************************************/





/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function returns negative value of input parameter and saturate if necessary.

@param[in]    f32In      Input value which negative value should be returned.

@return       Negative value of input parameter.

@details      This inline function returns the negative value of input parameter. The input values as well as output
              value is considered as 32-bit fractional data type. The output saturation is implemented in this function,
              thus in case the negation of input values is outside the (-1, 1) interval, the output value is limited to the
              boundary value.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_NegSat_F32
              \image rtf negsatEq1_f32.math "MLIB_NegSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In;
Frac32 f32Out;

void main(void)
{
  // input value = 0.25
  f32In  = FRAC32(0.25);

  // output should be FRAC32(-0.25) = 0xA0000000
  f32Out = MLIB_NegSat_F32(f32In);

  // output should be FRAC32(-0.25) = 0xA0000000
  f32Out = MLIB_NegSat(f32In, F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC32(-0.25) = 0xA0000000
  f32Out = MLIB_NegSat(f32In);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00037, DMLIB00031, DMLIB00033, DMLIB00035, DMLIB00040 */
static inline Frac32 MLIB_NegSat_F32(register Frac32 f32In)
/*
* @violates @ref MLIB_NegSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_NegSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Word32 w32Out;

  w32Out = F32TOINT32(MLIB_Neg_F32(f32In));
  w32Out = (w32Out == INT32_MIN) ? INT32_MAX : w32Out;
  /** @remarks Implements DMLIB00034 */
  return(INT32TOF32(w32Out));
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function returns negative value of input parameter and saturate if necessary.

@param[in]    f16In      Input value which negative value should be returned.

@return       Negative value of input parameter.

@details      This inline function returns the negative value of input parameter. The input values as well as output
              value is considered as 16-bit fractional data type. The output saturation is implemented in this function,
              thus in case the negation of input values is outside the (-1, 1) interval, the output value is limited to the
              boundary value.

              \par

              The output of the function is defined by the following simple equation:

              \anchor eq1_NegSat_F16
              \image rtf negsatEq1_f16.math "MLIB_NegSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In;
Frac16 f16Out;

void main(void)
{
  // input value = 0.25
  f16In  = FRAC16(0.25);

  // output should be FRAC16(-0.25) = 0xA000
  f16Out = MLIB_NegSat_F16(f16In);

  // output should be FRAC16(-0.25) = 0xA000
  f16Out = MLIB_NegSat(f16In, F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC16(-0.25) = 0xA000
  f16Out = MLIB_NegSat(f16In);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00037, DMLIB00031, DMLIB00032, DMLIB00035, DMLIB00041 */
static inline Frac16 MLIB_NegSat_F16(register Frac16 f16In)
/*
* @violates @ref MLIB_NegSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_NegSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Word16 w16Out;

  w16Out = F16TOINT16(MLIB_Neg_F16(f16In));
  w16Out = (w16Out == INT16_MIN) ? INT16_MAX : w16Out;
  /** @remarks Implements DMLIB00034 */
  return(INT16TOF16(w16Out));
}


#ifdef __cplusplus
}
#endif

#endif /* MLIB_NEGSAT_H */
