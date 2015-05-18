/*HEADER**********************************************************************
*
* Copyright 2009 Freescale Semiconductor, Inc.
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
*   The file contains low level eSDHC driver functions.
*
*
*END************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <mqx.h>
#include <nio.h>

#include <ioctl.h>
#include <fcntl.h>
#include <fs_supp.h>

#include <fsl_device_registers.h>
#include <fsl_port_hal.h>
#include <fsl_sim_hal.h>
#include <fsl_clock_manager.h>


#include <esdhc.h>
#include <esdhc_prv.h>


static int nio_esdhc_open(void *dev_context, const char *dev_name, int flags, void **fp_context, int *error);
static int nio_esdhc_close(void *dev_context, void *fp_context, int *error);
static int nio_esdhc_ioctl(void *dev_context, void *fp_context, int *error, unsigned long int cmd, va_list ap);
static int nio_esdhc_read(void *dev_context, void *fp_context, void *data_ptr, size_t n, int *error);
static int nio_esdhc_write(void *dev_context, void *fp_context, const void *data_ptr, size_t n, int *error);
static int nio_esdhc_deinit(void *dev_context, int *error);
static int nio_esdhc_init(void *init_data, void **dev_context, int *error);

static void _esdhc_isr(void *parameter);

const NIO_DEV_FN_STRUCT nio_esdhc_dev_fn = {
    .OPEN = nio_esdhc_open, .READ = nio_esdhc_read, .WRITE = nio_esdhc_write, .LSEEK = NULL, .IOCTL = nio_esdhc_ioctl, .CLOSE = nio_esdhc_close, .INIT = nio_esdhc_init, .DEINIT = nio_esdhc_deinit,
};

const ESDHC_INIT_STRUCT _bsp_esdhc_init = {
    SDHC_IRQn, /* SDHC IRQ number      */
    SDHC_BASE, /* Peripheral base addr */
    ESDHC_MAX_BAUDRATE_25MHz, /* ESDHC baudrate       */
};

#if 0

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_esdhc_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to ESDHC
*
*END*----------------------------------------------------------------------*/


_mqx_int _bsp_esdhc_io_init
(
    uint32_t base,
    uint16_t value
)
{
    /* ESDHC.D1  */
    /* ESDHC.D0  */
    /* ESDHC.CLK */
    /* ESDHC.CMD */
    /* ESDHC.D3  */
    /* ESDHC.D2  */

    uint32_t sdhc_port_base = PORTE_BASE;

    if(value)
    {

      PORT_HAL_SetPullMode(sdhc_port_base, 0, kPortPullUp);
      PORT_HAL_SetPullMode(sdhc_port_base, 1, kPortPullUp);
      PORT_HAL_SetPullMode(sdhc_port_base, 3, kPortPullUp);
      PORT_HAL_SetPullMode(sdhc_port_base, 4, kPortPullUp);
      PORT_HAL_SetPullMode(sdhc_port_base, 5, kPortPullUp);

      PORT_HAL_SetPullCmd(sdhc_port_base, 0, true);
      PORT_HAL_SetPullCmd(sdhc_port_base, 1, true);
      PORT_HAL_SetPullCmd(sdhc_port_base, 3, true);
      PORT_HAL_SetPullCmd(sdhc_port_base, 4, true);
      PORT_HAL_SetPullCmd(sdhc_port_base, 5, true);

      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 0, kPortHighDriveStrength);
      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 1, kPortHighDriveStrength);
      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 2, kPortHighDriveStrength);
      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 3, kPortHighDriveStrength);
      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 4, kPortHighDriveStrength);
      PORT_HAL_SetDriveStrengthMode(sdhc_port_base, 5, kPortHighDriveStrength);

      PORT_HAL_SetMuxMode(sdhc_port_base, 0, kPortMuxAlt4);
      PORT_HAL_SetMuxMode(sdhc_port_base, 1, kPortMuxAlt4);
      PORT_HAL_SetMuxMode(sdhc_port_base, 2, kPortMuxAlt4);
      PORT_HAL_SetMuxMode(sdhc_port_base, 3, kPortMuxAlt4);
      PORT_HAL_SetMuxMode(sdhc_port_base, 4, kPortMuxAlt4);
      PORT_HAL_SetMuxMode(sdhc_port_base, 5, kPortMuxAlt4);


    }
    else
    {

      PORT_HAL_SetPullCmd(sdhc_port_base, 0, false);
      PORT_HAL_SetPullCmd(sdhc_port_base, 1, false);
      PORT_HAL_SetPullCmd(sdhc_port_base, 3, false);
      PORT_HAL_SetPullCmd(sdhc_port_base, 4, false);
      PORT_HAL_SetPullCmd(sdhc_port_base, 5, false);

      PORT_HAL_SetMuxMode(sdhc_port_base, 0, kPortPinDisabled);
      PORT_HAL_SetMuxMode(sdhc_port_base, 1, kPortPinDisabled);
      PORT_HAL_SetMuxMode(sdhc_port_base, 2, kPortPinDisabled);
      PORT_HAL_SetMuxMode(sdhc_port_base, 3, kPortPinDisabled);
      PORT_HAL_SetMuxMode(sdhc_port_base, 4, kPortPinDisabled);
      PORT_HAL_SetMuxMode(sdhc_port_base, 5, kPortPinDisabled);

    }

    /* Enable clock gate to SDHC module */
    SIM_HAL_EnableClock(SIM_BASE, kSimClockGateSdhc0);

    return MQX_OK;
}

#endif


/*!
 * \brief Find and set closest divider values for given baudrate.
 *
 * \param[in] esdhc_device_ptr Device runtime information
 * \param[in] baudrate Desired baudrate in Hz
 *
 * \return ESDHC_OK on success
 */
static int32_t _esdhc_set_baudrate(
    /* [IN] Device runtime information */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr,

    /* [IN] Desired baudrate in Hz */
    uint32_t baudrate)
{
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;
    uint32_t clock;
    uint32_t pres, div, min, minpres = 0x80, mindiv = 0x0F;
    int32_t val;

    // TODO: fix harcoded instance 0
    clock = CLOCK_SYS_GetSdhcFreq(0);


    /* Find closest setting */
    min = (uint32_t)-1;
    for (pres = 2; pres <= 256; pres <<= 1)
    {
        for (div = 1; div <= 16; div++)
        {
            val = pres * div * baudrate - clock;
            if (val >= 0)
            {
                if (min > val)
                {
                    min = val;
                    minpres = pres;
                    mindiv = div;
                }
            }
        }
    }

    /* Disable ESDHC clocks */
    SDHC_CLR_SYSCTL(esdhc_base, SDHC_SYSCTL_SDCLKEN_MASK);

    /* Change dividers */
    div = SDHC_RD_SYSCTL(esdhc_base) & (~(SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK));
    SDHC_WR_SYSCTL(esdhc_base, div | SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(minpres >> 1) | SDHC_SYSCTL_DVS(mindiv - 1));

    /* Wait for stable clock */
    while (0 == (SDHC_RD_PRSSTAT(esdhc_base) & SDHC_PRSSTAT_SDSTB_MASK))
    {
        _sched_yield();
    }

    /* Enable ESDHC clocks */
    SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_SDCLKEN_MASK);

    SDHC_SET_IRQSTAT(esdhc_base, SDHC_IRQSTAT_DTOE_MASK);


    return ESDHC_OK;
}


/*!
 * \brief Get current baudrate of ESDHC peripheral.
 *
 * \param[in] esdhc_device_ptr Device runtime information
 *
 * \return Current baudrate set in the peripheral in Hz
 */
static uint32_t _esdhc_get_baudrate(
    /* [IN] Device runtime information */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr)
{
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;
    uint32_t clock;
    int32_t div;

    CLOCK_SYS_GetFreq(kSystemClock, &clock);

    /* Get total divider */
    div = SDHC_RD_SYSCTL_SDCLKFS(esdhc_base) << 1;
    div *= SDHC_RD_SYSCTL_DVS(esdhc_base) + 1;

    return (clock / div);
}

/*!
 * \brief Checks whether write/read operation is in progress. (Device busy)
 *
 * \param[in] esdhc_base Module base pointer
 *
 * \return true if busy, false if ready
 */
static bool _esdhc_is_running(
    /* [IN/OUT] Module base pointer */
    uint32_t esdhc_base)
{
    return (0 != (SDHC_RD_PRSSTAT((SDHC_Type *)esdhc_base) & (SDHC_PRSSTAT_RTA_MASK | SDHC_PRSSTAT_WTA_MASK | SDHC_PRSSTAT_DLA_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_CIHB_MASK)));
}

/*!
 * \brief Internal callback of NIO, initialization of the device and allocation of device context
 *
 * \param[in] init_data ESDHC_INIT_STRUCT_CPTR init structure
 * \param[out] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int nio_esdhc_init(

    void *init_data, void **dev_context, int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;

    /* Create device context */
    esdhc_device_ptr = _mem_alloc_system_zero(sizeof(ESDHC_DEVICE_STRUCT));
    if (NULL == esdhc_device_ptr)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }
    _mem_set_type(esdhc_device_ptr, MEM_TYPE_IO_ESDHC_DEVICE_STRUCT);

    /* Copy initialization data for future usage */
    esdhc_device_ptr->DEV_BASE = ((ESDHC_INIT_STRUCT_CPTR)init_data)->DEV_BASE;
    esdhc_device_ptr->VECTOR = ((ESDHC_INIT_STRUCT_CPTR)init_data)->VECTOR;
    esdhc_device_ptr->MAX_BAUD_RATE = ((ESDHC_INIT_STRUCT_CPTR)init_data)->MAX_BAUD_RATE;

#if ESDHC_IS_HANDLING_CACHE
    esdhc_device_ptr->ADMA2_DATA = _mem_alloc_system(sizeof(ESDHC_ADMA2_DATA));
    if (NULL == esdhc_device_ptr->ADMA2_DATA)
    {
        _mem_free(esdhc_device_ptr);
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }
#endif
    if (_lwevent_create(&esdhc_device_ptr->LWEVENT, 0) != MQX_OK)
    {
#if ESDHC_IS_HANDLING_CACHE
        _mem_free(esdhc_device_ptr->ADMA2_DATA);
#endif
        _mem_free(esdhc_device_ptr);
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Enable clock gate to SDHC module */
    SIM_HAL_EnableClock(SIM, kSimClockGateSdhc0);

    /* SDK uses PLL clock */
    // CLOCK_SYS_SetSdhcSrc(0, kClockSdhcSrcPllFllSel);

    /* Install the interrupt service routine */
    _int_install_isr(esdhc_device_ptr->VECTOR, _esdhc_isr, esdhc_device_ptr);
    _nvic_int_init(esdhc_device_ptr->VECTOR, BSP_ESDHC_INT_LEVEL, FALSE);

    *dev_context = esdhc_device_ptr;

    return 0;
}

/*!
 * \brief Internal callback of NIO, deinitialization of esdhc and its device context
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int nio_esdhc_deinit(void *dev_context, int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;

    if (NULL == esdhc_device_ptr)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* This should not happen, nio assures deinit is called after
       all files are closed! */
    if (esdhc_device_ptr->COUNT)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Cleanup */
    _nvic_int_disable(esdhc_device_ptr->VECTOR);
    _int_install_isr(esdhc_device_ptr->VECTOR, _int_get_default_isr(), NULL);
    _lwevent_destroy(&esdhc_device_ptr->LWEVENT);
#if ESDHC_IS_HANDLING_CACHE
    _mem_free(esdhc_device_ptr->ADMA2_DATA);
#endif
    _mem_free((void *)esdhc_device_ptr);

    return 0;
}

/*!
 * \brief ESDHC registers initialization and card detection.
 *
 * \param[in] esdhc_device_ptr  Device runtime information
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int32_t nio_esdhc_init_module(
    /* [IN/OUT] Device runtime information */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr)
{
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

    esdhc_device_ptr->CARD = ESDHC_CARD_NONE;

    /* Reset ESDHC */
    SDHC_WR_SYSCTL(esdhc_base, SDHC_SYSCTL_RSTA(1) | SDHC_SYSCTL_SDCLKFS(0x80));

    while (SDHC_RD_SYSCTL(esdhc_base) & SDHC_SYSCTL_RSTA_MASK)
    {
        ;
    }

    /* Just to keep card detection working */
    SDHC_WR_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN_MASK);

    /* Initial values */
    SDHC_WR_VENDOR(esdhc_base, 0); /* Switch off the external DMA requests */

    SDHC_WR_BLKATTR(esdhc_base, SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(512)); /* Set any default size of data block */

#if PSP_ENDIAN == MQX_BIG_ENDIAN
    SDHC_WR_PROCTL(esdhc_base, SDHC_PROCTL_EMODE(0) | SDHC_PROCTL_D3CD(1) | SDHC_PROCTL_DMAS(2));
#else
    SDHC_WR_PROCTL(esdhc_base, SDHC_PROCTL_EMODE(2) | SDHC_PROCTL_D3CD(1) | SDHC_PROCTL_DMAS(2));
#endif

    SDHC_WR_WML(esdhc_base, SDHC_WML_RDWML(1) | SDHC_WML_WRWML(1)); /* To do: set up the right value */

    /* Set the ESDHC initial baud rate divider and start */
    _esdhc_set_baudrate(esdhc_device_ptr, ESDHC_INIT_BAUDRATE);

    /* Poll inhibit bits */
    while (SDHC_RD_PRSSTAT(esdhc_base) & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK))
    {
        ;
    }

    /* Enable requests */
    SDHC_WR_IRQSTAT(esdhc_base, 0xFFFF);

    SDHC_WR_IRQSTATEN(esdhc_base, /*DMA Error Status Enable*/ SDHC_IRQSTATEN_DMAESEN(1) | /*Auto CMD12 Error Status Enable*/ SDHC_IRQSTATEN_AC12ESEN(1) | /*Data End Bit Error Status Enable*/ SDHC_IRQSTATEN_DEBESEN(1) | /*Data CRC Error Status Enable*/ SDHC_IRQSTATEN_DCESEN(1) | /*Data Timeout Error Status Enable*/ SDHC_IRQSTATEN_DTOESEN(1) | /*Command Index Error Status Enable*/ SDHC_IRQSTATEN_CIESEN(1) | /*Command End Bit Error Status Enable*/ SDHC_IRQSTATEN_CEBESEN(1) | /*Command CRC Error Status Enable*/ SDHC_IRQSTATEN_CCESEN(1) | /*Command Timeout Error Status Enable*/ SDHC_IRQSTATEN_CTOESEN(1) | /*Card Interrupt Status Disable*/ SDHC_IRQSTATEN_CINTSEN(0) | /*Card Removal Status Enable*/ SDHC_IRQSTATEN_CRMSEN(1) | /*Card Insertion Status Enable*/ SDHC_IRQSTATEN_CINSEN(1) | /*Buffer Read Ready Status Disable*/ SDHC_IRQSTATEN_BRRSEN(0) | /*Buffer Write Ready Status Disable*/ SDHC_IRQSTATEN_BWRSEN(0) | /*DMA Interrupt Status Enable*/ SDHC_IRQSTATEN_DINTSEN(1) | /*Block Gap Event Status Enable*/ SDHC_IRQSTATEN_BGESEN(1) | /*Transfer Complete Status Enable*/ SDHC_IRQSTATEN_TCSEN(1) | /*Command Complete Status Enable*/ SDHC_IRQSTATEN_CCSEN(1));


    /* 80 initial clocks */
    SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_INITA_MASK);
    while (SDHC_RD_SYSCTL(esdhc_base) & SDHC_SYSCTL_INITA_MASK)
    {
        ;
    }

    /* Check card */
    if (SDHC_RD_IRQSTAT(esdhc_base) & SDHC_IRQSTAT_CINS_MASK)  //!(BR_SDHC_IRQSTAT_CINS(esdhc_base))
    {
        esdhc_device_ptr->CARD = ESDHC_CARD_UNKNOWN;
    }


#if ESDHC_CARD_DETECTION_SUPPORT
    SDHC_WR_IRQSIGEN(esdhc_base, /*DMA Error Status Enable*/ SDHC_IRQSIGEN_DMAEIEN(1) | /*Auto CMD12 Error Status Enable*/ SDHC_IRQSIGEN_AC12EIEN(1) | /*Data End Bit Error Status Enable*/ SDHC_IRQSIGEN_DEBEIEN(1) | /*Data CRC Error Status Enable*/ SDHC_IRQSIGEN_DCEIEN(1) | /*Data Timeout Error Status Enable*/ SDHC_IRQSIGEN_DTOEIEN(1) | /*Command Index Error Status Enable*/ SDHC_IRQSIGEN_CIEIEN(1) | /*Command End Bit Error Status Enable*/ SDHC_IRQSIGEN_CEBEIEN(1) | /*Command CRC Error Status Enable*/ SDHC_IRQSIGEN_CCEIEN(1) | /*Command Timeout Error Status Enable*/ SDHC_IRQSIGEN_CTOEIEN(1) | /*Card Interrupt Status Disable*/ SDHC_IRQSIGEN_CINTIEN(0) | /*Card Removal Status Enable*/ SDHC_IRQSIGEN_CRMIEN(1) | /*Card Insertion Status Enable*/ SDHC_IRQSIGEN_CINSIEN(1) | /*Buffer Read Ready Status Disable*/ SDHC_IRQSIGEN_BRRIEN(0) | /*Buffer Write Ready Status Disable*/ SDHC_IRQSIGEN_BWRIEN(0) | /*DMA Interrupt Status Enable*/ SDHC_IRQSIGEN_DINTIEN(1) | /*Block Gap Event Status Enable*/ SDHC_IRQSIGEN_BGEIEN(1) | /*Transfer Complete Status Enable*/ SDHC_IRQSIGEN_TCIEN(1) | /*Command Complete Status Enable*/ SDHC_IRQSIGEN_CCIEN(1));
#else
    SDHC_WR_IRQSIGEN(esdhc_base, /*DMA Error Status Enable*/ SDHC_IRQSIGEN_DMAEIEN(1) | /*Auto CMD12 Error Status Enable*/ SDHC_IRQSIGEN_AC12EIEN(1) | /*Data End Bit Error Status Enable*/ SDHC_IRQSIGEN_DEBEIEN(1) | /*Data CRC Error Status Enable*/ SDHC_IRQSIGEN_DCEIEN(1) | /*Data Timeout Error Status Enable*/ SDHC_IRQSIGEN_DTOEIEN(1) | /*Command Index Error Status Enable*/ SDHC_IRQSIGEN_CIEIEN(1) | /*Command End Bit Error Status Enable*/ SDHC_IRQSIGEN_CEBEIEN(1) | /*Command CRC Error Status Enable*/ SDHC_IRQSIGEN_CCEIEN(1) | /*Command Timeout Error Status Enable*/ SDHC_IRQSIGEN_CTOEIEN(1) | /*Card Interrupt Status Disable*/ SDHC_IRQSIGEN_CINTIEN(0) | /*Card Removal Status Disable*/ SDHC_IRQSIGEN_CRMIEN(0) | /*Card Insertion Status Disable*/ SDHC_IRQSIGEN_CINSIEN(0) | /*Buffer Read Ready Status Disable*/ SDHC_IRQSIGEN_BRRIEN(0) | /*Buffer Write Ready Status Disable*/ SDHC_IRQSIGEN_BWRIEN(0) | /*DMA Interrupt Status Enable*/ SDHC_IRQSIGEN_DINTIEN(1) | /*Block Gap Event Status Enable*/ SDHC_IRQSIGEN_BGEIEN(1) | /*Transfer Complete Status Enable*/ SDHC_IRQSIGEN_TCIEN(1) | /*Command Complete Status Enable*/ SDHC_IRQSIGEN_CCIEN(1));
#endif

    _nvic_int_init(esdhc_device_ptr->VECTOR, BSP_ESDHC_INT_LEVEL, TRUE);

    return ESDHC_OK;
}


/*!
 * \brief One ESDHC command transaction.
 *
 * \param[in] esdhc_device_ptr  Device runtime information
 * \param[in] command  Command specification
 * \param[in] adma2_desc_ptr  Source/Destination data address for data transfers
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int32_t _esdhc_send_command(
    /* [IN/OUT] SDHC device pointer */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr,

    /* [IN/OUT] Command specification */
    ESDHC_COMMAND_STRUCT_PTR command,

    /* [IN] Source/Destination data address for data transfers */
    ESDHC_ADMA2_DESC *adma2_desc_ptr)
{
    uint32_t xfertyp;
    uint32_t blkattr;
    uint32_t mask;
    uint32_t temp;
    MQX_TICK_STRUCT start_tick;
    MQX_TICK_STRUCT end_tick;
    bool overflow;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

    /* Create the command (xfertyp & blkattr) */

    /* Is it necessary to send the application command? */
    if (command->COMMAND & ESDHC_COMMAND_ACMD_FLAG)
    {
        /* Send the CMD55 to enable next command from application group ACMDxx */
        xfertyp = SDHC_XFERTYP_CMDINX(55) | SDHC_XFERTYP_CICEN(1) | SDHC_XFERTYP_CCCEN(1) | SDHC_XFERTYP_RSPTYP(2);

        /* Wait for cmd line idle */
        while (SDHC_RD_PRSSTAT(esdhc_base) & SDHC_PRSSTAT_CIHB_MASK)
        {
            ;
        }

        /* Setup command */
        SDHC_WR_CMDARG(esdhc_base, 0);
        SDHC_WR_BLKATTR(esdhc_base, 0);
        /* Clear the events and issue the command in a critical section*/
        _nvic_int_disable(esdhc_device_ptr->VECTOR);
        _lwevent_clear(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_DONE | ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_CMD_TIMEOUT));
        SDHC_WR_XFERTYP(esdhc_base, xfertyp);
        /* Exit critical section */
        _nvic_int_enable(esdhc_device_ptr->VECTOR);
        if (_lwevent_wait_ticks(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_DONE | ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_CMD_TIMEOUT), FALSE, ESDHC_CMD_TICK_TIMEOUT) != MQX_OK)
        {
            /* Something is wrong - timeout or any error of light weight event */
            return ESDHC_ERR;
        }
        else
        {
            mask = _lwevent_get_signalled();
            if (mask & (ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_CMD_TIMEOUT))
                return ESDHC_ERR;
        }
    }

    /* Set the command index */
    xfertyp = SDHC_XFERTYP_CMDINX(command->COMMAND & ESDHC_COMMAND_CMDIX_MASK);

    /* DPSEL shall always be set for resume type commands */
    if (((command->COMMAND & ESDHC_COMMAND_CMDTYPE_MASK) == ESDHC_COMMAND_TYPE_RESUME) || (command->COMMAND & ESDHC_COMMAND_DATACMD_FLAG))
        xfertyp |= SDHC_XFERTYP_DPSEL(1) | SDHC_XFERTYP_DMAEN(1);

    if ((0 != command->BLOCKS) && (0 != command->BLOCKSIZE))
    {
        xfertyp |= SDHC_XFERTYP_DPSEL(1) | SDHC_XFERTYP_DMAEN(1);
        if (command->BLOCKS != 1)
        {
            /* Multiple block transfer */
            xfertyp |= SDHC_XFERTYP_MSBSEL(1);

            if (command->BLOCKS > 1)
                xfertyp |= SDHC_XFERTYP_AC12EN(1) | SDHC_XFERTYP_BCEN(1);
        }
        if ((uint32_t)-1 == command->BLOCKS)
        {
            /* Infinite transfer */
            blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(0xFFFF);
            xfertyp |= SDHC_XFERTYP_BCEN(1);
        }
        else
        {
            blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(command->BLOCKS);
        }
    }
    else
    {
        blkattr = 0;
    }

    if ((xfertyp & SDHC_XFERTYP_DPSEL(1)) && (command->COMMAND & ESDHC_COMMAND_DATA_READ_FLAG))
        xfertyp |= SDHC_XFERTYP_DTDSEL(1);

    /* Set up response type */
    if (command->COMMAND & (ESDHC_COMMAND_RESPONSE_R1 | ESDHC_COMMAND_RESPONSE_R5 | ESDHC_COMMAND_RESPONSE_R6))
        xfertyp |= SDHC_XFERTYP_CICEN(1) | SDHC_XFERTYP_CCCEN(1) | SDHC_XFERTYP_RSPTYP(2);
    else if (command->COMMAND & ESDHC_COMMAND_RESPONSE_R2)
        xfertyp |= SDHC_XFERTYP_CCCEN(1) | SDHC_XFERTYP_RSPTYP(1);
    else if (command->COMMAND & (ESDHC_COMMAND_RESPONSE_R3 | ESDHC_COMMAND_RESPONSE_R4))
        xfertyp |= SDHC_XFERTYP_RSPTYP(2);
    else if (command->COMMAND & (ESDHC_COMMAND_RESPONSE_R1b | ESDHC_COMMAND_RESPONSE_R5b))
        xfertyp |= SDHC_XFERTYP_CICEN(1) | SDHC_XFERTYP_CCCEN(1) | SDHC_XFERTYP_RSPTYP(3);

    /* Set up the command type */
    xfertyp |= SDHC_XFERTYP_CMDTYP((command->COMMAND & ESDHC_COMMAND_CMDTYPE_MASK) >> ESDHC_COMMAND_CMDTYPE_SHIFT);

    /* Wait for cmd line idle */
    _time_get_ticks(&start_tick);

    while (SDHC_RD_PRSSTAT(esdhc_base) & SDHC_PRSSTAT_CIHB_MASK)
    {
        _time_get_ticks(&end_tick);

        if (ESDHC_TRANSFER_TIMEOUT_MS < _time_diff_milliseconds(&end_tick, &start_tick, &overflow))
        {
            return ESDHC_ERR;
        }

        /* Give chance to other tasks */
        _sched_yield();
    }


    /*
    The host driver can issue CMD0, CMD12, CMD13 (for
    memory) and CMD52 (for SDIO) when the DAT lines are busy
    during a data transfer. These commands can be issued when
    Command Inhibit (CIHB) is set to zero. Other commands shall
    be issued when Command Inhibit (CDIHB) is set to zero.
    Possible changes to the SD Physical Specification may add
    other commands to this list in the future.
    */

    /* Wait for data line idle if neccesary */
    temp = (command->COMMAND & ESDHC_COMMAND_CMDIX_MASK);
    if ((temp != 0) && (temp != 12) && (temp != 13) && (temp != 52))
    {
        _time_get_ticks(&start_tick);

        while (SDHC_RD_PRSSTAT(esdhc_base) & SDHC_PRSSTAT_CDIHB_MASK)
        {
            _time_get_ticks(&end_tick);

            if (ESDHC_TRANSFER_TIMEOUT_MS < _time_diff_milliseconds(&end_tick, &start_tick, &overflow))
            {
                return ESDHC_ERR;
            }

            /* Enable clock to SD card to allow it to finish the operation */
            SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN(1) | SDHC_SYSCTL_HCKEN(1) | SDHC_SYSCTL_IPGEN(1));

            /* Let other tasks to do their job meanwhile */
            _sched_yield();
        }
    }

#if ESDHC_AUTO_CLK_GATING
    /*
    If a response with busy is expected or auto command CMD12 (also expects busy with response)
    then force the clock the SD card to allow card finish the operation after the transfer
    otherwise use automatic clock gating.
    */
    if (((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) == SDHC_XFERTYP_RSPTYP(3)) || (xfertyp & SDHC_XFERTYP_AC12EN_MASK))
    {
        SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN(1) | SDHC_SYSCTL_HCKEN(1) | SDHC_SYSCTL_IPGEN(1));
    }
    else
    {
        SDHC_CLR_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN(1) | SDHC_SYSCTL_HCKEN(1) | SDHC_SYSCTL_IPGEN(1));
    }
#endif

    /* Setup command */
    SDHC_WR_DSADDR(esdhc_base, 0);
    SDHC_WR_ADSADDR(esdhc_base, (uint32_t)adma2_desc_ptr);
    SDHC_WR_BLKATTR(esdhc_base, blkattr);
    SDHC_WR_CMDARG(esdhc_base, command->ARGUMENT);

    /* Clear the events and do the command in critical section*/
    _nvic_int_disable(esdhc_device_ptr->VECTOR);

    _lwevent_clear(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_DONE | ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_CMD_TIMEOUT));

    /* Issue command */
    SDHC_WR_XFERTYP(esdhc_base, xfertyp);

    /* Exit critical section */
    _nvic_int_enable(esdhc_device_ptr->VECTOR);

    /* When multiple block write is stopped by CMD12, the busy from the response of CMD12 is up to 500ms. */
    if ((command->COMMAND & ESDHC_COMMAND_CMDIX_MASK) == 12)
        temp = ESDHC_CMD12_TICK_TIMEOUT;
    else
        temp = ESDHC_CMD_TICK_TIMEOUT;

    if (_lwevent_wait_ticks(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_DONE | ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_CMD_TIMEOUT), FALSE, temp) != MQX_OK)
    {
        /* Something is wrong - timeout or any error of light weight event */
        return ESDHC_ERR;
    }
    else
    {
        mask = _lwevent_get_signalled();

        if (mask & ESDHC_LWEVENT_CMD_ERROR)
        {
            return ESDHC_ERROR_COMMAND_FAILED;
        }
        else if (mask & ESDHC_LWEVENT_CMD_TIMEOUT)
        {
            return ESDHC_ERROR_COMMAND_TIMEOUT;
        }
    }

    if (command->COMMAND & ESDHC_COMMAND_CMDRESPONSE_MASK)
    {
        command->RESPONSE[0] = SDHC_RD_CMDRSP(esdhc_base, 0);
        if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) == SDHC_XFERTYP_RSPTYP(1))
        {
            command->RESPONSE[1] = SDHC_RD_CMDRSP(esdhc_base, 1);
            command->RESPONSE[2] = SDHC_RD_CMDRSP(esdhc_base, 2);
            command->RESPONSE[3] = SDHC_RD_CMDRSP(esdhc_base, 3);
        }
    }

    return ESDHC_OK;
}


/*!
 * \brief Wait for the card to finish the operation
 *
 * \param[in] esdhc_device_ptr  Device runtime information
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int32_t _esdhc_wait_while_busy(
    /* [IN/OUT] SDHC device pointer */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr)
{
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;
    int result = ESDHC_OK;

    MQX_TICK_STRUCT start_tick, end_tick;
    bool overflow;

    _time_get_ticks(&start_tick);

    /* while card busy, errata e4624: test DLSL[0] instead of DLA */
    while (!(SDHC_RD_PRSSTAT_DLSL(esdhc_base) & 0x1))
    {
        if (!(SDHC_RD_PRSSTAT(esdhc_base) & SDHC_PRSSTAT_CINS_MASK))
        {
            /* card removed */
            break;
        }

        _time_get_ticks(&end_tick);
        if (ESDHC_TRANSFER_TIMEOUT_MS < _time_diff_milliseconds(&end_tick, &start_tick, &overflow))
        {
            result = ESDHC_ERR;
            break;
        }

        /* Enable clock to SD card to allow it to finish the operation */
        SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN(1) | SDHC_SYSCTL_HCKEN(1) | SDHC_SYSCTL_IPGEN(1));

        /* Let other tasks to do their job meanwhile */
        _sched_yield();
    }
    
#if ESDHC_AUTO_CLK_GATING    
    /* Switch the clock settings back to the automatic clock gating mode */
    SDHC_CLR_SYSCTL(esdhc_base, SDHC_SYSCTL_PEREN(1) | SDHC_SYSCTL_HCKEN(1) | SDHC_SYSCTL_IPGEN(1));
#endif

    return result;
}


/*!
 * \brief Function fills in the CARD in the device description
 *
 * \param[out] esdhc_device_ptr  Device runtime information
 *
 * \return ESDHC_OK on success, ESDHC_ERROR_x on failure
 */
static _mqx_int _esdhc_get_card_type(
    /* [IN] The I/O init data pointer */
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr)
{
    uint32_t card_type;
    int32_t val;
    ESDHC_COMMAND_STRUCT command;

    card_type = 0;

#define CARD_MEM_MASK 0x01
#define CARD_IO_MASK 0x02
#define CARD_MMC_MASK 0x04
#define CARD_CEATA_MASK 0x08
#define CARD_HC_MASK 0x10
#define CARD_MP_MASK 0x20

#define GET_CARD_TIMEOUT (100)

    /* CMD0 - Go to idle - reset card */
    command.COMMAND = ESDHC_CREATE_CMD(0, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_NO, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = 0;
    command.BLOCKS = 0;
    if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
    {
        return ESDHC_ERROR_INIT_FAILED;
    }

    /* CMD8 - Send interface condition - check HC support */
    command.COMMAND = ESDHC_CREATE_CMD(8, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = 0x000001AA;  // Voltage supply 2.7-3.6V 0x100 + 0xAA check pattern
    command.BLOCKS = 0;

    val = _esdhc_send_command(esdhc_device_ptr, &command, NULL);

    if (val == 0)
    {
        if (command.RESPONSE[0] != command.ARGUMENT)
        {
            return ESDHC_ERROR_INIT_FAILED;
        }
        card_type |= CARD_HC_MASK;
    }
    else if (val != ESDHC_ERROR_COMMAND_TIMEOUT)  // This command should not be supported by old card
    {
        return ESDHC_ERROR_INIT_FAILED;
    }


    /* CMD5 - Send operating conditions - test IO */
    command.COMMAND = ESDHC_CREATE_CMD(5, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R4, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = 0;
    command.BLOCKS = 0;
    val = _esdhc_send_command(esdhc_device_ptr, &command, NULL);

    if (val == 0)
    {
        if (((command.RESPONSE[0] >> 28) & 0x07) && (command.RESPONSE[0] & 0x300000))
        {
            /* CMD5 - Send operating conditions - init IO */
            command.COMMAND = ESDHC_CREATE_CMD(5, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R4, ESDHC_COMMAND_NONE_FLAG);
            command.ARGUMENT = 0x300000;
            command.BLOCKS = 0;
            val = 0;
            do
            {
                _time_delay(1);
                val++;
                if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
                {
                    return ESDHC_ERROR_INIT_FAILED;
                }

            } while ((0 == (command.RESPONSE[0] & 0x88000000)) && (val < GET_CARD_TIMEOUT));  // todo 0x80000000 or 0x88000000

            if (command.RESPONSE[0] & 0x80000000)
            {
                card_type |= CARD_IO_MASK;
            }
            if (command.RESPONSE[0] & 0x08000000)
            {
                card_type |= CARD_MP_MASK;
            }
        }
    }
    else
    {
        card_type |= CARD_MP_MASK;
    }

    if (card_type | CARD_MP_MASK)
    {
        /* CMD55 - Application specific command - check MMC */
        command.COMMAND = ESDHC_CREATE_CMD(55, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
        command.ARGUMENT = 0;
        command.BLOCKS = 0;
        val = _esdhc_send_command(esdhc_device_ptr, &command, NULL);
        if (val > 0)
        {
            return ESDHC_ERROR_INIT_FAILED;
        }

        if (val < 0)
        {
            /* MMC or CE-ATA */
            card_type &= ~(CARD_IO_MASK | CARD_MEM_MASK | CARD_HC_MASK);

            /* CMD1 - Send operating conditions - check HC */
            command.COMMAND = ESDHC_CREATE_CMD(1, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_NO, ESDHC_COMMAND_NONE_FLAG);
            command.ARGUMENT = 0x40300000;
            command.BLOCKS = 0;
            if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
            {
                return ESDHC_ERROR_INIT_FAILED;
            }
            if (0x20000000 == (command.RESPONSE[0] & 0x60000000))
            {
                card_type |= CARD_HC_MASK;
            }
            card_type |= CARD_MMC_MASK;

            /* CMD39 - Fast IO - check CE-ATA signature CE */
            command.COMMAND = ESDHC_CREATE_CMD(39, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R4, ESDHC_COMMAND_NONE_FLAG);
            command.ARGUMENT = 0x0C00;
            command.BLOCKS = 0;
            if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
            {
                return ESDHC_ERROR_INIT_FAILED;
            }

            if (0xCE == ((command.RESPONSE[0] >> 8) & 0xFF))
            {
                /* CMD39 - Fast IO - check CE-ATA signature AA */
                command.COMMAND = ESDHC_CREATE_CMD(39, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R4, ESDHC_COMMAND_NONE_FLAG);
                command.ARGUMENT = 0x0D00;
                command.BLOCKS = 0;
                if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
                {
                    return ESDHC_ERROR_INIT_FAILED;
                }
                if (0xAA == ((command.RESPONSE[0] >> 8) & 0xFF))
                {
                    card_type &= ~CARD_MMC_MASK;
                    card_type |= CARD_CEATA_MASK;
                }
            }
        }
        else
        {
            /* SD */
            /* ACMD41 - Send Operating Conditions */
            command.COMMAND = ESDHC_CREATE_CMD(41, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R3, 0 /*SDHC_COMMAND_ACMD_FLAG*/);
            command.ARGUMENT = 0;
            command.BLOCKS = 0;
            if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
            {
                return ESDHC_ERROR_INIT_FAILED;
            }
            if (command.RESPONSE[0] & 0x300000)
            {
                val = 0;
                do
                {
                    _time_delay(1);
                    val++;

                    command.COMMAND = ESDHC_CREATE_CMD(41, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R3, ESDHC_COMMAND_ACMD_FLAG);
                    if (card_type | CARD_HC_MASK)
                    {
                        command.ARGUMENT = 0x40300000;
                    }
                    else
                    {
                        command.ARGUMENT = 0x00300000;
                    }
                    command.BLOCKS = 0;
                    if (_esdhc_send_command(esdhc_device_ptr, &command, NULL))
                    {
                        return ESDHC_ERROR_INIT_FAILED;
                    }
                } while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < GET_CARD_TIMEOUT));

                if (val >= GET_CARD_TIMEOUT)
                {
                    card_type &= ~CARD_HC_MASK;
                }
                else
                {
                    card_type |= CARD_MEM_MASK;

                    if (!(command.RESPONSE[0] & 0x40000000))
                    {
                        card_type &= ~CARD_HC_MASK;
                    }
                }
            }
        }
    }

    switch (card_type & ~CARD_MP_MASK)
    {
        case CARD_MMC_MASK:
            esdhc_device_ptr->CARD = ESDHC_CARD_MMC;
            break;

        case CARD_CEATA_MASK:
            esdhc_device_ptr->CARD = ESDHC_CARD_CEATA;
            break;

        case CARD_MEM_MASK:
            esdhc_device_ptr->CARD = ESDHC_CARD_SD;
            break;

        case (CARD_MEM_MASK | CARD_HC_MASK):
            esdhc_device_ptr->CARD = ESDHC_CARD_SDHC;
            break;

        case CARD_IO_MASK:
            esdhc_device_ptr->CARD = ESDHC_CARD_SDIO;
            break;

        case (CARD_MEM_MASK | CARD_IO_MASK):
            esdhc_device_ptr->CARD = ESDHC_CARD_SDCOMBO;
            break;

        case (CARD_MEM_MASK | CARD_IO_MASK | CARD_HC_MASK):
            esdhc_device_ptr->CARD = ESDHC_CARD_SDHCCOMBO;
            break;

        default:
            esdhc_device_ptr->CARD = ESDHC_CARD_UNKNOWN;
            break;
    }

    return ESDHC_OK;
}

/*!
 * \brief Internal callback of NIO, This function opens the ESDHC device.
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 * \param[in] dev_name  Name of the device
 * \param[in] flags  The flags to be used during operation
 * \param[out] fp_context  The file context, unused by this driver
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int nio_esdhc_open(
    /* Device context */
    void *dev_context,

    /* Name of the device */
    const char *dev_name,

    /* The flags to be used during operation */
    int flags,

    /* The file context, unused in this driver */
    void **fp_context,

    int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;

    /* Exclusive access till close */
    _int_disable();
    if (esdhc_device_ptr->COUNT)
    {
        /* Device is already opened */
        _int_enable();
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }
    esdhc_device_ptr->COUNT++;
    esdhc_device_ptr->FLAGS = flags;
    _int_enable();

    if (nio_esdhc_init_module(esdhc_device_ptr))
    {
        _int_disable();
        esdhc_device_ptr->COUNT--;
        _int_enable();
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    return 0;
}


/*!
 * \brief Internal callback of NIO, This function closes an opened ESDHC device.
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 * \param[in] fp_context  The file context, unused by this driver
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int nio_esdhc_close(
    /* Device context */
    void *dev_context,

    /* File context*/
    void *fp_context,

    int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

    /* Disable ESDHC device */
    _int_disable();
    if (--esdhc_device_ptr->COUNT == 0)
    {
        SDHC_WR_SYSCTL(esdhc_base, SDHC_SYSCTL_RSTA(1) | SDHC_SYSCTL_SDCLKFS(0x80));
        while (SDHC_RD_SYSCTL(esdhc_base) & SDHC_SYSCTL_RSTA_MASK)
        {
            ;
        }
    }
    _int_enable();

    return 0;
}


/*!
 * \brief Internal callback of NIO. This function performs miscellaneous services for the ESDHC I/O device.
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[in] cmd  The command to perform
 * \param[out] ap  Parameters for the command
 *
 * \return ESDHC_OK on success, ESDHC_ERR on error
 */
static int nio_esdhc_ioctl(
    /* Device context */
    void *dev_context,

    /* File context*/
    void *fp_context,

    /* Error value to be filled */
    int *error,

    /* The command to perform */
    unsigned long int cmd,

    /* Parameters for the command */
    va_list ap)
{
    int32_t val;
    int result = 0;
    uint32_t *param32_ptr;
    void *param_ptr;

    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

    param_ptr = va_arg(ap, void *);
    param32_ptr = param_ptr;

    switch (cmd)
    {
        case IO_IOCTL_ESDHC_INIT:
            result = nio_esdhc_init_module(esdhc_device_ptr);
            break;

        case IO_IOCTL_ESDHC_SEND_COMMAND:
            /* Check if this is data transfer command to store it and send it as part of the read/write operation (when buffer address is known) */
            if (((ESDHC_COMMAND_STRUCT_PTR)param32_ptr)->COMMAND & ESDHC_COMMAND_DATACMD_FLAG)
            {
                esdhc_device_ptr->BUFFERED_CMD = *((ESDHC_COMMAND_STRUCT_PTR)param32_ptr);
            }
            else
            {
                esdhc_device_ptr->BUFFERED_CMD.COMMAND = 0;

                val = _esdhc_send_command(esdhc_device_ptr, (ESDHC_COMMAND_STRUCT_PTR)param32_ptr, NULL);
                if (val > 0)
                {
                    result = ESDHC_ERROR_COMMAND_FAILED;
                }
                if (val < 0)
                {
                    result = ESDHC_ERROR_COMMAND_TIMEOUT;
                }
            }
            break;

        case IO_IOCTL_ESDHC_GET_BAUDRATE:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                /* Get actual baudrate */
                *param32_ptr = _esdhc_get_baudrate(esdhc_device_ptr);
            }
            break;

        case IO_IOCTL_ESDHC_GET_BAUDRATE_MAX:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                if (SDHC_RD_HTCAPBLT(esdhc_base) & SDHC_HTCAPBLT_HSS_MASK)  //!(BR_SDHC_HTCAPBLT_HSS(esdhc_base))
                    val = ESDHC_MAX_BAUDRATE_50MHz;
                else
                    val = ESDHC_MAX_BAUDRATE_25MHz;

                if ((esdhc_device_ptr->MAX_BAUD_RATE == 0) || (esdhc_device_ptr->MAX_BAUD_RATE > val))
                    *param32_ptr = val;
                else
                    *param32_ptr = esdhc_device_ptr->MAX_BAUD_RATE;
            }
            break;

        case IO_IOCTL_ESDHC_SET_BAUDRATE:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else if (0 == (*param32_ptr))
            {
                result = ESDHC_ERR;
            }
            else
            {
                if (!_esdhc_is_running((uint32_t)esdhc_base))
                {
                    _esdhc_set_baudrate(esdhc_device_ptr, *param32_ptr);
                }
                else
                {
                    result = ESDHC_ERR;
                }
            }
            break;

        case IO_IOCTL_ESDHC_GET_BUS_WIDTH:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                /* Get actual ESDHC bus width */
                val = SDHC_RD_PROCTL_DTW(esdhc_base);
                if (/*ESDHC_PROCTL_DTW_1BIT*/ 0 == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_1BIT;
                }
                else if (/*ESDHC_PROCTL_DTW_4BIT*/ 1 == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_4BIT;
                }
                else if (/*ESDHC_PROCTL_DTW_8BIT*/ 2 == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_8BIT;
                }
                else
                {
                    result = ESDHC_ERROR_INVALID_BUS_WIDTH;
                }
            }
            break;

        case IO_IOCTL_ESDHC_SET_BUS_WIDTH:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                /* Set actual ESDHC bus width */
                if (!_esdhc_is_running((uint32_t)esdhc_base))
                {
                    if (ESDHC_BUS_WIDTH_1BIT == *param32_ptr)
                    {
                        SDHC_WR_PROCTL_DTW(esdhc_base, 0 /*ESDHC_PROCTL_DTW_1BIT*/);
                    }
                    else if (ESDHC_BUS_WIDTH_4BIT == *param32_ptr)
                    {
                        SDHC_WR_PROCTL_DTW(esdhc_base, 1 /*ESDHC_PROCTL_DTW_4BIT*/);
                    }
                    else if (ESDHC_BUS_WIDTH_8BIT == *param32_ptr)
                    {
                        SDHC_WR_PROCTL_DTW(esdhc_base, 2 /*ESDHC_PROCTL_DTW_8BIT*/);
                    }
                    else
                    {
                        result = ESDHC_ERROR_INVALID_BUS_WIDTH;
                    }
                }
                else
                {
                    result = ESDHC_ERR;
                }
            }
            break;

        case IO_IOCTL_ESDHC_GET_CARD:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                /* 80 clocks to update levels */
                SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_INITA(1));  //! BW_SDHC_SYSCTL_INITA(esdhc_base, 1);
                while (SDHC_RD_SYSCTL(esdhc_base) & SDHC_SYSCTL_INITA_MASK)  //! (BR_SDHC_SYSCTL_INITA(esdhc_base))
                {
                    ;
                }

                /* Update and return actual card status */
                if (SDHC_RD_IRQSTAT(esdhc_base) & SDHC_IRQSTAT_CRM_MASK)  //! (BR_SDHC_IRQSTAT_CRM(esdhc_base))
                {
                    SDHC_SET_IRQSTAT(esdhc_base, SDHC_IRQSTAT_CRM(1));  //! BW_SDHC_IRQSTAT_CRM(esdhc_base, 1);
                    esdhc_device_ptr->CARD = ESDHC_CARD_NONE;
                }

                if (SDHC_RD_IRQSTAT(esdhc_base) & SDHC_IRQSTAT_CINS_MASK)  //! (BR_SDHC_IRQSTAT_CINS(esdhc_base))
                {
                    if ((ESDHC_CARD_NONE == esdhc_device_ptr->CARD) || (ESDHC_CARD_UNKNOWN == esdhc_device_ptr->CARD))
                    {
                        esdhc_device_ptr->CARD = ESDHC_CARD_UNKNOWN;

                        /* Backup the current baudrate */
                        val = _esdhc_get_baudrate(esdhc_device_ptr);

                        /* Set up slow init baudrate */
                        if (ESDHC_OK != (result = _esdhc_set_baudrate(esdhc_device_ptr, ESDHC_INIT_BAUDRATE)))
                        {
                            break;
                        }

                        /* Recognize inserted card */
                        _esdhc_get_card_type(esdhc_device_ptr);

                        /* Restore original baudrate */
                        if (ESDHC_OK != (result = _esdhc_set_baudrate(esdhc_device_ptr, val)))
                        {
                            break;
                        }
                    }
                }
                else
                {
                    esdhc_device_ptr->CARD = ESDHC_CARD_NONE;
                }

                *param32_ptr = esdhc_device_ptr->CARD;
            }
            break;

        case IO_IOCTL_DEVICE_IDENTIFY:
            /* Get ESDHC device parameters */
            param32_ptr[IO_IOCTL_ID_PHY_ELEMENT] = IO_DEV_TYPE_PHYS_ESDHC;
            param32_ptr[IO_IOCTL_ID_LOG_ELEMENT] = IO_DEV_TYPE_LOGICAL_MFS;
            param32_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_ESDHC_ATTRIBS;
            if ((esdhc_device_ptr->FLAGS & O_ACCMODE) == O_RDONLY)
            {
                param32_ptr[IO_IOCTL_ID_ATTR_ELEMENT] &= (~IO_DEV_ATTR_WRITE);
            }
            break;

        case IO_IOCTL_FLUSH_OUTPUT:
            result = _esdhc_wait_while_busy(esdhc_device_ptr);
            break;

        case IO_IOCTL_ESDHC_SET_IO_CALLBACK:
            if (NULL == param_ptr || ((ESDHC_IO_INT_CALLBACK_STRUCT *)param_ptr)->CALLBACK == NULL)
            {
                /* Disable and clear pending IRQ */
                SDHC_CLR_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CINTIEN_MASK);  //! BW_SDHC_IRQSIGEN_CINTIEN(esdhc_base, 0);
                SDHC_SET_IRQSTAT(esdhc_base, SDHC_IRQSTAT_CINT(1));  //! BW_SDHC_IRQSTAT_CINT(esdhc_base, 1);

                /* Unregister callback */
                esdhc_device_ptr->IO_CALLBACK_STR.CALLBACK = NULL;
                esdhc_device_ptr->IO_CALLBACK_STR.USERDATA = NULL;
            }
            else
            {
                /* Register callback and enable IO card interrupt */
                esdhc_device_ptr->IO_CALLBACK_STR = *((ESDHC_IO_INT_CALLBACK_STRUCT *)param32_ptr);

                /* Clear pending IRQ and enable it */
                SDHC_SET_IRQSTAT(esdhc_base, SDHC_IRQSTAT_CINT(1));  //! BW_SDHC_IRQSTAT_CINT(esdhc_base, 1);
                SDHC_SET_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CINTIEN(1));  //! BW_SDHC_IRQSIGEN_CINTIEN(esdhc_base, 1);
            }
            break;

#if ESDHC_CARD_DETECTION_SUPPORT
        case IO_IOCTL_ESDHC_SET_CARD_CALLBACK:
            if (NULL == param_ptr)
            {
                esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.CALLBACK = NULL;
                esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.USERDATA = NULL;
            }
            else
            {
                esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR = *((ESDHC_CARD_PRESENCE_CALLBACK_STRUCT_PTR)param_ptr);
            }
            break;
#endif

        case IO_IOCTL_ESDHC_GET_CARD_PRESENCE:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
            }
            else
            {
                if (SDHC_RD_IRQSTAT(esdhc_base) & SDHC_IRQSTAT_CINS_MASK)  //!(BR_SDHC_IRQSTAT_CINS(esdhc_base))
                    *param32_ptr = TRUE;
                else
                    *param32_ptr = FALSE;
            }
            break;

        case IO_IOCTL_GET_REQ_ALIGNMENT:
            if (NULL == param32_ptr)
            {
                result = ESDHC_ERR;
                break;
            }
            *param32_ptr = 4; /* 32-bit word alignment required for ADMA2 operation */
            break;

        default:
            result = ESDHC_ERR;
            break;
    }

    if (result)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    return 0;
}

/*!
 * \brief Internal callback of NIO. Reads the data into provided buffer.
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[out] data_ptr  Where the characters are to be stored
 * \param[in] n  The number of bytes to read
 *
 * \return Returns the number of bytes received on success, ESDHC_ERR on error (negative)
 */
static int nio_esdhc_read(
    /* Device context */
    void *dev_context,

    /* File context */
    void *fp_context,

    /* [OUT] Where the characters are to be stored */
    void *data_ptr,

    /* [IN] The number of bytes to read */
    size_t n,

    int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

#if ESDHC_IS_HANDLING_CACHE
    ESDHC_ADMA2_DESC *adma2_desc = esdhc_device_ptr->ADMA2_DATA->DESC;
    int adma2_desc_idx;

    uint8_t *head;
    uint32_t head_len;

    uint8_t *body;
    uint32_t body_len;

    uint8_t *tail;
    uint32_t tail_len;
#else
    ESDHC_ADMA2_DESC adma2_desc[1];
#endif

    /* Check if the buffered command is ready */
    if ((esdhc_device_ptr->BUFFERED_CMD.COMMAND & (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG)) != (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG))
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Infinite transfers are not supported */
    if (esdhc_device_ptr->BUFFERED_CMD.BLOCKS == ((uint32_t)-1))
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Check if the transfer fits into the buffer (size block * count of size must be <= n) */
    if (esdhc_device_ptr->BUFFERED_CMD.BLOCKS * esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE > n)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Zero length transfer is a no-op */
    if (n == 0)
    {
        return 0;
    }

    /* Check max transfer size */
    if (n >= 0x10000)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Check buffer alignment */
    if (((uint32_t)data_ptr) & 0x03)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

#if ESDHC_IS_HANDLING_CACHE

    /* Prepare the section lengths */
    head_len = (PSP_MEMORY_ALIGNMENT + 1) - ((uint32_t)data_ptr & PSP_MEMORY_ALIGNMENT);
    body_len = (n - head_len) & ~((uint32_t)PSP_MEMORY_ALIGNMENT);
    tail_len = (n - head_len - body_len);

    adma2_desc_idx = -1; /* Points to last valid descriptor, which is none at the moment */
    if (head_len)
    {
        head = esdhc_device_ptr->ADMA2_DATA->HEAD_BUF;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (head_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)head;
    }
    if (body_len)
    {
        body = ((uint8_t *)data_ptr) + head_len;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (body_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)body;
    }
    if (tail_len)
    {
        tail = esdhc_device_ptr->ADMA2_DATA->TAIL_BUF;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (tail_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)tail;
    }
    adma2_desc[adma2_desc_idx].LEN_ATTR |= ESDHC_ADMA2_FLAG_END;
    _DCACHE_FLUSH_MBYTES(adma2_desc, 3 * sizeof(ESDHC_ADMA2_DESC));

#else
    adma2_desc[0].LEN_ATTR = (n << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID | ESDHC_ADMA2_FLAG_END;
    adma2_desc[0].DATA_ADDR = (uint32_t)data_ptr;
#endif

    _lwevent_clear(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_TRANSFER_DONE | ESDHC_LWEVENT_TRANSFER_ERROR));

    if (_esdhc_send_command(esdhc_device_ptr, &(esdhc_device_ptr->BUFFERED_CMD), adma2_desc) != MQX_OK)
    {
        esdhc_device_ptr->BUFFERED_CMD.COMMAND = 0;
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Wait for transfer to finish. Timeout depends on number of blocks. */
    if (_lwevent_wait_ticks(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_TRANSFER_DONE | ESDHC_LWEVENT_TRANSFER_ERROR), FALSE, ESDHC_CMD12_TICK_TIMEOUT) != MQX_OK)
    {
        /* Something is wrong - timeout or any error of light weight event */
        if (error)
        {
            *error = NIO_ETIMEDOUT;
        }
        return -1;
    }
    else
    {
        if (_lwevent_get_signalled() & ESDHC_LWEVENT_TRANSFER_ERROR)
        {
            if (esdhc_device_ptr->BUFFERED_CMD.BLOCKS > 1)
            {
                /* In this case the peripheral doesn't automatically send the CMD12, so MUST be sent manually */
                ESDHC_COMMAND_STRUCT command;

                // FIXME: This is probably wrong; Correctly compute the count of blocks actually read
                int32_t count = (int32_t)((uint32_t)SDHC_RD_ADSADDR(esdhc_base) - (uint32_t)data_ptr);
                // int32_t count = (esdhc_ptr->DSADDR & (esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE - 1) - esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE -
                // (uint32_t)data_ptr);

                command.COMMAND = ESDHC_CREATE_CMD(12, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_NONE_FLAG));
                command.ARGUMENT = 0;
                command.BLOCKS = 0;

                if (_esdhc_send_command(esdhc_device_ptr, &command, NULL) != MQX_OK)
                {
                    if (error)
                    {
                        *error = ESDHC_ERR;
                    }
                    return -1;
                }

                SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_RSTD(1));  //!BW_SDHC_SYSCTL_RSTD(esdhc_base, 1);
                return count;
            }
            else
            {
                SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_RSTD(1));  //!BW_SDHC_SYSCTL_RSTD(esdhc_base, 1);
                return 0;
            }
        }
    }

#if ESDHC_IS_HANDLING_CACHE
    if (head_len)
    {
        _DCACHE_INVALIDATE_LINE(head);
        _mem_copy(head, data_ptr, head_len);
    }

    if (body_len)
    {
        _DCACHE_INVALIDATE_MBYTES(body, body_len);
    }

    if (tail_len)
    {
        _DCACHE_INVALIDATE_LINE(tail);
        _mem_copy(tail, ((uint8_t *)data_ptr) + head_len + body_len, tail_len);
    }
#endif

    return (esdhc_device_ptr->BUFFERED_CMD.BLOCKS * esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE);
}

/*!
 * \brief Internal callback of NIO. Writes the provided data buffer to the device.
 *
 * \param[in] dev_context  ESDHC_DEVICE_STRUCT containing esdhc context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[in] data_ptr  Where the characters are to be taken from
 * \param[in] n  The number of bytes to write
 *
 * \return Returns number of bytes transmitted on success, ESDHC_ERR on error (negative)
 */
static int nio_esdhc_write(
    /* Device context */
    void *dev_context,

    /* File context */
    void *fp_context,

    /* [OUT] Where the characters are to be taken from */
    const void *data_ptr,

    /* [IN] The number of bytes to write */
    size_t n,

    int *error)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)dev_context;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;

#if ESDHC_IS_HANDLING_CACHE
    ESDHC_ADMA2_DESC *adma2_desc = esdhc_device_ptr->ADMA2_DATA->DESC;
    int adma2_desc_idx;

    uint8_t *head;
    uint32_t head_len;

    uint8_t *body;
    uint32_t body_len;

    uint8_t *tail;
    uint32_t tail_len;
#else
    ESDHC_ADMA2_DESC adma2_desc[1];
#endif

    /* Check if flush is requested */
    if ((data_ptr == NULL) && (n == 0))
    {
        return _esdhc_wait_while_busy(esdhc_device_ptr);
    }

    /* Check if the buffered command is ready */
    if ((esdhc_device_ptr->BUFFERED_CMD.COMMAND & (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG)) != (ESDHC_COMMAND_DATACMD_FLAG))
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Infinite transfers are not supported */
    if (esdhc_device_ptr->BUFFERED_CMD.BLOCKS == ((uint32_t)-1))
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Check if the transfer fits into the buffer (size block * count of size must be <= n) */
    if ((esdhc_device_ptr->BUFFERED_CMD.BLOCKS * esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE) > n)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Zero length transfer is a no-op */
    if (n == 0)
    {
        return 0;
    }

    /* Check max transfer size */
    if (n >= 0x10000)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Check buffer alignment */
    if (((uint32_t)data_ptr) & 0x03)
    {
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

#if ESDHC_IS_HANDLING_CACHE

    /* Prepare the section lengths */
    head_len = (PSP_MEMORY_ALIGNMENT + 1) - ((uint32_t)data_ptr & PSP_MEMORY_ALIGNMENT);
    body_len = (n - head_len) & ~((uint32_t)PSP_MEMORY_ALIGNMENT);
    tail_len = (n - head_len - body_len);

    adma2_desc_idx = -1; /* Points to last valid descriptor, which is none at the moment */
    if (head_len)
    {
        head = esdhc_device_ptr->ADMA2_DATA->HEAD_BUF;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (head_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)head;
    }
    if (body_len)
    {
        body = ((uint8_t *)data_ptr) + head_len;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (body_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)body;
    }
    if (tail_len)
    {
        tail = esdhc_device_ptr->ADMA2_DATA->TAIL_BUF;
        adma2_desc_idx++;
        adma2_desc[adma2_desc_idx].LEN_ATTR = (tail_len << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID;
        adma2_desc[adma2_desc_idx].DATA_ADDR = (uint32_t)tail;
    }
    adma2_desc[adma2_desc_idx].LEN_ATTR |= ESDHC_ADMA2_FLAG_END;
    _DCACHE_FLUSH_MBYTES(adma2_desc, 3 * sizeof(ESDHC_ADMA2_DESC));

    /* Flush caches */
    if (head_len)
    {
        _mem_copy(data_ptr, head, head_len);
        _DCACHE_FLUSH_LINE(head);
    }

    if (body_len)
    {
        _DCACHE_FLUSH_MBYTES(body, body_len);
    }

    if (tail_len)
    {
        _mem_copy(((uint8_t *)data_ptr) + head_len + body_len, tail, tail_len);
        _DCACHE_FLUSH_LINE(tail);
    }

#else
    adma2_desc[0].LEN_ATTR = (n << 16) | ESDHC_ADMA2_FLAG_TRAN | ESDHC_ADMA2_FLAG_VALID | ESDHC_ADMA2_FLAG_END;
    adma2_desc[0].DATA_ADDR = (uint32_t)data_ptr;
#endif

    _lwevent_clear(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_TRANSFER_DONE | ESDHC_LWEVENT_TRANSFER_ERROR));

    if (_esdhc_send_command(esdhc_device_ptr, &(esdhc_device_ptr->BUFFERED_CMD), adma2_desc) != MQX_OK)
    {
        esdhc_device_ptr->BUFFERED_CMD.COMMAND = 0;
        if (error)
        {
            *error = ESDHC_ERR;
        }
        return -1;
    }

    /* Wait for transfer to complete. Timeout depends on number of blocks. */
    if (_lwevent_wait_ticks(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_TRANSFER_DONE | ESDHC_LWEVENT_TRANSFER_ERROR), FALSE, /*(esdhc_device_ptr->BUFFERED_CMD.BLOCKS > 1)? */ ESDHC_CMD12_TICK_TIMEOUT /*: ESDHC_CMD_TICK_TIMEOUT*/) != MQX_OK)
    {
        if (error)
        {
            *error = NIO_ETIMEDOUT;
        }
        return -1;
    }
    else
    {
        if (_lwevent_get_signalled() & ESDHC_LWEVENT_TRANSFER_ERROR)
        {
            SDHC_SET_SYSCTL(esdhc_base, SDHC_SYSCTL_RSTD(1));  //!BW_SDHC_SYSCTL_RSTD(esdhc_base, 1);

            if (esdhc_device_ptr->BUFFERED_CMD.BLOCKS > 1)
            {
                /* In this case the peripheral doesn't automatically send the CMD12, so MUST be sent manually */
                ESDHC_COMMAND_STRUCT command;
                command.COMMAND = ESDHC_CREATE_CMD(12, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1b, (ESDHC_COMMAND_NONE_FLAG));
                command.ARGUMENT = 0;
                command.BLOCKS = 0;

                _esdhc_send_command(esdhc_device_ptr, &command, NULL);
                /* Don't care about the result because the operation ends always by IO_ERROR. */
            }

            if (error)
            {
                *error = ESDHC_ERR;
            }
            return -1;
        }
    }

    return (esdhc_device_ptr->BUFFERED_CMD.BLOCKS * esdhc_device_ptr->BUFFERED_CMD.BLOCKSIZE);
}


/*!
 * \brief ESDHC ISR. Used for event rising.
 *
 * \param[in] parameter  ESDHC_DEVICE_STRUCT_PTR Device runtime information
 *
 * \return None
 */
static void _esdhc_isr(
    /* [IN] The address of the device specific information */
    void *parameter)
{
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr = (ESDHC_DEVICE_STRUCT_PTR)parameter;
    SDHC_Type *esdhc_base = (SDHC_Type *)esdhc_device_ptr->DEV_BASE;
    uint32_t sdhc_irqstat;
    uint32_t sdhc_irqsigen;

    /* Back up the IRQ status */
    sdhc_irqstat = SDHC_RD_IRQSTAT(esdhc_base);
    sdhc_irqsigen = SDHC_RD_IRQSIGEN(esdhc_base);
    // Clear the all sets IRQ status bits
    SDHC_WR_IRQSTAT(esdhc_base, sdhc_irqstat);


    /*
        DMA Error
      Occurs when an Internal DMA transfer has failed. This bit is set to 1, when some error occurs in the data
      transfer. This error can be caused by either Simple DMA or ADMA, depending on which DMA is in use.
      The value in DMA System Address register is the next fetch address where the error occurs. Since any
      error corrupts the whole data block, the host driver shall re-start the transfer from the corrupted block
      boundary. The address of the block boundary can be calculated either from the current DSADDR value or
      from the remaining number of blocks and the block size.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_DMAEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_DMAE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_ERROR);
    }

    /*
        Auto CMD12 Error
      Occurs when detecting that one of the bits in the Auto CMD12 Error Status register has changed from 0 to
      1. This bit is set to 1, not only when the errors in Auto CMD12 occur, but also when the Auto CMD12 is
      not executed due to the previous command error.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_AC12EIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_AC12E_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_TRANSFER_ERROR));
    }

    /*
        Data End Bit Error
      Occurs either when detecting 0 at the end bit position of read data, which uses the DAT line, or at the end
      bit position of the CRC.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_DEBEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_DEBE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_ERROR);
    }

    /*
        Data CRC Error
      Occurs when detecting a CRC error when transferring read data, which uses the DAT line, or when
      detecting the Write CRC status having a value other than 010.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_DCEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_DCE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_ERROR);
    }

    /*
        Data Timeout Error
      Occurs when detecting one of following time-out conditions.
        • Busy time-out for R1b,R5b type
        • Busy time-out after Write CRC status
        • Read Data time-out
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_DTOEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_DTOE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_ERROR);
    }

    /*
        Command Index Error
      Occurs if a Command Index error occurs in the command response.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CIEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CIE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_CMD_ERROR);
    }

    /*
        Command End Bit Error
      Occurs when detecting that the end bit of a command response is 0.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CEBEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CEBE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_CMD_ERROR);
    }


    /*
        Command CRC Error
      Command CRC Error is generated in two cases.
      • If a response is returned and the Command Timeout Error is set to 0 (indicating no time-out), this bit
      is set when detecting a CRC error in the command response.
      • The SDHC detects a CMD line conflict by monitoring the CMD line when a command is issued. If
      the SDHC drives the CMD line to 1, but detects 0 on the CMD line at the next SDCLK edge, then
      the SDHC shall abort the command (Stop driving CMD line) and set this bit to 1. The Command
      Timeout Error shall also be set to 1 to distinguish CMD line conflict.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CCEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CCE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_CMD_ERROR);
    }


    /*
        Command Timeout Error
      Occurs only if no response is returned within 64 SDCLK cycles from the end bit of the command. If the
      SDHC detects a CMD line conflict, in which case a Command CRC Error shall also be set, this bit shall be
      set without waiting for 64 SDCLK cycles. This is because the command will be aborted by the SDHC.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CTOEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CTOE_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_CMD_TIMEOUT);
    }

    /*
        Card Interrupt
      This status bit is set when an interrupt signal is detected from the external card. In 1-bit mode, the SDHC
      will detect the Card Interrupt without the SD Clock to support wakeup. In 4-bit mode, the card interrupt
      signal is sampled during the interrupt cycle, so the interrupt from card can only be sampled during
      interrupt cycle, introducing some delay between the interrupt signal from the SDIO card and the interrupt
      to the host system. Writing this bit to 1 can clear this bit, but as the interrupt factor from the SDIO card
      does not clear, this bit is set again. In order to clear this bit, it is required to reset the interrupt factor from
      the external card followed by a writing 1 to this bit.
      When this status has been set, and the host driver needs to service this interrupt, the Card Interrupt
      Signal Enable in the Interrupt Signal Enable register should be 0 to stop driving the interrupt signal to the
      host system. After completion of the card interrupt service (It should reset the interrupt factors in the SDIO
      card and the interrupt signal may not be asserted), write 1 to clear this bit, set the Card Interrupt Signal
      Enable to 1, and start sampling the interrupt signal again.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CINTIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CINT_MASK))
    {
        if (esdhc_device_ptr->IO_CALLBACK_STR.CALLBACK)
            esdhc_device_ptr->IO_CALLBACK_STR.CALLBACK(esdhc_device_ptr->IO_CALLBACK_STR.USERDATA);
    }

#if ESDHC_CARD_DETECTION_SUPPORT
    /*
        Card Removal
      This status bit is set if the Card Inserted bit in the Present State register changes from 1 to 0. When the
      host driver writes this bit to 1 to clear this status, the status of the Card Inserted in the Present State
      register should be confirmed. Because the card state may possibly be changed when the host driver
      clears this bit and the interrupt event may not be generated. When this bit is cleared, it will be set again if
      no card is inserted. In order to leave it cleared, clear the Card Removal Status Enable bit in Interrupt
      Status Enable register.
        0b Card state unstable or inserted
        1b Card removed
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CRMIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CRM_MASK))
    {
        // Set card to NONE
        esdhc_device_ptr->CARD = ESDHC_CARD_NONE;

        // Switch of Card Insert interrupts
        SDHC_CLR_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CRMIEN);  //!BW_SDHC_IRQSIGEN_CRMIEN(esdhc_base, 0);
        SDHC_SET_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CINSIEN(1));  //!BW_SDHC_IRQSIGEN_CINSIEN(esdhc_base, 1);

        // Switch the peripheral to 1 bit mode to enable correct work of Dat3 signal to card insert detection

        SDHC_WR_PROCTL_DTW(esdhc_base, ESDHC_PROCTL_DTW_1BIT);

        // Notify higher layer if requested
        if (esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.CALLBACK)
            esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.CALLBACK(esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.USERDATA, FALSE);

        // Set the ERRORS for internal events to manage active trasfers
        _lwevent_set(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_TRANSFER_ERROR));
    }

    /*
        Card Insertion
      This status bit is set if the Card Inserted bit in the Present State register changes from 0 to 1. When the
      host driver writes this bit to 1 to clear this status, the status of the Card Inserted in the Present State
      register should be confirmed. Because the card state may possibly be changed when the host driver
      clears this bit and the interrupt event may not be generated. When this bit is cleared, it will be set again if
      a card is inserted. In order to leave it cleared, clear the Card Inserted Status Enable bit in Interrupt Status
      Enable register.
        0b Card state unstable or removed
        1b Card inserted
    */
    else if ((sdhc_irqsigen & SDHC_IRQSIGEN_CINSIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CINS_MASK))
    {
        // Switch off Card Insert interrupts


        SDHC_CLR_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CINSIEN_MASK);  //!BW_SDHC_IRQSIGEN_CINSIEN(esdhc_base, 0);
        SDHC_SET_IRQSIGEN(esdhc_base, SDHC_IRQSIGEN_CRMIEN(1));  //!BW_SDHC_IRQSIGEN_CRMIEN(esdhc_base, 1);

        // If there was none card, just change the state to Unkown card
        if (esdhc_device_ptr->CARD == ESDHC_CARD_NONE)
        {
            esdhc_device_ptr->CARD = ESDHC_CARD_UNKNOWN;

            // Notify higher layer if requested
            if (esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.CALLBACK)
                esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.CALLBACK(esdhc_device_ptr->CARD_PRESENCE_CALLBACK_STR.USERDATA, TRUE);
        }

        // Set the ERRORS for internal events to manage active trasfers
        _lwevent_set(&esdhc_device_ptr->LWEVENT, (ESDHC_LWEVENT_CMD_ERROR | ESDHC_LWEVENT_TRANSFER_ERROR));
    }

#endif
    /*
        Buffer Read Ready
      This status bit is set if the Buffer Read Enable bit, in the Present State register, changes from 0 to 1.
      Refer to the Buffer Read Enable bit in the Present State register for additional information.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_BRRIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_BRR_MASK))
    {
    }

    /*
        Buffer Write Ready
      This status bit is set if the Buffer Write Enable bit, in the Present State register, changes from 0 to 1. Refer
      to the Buffer Write Enable bit in the Present State register for additional information.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_BWRIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_BWR_MASK))
    {
    }

    /*
        DMA Interrupt
      Occurs only when the internal DMA finishes the data transfer successfully. Whenever errors occur during
      data transfer, this bit will not be set. Instead, the DMAE bit will be set. Either Simple DMA or ADMA
      finishes data transferring, this bit will be set.
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_DINTIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_DINT_MASK))
    {
        //_lwevent_set( &esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_DONE);
    }

    /*
        Block Gap Event
      If the PROCTL[SABGREQ] is set, this bit is set when a read or write transaction is stopped at a block gap.
      If PROCTL[SABGREQ] is not set to 1, this bit is not set to 1.
      In the case of a read transaction: This bit is set at the falling edge of the DAT line active status (When the
      transaction is stopped at SD Bus timing). The read wait must be supported in order to use this function.
      In the case of write transaction: This bit is set at the falling edge of write transfer active status (After
      getting CRC status at SD bus timing).
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_BGEIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_BGE_MASK))
    {
        //_lwevent_set( &esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_ERROR);
    }

    /*
        Transfer Complete
      This bit is set when a read or write transfer is completed.
      In the case of a read transaction: This bit is set at the falling edge of the read transfer active status. There
      are two cases in which this interrupt is generated. The first is when a data transfer is completed as
      specified by the data length (after the last data has been read to the host system). The second is when
      data has stopped at the block gap and completed the data transfer by setting the PROCTL[SABGREQ]
      (after valid data has been read to the host system).
      In the case of a write transaction: This bit is set at the falling edge of the DAT line active status. There are
      two cases in which this interrupt is generated. The first is when the last data is written to the SD card as
      specified by the data length and the busy signal is released. The second is when data transfers are
      stopped at the block gap, by setting the PROCTL[SABGREQ], and the data transfers are completed.
      (after valid data is written to the SD card and the busy signal released).
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_TCIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_TC_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_TRANSFER_DONE);
    }

    /*
        Command Complete
      This bit is set when you receive the end bit of the command response (except Auto CMD12). Refer to the
      PRSSTAT[CIHB].
    */
    if ((sdhc_irqsigen & SDHC_IRQSIGEN_CCIEN_MASK) && (sdhc_irqstat & SDHC_IRQSTAT_CC_MASK))
    {
        _lwevent_set(&esdhc_device_ptr->LWEVENT, ESDHC_LWEVENT_CMD_DONE);
    }
}

/* EOF */
