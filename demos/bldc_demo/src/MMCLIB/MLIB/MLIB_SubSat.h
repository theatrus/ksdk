/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_SubSat.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_SubSat function
*
******************************************************************************/
#ifndef MLIB_SUBSAT_H
#define MLIB_SUBSAT_H
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
* @section MLIB_SubSat_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_SubSat_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_SubSat_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_SubSat_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_SubSat_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_SubSat_h_REF_6
* Violates MISRA 2004 Required Rule 12.7, Bitwise operator applied to signed underlying type.
* The fractional arithmetic requires the bit-wise operations on signed values.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Sub.h"
#else
  /* Following include serves for Freescale internal testing purposes only.
  *  This header is not part of the release. */
  #include "CCOV_MLIB_Sub.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_SubSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_SubSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_SubSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_SubSat(...)     macro_dispatcher(MLIB_SubSat, __VA_ARGS__)(__VA_ARGS__)     /*!< This function subtracts the second parameter from the first one and saturate if necessary. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_SubSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_SubSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_SubSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00078 */
    #define MLIB_SubSat_Dsptchr_2(In1,In2)     MLIB_SubSat_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_SubSat_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_SubSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_SubSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_SubSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00078 */
    #define MLIB_SubSat_Dsptchr_2(In1,In2)     MLIB_SubSat_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_SubSat_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_SubSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_SubSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_SubSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_SubSat_Dsptchr_3(In1,In2,Impl)    MLIB_SubSat_Dsptchr_(In1,In2,Impl)     /*!< Function dispatcher for MLIB_SubSat_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_SubSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_SubSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_SubSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref MLIB_SubSat_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00080 */
  #define MLIB_SubSat_Dsptchr_(In1,In2,Impl)     MLIB_SubSat_##Impl(In1,In2)            /*!< Function dispatcher for MLIB_SubSat_Dsptchr_, do not modify!!! */
#endif /* _MATLAB_BAM_CREATE */

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

@brief        This function subtracts the second parameter from the first one and
              saturate if necessary.

@param[in]    f32In1     Operand is a 32-bit number normalized between [-1,1).

@param[in]    f32In2     Operand is a 32-bit number normalized between [-1,1).

@return       The subtraction of the second argument from the first argument.

@details      Subtraction with overflow control of two fractional 32-bit numbers. The second argument is subtracted
              from the first one. The input values as well as output value are considered as 32-bit fractional data
              type. The output saturation is implemented in this function, thus in case the subtraction of input
              parameters is outside the (-1, 1) interval, the output value is limited to the boundary value.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_SubSat_F32
              \image rtf subsatEq1_f32.math "MLIB_SubSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac32 f32In2;
Frac32 f32Out;

void main(void)
{
  // first input = 0.5
  f32In1  = FRAC32(0.5);

  // second input = 0.25
  f32In2  = FRAC32(0.25);

  // output should be 0x20000000
  f32Out = MLIB_SubSat_F32(f32In1,f32In2);

  // output should be 0x20000000
  f32Out = MLIB_SubSat(f32In1,f32In2,F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be 0x20000000
  f32Out = MLIB_SubSat(f32In1,f32In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00079, DMLIB00073, DMLIB00075, DMLIB00077, DMLIB00082 */
static inline Frac32 MLIB_SubSat_F32(register Frac32 f32In1, register Frac32 f32In2)
/*
* @violates @ref MLIB_SubSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_SubSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Word32 w32Temp;
  /*
  * @violates @ref MLIB_SubSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Word32 w32satmin, w32satmax;

  w32Temp = F32TOINT32(MLIB_Sub_F32(f32In1, f32In2));

  /*
  * @violates @ref MLIB_SubSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
  * signed underlying type (&).
  */
  w32satmax = (~F32TOINT32(f32In1) & F32TOINT32(f32In2)) & w32Temp;
  /*
  * @violates @ref MLIB_SubSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
  * signed underlying type (~).
  */
  w32satmin = F32TOINT32(f32In1) & (~F32TOINT32(f32In2)) & (~w32Temp);

  w32Temp = (w32satmin < 0) ? INT32_MIN : w32Temp;
  w32Temp = (w32satmax < 0) ? INT32_MAX : w32Temp;
  /** @remarks Implements DMLIB00076 */
  return(INT32TOF32(w32Temp));
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function subtracts the second parameter from the first one
              and saturate if necessary.

@param[in]    f16In1     Operand is a 16-bit number normalized between [-1,1).

@param[in]    f16In2     Operand is a 16-bit number normalized between [-1,1).

@return       The subtraction of the second argument from the first argument.

@details      Subtraction with overflow control of two fractional 16-bit numbers. The second argument is subtracted from
              the first one. The input values as well as output value are considered as 16-bit fractional data
              type. The output saturation is implemented in this function, thus in case the subtraction of input
              parameters is outside the (-1, 1) interval, the output value is limited to the boundary value.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_SubSat_F16
              \image rtf subsatEq1_f16.math "MLIB_SubSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1;
Frac16 f16In2;
Frac16 f16Out;

void main(void)
{
  // first input = 0.5
  f16In1  = FRAC16(0.5);

  // second input = 0.25
  f16In2  = FRAC16(0.25);

  // output should be 0x2000
  f16Out = MLIB_SubSat_F16(f16In1,f16In2);

  // output should be 0x2000
  f16Out = MLIB_SubSat(f16In1,f16In2,F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be 0x2000
  f16Out = MLIB_SubSat(f16In1,f16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00079, DMLIB00073, DMLIB00074, DMLIB00077, DMLIB00083 */
static inline Frac16 MLIB_SubSat_F16(register Frac16 f16In1,register Frac16 f16In2)
/*
* @violates @ref MLIB_SubSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_SubSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Word32 w32temp;

  w32temp = F32TOINT32(MLIB_Sub_F32((Frac32)f16In1, (Frac32)f16In2));

  w32temp = (w32temp > (Word32) INT16_MAX) ? (Word32) INT16_MAX : w32temp;
  w32temp = (w32temp < (Word32) INT16_MIN) ? (Word32) INT16_MIN : w32temp;
  /** @remarks Implements DMLIB00076 */
  return(INT32TOF16(w32temp));
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_SUBSAT_H */
