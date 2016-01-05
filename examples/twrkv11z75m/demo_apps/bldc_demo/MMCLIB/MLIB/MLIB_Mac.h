/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Mac.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Mac function
*
******************************************************************************/
#ifndef MLIB_MAC_H
#define MLIB_MAC_H
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
* @section MLIB_Mac_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Mac_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Mac_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Mac_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Mac_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Add.h"
  #include "MLIB_Mul.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_Add.h"
  #include "CCOV_MLIB_Mul.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Mac_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Mac_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Mac_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Mac(...)     macro_dispatcher(MLIB_Mac, __VA_ARGS__)(__VA_ARGS__)     /*!< This function implements the multiply accumulate function. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_Mac_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Mac_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Mac_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00216 */
    #define MLIB_Mac_Dsptchr_3(In1,In2,In3)     MLIB_Mac_Dsptchr_4(In1,In2,In3,F32)     /*!< Function dispatcher for MLIB_Mac_Dsptchr_4, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_Mac_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Mac_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Mac_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00216 */
    #define MLIB_Mac_Dsptchr_3(In1,In2,In3)     MLIB_Mac_Dsptchr_4(In1,In2,In3,F16)     /*!< Function dispatcher for MLIB_Mac_Dsptchr_4, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_Mac_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Mac_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Mac_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Mac_Dsptchr_4(In1,In2,In3,Impl)    MLIB_Mac_Dsptchr_(In1,In2,In3,Impl)     /*!< Function dispatcher for MLIB_Mac_Dsptchr_5, do not modify!!! */

  /*
  * @violates @ref MLIB_Mac_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Mac_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Mac_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  * @violates @ref MLIB_Mac_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00218 */
  #define MLIB_Mac_Dsptchr_(In1,In2,In3,Impl)     MLIB_Mac_##Impl(In1,In2,In3)            /*!< Function dispatcher for MLIB_Mac_Dsptchr_, do not modify!!! */
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

@brief        This function implements the multiply accumulate function.

@param[in]    f32In1     Input value to be add.

@param[in]    f32In2     First value to be multiplied.

@param[in]    f32In3     Second value to be multiplied.

@return       Multiplied second and third input value with adding of first input value.

@details      This inline function returns the multiplied second and third input value with adding of first input value.
              The input values as well as output value is considered as 32-bit fractional values. The output saturation
              is not implemented in this function, thus in case the output value is outside the (-1, 1) interval,
              the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Mac_F32
              \image rtf macEq1_f32.math "MLIB_Mac_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac32 f32In2;
Frac32 f32In3;
Frac32 f32Out;

void main(void)
{
  // input1 value = 0.25
  f32In1  = FRAC32(0.25);

  // input2 value = 0.15
  f32In2  = FRAC32(0.15);

  // input3 value = 0.35
  f32In3  = FRAC32(0.35);

  // output should be FRAC32(0.3025) = 0x26B851EB
  f32Out = MLIB_Mac_F32(f32In1, f32In2, f32In3);

  // output should be FRAC32(0.3025) = 0x26B851EB
  f32Out = MLIB_Mac(f32In1, f32In2, f32In3, F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC32(0.3025) = 0x26B851EB
  f32Out = MLIB_Mac(f32In1, f32In2, f32In3);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00217, DMLIB00209, DMLIB00212, DMLIB00215 */
static inline Frac32 MLIB_Mac_F32(register Frac32 f32In1,register Frac32 f32In2,register Frac32 f32In3)
/*
* @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /*
  * @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  Frac32 f32Temp;

  f32Temp = MLIB_Mul_F32(f32In2,f32In3);
  /** @remarks Implements DMLIB00213 */
  return(MLIB_Add_F32(f32In1, f32Temp));
}





/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function implements the multiply accumulate function.

@param[in]    f32In1     Input value to be add.

@param[in]    f16In2     First value to be multiplied.

@param[in]    f16In3     Second value to be multiplied.

@return       Multiplied second and third input value with adding of first input value.

@details      This inline function returns the multiplied second and third input value with adding of first input value.
              The first input value as well as output value is considered as 32-bit fractional values. The second and
              third input values are considered as 16-bit fractional values.  The output saturation is not implemented
              in this function, thus in case the output value is outside the (-1, 1) interval, the output value
              will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Mac_F32F16F16
              \image rtf macEq1_f32f16f16.math "MLIB_Mac_Eq1"

              This implementation is available if 32-bit fractional implementations are enabled. However it is not
              possible to use the default implementation based function call, thus the implementation post-fix or additional
              parameter function call shall be used.

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac16 f16In2;
Frac16 f16In3;
Frac32 f32Out;

void main(void)
{
  // input1 value = 0.25
  f32In1  = FRAC32(0.25);

  // input2 value = 0.15
  f16In2  = FRAC16(0.15);

  // input3 value = 0.35
  f16In3  = FRAC16(0.35);

  // output should be FRAC32(0.3025) = 0x26B851EB
  f32Out = MLIB_Mac_F32F16F16(f32In1, f16In2, f16In3);

  // output should be FRAC32(0.3025) = 0x26B851EB
  f32Out = MLIB_Mac(f32In1, f32In2, f32In3, F32F16F16);

}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00217, DMLIB00209, DMLIB00213, DMLIB00215 */
static inline Frac32 MLIB_Mac_F32F16F16(register Frac32 f32In1,register Frac16 f16In2,register Frac16 f16In3)
/*
* @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /*
  * @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  Frac32 f32Temp;

  f32Temp = MLIB_Mul_F32F16F16(f16In2,f16In3);
  /** @remarks Implements DMLIB00214 */
  return(MLIB_Add_F32(f32In1, f32Temp));
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function implements the multiply accumulate function.

@param[in]    f16In1     Input value to be add.

@param[in]    f16In2     First value to be multiplied.

@param[in]    f16In3     Second value to be multiplied.

@return       Multiplied second and third input value with adding of first input value.

@details      This inline function returns the multiplied second and third input value with adding of first input value.
              The input values as well as output value is considered as 16-bit fractional values. The output saturation
              is not implemented in this function, thus in case the output value is outside the (-1, 1) interval,
              the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Mac_F16
              \image rtf macEq1_f16.math "MLIB_Mac_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1;
Frac16 f16In2;
Frac16 f16In3;
Frac16 f16Out;

void main(void)
{
  // input1 value = 0.25
  f16In1  = FRAC16(0.25);

  // input2 value = 0.15
  f16In2  = FRAC16(0.15);

  // input3 value = 0.35
  f16In3  = FRAC16(0.35);

  // output should be FRAC16(0.3025) = 0x26B8
  f16Out = MLIB_Mac_F16(f16In1, f16In2, f16In3);

  // output should be FRAC16(0.3025) = 0x26B8
  f16Out = MLIB_Mac(f16In1, f16In2, f16In3, F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC16(0.3025) = 0x26B8
  f16Out = MLIB_Mac(f16In1, f16In2, f16In3);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00217, DMLIB00209, DMLIB00211, DMLIB00215 */
static inline Frac16 MLIB_Mac_F16(register Frac16 f16In1,register Frac16 f16In2,register Frac16 f16In3)
/*
* @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /*
  * @violates @ref MLIB_Mac_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  Frac16 f16Temp;

  f16Temp = MLIB_Mul_F16(f16In2,f16In3);
  /** @remarks Implements DMLIB00213 */
  return(MLIB_Add_F16(f16In1,f16Temp));
}





#ifdef __cplusplus
}
#endif

#endif /* MLIB_MAC_H */
