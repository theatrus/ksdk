/*HEADER**********************************************************************
*
* Copyright 2015 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   Example using RTCS Library.
*
*END************************************************************************/

#include "app_rtos.h"
#include <rtcs.h>

void hardware_init(void) {
    uint8_t i;

    /* enable clock for PORTs */
    for (i = 0; i < PORT_INSTANCE_COUNT; i++)
    {
      CLOCK_SYS_EnablePortClock(i);
    }

    /* Init board clock */
    BOARD_ClockInit();
    /* In case IO sub is turned off, dbg console should be used for printing */
#if !BSPCFG_ENABLE_IO_SUBSYSTEM
    dbg_uart_init();
#endif
    /* Set clock source for Ethernet */
    if (0 == strcmp("TWR-K65F180M", BOARD_NAME))
    {
        CLOCK_SYS_SetEnetRmiiSrc(ENET_IDX, kClockRmiiSrcExt);
    }
}
