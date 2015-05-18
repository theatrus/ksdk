/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_ShBi.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_ShBi function
*
******************************************************************************/
#ifndef MLIB_SHBI_H
#define MLIB_SHBI_H
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
* @section MLIB_ShBi_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_ShBi_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_ShBi_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_ShBi_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_ShBi_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_ShBi_h_REF_6
* Violates MISRA 2004 Required Rule 10.3, Cast of complex expression changes signedness.
* The sign of the w16In2 parameter is changed only if its value is negative, thus the absolute value of the shifting amount is required and the signedness changing is intentionally handled.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_ShL.h"
  #include "MLIB_ShR.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_ShL.h"
  #include "CCOV_MLIB_ShR.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_ShBi_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_ShBi_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShBi_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_ShBi(...)     macro_dispatcher(MLIB_ShBi, __VA_ARGS__)(__VA_ARGS__)    /*!< Based on sign of second parameter this function shifts the first parameter to right or left. If the sign of second parameter is negative, shift to right. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_ShBi_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_ShBi_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_ShBi_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00145 */
    #define MLIB_ShBi_Dsptchr_2(In1,In2)     MLIB_ShBi_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_ShBi_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_ShBi_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_ShBi_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_ShBi_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00145 */
    #define MLIB_ShBi_Dsptchr_2(In1,In2)     MLIB_ShBi_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_ShBi_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_ShBi_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_ShBi_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShBi_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_ShBi_Dsptchr_3(In1,In2,Impl)    MLIB_ShBi_Dsptchr_(In1,In2,Impl)   /*!< Function dispatcher for MLIB_ShBi_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_ShBi_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_ShBi_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShBi_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  * @violates @ref MLIB_ShBi_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00147 */
  #define MLIB_ShBi_Dsptchr_(In1,In2,Impl)     MLIB_ShBi_##Impl(In1,In2)          /*!< Function dispatcher for MLIB_ShBi_Dsptchr_, do not modify!!! */
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

@brief        This function shifts the first argument to left or right by number defined
              by second argument.

@param[in]         f32In1     First value to be shift.

@param[in]         w16In2     The shift amount value.

@return       32-bit fractional value shifted to left or right by the shift amount.
              The bits beyond the 32-bit boundary are discarded.

@details      Based on sign of second parameter this function shifts the first
              parameter to right or left. If the sign of second parameter is
              negative, shift to right. Overflow is not detected.

@note         The shift amount cannot exceed in magnitude the bit-width of the
              shift value, that means must be within the range -31...31. Otherwise
              the result of the function is undefined.
              Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac32 f32Out;
Word16 w16In2;

void main(void)
{
    // first input = 0.25
    f32In1 = FRAC32(0.25);
    // second input = -1
    w16In2 = -1;

    // output should be 0x10000000 ~ FRAC32(0.125)
    f32Out = MLIB_ShBi_F32(f32In1, w16In2);

    // output should be 0x10000000 ~ FRAC32(0.125)
    f32Out = MLIB_ShBi(f32In1, w16In2, F32);

    // ##############################################################
    // Available only if 32-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x10000000 ~ FRAC32(0.125)
    f32Out = MLIB_ShBi(f32In1, w16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00146, DMLIB00140, DMLIB00144 */
static inline Frac32 MLIB_ShBi_F32(register Frac32 f32In1,register Word16 w16In2)
/*
* @violates @ref MLIB_ShBi_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_ShBi_h_REF_6 MISRA 2004 Required Rule 10.3, Cast of complex expression
  * changes signedness.
  */
  /** @remarks Implements DMLIB00142, DMLIB00143 */
  return (w16In2<(Word16)0) ? MLIB_ShR_F32(f32In1, (UWord16)(-w16In2)) : MLIB_ShL_F32(f32In1, (UWord16)(w16In2));
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function shifts the first argument to left or right by number defined
              by second argument.

@param[in]         f16In1     First value to be left shift.

@param[in]         w16In2     The shift amount value.

@return       16-bit fractional value shifted to left or right by the shift amount.
              The bits beyond the 16-bit boundary are discarded.

@details      Based on sign of second parameter this function shifts the first
              parameter to right or left. If the sign of second parameter is
              negative, shift to right. Overflow is not detected.

@note         The shift amount cannot exceed in magnitude the bit-width of the
              shift value, that means must be within the range -15...15. Otherwise
              the result of the function is undefined.
              Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1;
Frac16 f16Out;
Word16 w16In2;

void main(void)
{
    // first input = 0.25
    f16In1 = FRAC16(0.25);
    // second input = -1
    w16In2 = -1;

    // output should be 0x1000 ~ FRAC16(0.125)
    f16Out = MLIB_ShBi_F16(f16In1, w16In2);

    // output should be 0x1000 ~ FRAC16(0.125)
    f16Out = MLIB_ShBi(f16In1, w16In2, F16);

    // ##############################################################
    // Available only if 16-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x1000 ~ FRAC16(0.125)
    f16Out = MLIB_ShBi(f16In1, w16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00146, DMLIB00140, DMLIB00144 */
static inline Frac16 MLIB_ShBi_F16(register Frac16 f16In1,register Word16 w16In2)
/*
* @violates @ref MLIB_ShBi_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_ShBi_h_REF_6 MISRA 2004 Required Rule 10.3, Cast of complex expression
  * changes signedness.
  */
  /** @remarks Implements DMLIB00141, DMLIB00143 */
  return (w16In2<(Word16)0) ? MLIB_ShR_F16(f16In1, (UWord16)(-w16In2)) : MLIB_ShL_F16(f16In1, (UWord16)(w16In2));
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_SHBI_H */
