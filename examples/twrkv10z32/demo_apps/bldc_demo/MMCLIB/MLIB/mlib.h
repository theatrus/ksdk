/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
***************************************************************************/
/*!
*
* @file     mlib.h
*
* @version  1.0.1.0
*
* @date     Oct-9-2013
*
* @brief    Master header file.
*
******************************************************************************/
#ifndef MLIB_H
#define MLIB_H

/******************************************************************************
* Includes
******************************************************************************/
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Abs.h"
  #include "MLIB_AbsSat.h"
  #include "MLIB_Neg.h"
  #include "MLIB_NegSat.h"
  #include "MLIB_Add.h"
  #include "MLIB_AddSat.h"
  #include "MLIB_Sub.h"
  #include "MLIB_SubSat.h"
  #include "MLIB_Mul.h"
  #include "MLIB_MulSat.h"
  #include "MLIB_Div.h"
  #include "MLIB_DivSat.h"
  #include "MLIB_ShBi.h"
  #include "MLIB_ShBiSat.h"
  #include "MLIB_ShL.h"
  #include "MLIB_ShLSat.h"
  #include "MLIB_ShR.h"
  #include "MLIB_Norm.h"
  #include "MLIB_Round.h"
  #include "MLIB_Mac.h"
  #include "MLIB_MacSat.h"
  #include "MLIB_VMac.h"
  #include "MLIB_ConvertPU.h"
  #include "MLIB_Convert.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_Abs.h"
  #include "CCOV_MLIB_AbsSat.h"
  #include "CCOV_MLIB_Neg.h"
  #include "CCOV_MLIB_NegSat.h"
  #include "CCOV_MLIB_Add.h"
  #include "CCOV_MLIB_AddSat.h"
  #include "CCOV_MLIB_Sub.h"
  #include "CCOV_MLIB_SubSat.h"
  #include "CCOV_MLIB_Mul.h"
  #include "CCOV_MLIB_MulSat.h"
  #include "CCOV_MLIB_Div.h"
  #include "CCOV_MLIB_DivSat.h"
  #include "CCOV_MLIB_ShBi.h"
  #include "CCOV_MLIB_ShBiSat.h"
  #include "CCOV_MLIB_ShL.h"
  #include "CCOV_MLIB_ShLSat.h"
  #include "CCOV_MLIB_ShR.h"
  #include "CCOV_MLIB_Norm.h"
  #include "CCOV_MLIB_Round.h"
  #include "CCOV_MLIB_Mac.h"
  #include "CCOV_MLIB_MacSat.h"
  #include "CCOV_MLIB_VMac.h"
  #include "CCOV_MLIB_ConvertPU.h"
  #include "CCOV_MLIB_Convert.h"
#endif

#endif /* MLIB_H */
