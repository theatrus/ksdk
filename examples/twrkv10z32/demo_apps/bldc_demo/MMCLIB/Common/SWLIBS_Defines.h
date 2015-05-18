/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file     SWLIBS_Defines.h
*
* @version  1.0.1.0
*
* @date     Oct-9-2013
*
* @brief    Defines and common Macros definition file.
*
*******************************************************************************
*
******************************************************************************/

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section SWLIBS_Defines_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section SWLIBS_Defines_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section SWLIBS_Defines_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*/


#ifndef SWLIBS_DEFINES_H
#define SWLIBS_DEFINES_H

#include "SWLIBS_Typedefs.h"

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/

/*! Definition of inline directive for all supported compilers. */
#if defined(__ghs__) || defined(__MWERKS__) || defined(__CWCC__)
	/*
	* @violates @ref SWLIBS_Defines_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
	* macro.
	*/
	#define inline __inline
#elif defined(__DCC__)
	#define inline __inline__
#elif defined(__CSMC__)
	#define inline @inline
#else
  #define inline
#endif

/*! Constant representing the maximal negative value of a signed 16-bit fixed point fractional number, equal to -1.0. */
#define SFRACT_MIN          (-1.0)

/*! Constant representing the maximal positive value of a signed 16-bit fixed point fractional
 *  number, equal to 0.999969482421875. */
#define SFRACT_MAX          (0.999969482421875)

/*! Constant representing the maximal negative value of signed 32-bit fixed point fractional number, equal to -1.0. */
#define FRACT_MIN           (-1.0)

/*! Constant representing the maximal positive value of a signed 32-bit fixed point fractional
 *  number, equal to 0.9999999995343387126922607421875. */
#define FRACT_MAX           (0.9999999995343387126922607421875)

/*! Value 0.5 in 32-bit fixed point fractional format. */
#define FRAC32_0_5           ((Frac32) 0x40000000)

/*! Value 0.5 in 16-bit fixed point fractional format. */
#define FRAC16_0_5           ((Frac16) 0x4000)

/*! Value 0.25 in 32-bit fixed point fractional format. */
#define FRAC32_0_25           ((Frac32) 0x20000000)

/*! Value 0.25 in 16-bit fixed point fractional format. */
#define FRAC16_0_25           ((Frac16) 0x2000)

/*! Constant representing the maximal positive value of a unsigned 16-bit fixed point integer
 *  number, equal to 2\f$^{15}\f$ = 0x8000. */
#ifndef UINT16_MAX
#define UINT16_MAX          ((UWord16) 0x8000)
#endif

/*! Constant representing the maximal positive value of a signed 16-bit fixed point integer
 *  number, equal to 2\f$^{15}\f$-1 = 0x7fff. */
#ifndef INT16_MAX
#define INT16_MAX           ((Word16) 0x7fff)
#endif

/*! Constant representing the maximal negative value of a signed 16-bit fixed point integer
 *  number, equal to -2\f$^{15}\f$ = 0x8000. */
#ifndef INT16_MIN
#define INT16_MIN           ((Word16) 0x8000)
#endif

/*! Constant representing the maximal positive value of a unsigned 32-bit fixed point integer
 *  number, equal to 2\f$^{31}\f$ = 0x80000000. */
#ifndef UINT32_MAX
#define UINT32_MAX          ((UWord32) 0x80000000U)
#endif

/*! Constant representing the maximal positive value of a signed 32-bit fixed point integer
 *  number, equal to 2\f$^{31}\f$-1 = 0x7fff ffff. */
#ifndef INT32_MAX
#define INT32_MAX           ((Word32) 0x7fffffff)
#endif

/*! Constant representing the maximal negative value of a signed 32-bit fixed point integer
 *  number, equal to -2\f$^{31}\f$ = 0x8000 0000. */
#ifndef INT32_MIN
#define INT32_MIN           ((Word32) 0x80000000U)
#endif

/*! Constant representing the maximal negative value of the 32-bit float type. */
#ifndef FLOAT_MIN
#define FLOAT_MIN           ((Float)(-3.40282346638528860e+38))
#endif

/*! Constant representing the maximal positive value of the 32-bit float type. */
#ifndef FLOAT_MAX
#define FLOAT_MAX           ((Float)(3.40282346638528860e+38))
#endif

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 16-bit integer value cast to a signed 32-bit integer. */
#define INT16TOINT32(x)        ((Word32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 32-bit integer value cast to a signed 16-bit integer. */
#define INT32TOINT16(x)        ((Word16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 32-bit integer value cast to a signed 64-bit integer. */
#define INT32TOINT64(x)        ((Word64) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 64-bit integer value cast to a signed 32-bit integer. */
#define INT64TOINT32(x)        ((Word32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 16-bit fractional value cast to a signed 16-bit integer. */
#define F16TOINT16(x)       ((Word16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 16 bits of a signed 32-bit fractional value cast to a signed 16-bit integer. */
#define F32TOINT16(x)       ((Word16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 16 bits of a signed 64-bit fractional value cast to a signed 16-bit integer. */
#define F64TOINT16(x)       ((Word16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - a signed 16-bit fractional value cast to a signed 32-bit integer, the value placed at the lower
*   16-bits of the 32-bit result. */
#define F16TOINT32(x)       ((Word32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 32-bit fractional value cast to a signed 32-bit integer. */
#define F32TOINT32(x)       ((Word32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 32 bits of a signed 64-bit fractional value cast to a signed 32-bit integer. */
#define F64TOINT32(x)       ((Word32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 16-bit fractional value cast to a signed 64-bit integer, the value placed at the lower
*   16-bits of the 64-bit result. */
#define F16TOINT64(x)       ((Word64) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 32-bit fractional value cast to a signed 64-bit integer, the value placed at the lower
*   32-bits of the 64-bit result. */
#define F32TOINT64(x)       ((Word64) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 64-bit fractional value cast to a signed 64-bit integer. */
#define F64TOINT64(x)       ((Word64) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 16-bit integer value cast to a signed 16-bit fractional. */
#define INT16TOF16(x)       ((Frac16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 16-bit integer value cast to a signed 32-bit fractional, the value placed at the lower
*   16 bits of the 32-bit result. */
#define INT16TOF32(x)       ((Frac32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 16-bits of a signed 32-bit integer value cast to a signed 16-bit fractional. */
#define INT32TOF16(x)       ((Frac16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - signed 32-bit integer value cast to a signed 32-bit fractional. */
#define INT32TOF32(x)       ((Frac32) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 16-bits of a signed 64-bit integer value cast to a signed 16-bit fractional. */
#define INT64TOF16(x)       ((Frac16) (x))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
*/
/*! Type casting - lower 32-bits of a signed 64-bit integer value cast to a signed 32-bit fractional. */
#define INT64TOF32(x)       ((Frac32) (x))

/*! One over sqrt(3) with a 16-bit result, the result rounded for a better
 * precision, i.e. round(1/\f$\sqrt\f$(3)*2\f$^{15}\f$). */
#define F16_1_DIVBY_SQRT3   ((Frac16) 0x49E7)

/*! One over sqrt(3) with a 32-bit result, the result rounded for a better
 *  precision, i.e. round(1/\f$\sqrt\f$(3)*2\f$^{31}\f$). */
#define F32_1_DIVBY_SQRT3   ((Frac32) 0x49E69D16)

/*! Sqrt(3) divided by two with a 16-bit result, the result rounded for a better
 * precision, i.e. round(\f$\sqrt\f$(3)/2*2\f$^{15}\f$). */
#define F16_SQRT3_DIVBY_2   ((Frac16) 0x6EDA)

/*! Sqrt(3) divided by two with a 32-bit result, the result rounded for a better
 * precision, i.e. round(\f$\sqrt\f$(3)/2*2\f$^{31}\f$). */
#define F32_SQRT3_DIVBY_2   ((Frac32) 0x6ED9EBA1)

/*! Sqrt(2) divided by two with a 16-bit result, the result rounded for a better
 * precision, i.e. round(\f$\sqrt\f$(2)/2*2\f$^{15}\f$). */
#define F16_SQRT2_DIVBY_2   ((Frac16) 0x5A82)

/*! Sqrt(2) divided by two with a 32-bit result, the result rounded for a better
 * precision, i.e. round(\f$\sqrt\f$(2)/2*2\f$^{31}\f$). */
#define F32_SQRT2_DIVBY_2   ((Frac32) 0x5A82799A)

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* @violates @ref SWLIBS_Defines_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
* parameter in definition of macro.
*/
/*! Macro converting a signed fractional [-1,1) number into a 16-bit fixed point number in format Q1.15.*/
#define FRAC16(x)           ((Frac16) (((Float) x) < (SFRACT_MAX) ? (((Float) x) >= SFRACT_MIN ? ((Float) x)*((Float) 0x8000U) : INT16_MIN) : INT16_MAX))

/*
* @violates @ref SWLIBS_Defines_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* @violates @ref SWLIBS_Defines_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
* parameter in definition of macro.
*/
/*! Macro converting a signed fractional [-1,1) number into a 32-bit fixed point number in format Q1.31.*/
#define FRAC32(x)           ((Frac32) (((Float) x) < (FRACT_MAX) ? (((Float) x) >= FRACT_MIN ? ((Float) x)*((Float) 0x80000000U) : INT32_MIN) : INT32_MAX))

/*! One over sqrt(3) in single precision floating point format. */
#define FLOAT_DIVBY_SQRT3   ((Float) 0.5773502691896258)

/*! Sqrt(3) divided by two in single precision floating point format. */
#define FLOAT_SQRT3_DIVBY_2 ((Float) 0.866025403784439)

/*! Sqrt(3) divided by four in single precision floating point format. */
#define FLOAT_SQRT3_DIVBY_4 ((Float) 0.4330127018922190)

/*! Sqrt(3) divided by four correction constant. */
#define FLOAT_SQRT3_DIVBY_4_CORRECTION ((Float)0)

/*! 2*\f$\Pi\f$ in single precision floating point format. */
#define FLOAT_2_PI          ((Float) 6.28318530717958)

/*! \f$\Pi\f$ in single precision floating point format. */
#define FLOAT_PI            ((Float) 3.14159265358979)

/*! \f$\Pi\f$/2 in single precision floating point format. */
#define FLOAT_PI_DIVBY_2    ((Float) 1.57079632679490)

/*! Tan(\f$\Pi\f$/6) in single precision floating point format. */
#define FLOAT_TAN_PI_DIVBY_6   ((Float)0.577350269189626000)

/*! Tan(\f$\Pi\f$/12) in single precision floating point format. */
#define FLOAT_TAN_PI_DIVBY_12   ((Float)0.267949192431123000)

/*! \f$\Pi\f$/6 in single precision floating point format. */
#define FLOAT_PI_DIVBY_6   ((Float)0.523598775598299000)

/*! Double to single precision correction constant for \f$\Pi\f$, equal to (\f$\Pi\f$(Double) - \f$\Pi\f$(Single)). */
 #define FLOAT_PI_SINGLE_CORRECTION ((Float)4.37102068E-8)

/*! Double to single precision correction constant for \f$\Pi\f$, equal to  (2*(\f$\Pi\f$(Double) - \f$\Pi\f$(Single)). */
#define FLOAT_PI_CORRECTION ((Float)8.74204136E-8)

/*! \f$\Pi\f$/4 in single precision floating point format. */
#define FLOAT_PI_DIVBY_4    ((Float) 0.7853981633974480)

/*! Number four divided by \f$\Pi\f$ in single precision floating point format. */
#define FLOAT_4_DIVBY_PI    ((Float) 1.2732395447351600)

/*! Value 0.5 in single precision floating point format. */
#define FLOAT_0_5           ((Float) 0.5)

/*! Value 1 in single precision floating point format. */
#define FLOAT_PLUS_1        ((Float) 1)

/*! Value -1 in single precision floating point format. */
#define FLOAT_MINUS_1       ((Float) -1)

#endif /* SWLIBS_DEFINES_H */
