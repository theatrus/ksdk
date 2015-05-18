var group__sdhc__hal =
[
    [ "sdhc_hal_adma2_descriptor_t", "group__sdhc__hal.html#structsdhc__hal__adma2__descriptor__t", [
      [ "attribute", "group__sdhc__hal.html#a1e64d42b1a5d3a8ed2bf22ac9ced039f", null ],
      [ "address", "group__sdhc__hal.html#aeed43fe27f299b0c84b32c83f8121087", null ]
    ] ],
    [ "sdhc_hal_basic_info_t", "group__sdhc__hal.html#structsdhc__hal__basic__info__t", [
      [ "specVer", "group__sdhc__hal.html#a05823d81d9d2b53e00cf11ceaa9bd488", null ],
      [ "vendorVer", "group__sdhc__hal.html#ad4f972e321715eb5cd6891190be69cae", null ],
      [ "maxBlkLen", "group__sdhc__hal.html#abfc8670fa6aa52bf1c9d306411bfdc34", null ],
      [ "capability", "group__sdhc__hal.html#a141cd68b89fb65e2af9227e9b3772d54", null ]
    ] ],
    [ "sdhc_hal_sdclk_config_t", "group__sdhc__hal.html#structsdhc__hal__sdclk__config__t", [
      [ "enable", "group__sdhc__hal.html#a82da05402ba77e6c8c7d61ef83322062", null ],
      [ "maxHostClk", "group__sdhc__hal.html#a3036c7f822843927b8d2bc72914710e7", null ],
      [ "destClk", "group__sdhc__hal.html#ae894abda6e8aa535cfe4af9473c3044b", null ]
    ] ],
    [ "sdhc_mmcboot_param_t", "group__sdhc__hal.html#structsdhc__mmcboot__param__t", [
      [ "ackTimeout", "group__sdhc__hal.html#ad22b98652e482554a3eb19d2997d5fe8", null ],
      [ "mode", "group__sdhc__hal.html#adca0646c9dabd37dcab69e9c2f95c270", null ],
      [ "blockCount", "group__sdhc__hal.html#aa83b7b606746547227f80074a593decd", null ],
      [ "enFlags", "group__sdhc__hal.html#a8d3df4dca8a36a3f162bd3a92f280171", null ]
    ] ],
    [ "sdhc_hal_config_t", "group__sdhc__hal.html#structsdhc__hal__config__t", [
      [ "ledState", "group__sdhc__hal.html#a5121a6a6ec1fa0b1ba3e5484c754f0f3", null ],
      [ "endianMode", "group__sdhc__hal.html#a09ea007c5fdb165319a6850f1b933498", null ],
      [ "dmaMode", "group__sdhc__hal.html#aa1be1580f824eb06f29cf27e3f8da978", null ],
      [ "writeWatermarkLevel", "group__sdhc__hal.html#adb50822b97194c6e853cc49afdcf2cbd", null ],
      [ "readWatermarkLevel", "group__sdhc__hal.html#a643403d58c6dd6c94719500af472eecf", null ],
      [ "enFlags", "group__sdhc__hal.html#a8042faaac551a43866ae71b6a503efa0", null ],
      [ "bootParams", "group__sdhc__hal.html#a0a894e43591d19cbf20e5780345e8b64", null ]
    ] ],
    [ "sdhc_hal_cmd_req_t", "group__sdhc__hal.html#structsdhc__hal__cmd__req__t", [
      [ "dataBlkSize", "group__sdhc__hal.html#a50f3720a789101aa4e692e3569e324e7", null ],
      [ "dataBlkCount", "group__sdhc__hal.html#ab7fbde1bf32e258786021a313fbfa9c8", null ],
      [ "arg", "group__sdhc__hal.html#afc5e3e13b9f6ec26fe7848b897fec7a3", null ],
      [ "index", "group__sdhc__hal.html#a8d1e28772ae70f7108d5b8b651d6a056", null ],
      [ "flags", "group__sdhc__hal.html#a148d890d85e3595c20a420241b70fb12", null ]
    ] ],
    [ "SDHC_HAL_ADMA1_ADDR_ALIGN", "group__sdhc__hal.html#ga94a8c05b5416cefed931dec1cb2c5fd0", null ],
    [ "sdhc_hal_mmcboot_t", "group__sdhc__hal.html#gac9affd5bc78046a46e0ce4c5b3c7a759", null ],
    [ "sdhc_hal_led_t", "group__sdhc__hal.html#gad141b05ea192211e52e01fb5888a018b", null ],
    [ "sdhc_hal_dtw_t", "group__sdhc__hal.html#ga1b2abe4e9cba2ed6f6f8021613c2dc7a", null ],
    [ "sdhc_hal_endian_t", "group__sdhc__hal.html#ga773e8cc0c854cb041ce6c64545ed9b57", null ],
    [ "sdhc_hal_dma_mode_t", "group__sdhc__hal.html#ga3c91df5d99e24d84b4272cb67fe04975", null ],
    [ "sdhc_hal_curstat_type_t", "group__sdhc__hal.html#gabc69135b92ea32489801b9a906912748", [
      [ "kSdhcHalIsCmdInhibit", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a62ad54c24e80478255461266d9551630", null ],
      [ "kSdhcHalIsDataInhibit", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a81ce50981bb440bd202666e031e265ec", null ],
      [ "kSdhcHalIsDataLineActive", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748ae39230bcd41bb37cedb7f672570d3818", null ],
      [ "kSdhcHalIsSdClockStable", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748af45744b431d62c13d90f785a8a48d598", null ],
      [ "kSdhcHalIsIpgClockOff", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a26a0420408ac2694a3f2dac9c2cbee4d", null ],
      [ "kSdhcHalIsSysClockOff", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a8d4285a4dd1e6b1856ffbadf975d4ad3", null ],
      [ "kSdhcHalIsPeripheralClockOff", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748ac3125340997be281741e600fda76aac1", null ],
      [ "kSdhcHalIsSdClkOff", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748ac943a722467b2c932b7375972d0d4bda", null ],
      [ "kSdhcHalIsWriteTransferActive", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748aed2ed28d48d471040056bb4d55287695", null ],
      [ "kSdhcHalIsReadTransferActive", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748acb63ad953a9ddbbece7c9c8726f0658c", null ],
      [ "kSdhcHalIsBuffWriteEnabled", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a1fbe53a3302aa03b81697ae01db37081", null ],
      [ "kSdhcHalIsBuffReadEnabled", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748aaa5a318297b91b89a73dce6e3d22254f", null ],
      [ "kSdhcHalIsCardInserted", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a2ac546ab578ed17484ac3f19590d25a5", null ],
      [ "kSdhcHalIsCmdLineLevelHigh", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748aecc9cd9e74008362741d44b29077e8bf", null ],
      [ "kSdhcHalGetDataLine0Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a901d11ecd3aa7646cf1ad691d98676d5", null ],
      [ "kSdhcHalGetDataLine1Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a26221ea2700bc89938a6573eb5499214", null ],
      [ "kSdhcHalGetDataLine2Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a6c70755e24976f96b496f0c858ffecbc", null ],
      [ "kSdhcHalGetDataLine3Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a073d3c75b1fa578e643994b5e09c7564", null ],
      [ "kSdhcHalGetDataLine4Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a371ec209858cf72c055cac50dcf97616", null ],
      [ "kSdhcHalGetDataLine5Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a8d82a2e9f934dfb980a6a48c6b86c38c", null ],
      [ "kSdhcHalGetDataLine6Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a572aadd287698216d7bb85fc85a96c35", null ],
      [ "kSdhcHalGetDataLine7Level", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748af3dec876069680f1b17fc538ab85642a", null ],
      [ "kSdhcHalGetCdTestLevel", "group__sdhc__hal.html#ggabc69135b92ea32489801b9a906912748a0df8d982f2426282c56c98c04112bdca", null ]
    ] ],
    [ "sdhc_hal_err_type_t", "group__sdhc__hal.html#ga44ffbfc01a0e684f6ab069347e871e8f", [
      [ "kAc12Err", "group__sdhc__hal.html#gga44ffbfc01a0e684f6ab069347e871e8fa4408992434d9a182294c2d47bf81713c", null ],
      [ "kAdmaErr", "group__sdhc__hal.html#gga44ffbfc01a0e684f6ab069347e871e8fa9095bea2528eb5897626f54e12d77adf", null ]
    ] ],
    [ "SDHC_HAL_SendCmd", "group__sdhc__hal.html#gabe733ec4923787104ff5168bf7cdabf7", null ],
    [ "SDHC_HAL_SetData", "group__sdhc__hal.html#gab318fb35a3b2f18cc50a9cfdb438287a", null ],
    [ "SDHC_HAL_GetData", "group__sdhc__hal.html#ga615189e23d7a834d0b5e634c0f406c99", null ],
    [ "SDHC_HAL_GetCurState", "group__sdhc__hal.html#gaf08d5a02b0af8bea28b9689812c3a62f", null ],
    [ "SDHC_HAL_SetDataTransferWidth", "group__sdhc__hal.html#gafad5a521327bc79cf96cd2d0a46f7860", null ],
    [ "SDHC_HAL_SetContinueRequest", "group__sdhc__hal.html#gac51e82b778e228f520f4136351a264c9", null ],
    [ "SDHC_HAL_Config", "group__sdhc__hal.html#gae315c4db48bf048bac4636152a438f58", null ],
    [ "SDHC_HAL_ConfigSdClock", "group__sdhc__hal.html#ga84153155652abcbd423fe26f7b2bd3f8", null ],
    [ "SDHC_HAL_GetIntFlags", "group__sdhc__hal.html#ga1817bcf051fbcaa8bd7560fc77a618f9", null ],
    [ "SDHC_HAL_ClearIntFlags", "group__sdhc__hal.html#ga89182f2f0362cdbc4ed569ec98cdf567", null ],
    [ "SDHC_HAL_GetAllErrStatus", "group__sdhc__hal.html#gabeb981bf44b0d9de2a8e4d297cfc3683", null ],
    [ "SDHC_HAL_SetForceEventFlags", "group__sdhc__hal.html#ga2fad9b8532fc441cc7e81fe570129276", null ],
    [ "SDHC_HAL_SetAdmaAddress", "group__sdhc__hal.html#ga2f4daaa2983db5dbcc77c927d8f3b61e", null ],
    [ "SDHC_HAL_GetResponse", "group__sdhc__hal.html#ga53c8593881ced4c20dab0477e56bb3bf", null ],
    [ "SDHC_HAL_SetIntSignal", "group__sdhc__hal.html#gabfacaeb84a546da1801289b0c9385f7c", null ],
    [ "SDHC_HAL_SetIntState", "group__sdhc__hal.html#ga79f96e224ace1e6e846b290696401ca9", null ],
    [ "SDHC_HAL_Reset", "group__sdhc__hal.html#gac48b48ee84846fe957a8b715b41ed86a", null ],
    [ "SDHC_HAL_InitCard", "group__sdhc__hal.html#gaa5d01ed8ba43a6bf711501c72f1ba5b2", null ],
    [ "SDHC_HAL_Init", "group__sdhc__hal.html#ga78a2b6c7de87fd2dbd57393c4f0fda2f", null ],
    [ "SDHC_HAL_GetBasicInfo", "group__sdhc__hal.html#gacd8fd80ac5345816bf85d6443a5abbef", null ]
];