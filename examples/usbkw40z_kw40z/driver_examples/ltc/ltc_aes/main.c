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
 /*
 * This example will show how to easily use LTC driver to encrypt/decrypt some data
 * using AES-CBC and AES-CCM algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"
#include "fsl_ltc_driver.h"
#include "fsl_pit_driver.h"

#if FSL_FEATURE_SOC_LMEM_COUNT
#include "fsl_lmem_cache_driver.h"
#endif

///////////////////////////////////////////////////////////////////////////////
//  Defines
///////////////////////////////////////////////////////////////////////////////
#define OUTPUT_ARRAY_LEN        512
#define LTC_TIMER_PIT_CHANNEL   1
#define RELOAD                  0xFFFFFFFF
#define AES_BLOCK_LENGTH        16
#define AES128                  128
#define AES128_KEY_SIZE         16
#define CCM_PLAIN_SIZE          32
#define CCM_IV_SIZE             12
#define CCM_AAD_SIZE            20
#define CCM_KEY_SIZE            16
#define CCM_TAG_SIZE            8
#define CMAC_HASH_SIZE          16
#define CMAC_KEY_SIZE           16

/* macro to conversion hw ticks to uSeconds */
#define TICKStoUS(ticks) \
    ticks/(pit_freq/1000000)

/* PIT timer used to measure elapsed time */
PIT_Type * const pitBase[] = PIT_BASE_PTRS;
/* pit timer frequency */
static unsigned int pit_freq;
/* Sample text length */
static unsigned int length;
/* LTC peripheral instance number */
static unsigned int ltc_instance;

/*8-byte multiple*/
static const uint8_t aes_test_full[] = \
  "Once upon a midnight dreary, while I pondered weak and weary,"
  "Over many a quaint and curious volume of forgotten lore,"
  "While I nodded, nearly napping, suddenly there came a tapping,"
  "As of some one gently rapping, rapping at my chamber door"
  "Its some visitor, I muttered, tapping at my chamber door"
  "Only this, and nothing more.";

/*16 bytes key: "ultrapassword123"*/
static const uint8_t key128[AES128_KEY_SIZE] = { 0x75, 0x6c, 0x74, 0x72, 0x61, 0x70, 0x61, 0x73,
                                                 0x73, 0x77, 0x6f, 0x72, 0x64, 0x31, 0x32, 0x33 };
/*initialization vector: 16 bytes: "mysecretpassword"*/
static uint8_t ive[AES_BLOCK_LENGTH] = { 0x6d,0x79,0x73,0x65,0x63,0x72,0x65,0x74,
                                               0x70,0x61,0x73,0x73,0x77,0x6f,0x72,0x64 };

static uint8_t cipher[OUTPUT_ARRAY_LEN];
static uint8_t output[OUTPUT_ARRAY_LEN];

/* Function prototypes */
static void ltc_timer_init(void);
static void ltc_timer_start(void);
static void ltc_print_msg(uint8_t *data, uint32_t length);
static void ltc_example_task(void);
static void ltc_aes_cbc_exam(void);
static void ltc_aes_ccm_exam(void);
static void ltc_aes_cmac_exam(void);
static unsigned int ltc_timer_stop(void);

///////////////////////////////////////////////////////////////////////////////
//  Codes
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Function ltc_print_msg prints data into console.
 */
static void ltc_print_msg(uint8_t *data, uint32_t length)
{
    uint32_t i;

    PRINTF("          ");
    for (i = 0; i < length; i++)
    {
        PUTCHAR(data[i]);
        if (data[i] == ',')
        {
            PRINTF("\r\n          ");
        }
    }
    PRINTF("\r\n");
}

/*!
 * @brief Function ltc_timer_init initialize PIT timer
 * to measure time.
 */
static void ltc_timer_init(void)
{
    SIM_HAL_EnableClock(SIM, kSimClockGatePit0);
    PIT_HAL_Enable(pitBase[0]);
    PIT_HAL_StopTimer(pitBase[0], LTC_TIMER_PIT_CHANNEL);
    PIT_HAL_SetTimerPeriodByCount(pitBase[0], LTC_TIMER_PIT_CHANNEL, RELOAD);
    PIT_HAL_SetIntCmd(pitBase[0], LTC_TIMER_PIT_CHANNEL, false);
    PIT_HAL_SetTimerRunInDebugCmd(pitBase[0], false); /* timer stop counting in debug mode */
    PIT_HAL_ClearIntFlag(pitBase[0], LTC_TIMER_PIT_CHANNEL);
}

/*!
 * @brief Function ltc_timer_stop halt pit timer
 * and returns elapsed ticks.
 */
static unsigned int ltc_timer_stop(void)
{
    uint32_t curr_ticks = PIT_HAL_ReadTimerCount(pitBase[0], LTC_TIMER_PIT_CHANNEL);
    PIT_HAL_StopTimer(pitBase[0], LTC_TIMER_PIT_CHANNEL);
    return (0xFFFFFFFFu - curr_ticks);
}

/*!
 * @brief Function ltc_timer_start start pit timer.
 */
static void ltc_timer_start(void)
{
    PIT_HAL_StartTimer(pitBase[0], LTC_TIMER_PIT_CHANNEL);
}

/*!
 * @brief Function ltc_aes_cbc_exam demonstrates
   use of AES CBC method, measure elapsed time and
   print decrypted text.
 */
static void ltc_aes_cbc_exam(void)
{
    ltc_status_t status;
    unsigned int ticks;

    PRINTF("----------------------------------- AES-CBC method --------------------------------------\r\n");
    /*   ENCRYPTION   */
    PRINTF("AES CBC Encryption of %u bytes.\r\n", length);
    /* Start timer to measure ticks */
    ltc_timer_start();
    /* Turn on the led1 */
    LED1_ON;
    /* Call AES_cbc encryption */
    status = LTC_DRV_aes_encrypt_cbc(ltc_instance, aes_test_full, length, ive, key128,
                                  AES128_KEY_SIZE, cipher);
    if (status != kStatus_LTC_Success)
    {
        PRINTF("AES CBC encryption failed !\r\n");
        return;
    }
    /* turn off the led1 */
    LED1_OFF;
    /* Get elapsed ticks */
    ticks = ltc_timer_stop();
    /* Result message */
    PRINTF("AES CBC encryption finished in %u ticks / %u useconds \r\n\r\n",
            ticks, TICKStoUS(ticks));

    /*   DECRYPTION   */
    PRINTF("AES CBC Decryption of %u bytes.\r\n", length);
    /* Start timer to measure ticks */
    ltc_timer_start();
    /* Turn on the led1 */
    LED1_ON;
    /* Call AES_cbc decryption */
    status = LTC_DRV_aes_decrypt_cbc(ltc_instance, cipher, length, ive, key128,
                                  AES128_KEY_SIZE, kLtcEncryptKey, output);
    if (status != kStatus_LTC_Success)
    {
        PRINTF("AES CBC decryption failed !\r\n");
        return;
    }
    /* turn off the led1 */
    LED1_OFF;
    /* Get elapsed ticks */
    ticks = ltc_timer_stop();
    /* Result message */
    PRINTF("AES CBC decryption finished in %u ticks / %u useconds \r\n",
           ticks, TICKStoUS(ticks));
    /* Print decrypted string */
    PRINTF("Decrypted string :\r\n");
    ltc_print_msg(output, length);

    /* Just format console */
    PRINTF("\r\n");
}

/*!
 * @brief Function ltc_aes_ccm_exam demonstrates
   use of AES CCM method, measure elapsed time and
   print decrypted text.
 */
static void ltc_aes_ccm_exam(void)
{
    unsigned int j, i;
    unsigned int ticks;
    uint8_t nonce[] = {0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
    uint8_t key[CCM_KEY_SIZE] = {0};
    uint8_t aad[CCM_AAD_SIZE];
    uint8_t tag[CCM_TAG_SIZE] = {0};

    PRINTF("----------------------------------- AES-CCM method --------------------------------------\r\n");
    /* Generate sample key and aad */
    j = 0;
    for ( i=0x40; i<=0x4f; i++)
        key[j++] = (uint8_t)i;

    for ( i=0x0; i<=0x13; i++)
        aad[i] = (uint8_t)i;
    /* clear working buffers */
    memset(&cipher[0], 0, length);
    memset(&output[0], 0, length);

    /*   ENCRYPTION   */
    PRINTF("AES CCM Encryption of  %u bytes.\r\n", length);
    PRINTF("    using iv length  : %u bytes \r\n", CCM_IV_SIZE);
    PRINTF("    using aad length : %u bytes \r\n", CCM_AAD_SIZE);
    PRINTF("    using key length : %u bytes \r\n", CCM_KEY_SIZE);
    PRINTF("    using tag length : %u bytes \r\n", CCM_TAG_SIZE);

    /* Start timer to measure ticks */
    ltc_timer_start();
    /* Turn on user led */
    LED1_ON;
    /* Call AES CCM encryption */
    if (kStatus_LTC_Success!=
        LTC_DRV_aes_encrypt_tag_ccm(ltc_instance, &aes_test_full[0], length, &nonce[0], CCM_IV_SIZE,
                            &aad[0], CCM_AAD_SIZE, &key[0], CCM_KEY_SIZE, &cipher[0], &tag[0], CCM_TAG_SIZE))
    {
        PRINTF("AES CCM encryption failed !!!\r\n");
        return;
    }
    /* turn off the led1 */
    LED1_OFF;
    /* Get elapsed ticks */
    ticks = ltc_timer_stop();
    /* Result message */
    PRINTF("AES CCM encryption finished in %u ticks / %u useconds \r\n\r\n", ticks, TICKStoUS(ticks));

    /*   DECRYPTION   */
    PRINTF("AES CCM decryption of %u bytes.\r\n", length);
    /* Start timer to measure ticks */
    ltc_timer_start();
    /* Turn on the led1 */
    LED1_ON;
    /* Call AES CCM decryption */
    if (kStatus_LTC_Success !=
        LTC_DRV_aes_decrypt_tag_ccm(ltc_instance, &cipher[0], length, &nonce[0], CCM_IV_SIZE,
                    &aad[0], CCM_AAD_SIZE, &key[0], CCM_KEY_SIZE, &output[0], &tag[0], CCM_TAG_SIZE))
    {
        PRINTF("AES CCM decryption failed !!!\r\n");
        return;
    }
    /* turn off the led1 */
    LED1_OFF;
    /* Get elapsed ticks */
    ticks = ltc_timer_stop();
    /* Result message */
    PRINTF("AES CCM decryption finished in %u ticks / %u useconds \r\n", ticks, TICKStoUS(ticks));
    PRINTF("Decrypted string: \r\n");
    ltc_print_msg(output, length);
    PRINTF("\r\n");
}

/*!
 * @brief Function ltc_aes_cmac_exam demonstrates
   use of AES CMAC functionality. Initializes CMAC driver and
   put sample text to compute hash.
 */
static void ltc_aes_cmac_exam(void)
{
    unsigned int i, j, hash_size;
    uint8_t key[CMAC_KEY_SIZE] = {0};
    uint8_t hash[CMAC_HASH_SIZE] = {0};
    ltc_drv_hash_ctx ctx;   /* CMAC context */

    PRINTF("----------------------------------- AES-XCBC-MAC --------------------------------------\r\n");
    PRINTF("AES XCBC-MAC Computing hash of %u bytes \r\n", length);

    /* Create sample key */
    j = 0;
    for ( i=0x40; i<=0x4f; i++)
        key[j++] = (uint8_t)i;

    /* Initialize the CMAC with a given key, and selects a block cipher to use. */
    if (kStatus_LTC_Success != LTC_DRV_hash_init(ltc_instance, &ctx, kLtcXcbcMac, &key[0], CMAC_KEY_SIZE))
    {
        PRINTF("Initialization of LTC HASH driver error\r\n");
        return;
    }
    /* Call CMAC hash update to add data to create hash.
       It can be called once with whole array size, but this shows that
       function update can be called asynchronously (e.g during receiving some data).
    */
    for (i = 0; i < length; i++)
    {   /* cmac context, data, data length */
        if (kStatus_LTC_Success != LTC_DRV_hash_update(&ctx, &aes_test_full[0], 1))
        {
            PRINTF("Update input hash data error\r\n");
            return;
        }
    }
    /* Save hash size into variable, because in case that hash_size was set bigger
       as 16 bytes, driver will re-set to 16 bytes length */
    hash_size = CMAC_HASH_SIZE;

    /* Call LTC_DRV_hash_finish function to get counted hash.
       standard(maximum) hash size is 16 bytes */
    if (kStatus_LTC_Success != LTC_DRV_hash_finish(&ctx, &hash[0], (uint32_t*)&hash_size))\
    {
        PRINTF("Get final hash error\r\n");
        return;
    }

    PRINTF("Computed hash:\r\n");
    /* Print hash */
    for (i=0;i<hash_size;i++)
        PRINTF("%.2x ", hash[i]);
    /* Format console output */
    PRINTF("\r\n\r\n");
}

/*!
 * @brief Function ltc_example_task demonstrates use of LTC driver
 * encryption/decryption functions on short sample text. Function also
 * measures elapsed time during encryption/decryption, prints result to terminal and toggles on-board led.
 */
static void ltc_example_task(void)
{
    ltc_status_t status;
    pit_freq = 1;


    /* Set default ltc device instance num */
#if !FSL_FEATURE_SOC_LTC_COUNT
    PRINTF("Unsupported platform selected !\r\n");
    return;
#endif
    ltc_instance = 0;

    /* Initialize LTC driver.
     * This enables clocking and resets the module to a known state. */
    status = LTC_DRV_Init(ltc_instance);

    if (status == kStatus_LTC_Success)
    {
    #if FSL_FEATURE_LTC_HAS_DPAMS
        LTC_DRV_SetDPAMaskSeed(ltc_instance, SIM_RD_UIDL(SIM));
    #endif /* FSL_FEATURE_LTC_HAS_DPAMS */
    #if FSL_FEATURE_SOC_LMEM_COUNT
        LMEM_DRV_CodeCacheEnable(0);
        LMEM_DRV_SystemCacheEnable(0);
    #endif
    }

    /* Initialize pit timer to measure ltc performance */
    ltc_timer_init();
    /* Get PIT timer input frequency */
    pit_freq = CLOCK_SYS_GetPitFreq(0);
    /* Initialize led */
    LED1_EN;
    /* Print welcome string */
    PRINTF("............................. LTC (AES)  DRIVER  EXAMPLE ............................. \r\n\r\n");
    PRINTF("Measuring timer is driven %u MHz. (tick = %u picoSeconds) \r\n",
            pit_freq/1000000, 1000000000/pit_freq);

    memset(&output[0], 0, OUTPUT_ARRAY_LEN);
    memset(&cipher[0], 0, OUTPUT_ARRAY_LEN);
    length = strlen((const char *) aes_test_full);

    PRINTF("Testing input string: \r\n");
    ltc_print_msg((uint8_t*)&aes_test_full[0], length);
    /* Format console output */
    PRINTF("\r\n");

    /******* FIRST PART USING AES-CBC method *********/
    ltc_aes_cbc_exam();

    /******* SECOND PART USING AES-CCM method ********/
    ltc_aes_ccm_exam();

    /******* THIRD PART USES HASH FUNCTIONALITY ******/
    ltc_aes_cmac_exam();

    /* Goodbye message */
    PRINTF(".............. THE  END  OF  THE  LTC (AES)  DRIVER  EXAMPLE ................................\r\n");
}


int main(void)
{
    /* Initialize board hardware */
    hardware_init();
    OSA_Init();
	/* Initialize debug serial interface */
    dbg_uart_init();
    /* Call example task */
    ltc_example_task();

    while(1);
}
