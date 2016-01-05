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

///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>

// SDK Included Files
#include "fsl_debug_console.h"
#include "fsl_dspi_master_driver.h"
#include "board.h"

#include "fsl_sdmmc_card.h"
#include "fsl_sdcard_spi.h"

#if defined SPI_USING_DMA
#include "fsl_edma_driver.h"
#include "fsl_dspi_edma_master_driver.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
#define SPI_TRANSFER_TIMEOUT           100U
#define SDSPI_DEFAULT_BLOCK_SIZE       512U
#define EXAMPLE_BLOCK_NUM              8U
#define EXAMPLE_START_BLOCK            4U
#define EXAMPLE_WRITE_LOOP_TIMES       100U

// Buffer data prepares to write into SDHC card
uint8_t gWriteData[SDSPI_DEFAULT_BLOCK_SIZE * EXAMPLE_BLOCK_NUM];
// Buffer data stores datas that read from SDHC card
uint8_t gReadData[SDSPI_DEFAULT_BLOCK_SIZE * EXAMPLE_BLOCK_NUM];
// Store status flag to recognize when card was inserted
volatile uint32_t card_inserted = 0;
// Runtime state structure for the DSPI master driver
dspi_master_state_t g_dspiState;
// Data structure containing information about a device on the SPI bus
dspi_device_t g_dspiDevice;

semaphore_t cd;
const char *g_month_str[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

#if defined SPI_USING_DMA
edma_state_t g_dmaState;
#if defined (__GNUC__) || defined (__CC_ARM)
__attribute__((aligned(32)))
#elif defined (__ICCARM__)
#pragma data_alignment=32
#else
#error unknown compiler
#endif
edma_software_tcd_t g_tcd;
dspi_edma_master_state_t g_edmaDspiMasterState;
dspi_edma_device_t g_edmaDspiDevice;
#endif

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
// A function to gets spi max Frequency
uint32_t getSpiMaxFrequency(sdspi_spi_t *spi)
{
    return spi->busBaudRate;
}

// A function to set spi max Frequency
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
    PRINTF("BaudRate set to %dHz\r\n", calculatedBaudRate);
    return 0;
}

// A function to exchange data
uint32_t spiExchange(sdspi_spi_t *spi, const uint8_t *in, uint8_t *out, uint32_t size)
{
    if (kStatus_DSPI_Success == DSPI_DRV_MasterTransferBlocking(spi->spiInstance,
                  NULL, in, out, size, SPI_TRANSFER_TIMEOUT))
    {
        return 0;
    }
    return 1;
}

// A function send Word to device
uint8_t spiSendWord(sdspi_spi_t *spi, uint8_t word)
{
    uint8_t result = 0;

    if (kStatus_DSPI_Success != DSPI_DRV_MasterTransferBlocking(spi->spiInstance,
                NULL, &word, &result, 1, SPI_TRANSFER_TIMEOUT))
    {
        result = 0xFF;
    }

    return result;
}

/*!
 * @brief A function to fill a memory buffer of 'len' no of bytes
 * with a reference data governed by a seed value.
 */
static uint32_t fill_reference_data(uint8_t *pdata, uint8_t seed, uint32_t len)
{
    uint32_t i, j;
    if (!pdata || !len)
        return 1;

    // clear buffer with all '0'
    memset(pdata, 0, len);

    // fill in the buffer processed with seed value
    for (i = 0; i < len; i++)
    {
        j = i % 4;
        switch(j)
        {
            case 0:
                pdata[i] = 0xA0 | (seed & 0x0F);
                break;
            case 1:
                pdata[i] = 0x0B | (seed & 0xF0);
                break;
            case 2:
                pdata[i] = 0xC0 | (seed & 0x0F);
                break;
            case 3:
                pdata[i] = 0x0D | (seed & 0xF0);
                break;
        }
    }
    return 0;
}

#if defined SHOW_CARD_INFO
/*!
 * @brief Function to show few card information
 */
static void show_card_cid(sdspi_card_t *card)
{
    char temp[32];
    uint32_t i, j;
    uint8_t *rawCid = card->rawCid;

    PRINTF("-------- Card CID --------\r\n");
    PRINTF("Manufacturer ID: 0x%X\r\n", rawCid[0]);
    PRINTF("OEM ID: 0x%X\r\n", ((uint16_t)(rawCid[1] << 8) | rawCid[2]));
    memset(temp, 0, sizeof(temp));
    for (i = 3, j = 0; i < 8; i++, j++)
    {
        temp[j] = rawCid[i];
    }
    PRINTF("Product name: %s\r\n", temp);
    PRINTF("Product revision: %u.%u\r\n", (rawCid[8] & 0xF0) >> 4, (rawCid[8] & 0xF));
    for (i = 9, j = 0; i < 13; i++)
    {
        j |= (uint32_t) rawCid[i] << ((12 - i) * 8);
    }
    PRINTF("Product serial number: 0x%X\r\n", j);

    i = (rawCid[14] & 0xF) - 1;
    j = ((rawCid[13] & 0x0F) << 4) | ((rawCid[14] & 0xF0) >> 4);
    PRINTF("Manufacturing data: %s %d\r\n", g_month_str[i], 2000 + j);
}

/*!
 * @brief Function to decode card read access time '1' value
 */
static float decode_taac(uint32_t tu, uint32_t tv)
{
    // Below parsing done as per SD Physical Layer specification
    switch(tu)
    {
        case 0:
            tu = 1;
            break;
        case 1:
            tu = 10;
            break;
        case 2:
            tu = 100;
            break;
        case 3:
            tu = 1000;
            break;
        case 4:
            tu = 10000;
            break;
        case 5:
            tu = 100000;
            break;
        case 6:
            tu = 1000000;
            break;
        case 7:
            tu = 10000000;
            break;
    }
    switch(tv)
    {
        case 1:
            tv = 10;
            break;
        case 2:
            tv = 12;
            break;
        case 3:
            tv = 13;
            break;
        case 4:
            tv = 15;
            break;
        case 5:
            tv = 20;
            break;
        case 6:
            tv = 25;
            break;
        case 7:
            tv = 30;
            break;
        case 8:
            tv = 35;
            break;
        case 9:
            tv = 40;
            break;
        case 0xa:
            tv = 45;
            break;
        case 0xb:
            tv = 50;
            break;
        case 0xc:
            tv = 55;
            break;
        case 0xd:
            tv = 60;
            break;
        case 0xe:
            tv = 70;
            break;
        case 0xf:
            tv = 80;
            break;
    }
    return (float) (tu * tv) / 10;
}


/*!
 * @brief Function to decode card data transfer speed
 */
static float decode_transpeed(uint32_t ru, uint32_t tv)
{
    switch(ru)
    {
        case 0:
            ru = 100;
            break;
        case 1:
            ru = 1000000;
            break;
        case 2:
            ru = 10000000;
            break;
        case 3:
            ru = 100000000;
            break;
        default:
            ru = 0;
            break;
    }
    switch(tv)
    {
        case 1:
            tv = 10;
            break;
        case 2:
            tv = 12;
            break;
        case 3:
            tv = 13;
            break;
        case 4:
            tv = 15;
            break;
        case 5:
            tv = 20;
            break;
        case 6:
            tv = 25;
            break;
        case 7:
            tv = 30;
            break;
        case 8:
            tv = 35;
            break;
        case 9:
            tv = 40;
            break;
        case 0xa:
            tv = 45;
            break;
        case 0xb:
            tv = 50;
            break;
        case 0xc:
            tv = 55;
            break;
        case 0xd:
            tv = 60;
            break;
        case 0xe:
            tv = 70;
            break;
        case 0xf:
            tv = 80;
            break;
        default:
            tv = 0;
            break;
    }
    return (float) (ru * tv) / 10;
}

/*!
 * @brief Function to parse and show Card Specific Data fields
 */
static void show_card_csd(sdspi_card_t *card)
{
    uint32_t i = 0, ccc;
    uint8_t *rawCsd;

    PRINTF("-------- Card CSD --------\r\n");
    rawCsd = card->rawCsd;
    PRINTF("CSD Structure: 0x%X\r\n", SDMMC_CSD_CSDSTRUCTURE_VERSION(rawCsd));
    PRINTF("taac: %.02f us\r\n", decode_taac(SDMMC_CSD_TAAC_TU(rawCsd), SDMMC_CSD_TAAC_TV(rawCsd)) / 1000);
    PRINTF("nsac: %d clks\r\n", SDMMC_CSD_NSAC(rawCsd));
    PRINTF("tran speed: %.2f kbps\r\n", decode_transpeed(SDMMC_CSD_TRANSPEED_RU(rawCsd), SDMMC_CSD_TRANSPEED_TV(rawCsd)));
    PRINTF("ccc: class ");
    ccc = SDMMC_CSD_CCC(rawCsd);
    while (ccc && i < 12)
    {
        if (ccc & 0x1)
        {
            PRINTF("%d ", i);
        }
        i++;
        ccc >>= 1;
    }
    PRINTF("\r\n");
    PRINTF("max read block length: %d Bytes\r\n", 1 << SDMMC_CSD_READBLK_LEN(rawCsd));
    PRINTF("max write block length: %d Bytes\r\n", 1 << SDMMC_CSD_WRITEBLK_LEN(rawCsd));
    if (SDMMC_CSD_READBLK_PARTIAL(rawCsd))
        PRINTF("Support partial read\r\n");
    if (SDMMC_CSD_WRITEBLK_MISALIGN(rawCsd))
        PRINTF("Support crossing physical block boundaries writing is allowed\r\n");
    if (SDMMC_CSD_READBLK_MISALIGN(rawCsd))
        PRINTF("Support crossing physical block boundaries reading is allowed\r\n");
    if (SDMMC_CSD_DSRIMP(rawCsd))
        PRINTF("DSR is implemented\r\n");
    if (!SDMMC_CSD_CSDSTRUCTURE_VERSION(rawCsd))
    {
        PRINTF("VDD_R_CURR_MIN: 0x%X\r\n", SDMMC_CSD_VDD_R_CURR_MIN(rawCsd));
        PRINTF("VDD_R_CURR_MAX: 0x%X\r\n", SDMMC_CSD_VDD_R_CURR_MAX(rawCsd));
        PRINTF("VDD_W_CURR_MIN: 0x%X\r\n", SDMMC_CSD_VDD_W_CURR_MIN(rawCsd));
        PRINTF("VDD_W_CURR_MAX: 0x%X\r\n", SDMMC_CSD_VDD_W_CURR_MAX(rawCsd));
        PRINTF("c_size_mult: %d\r\n", SDMMC_CSD_CSIZEMULT(rawCsd));
        PRINTF("c_size: %d\r\n", SDMMC_CSD_CSIZE(rawCsd));
    }
    else
    {
        PRINTF("c_size: %d\r\n", SDV20_CSD_CSIZE(rawCsd));
    }
    if (card->cardType == kCardTypeSd)
    {
        if (SD_CSD_ERASE_BLK_ENABLE(rawCsd))
            PRINTF("Erase unit size is one or multiple units of 512 bytes\r\n");
        else
            PRINTF("Erase unit size is one or multiple units of %d blocks\r\n", SD_CSD_SECTOR_SIZE(rawCsd) + 2);
        PRINTF("The size of write protected group is %d blocks\r\n", SD_CSD_WP_GRP_SIZE(rawCsd) + 2);
        if (SD_CSD_PERM_WRITEPROTECT(rawCsd))
            PRINTF("The content is write protected permanently\r\n");
        if (SD_CSD_TEMP_WRITEPROTECT(rawCsd))
            PRINTF("The content is write protected temporarily\r\n");
    }

    if (SDMMC_CSD_WP_GRP_ENABLE(rawCsd))
        PRINTF("Write protection group is defined\r\n");
    PRINTF("R2W_Factor: %d\r\n", SDMMC_CSD_R2W_FACTOR(rawCsd));
    if (SDMMC_CSD_WRITEBLK_PARTIAL(rawCsd))
        PRINTF("Smaller blocks can be used to write\r\n");
    if (SDMMC_CSD_COPY(rawCsd))
        PRINTF("The content is copied\r\n");
    if (!(SDMMC_CSD_FILEFORMAT_GRP(rawCsd)))
    {
        switch(SDMMC_CSD_FILE_FORMAT(rawCsd))
        {
            case 0:
                PRINTF("Hard disk-like file system with partition table\r\n");
                break;
            case 1:
                PRINTF("DOS FAT (floppy-like) with boot sector only\r\n");
                break;
            case 2:
                PRINTF("Universla file format\r\n");
                break;
            case 3:
                PRINTF("Others/Unknown\r\n");
                break;
        }
    }
}

/*!
 * @brief Function to show Card informations
 */
static void show_card_info(sdspi_card_t *card, bool showDetail)
{
    double temp;

    PRINTF("\r\n------- Card Information -------\r\n");
    PRINTF("Card Type: ");

    switch(card->cardType)
    {
        case kCardTypeMmc:
            PRINTF("MMC");
            break;
        case kCardTypeSd:
            PRINTF("SD");
            if (card->caps & SDSPI_CAPS_SDHC)
            {
                PRINTF("HC");
            }
            else if (card->caps & SDSPI_CAPS_SDXC)
            {
                PRINTF("XC");
            }
            else
            {
                PRINTF("SC");
            }
            break;
        case kCardTypeSdio:
            PRINTF("SDIO");
            break;
        default:
            PRINTF("Unknown");
            break;
    }
    PRINTF("\r\nCard Capacity: ");
    temp = 1000000000 / card->blockSize;
    temp = ((float)card->blockCount / (float)temp);
    if (temp > 1.0)
    {
        PRINTF("%.02f GB\r\n", temp);
    }
    else
    {
        PRINTF("%.02f MB\r\n", temp * 1000);
    }

    show_card_cid(card);
    if (IS_SD_CARD(card) && showDetail)
    {
        show_card_csd(card);
    }
}

#endif
/*!
 * @brief Function to indicate whether a card is detected or not
 */
void spiSDcard_card_detection(void)
{
    card_inserted = BOARD_IsSDCardDetected();
    OSA_SemaPost(&cd);
}

/*!
 * @brief This function demonstrates read, write and erase functionalities of the card
 */
static void demo_card_data_access(uint8_t instance)
{
    sdspi_card_t card;
    sdspi_spi_t spi = {0};
    sdspi_ops_t ops = {0};
    uint32_t i, j, status, ms, calculatedBaudRate;

    /* Set SPI operation API */
    ops.getMaxFrequency = &getSpiMaxFrequency;
    ops.setFrequency = &setSpiFrequency;
    ops.exchange = &spiExchange;
    ops.sendWord = &spiSendWord;

    spi.ops = &ops;

    // wait for a card detection
    spiSDcard_card_detection();

    // card insertion is detected based on interrupt
    if (!card_inserted)
    {
        PRINTF("Please insert an SD card\r\n");
        do
        {
            status = OSA_SemaWait(&cd, OSA_WAIT_FOREVER);
        } while ((status == kStatus_OSA_Idle) ||(!card_inserted));
    }

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
    DSPI_DRV_EdmaMasterInit(instance, &g_edmaDspiMasterState, &userConfig, &g_tcd);

    /* config member of the spi device config*/
    g_edmaDspiDevice.dataBusConfig.bitsPerFrame = 8;
    g_edmaDspiDevice.dataBusConfig.clkPhase = kDspiClockPhase_FirstEdge;
    g_edmaDspiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    g_edmaDspiDevice.dataBusConfig.direction = kDspiMsbFirst;
    g_edmaDspiDevice.bitsPerSec = 20000000;


    /* configure the spi bus*/
    if (DSPI_DRV_EdmaMasterConfigureBus(instance, &g_edmaDspiDevice, &calculatedBaudRate)
        != kStatus_DSPI_Success)
    {
        PRINTF("\r  edma configure bus failed\r\n");

    }

    spi.spiInstance = instance;
    spi.spiState = &g_edmaDspiMasterState;
    spi.spiDevice = &g_edmaDspiDevice;
    spi.busBaudRate = calculatedBaudRate;
#else    /* None DMA mode */
    dspi_master_user_config_t dspiConfig;

    memset(&dspiConfig, 0, sizeof(dspiConfig));

    /* Dspi none DMA mode Init*/
    dspiConfig.isChipSelectContinuous = true;
    dspiConfig.isSckContinuous = false;
    dspiConfig.pcsPolarity = kDspiPcs_ActiveLow;
    dspiConfig.whichCtar = kDspiCtar0;
    dspiConfig.whichPcs = kDspiPcs0;

    DSPI_DRV_MasterInit(instance, &g_dspiState, &dspiConfig);

    g_dspiDevice.dataBusConfig.bitsPerFrame = 8;
    g_dspiDevice.dataBusConfig.clkPhase = kDspiClockPhase_FirstEdge;
    g_dspiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    g_dspiDevice.dataBusConfig.direction = kDspiMsbFirst;
    g_dspiDevice.bitsPerSec = 20000000;
    DSPI_DRV_MasterConfigureBus(instance, &g_dspiDevice, &calculatedBaudRate);
    spi.spiInstance = instance;
    spi.spiState = &g_dspiState;
    spi.spiDevice = &g_dspiDevice;
    spi.busBaudRate = calculatedBaudRate;
#endif

    /* Begin test SD card. */
    memset(&card, 0, sizeof(card));
    if (kStatus_SDSPI_NoError != SDSPI_DRV_Init(&spi, &card))
    {
        PRINTF("SDCARD_SPI_DRV_Init failed\r\n");
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return;
    }

#if defined SHOW_CARD_INFO
    show_card_info(&card, true);
#endif
     // Check if the card is read only or not
#if ( defined (SDCARD_CARD_WRITE_PROTECTION_GPIO_PORT) && defined (SDCARD_CARD_WRITE_PROTECTION_GPIO_PIN))
    if (SDSPI_DRV_CheckReadOnly(&spi, &card)|| (GPIO_DRV_ReadPinInput(GPIO_MAKE_PIN(SDCARD_CARD_WRITE_PROTECTION_GPIO_PORT, SDCARD_CARD_WRITE_PROTECTION_GPIO_PIN))))
#else
    if (SDSPI_DRV_CheckReadOnly(&spi, &card))
#endif
    {
        PRINTF("Card is write-protected, skip writing \r\n");

        memset(gWriteData, 0, sizeof(gWriteData));
        if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&spi, &card, gReadData, 2, 1))
        {
            PRINTF("ERROR: SDSPI_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
        PRINTF("Single block read example passed!\r\n");
        memset(gWriteData, 0, sizeof(gWriteData));
        if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&spi, &card, gReadData, EXAMPLE_START_BLOCK, sizeof(gWriteData)/SDSPI_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("ERROR: SDSPI_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
        PRINTF("multiple block read example passed!\r\n");
        SDSPI_DRV_Shutdown(&spi, &card);
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
    }

    PRINTF("\r\nStart read/write example...\r\n");

    memset(gWriteData, 0, sizeof(gWriteData));
    if (fill_reference_data(gWriteData, 0x11, sizeof(gWriteData))) {
        PRINTF("ERROR: prepare reference data failed\r\n");
        SDSPI_DRV_Shutdown(&spi, &card);
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return;
    }

    if (kStatus_SDSPI_NoError != SDSPI_DRV_WriteBlocks(&spi, &card, gWriteData, 2, 1))
    {
        PRINTF("ERROR: SDSPI_DRV_WriteBlocks failed, line %d\r\n", __LINE__);
        SDSPI_DRV_Shutdown(&spi, &card);
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return;
    }

    if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&spi, &card, gReadData, 2, 1))
    {
        PRINTF("ERROR: SDSPI_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
        SDSPI_DRV_Shutdown(&spi, &card);
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return;
    }

    if (memcmp(gReadData, gWriteData, SDSPI_DEFAULT_BLOCK_SIZE))
    {
        PRINTF("ERROR: data comparison failed, line %d\r\n", __LINE__);
        SDSPI_DRV_Shutdown(&spi, &card);
        DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
        EDMA_DRV_Deinit();
#endif
        return;
    }
    PRINTF("Single block read/write example passed!\r\n");

    memset(gWriteData, 0, sizeof(gWriteData));
    for (i = 0; i < sizeof(gWriteData)/SDSPI_DEFAULT_BLOCK_SIZE; i++)
    {
        if (fill_reference_data(&gWriteData[i * SDSPI_DEFAULT_BLOCK_SIZE], 0x44 + i, SDSPI_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("ERROR: fill data failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
    }

    ms = OSA_TimeGetMsec();

    i = EXAMPLE_WRITE_LOOP_TIMES;
    while(i--)
    {
        if (kStatus_SDSPI_NoError != SDSPI_DRV_WriteBlocks(&spi, &card, gWriteData, EXAMPLE_START_BLOCK, sizeof(gWriteData)/SDSPI_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("ERROR: SDSPI_DRV_WriteBlocks failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
    }
    ms = OSA_TimeGetMsec() - ms;
    PRINTF("Writing %lu bytes for %u times in %u ms, at %u kB/s\r\n", sizeof(gWriteData), EXAMPLE_WRITE_LOOP_TIMES, ms, sizeof(gWriteData) * EXAMPLE_WRITE_LOOP_TIMES / ms);

    ms = OSA_TimeGetMsec();
    i = EXAMPLE_WRITE_LOOP_TIMES;
    while(i--)
    {
        if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&spi, &card, gReadData, EXAMPLE_START_BLOCK, sizeof(gReadData)/SDSPI_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("ERROR: SDSPI_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
    }
    ms = OSA_TimeGetMsec() - ms;
    PRINTF("Reading %lu bytes for %u times in %u ms, at %u kB/s\r\n", sizeof(gReadData), EXAMPLE_WRITE_LOOP_TIMES, ms, sizeof(gReadData) * EXAMPLE_WRITE_LOOP_TIMES / ms);

    j = EXAMPLE_WRITE_LOOP_TIMES;
    while(j--)
    {
        memset(gReadData, 0, sizeof(gReadData));
        if (kStatus_SDSPI_NoError != SDSPI_DRV_ReadBlocks(&spi, &card, gReadData, EXAMPLE_START_BLOCK, sizeof(gReadData)/SDSPI_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("ERROR: SDSPI_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }

        if (memcmp(gReadData, gWriteData, sizeof(gReadData)))
        {
            PRINTF("ERROR: data comparison failed, line %d\r\n", __LINE__);
            SDSPI_DRV_Shutdown(&spi, &card);
            DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
            EDMA_DRV_Deinit();
#endif
            return;
        }
    }
    PRINTF("Multi-block read/write example passed!\r\n");

    SDSPI_DRV_Shutdown(&spi, &card);
    DSPI_DRV_MasterDeinit(spi.spiInstance);
#if defined SPI_USING_DMA
    EDMA_DRV_Deinit();
#endif
}

/*
 * This example will check the function of the read/write single/multiple data block into sdcard.
 * Checking Sdcard status was lock/unlocked before execute and then correct datas that just wrote down.
 */

int main(void)
{
    hardware_init();

    OSA_Init();

    // init sync object for card detection
    OSA_SemaCreate(&cd, 0);

    // Set input, muxing, interrupting for card detection (CD) pin
    PRINTF("SPI SD Card Demo Start!\r\n\r\n");
    PRINTF("\r\n");

    demo_card_data_access(SDCARD_SPI_HW_INSTANCE);

    // free sync object used for card detection
    OSA_SemaDestroy(&cd);

    PRINTF("\r\nSPI SD Card Demo End!\r\n\r\n");

    while(1);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

