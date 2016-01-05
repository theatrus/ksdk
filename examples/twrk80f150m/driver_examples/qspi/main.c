/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
 //  Includes
 ///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "fsl_qspi_driver.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
     
 ////////////////////////////////////////////////////////////////////////////////
 // Definitions
 ////////////////////////////////////////////////////////////////////////////////
#define FLASH_PAGE_SIZE  (256)
#define FLASH_SECTORE_SIZE (4096)
#define FLASH_SIZE (0x00400000U)
#define DUMMY_MASTER (0xE)
 
 
 qspi_flash_config_t single_config = 
 {
     .flashA1Size = FLASH_SIZE, /* 4MB */
     .lookuptable = 
      {
         // Seq0 :Quad Read
         // CMD:        0xEB - Quad Read, Single pad
         // ADDR:       0x18 - 24bit address, Quad pads
         // DUMMY:      0x06 - 6 clock cyles, Quad pads
         // READ:       0x80 - Read 128 bytes, Quad pads
         // JUMP_ON_CS: 0
         [0] = 0x0A1804EB,
         [1] = 0x1E800E06,
         [2] = 0x2400,
 
         // Seq1: Write Enable
         // CMD:      0x06 - Write Enable, Single pad
         [4] = 0x406,
 
         // Seq2: Erase All
         // CMD:    0x60 - Erase All chip, Single pad
         [8] = 0x460,
 
         // Seq3: Read Status
         // CMD:    0x05 - Read Status, single pad
         // READ:   0x01 - Read 1 byte
         [12] = 0x1c010405,
 
         // Seq4: Page Program
         // CMD:    0x02 - Page Program, Single pad
         // ADDR:   0x18 - 24bit address, Single pad
         // WRITE:  0x80 - Write 128 bytes at one pass, Single pad
         [16] = 0x08180402,
         [17] = 0x2080,
 
         // Seq5: Write Register
         // CMD:    0x01 - Write Status Register, single pad
         // WRITE:  0x01 - Write 1 byte of data, single pad
         [20] = 0x20010401,
 
         // Seq6: Read Config Register
         // CMD:  0x05 - Read Config register, single pad
         // READ: 0x01 - Read 1 byte
         [24] = 0x1c010405,
 
         // Seq7: Erase Sector
         // CMD:  0x20 - Sector Erase, single pad
         // ADDR: 0x18 - 24 bit address, single pad
         [28] = 0x08180420,
         
         // Seq8: Dummy
         // CMD:    0xFF - Dummy command, used to force SPI flash to exit continuous read mode
         [32] = 0x4FF,
 
         // Seq9: Fast Single read
         // CMD:        0x0B - Fast Read, Single Pad
         // ADDR:       0x18 - 24bit address, Single Pad
         // DUMMY:      0x08 - 8 clock cyles, Single Pad
         // READ:       0x80 - Read 128 bytes, Single Pad
         // JUMP_ON_CS: 0
         [36] = 0x0818040B,
         [37] = 0x1C800C08,
         [38] = 0x2400,
 
         // Seq10: Fast Dual read
         // CMD:        0x3B - Dual Read, Single Pad
         // ADDR:       0x18 - 24bit address, Single Pad
         // DUMMY:      0x08 - 8 clock cyles, Single Pad
         // READ:       0x80 - Read 128 bytes, Dual pads
         // JUMP_ON_CS: 0
         [40] = 0x0818043B,
         [41] = 0x1D800C08,
         [42] = 0x2400,
      },
      .endian = kQspi64LittleEndian,
 };
 
 static uint32_t buff[64]; /* Test data */
 qspi_state_t state;

 ////////////////////////////////////////////////////////////////////////////////
 // Code
 ////////////////////////////////////////////////////////////////////////////////
 
 //Serial Flash dummy command
 void send_dummy()
 {
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_ExecuteFlashCommand(0,32);
 }
 
 //Check if serial flash erase or program finished.
 void check_if_finished()
 {
     uint32_t val = 0;
     //Check WIP bit
     do
     {
         while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
         QSPI_DRV_ClearFifo(0, kQspiRxFifo);
         QSPI_DRV_ExecuteFlashCommand(0,12);
         while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
         val = *(volatile uint32_t *)(FSL_FEATURE_QSPI_ARDB_BASE);
         //Clear ARDB area
         QSPI_DRV_ClearFlag(0, kQspiRxBufferDrain);
     }while(val & 0x1);
 }
 
 //Write enable command
 void cmd_write_enable()
 {
     send_dummy();
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_ExecuteFlashCommand(0,4);
 }
 
 //Enable Quad mode
 void enable_quad_mode()
 {
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_SetIPCommandAddr(0,FSL_FEATURE_QSPI_AMBA_BASE);
     //Clear Tx FIFO
     QSPI_DRV_ClearFifo(0, kQspiTxFifo);
     //Should write at least 16 bytes to TX FIFO
     uint32_t val[4] = {0x40,0x0,0x0,0x0};
     //Write enable
     cmd_write_enable();
     //Write data into TX FIFO
     QSPI_DRV_WriteDataBlocking(0, val, 16);
     //Set seq id, write register
     QSPI_DRV_ExecuteFlashCommand(0,20);
     //Wait until finished
     check_if_finished();
 }
 
 //Erase sector
 void erase_sector(uint32_t addr)
 {
     //Clear tx buffer
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_ClearFifo(0, kQspiTxFifo);
     QSPI_DRV_SetIPCommandAddr(0,addr);
     cmd_write_enable();
     QSPI_DRV_ExecuteFlashCommand(0,28);
     check_if_finished();
 }
 
 //Erase all command
 void erase_all()
 {
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_SetIPCommandAddr(0, FSL_FEATURE_QSPI_AMBA_BASE);
     /*Write enable*/
     cmd_write_enable();
     QSPI_DRV_ExecuteFlashCommand(0,8);
     check_if_finished();
 }
 
 //Program page into serial flash using QSPI polling way
 void program_page(uint32_t dest_addr, uint32_t *src_addr)
 {
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     QSPI_DRV_ClearFifo(0, kQspiTxFifo);
     QSPI_DRV_SetIPCommandAddr(0,dest_addr);
     cmd_write_enable();
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
     //First write some data into TXFIFO to prevent from underrun
     QSPI_DRV_WriteDataBlocking(0,src_addr, FSL_FEATURE_QSPI_TXFIFO_DEPTH * 4);
     src_addr += FSL_FEATURE_QSPI_TXFIFO_DEPTH;
 
     //Start the program
     QSPI_DRV_SetIPCommandSize(0, FLASH_PAGE_SIZE);
     QSPI_DRV_ExecuteFlashCommand(0,16);
 
     uint32_t leftLongWords = (FLASH_PAGE_SIZE- 16 * sizeof(uint32_t))>>2;
     QSPI_DRV_WriteDataBlocking(0, src_addr,leftLongWords * 4);
     //Wait until flash finished program
     check_if_finished();
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
     QSPI_DRV_SoftwareReset(0);
     while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
 }

 //Program page into serial flash, using qspi interrupt way.
void program_page_int(uint32_t dest_addr, uint32_t *src_addr)
{
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
    QSPI_DRV_ClearFifo(0, kQspiTxFifo);
    QSPI_DRV_SetIPCommandAddr(0,dest_addr);
    cmd_write_enable();
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
    QSPI_DRV_WriteDataEdma(0, src_addr, FLASH_PAGE_SIZE);
    
    //Start the program
    QSPI_DRV_SetIPCommandSize(0, FLASH_PAGE_SIZE);
    QSPI_DRV_ExecuteFlashCommand(0,16);
    //Wait until data all written into TXFIFO
    while(QSPI_DRV_GetTxStatus(0) == kStatus_QSPI_DeviceBusy)
    {}
    //Wait until flash finished program
    check_if_finished();
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
    QSPI_DRV_SoftwareReset(0);
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
}

//Program page into serial flash, using qspi interrupt way.
void program_page_dma(uint32_t dest_addr, uint32_t *src_addr)
{
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
    QSPI_DRV_ClearFifo(0, kQspiTxFifo);
    QSPI_DRV_SetIPCommandAddr(0,dest_addr);
    cmd_write_enable();
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy));
    QSPI_DRV_WriteDataEdma(0, src_addr, FLASH_PAGE_SIZE);

    /* Start the program */
    QSPI_DRV_SetIPCommandSize(0, FLASH_PAGE_SIZE);
    QSPI_DRV_ExecuteFlashCommand(0,16);
    //Wait until data all written into TXFIFO 
    while(QSPI_DRV_GetTxStatus(0) == kStatus_QSPI_DeviceBusy)
    {}
    //Wait until flash finished program
    check_if_finished();
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
    QSPI_DRV_SoftwareReset(0);
    while(QSPI_DRV_GetQspiStatus(0, kQspiBusy) && QSPI_DRV_GetQspiStatus(0, kQspiIPAccess));
}

//Use QSPI polling way to program serial flash
void qspi_polling(void)
{
    uint32_t i = 0, err = 0;
    erase_sector(FSL_FEATURE_QSPI_AMBA_BASE);
    PRINTF("Erase finished!\n\r");
    //Program a page
    program_page(FSL_FEATURE_QSPI_AMBA_BASE, buff);
    printf("Program data finished!\n\r");

    for (i = 0; i < 64; i ++)
    {
        if (((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i] != buff[i])
        {
            PRINTF("The data in %d is wrong!!\n\r",i);
            PRINTF("The flash value in %d is %d\n\r", i, ((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i]);
            err ++;
        }
    }
    if (err == 0)
    {
        PRINTF("\n\rProgram through QSPI polling succeed  !\n\r");
    }
}

//Use QSPI interrupt to program serial flash
void qspi_int(void)
{
    uint32_t i = 0, err = 0;
    erase_sector(FSL_FEATURE_QSPI_AMBA_BASE);
    PRINTF("Erase finished!\n\r");
    //Program a page
    program_page_int(FSL_FEATURE_QSPI_AMBA_BASE, buff);
    printf("Program data finished!\n\r");

    for (i = 0; i < 64; i ++)
    {
        //Read back the data through AHB bus
        if (((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i] != buff[i])
        {
            PRINTF("The data in %d is wrong!!\n\r",i);
            PRINTF("The flash value in %d is %d\n\r", i, ((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i]);
            err ++;
        }
    }
    if (err == 0)
    {
        PRINTF("\n\rProgram through QSPI Interrupt succeed !\n\r");
    }
}

//Use dma to program the serial flash
void qspi_dma(void)
{
    uint32_t i = 0, err = 0;
    erase_sector(FSL_FEATURE_QSPI_AMBA_BASE);
    PRINTF("Erase finished!\n\r");
    //Program a page
    program_page_dma(FSL_FEATURE_QSPI_AMBA_BASE, buff);
    printf("Program data finished!\n\r");

    for (i = 0; i < 64; i ++)
    {
        if (((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i] != buff[i])
        {
            PRINTF("The data in %d is wrong!!\n\r",i);
            PRINTF("The flash value in %d is %d\n\r", i, ((uint32_t *)FSL_FEATURE_QSPI_AMBA_BASE)[i]);
            err ++;
        }
    }
    if (err == 0)
    {
        PRINTF("\n\rProgram through QSPI DMA succeed !\n\r");
    }
}

int main()
{
    uint32_t i = 0;
    hardware_init();
    //Init EDMA module
    edma_state_t         edmaState;
    edma_user_config_t   edmaUserConfig;
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
    //Enable QSPI clock
    PRINTF("QSPI example started!\n\r");
    QSPI_DRV_Init(0, &state);
    //Get QSPI default settings and configure the qspi
    qspi_config_t config;
    QSPI_DRV_GetDefaultQspiConfig(&config);
    //Set dma source
    config.txSource = kDmaRequestMux0Group1QSPI0Tx;
    config.rxSource = kDmaRequestMux0Group1QSPI0Rx;
    //Set AHB buffer size for reading data through AHB bus
    config.AHBbufferSize[3] = FLASH_PAGE_SIZE;
    QSPI_DRV_ConfigQspi(0, &config);
    //According to serial flash feature to configure flash settings
    QSPI_DRV_ConfigFlash(0, &single_config);

    //Initialize data buffer
    for (i = 0; i < 64; i ++)
    {
        buff[i] = i;
    }
    enable_quad_mode();
    //Use polling,interrupt and dma way to program flash.
    while(1)
    {
        qspi_polling();
        qspi_int();
        qspi_dma();
        PRINTF("\r\nPress any key to continue again!\r\n");
        GETCHAR();
    }
}

