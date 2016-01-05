/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   peripherals_init.h
*
* @brief  Init functions
*        
* @version 1.0.2.0
* 
* @date Oct-9-2013
* 
*******************************************************************************/
#ifndef _PERIPHERALS_
#define _PERIPHERALS_

#include "mlib.h"
#include "gflib.h"
#include "gdflib.h"
#include "gmclib.h"
#include "MC33927.h"
#include "arm_cm0.h"
#include "fsl_device_registers.h"
#include "hwconfig.h"
#include "fsl_sim_hal.h"
#include "fsl_clock_manager.h"
#include "Fsl_mcg_hal.h"
#include "fsl_adc16_hal.h"
#include "fsl_ftm_hal.h"
#include "fsl_port_hal.h"
#include "fsl_gpio_hal.h"
#include "fsl_interrupt_manager.h"
#include "fsl_dspi_hal.h"
#include "fsl_uart_hal.h"
#include "fsl_pdb_hal.h"
#include "fsl_pdb_driver.h"

/******************************************************************************
* Local functions
******************************************************************************/
void MCU_init(void);
void Clock_init(void);
void FTM0_init(void);
void FTM1_init(void);
void FTM2_init(void);
void SPI0_init(void);
void GPIO_init(void);
void MC33927_config(void);
int ADC0_init(void);
void PDB_init(void);
void UART1_init(void);
unsigned char SPI_Send(unsigned char);
void MC33927_readStatus(void);
void FTM0_SetDutyCycle(int16);
void FTM0_SetPwmOutput(int16 sector);

/******************************************************************************
* Local types
******************************************************************************/
/******************************************************************************
 * ADC calibration data structure definition                                  *
 ******************************************************************************/
typedef struct { uint32 OFS, PG, MG, CLPD, CLPS, CLP[5], CLMD, CLMS, CLM[5]; } tADC_CALIB;


#endif /* _PERIPHERALS_ */

