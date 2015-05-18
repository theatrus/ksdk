/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "diskio.h"

#include "fsl_dspi_master_driver.h"
#include "fsl_dspi_edma_master_driver.h"
#include "fsl_sdcard_spi.h"

#define SDSPI_SPI_INSTANCE 0
#define SDSPI_SPI_PCS   (1 << 0)
static uint32_t g_card_initialized = 0;
static sdspi_card_t g_card;
static sdspi_spi_t g_spi;
static sdspi_ops_t g_ops;
static dspi_master_state_t g_dspiState;
static dspi_device_t g_dspiDevice;

#define SPI_TRANSFER_TIMEOUT 1000

/*
 * spi interfaces
 */
uint32_t getSpiMaxFrequency(sdspi_spi_t *spi)
{
    return (spi->busBaudRate);
}

uint32_t setSpiFrequency(sdspi_spi_t *spi, uint32_t frequency)
{
    uint32_t calculatedBaudRate;
    calculatedBaudRate = DSPI_HAL_SetBaudRate(g_dspiBase[spi->spiInstance],
            ((dspi_master_state_t *)(spi->spiState))->whichCtar,
            frequency,
            ((dspi_master_state_t *)(spi->spiState))->dspiSourceClock);
    if (0 == calculatedBaudRate)
    {
        return 1;
    }
    return 0;
}

#if defined SPI_USING_DMA
static edma_state_t g_dmaState;
#if defined (__GNUC__) || defined (__CC_ARM)
__attribute__((aligned(32)))
#elif defined (__ICCARM__)
#pragma data_alignment=32
#else
#error unknown compiler
#endif
static edma_software_tcd_t g_tcd;
static dspi_edma_master_state_t g_edmaDspiMasterState;
static dspi_edma_device_t g_edmaDspiDevice;

uint32_t spiExchange(sdspi_spi_t *spi, const uint8_t *in, uint8_t *out, uint32_t size)
{
    if (kStatus_DSPI_Success == DSPI_DRV_EdmaMasterTransferBlocking(spi->spiInstance,
                  NULL, in, out, size, SPI_TRANSFER_TIMEOUT))
    {
        return 0;
    }
    return 1;
}

uint8_t spiSendWord(sdspi_spi_t *spi, uint8_t word)
{
    uint8_t result;

    if (kStatus_DSPI_Success != DSPI_DRV_EdmaMasterTransferBlocking(spi->spiInstance,
                NULL, &word, &result, 1, SPI_TRANSFER_TIMEOUT))
    {
        result = 0xFF;
    }

    return result;
}

#else
/* None DMA mode */
/*
 * spi interfaces
 */
uint32_t spiExchange(sdspi_spi_t *spi, const uint8_t *in, uint8_t *out, uint32_t size)
{
    if (kStatus_DSPI_Success == DSPI_DRV_MasterTransferBlocking(spi->spiInstance,
                  NULL, in, out, size, SPI_TRANSFER_TIMEOUT))
    {
        return 0;
    }
    return 1;
}

uint8_t spiSendWord(sdspi_spi_t *spi, uint8_t word)
{
    uint8_t result;

    if (kStatus_DSPI_Success != DSPI_DRV_MasterTransferBlocking(spi->spiInstance,
                NULL, &word, &result, 1, SPI_TRANSFER_TIMEOUT))
    {
        result = 0xFF;
    }

    return result;
}
#endif /*None DMA operation*/

#if _USE_WRITE
DRESULT sdcard_disk_write(uint8_t pdrv, const uint8_t *buff, uint32_t sector, uint8_t count)
{
    if (pdrv != SD)
    {
        return RES_PARERR;
    }
    if (g_card.cardType == 0 || g_card.cardType == kCardTypeUnknown)
    {
        return RES_NOTRDY;
    }
    if (kStatus_SDSPI_NoError != SDSPI_DRV_WriteBlocks(&g_spi, &g_card, (uint8_t *)buff, sector, count))
    {
        return RES_ERROR;
    }
    return RES_OK;
}
#endif

DRESULT sdcard_disk_read(uint8_t pdrv, uint8_t *buff, uint32_t sector, uint8_t count)
{
    if (pdrv != SD)
    {
        return RES_PARERR;
    }

    if (g_card.cardType == 0 || g_card.cardType == kCardTypeUnknown)
    {
        return RES_NOTRDY;
    }
    if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&g_spi, &g_card, buff, sector, count))
    {
        return RES_ERROR;
    }
    return RES_OK;
}

#if _USE_IOCTL
DRESULT sdcard_disk_ioctl(uint8_t pdrv, uint8_t cmd, void *buff)
{
    DRESULT res = RES_OK;

    if (pdrv != SD)
    {
        return RES_PARERR;
    }

    switch(cmd)
    {
        case GET_SECTOR_COUNT:
            if (buff)
            {
                *(uint32_t *)buff = g_card.blockCount;
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case GET_SECTOR_SIZE:
            if (buff)
            {
                *(uint32_t *)buff = g_card.blockSize;
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case GET_BLOCK_SIZE:
            if (buff)
            {
                if (IS_SD_CARD(&g_card))
                {
                    if (g_card.version == kSdCardVersion_1_x)
                    {
                        *(uint32_t *)buff = SD_CSD_SECTOR_SIZE(g_card.rawCsd);
                    }
                    else
                    {
                        *(uint32_t *)buff = SDV20_CSD_SECTOR_SIZE(g_card.rawCsd);
                    }
                }
                else
                {
                    res = RES_PARERR;
                }
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case CTRL_SYNC:
            res = RES_OK;
            break;
        case MMC_GET_TYPE:
            if (buff)
            {
                switch (g_card.cardType)
                {
                    case kCardTypeMmc:
                        *(uint32_t *)buff = CT_MMC;
                        break;
                    case kCardTypeSd:
                        if (g_card.version == kSdCardVersion_1_x)
                        {
                        *(uint32_t *)buff = CT_SD1;
                        }
                        else
                        {
                            *(uint32_t *)buff = CT_SD2;
                            if ((g_card.caps & SDSPI_CAPS_SDHC) ||
                                (g_card.caps & SDSPI_CAPS_SDXC))
                            {
                                *(uint32_t *)buff |= CT_BLOCK;
                            }
                        }
                        break;
                    default:
                        res = RES_PARERR;
                        break;
                }
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case MMC_GET_CSD:
            if (buff)
            {
                memcpy(buff, g_card.rawCsd, sizeof(g_card.rawCsd));
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case MMC_GET_CID:
            if (buff)
            {
                memcpy(buff, g_card.rawCid, sizeof(g_card.rawCid));
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        case MMC_GET_OCR:
            if (buff)
            {
                *(uint32_t *)buff = g_card.ocr;
            }
            else
            {
                res = RES_PARERR;
            }
            break;
        default:
            res = RES_PARERR;
            break;

    }

    return res;
}
#endif

DSTATUS sdcard_disk_status(uint8_t pdrv)
{
    if (pdrv != SD)
    {
        return STA_NOINIT;
    }

    return 0;
}

static void reset_all_states()
{
    SDSPI_DRV_Shutdown(&g_spi, &g_card);
    DSPI_DRV_MasterDeinit(g_spi.spiInstance);
#if SPI_USING_DMA
    EDMA_DRV_Deinit();
#endif
    g_card_initialized = 0;

    memset(&g_spi, 0, sizeof(g_spi));
    memset(&g_ops, 0, sizeof(g_ops));
    memset(&g_dspiState, 0, sizeof(g_dspiState));
    memset(&g_dspiDevice, 0, sizeof(g_dspiDevice));
#if SPI_USING_DMA
    memset(&g_dmaState, 0, sizeof(g_dmaState));
#endif
}

DSTATUS sdcard_disk_initialize(uint8_t pdrv)
{
#if SPI_USING_DMA
    edma_user_config_t dmaConfig;
#endif
    uint32_t calculatedBaudRate;

    if (pdrv != SD)
    {
        return STA_NOINIT;
    }

    if (g_card_initialized)
    {
        reset_all_states();
    }

    g_spi.ops = &g_ops;
    g_spi.spiInstance = SDSPI_SPI_INSTANCE;
    g_spi.ops->getMaxFrequency = &getSpiMaxFrequency;
    g_spi.ops->setFrequency = &setSpiFrequency;
    g_spi.ops->exchange = &spiExchange;
    g_spi.ops->sendWord = &spiSendWord;

        /* SPI DMA mode Init*/
#if defined SPI_USING_DMA
    dspi_edma_master_user_config_t userConfig;
    edma_user_config_t edmaUserConfig;
      
    memset(&g_dmaState, 0, sizeof(g_dmaState));
    memset(&edmaUserConfig, 0, sizeof(edmaUserConfig));
    
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = 0;
    EDMA_DRV_Init(&g_dmaState, &edmaUserConfig);
    
   /* configure the members of the user config*/
    userConfig.isChipSelectContinuous = true;
    userConfig.isSckContinuous = false;
    userConfig.pcsPolarity = kDspiPcs_ActiveLow;
    userConfig.whichCtar = kDspiCtar0;
    userConfig.whichPcs = kDspiPcs0;
    DSPI_DRV_EdmaMasterInit(g_spi.spiInstance, &g_edmaDspiMasterState, &userConfig, &g_tcd);
    
    /* config member of the spi device config*/
    g_edmaDspiDevice.dataBusConfig.bitsPerFrame = 8;
    g_edmaDspiDevice.dataBusConfig.clkPhase = kDspiClockPhase_FirstEdge;
    g_edmaDspiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    g_edmaDspiDevice.dataBusConfig.direction = kDspiMsbFirst;
    g_edmaDspiDevice.bitsPerSec = 20000000;
    
       
    /* configure the spi bus*/
    if (DSPI_DRV_EdmaMasterConfigureBus(g_spi.spiInstance, &g_edmaDspiDevice, &calculatedBaudRate)
        != kStatus_DSPI_Success)
    {
        printf("\r  edma configure bus failed\n");
        
    }
    g_spi.spiState = &g_edmaDspiMasterState;
    g_spi.spiDevice = &g_edmaDspiDevice;
    g_spi.busBaudRate = calculatedBaudRate;
    
#else    /* None DMA mode */
    dspi_master_user_config_t dspiConfig;

    memset(&dspiConfig, 0, sizeof(dspiConfig));
    
    /* Dspi none DMA mode Init*/
    dspiConfig.isChipSelectContinuous = true;
    dspiConfig.isSckContinuous = false;
    dspiConfig.pcsPolarity = kDspiPcs_ActiveLow;
    dspiConfig.whichCtar = kDspiCtar0;
    dspiConfig.whichPcs = kDspiPcs0;

    DSPI_DRV_MasterInit(g_spi.spiInstance, &g_dspiState, &dspiConfig);

    g_dspiDevice.dataBusConfig.bitsPerFrame = 8;
    g_dspiDevice.dataBusConfig.clkPhase = kDspiClockPhase_FirstEdge;
    g_dspiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    g_dspiDevice.dataBusConfig.direction = kDspiMsbFirst;
    g_dspiDevice.bitsPerSec = 20000000; /* 100KHz */
    if (DSPI_DRV_MasterConfigureBus(g_spi.spiInstance, &g_dspiDevice, &calculatedBaudRate) != kStatus_DSPI_Success)
    {
        DSPI_DRV_MasterDeinit(g_spi.spiInstance);
#if SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return STA_NOINIT;
    }

    g_spi.spiState = &g_dspiState;    
    g_spi.spiDevice = &g_dspiDevice;
    g_spi.busBaudRate = calculatedBaudRate;
#endif

    if (kStatus_SDSPI_NoError != SDSPI_DRV_Init(&g_spi, &g_card))
    {
        SDSPI_DRV_Shutdown(&g_spi, &g_card);
        DSPI_DRV_MasterDeinit(g_spi.spiInstance);
#if SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return STA_NOINIT;
    }

    g_card_initialized = 1;
    return 0;
}
