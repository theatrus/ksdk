/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file arm_cm0.h
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun. 25, 2013
*
* @brief provide generic high-level routines for ARM Cortex M0/M0+ processors. 
*
*******************************************************************************/

#ifndef _CPU_ARM_CM0_H
#define _CPU_ARM_CM0_H

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief ARM Cortex M0 implementation for interrupt priority shift
#define ARM_INTERRUPT_LEVEL_BITS          2

//! @brief Macro to enable all interrupts.
#if defined __CC_ARM
#define EnableInterrupts    __enable_irq()
#else
#define EnableInterrupts    asm(" CPSIE i");
#endif

//! @brief Macro to disable all interrupts.
#if defined __CC_ARM
#define DisableInterrupts   __disable_irq()
#else
#define DisableInterrupts   asm(" CPSID i");
#endif

#define disable_irq(irq)             NVIC_DisableIRQ(irq)
#define enable_irq(irq)              NVIC_EnableIRQ(irq)
#define set_irq_priority(irq, prio)  NVIC_SetPriority(irq, prio)

//! @brief Misc. Defines
#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE   (0)

#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE    (1)

#ifdef  NULL
#undef  NULL
#endif
#define NULL    (0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#undef  ENABLE
#define ENABLE  (1)

#undef  DISABLE
#define DISABLE  (0)

//! @brief The basic data types
typedef unsigned char       uint8;      //!< 8 bits
typedef unsigned short int  uint16;     //!< 16 bits
typedef unsigned long int   uint32;     //!< 32 bits

typedef char                int8;       //!< 8 bits
typedef short int           int16;      //!< 16 bits
typedef int                 int32;      //!< 32 bits

typedef volatile int8       vint8;      //!<  8 bits
typedef volatile int16      vint16;     //!< 16 bits
typedef volatile int32      vint32;     //!< 32 bits

typedef volatile uint8      vuint8;     //!< 8 bits
typedef volatile uint16     vuint16;    //!< 16 bits
typedef volatile uint32     vuint32;    //!< 32 bits

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Function prototype for main function
 */
int main(void);

/*!
 * @brief Function prototype for arm_cm0.c
 */
void stop (void);
void wait (void);
void write_vtor (int);

#endif  /* _CPU_ARM_CM4_H */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

