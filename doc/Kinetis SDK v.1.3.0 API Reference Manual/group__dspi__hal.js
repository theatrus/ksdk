var group__dspi__hal =
[
    [ "dspi_data_format_config_t", "group__dspi__hal.html#structdspi__data__format__config__t", [
      [ "bitsPerFrame", "group__dspi__hal.html#a99d8d0dec68d5d2c1de35f8bb8b66a75", null ],
      [ "clkPolarity", "group__dspi__hal.html#a8926c27980536d08b063f984cc5dc751", null ],
      [ "clkPhase", "group__dspi__hal.html#ad751cbe15bfe01f02088e2a36059774a", null ],
      [ "direction", "group__dspi__hal.html#a7b5fc2246586c461ebd0325bba2e3113", null ]
    ] ],
    [ "dspi_baud_rate_divisors_t", "group__dspi__hal.html#structdspi__baud__rate__divisors__t", [
      [ "doubleBaudRate", "group__dspi__hal.html#a0f48ca03566631b4a13e24450644df29", null ],
      [ "prescaleDivisor", "group__dspi__hal.html#a02c5427e2aac3b69148d10313355ef49", null ],
      [ "baudRateDivisor", "group__dspi__hal.html#a7ae1b66ab2f4a6d2d1208cb28fe4f286", null ]
    ] ],
    [ "dspi_command_config_t", "group__dspi__hal.html#structdspi__command__config__t", [
      [ "isChipSelectContinuous", "group__dspi__hal.html#a6755300868dc9a42caf2d60f7b639b88", null ],
      [ "whichCtar", "group__dspi__hal.html#ac142820d982b6ba9957cda219f75c622", null ],
      [ "whichPcs", "group__dspi__hal.html#a5d159d52ac265c93454da2789f17aa01", null ],
      [ "isEndOfQueue", "group__dspi__hal.html#a54b6b3382baca02c2c60e6d44d72f1cf", null ],
      [ "clearTransferCount", "group__dspi__hal.html#aa9ec273f1b798a04c27b327062f369cd", null ]
    ] ],
    [ "dspi_status_t", "group__dspi__hal.html#ga34b3b8e88f67e380c7e5f50e2422774c", [
      [ "kStatus_DSPI_SlaveTxUnderrun", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca9e7142dae4811be5426a39b6ddb40c31", null ],
      [ "kStatus_DSPI_SlaveRxOverrun", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774caba716de1ad68e6351cd2d26d0985b2e6", null ],
      [ "kStatus_DSPI_Timeout", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca2509eed6de02d7a5c463d3c7b8b68eef", null ],
      [ "kStatus_DSPI_Busy", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774cad5ecc8346da4119b8609c6bcb4c57e40", null ],
      [ "kStatus_DSPI_NoTransferInProgress", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774cacec0b51b1aaabedb0343cdbb07754d01", null ],
      [ "kStatus_DSPI_InvalidBitCount", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca41231c8e5ed53006eaa1a49ed5a5a904", null ],
      [ "kStatus_DSPI_InvalidInstanceNumber", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca6c53f1865e5e78c67f06ff1238b27fdc", null ],
      [ "kStatus_DSPI_OutOfRange", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774cac1713712f0410e28da008d714734a6bd", null ],
      [ "kStatus_DSPI_InvalidParameter", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774caf9d86eb05e2c36c02d48a0a7d7211f54", null ],
      [ "kStatus_DSPI_NonInit", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca7ed3bba3d9e8ca3004a606ba549f2207", null ],
      [ "kStatus_DSPI_Initialized", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774caceee7c3c74451633151cf7525bc25c43", null ],
      [ "kStatus_DSPI_DMAChannelInvalid", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774ca4e0cc112020f92b3d0d6650969750f11", null ],
      [ "kStatus_DSPI_Error", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774caaae2ba17a6a622142816b0ffec7b9f7a", null ],
      [ "kStatus_DSPI_EdmaStcdUnaligned32Error", "group__dspi__hal.html#gga34b3b8e88f67e380c7e5f50e2422774cadee62efed1219cb7f4d77f050a7cbc2d", null ]
    ] ],
    [ "dspi_master_slave_mode_t", "group__dspi__hal.html#gad7f974015f32db057dafada8b95641aa", [
      [ "kDspiMaster", "group__dspi__hal.html#ggad7f974015f32db057dafada8b95641aaabf70091c32d55b88b5cdf4993fd252b6", null ],
      [ "kDspiSlave", "group__dspi__hal.html#ggad7f974015f32db057dafada8b95641aaaff050dc09b18c67f40105d176c787e0a", null ]
    ] ],
    [ "dspi_clock_polarity_t", "group__dspi__hal.html#ga1e0a9074742794ef89f597d220296651", [
      [ "kDspiClockPolarity_ActiveHigh", "group__dspi__hal.html#gga1e0a9074742794ef89f597d220296651afffdb6de18c3b46c509c406ac7230586", null ],
      [ "kDspiClockPolarity_ActiveLow", "group__dspi__hal.html#gga1e0a9074742794ef89f597d220296651a415b9652394fc6b7c50f5d5377426b13", null ]
    ] ],
    [ "dspi_clock_phase_t", "group__dspi__hal.html#ga4269ec144334dd60666a92e6fd2c1476", [
      [ "kDspiClockPhase_FirstEdge", "group__dspi__hal.html#gga4269ec144334dd60666a92e6fd2c1476ada723d5970a0b0a28d96a0e707cabd9c", null ],
      [ "kDspiClockPhase_SecondEdge", "group__dspi__hal.html#gga4269ec144334dd60666a92e6fd2c1476a4f1c4fe8e246d3a87f60c99d214d8921", null ]
    ] ],
    [ "dspi_shift_direction_t", "group__dspi__hal.html#ga06fad8ae17b680f6dddfd798c9d3b30d", [
      [ "kDspiMsbFirst", "group__dspi__hal.html#gga06fad8ae17b680f6dddfd798c9d3b30daed75ebb641f649ff0c4eb77d2624bcf2", null ],
      [ "kDspiLsbFirst", "group__dspi__hal.html#gga06fad8ae17b680f6dddfd798c9d3b30da582f124b275827ec23b5e4a1a70b25a7", null ]
    ] ],
    [ "dspi_ctar_selection_t", "group__dspi__hal.html#ga992d5562af4cf4c45371feb8c5c1a1bf", [
      [ "kDspiCtar0", "group__dspi__hal.html#gga992d5562af4cf4c45371feb8c5c1a1bfa21d94da5d7c4e6134907fad358147b35", null ],
      [ "kDspiCtar1", "group__dspi__hal.html#gga992d5562af4cf4c45371feb8c5c1a1bfa46f7fb3102bd17be1feeec4ef83f9573", null ]
    ] ],
    [ "dspi_pcs_polarity_config_t", "group__dspi__hal.html#gab466e73cb54b2c023459d43918c4197d", [
      [ "kDspiPcs_ActiveHigh", "group__dspi__hal.html#ggab466e73cb54b2c023459d43918c4197dac82558209fcd03cfcff410f43ccd13df", null ],
      [ "kDspiPcs_ActiveLow", "group__dspi__hal.html#ggab466e73cb54b2c023459d43918c4197daf89a174f7da2c2d1dd94eaee5f451ac7", null ]
    ] ],
    [ "dspi_which_pcs_config_t", "group__dspi__hal.html#ga3d9d9a8fe16a51e6c697a547ab65bef1", [
      [ "kDspiPcs0", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1abf98b398d467bec5ce1332e7769b9c97", null ],
      [ "kDspiPcs1", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1aae6b37fc82d8d2b7425b85dd63172acf", null ],
      [ "kDspiPcs2", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1aa57c28d2307f3a08d2ca37fede94704b", null ],
      [ "kDspiPcs3", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1a009e764ff551a29673559bdf51e68b37", null ],
      [ "kDspiPcs4", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1a68dbb918dfd1398b2b161142e99a2b76", null ],
      [ "kDspiPcs5", "group__dspi__hal.html#gga3d9d9a8fe16a51e6c697a547ab65bef1a4fa2a14f449a06e76693a89b6861157c", null ]
    ] ],
    [ "dspi_master_sample_point_t", "group__dspi__hal.html#gae783895e2917abe07adbe27a253510a2", [
      [ "kDspiSckToSin_0Clock", "group__dspi__hal.html#ggae783895e2917abe07adbe27a253510a2a6031fe2cc4a3f05a525ed515736fe8fc", null ],
      [ "kDspiSckToSin_1Clock", "group__dspi__hal.html#ggae783895e2917abe07adbe27a253510a2a369e0c0de897725cc26ecbe774c52cea", null ],
      [ "kDspiSckToSin_2Clock", "group__dspi__hal.html#ggae783895e2917abe07adbe27a253510a2a22ba66b2d6cfc7ab9e75415894aa457d", null ]
    ] ],
    [ "dspi_dma_or_int_mode_t", "group__dspi__hal.html#ga3c93ad318e54430c1230fc1dfff06894", [
      [ "kDspiGenerateIntReq", "group__dspi__hal.html#gga3c93ad318e54430c1230fc1dfff06894ae6f189b379c53dc2ddc1ac1b509dae9e", null ],
      [ "kDspiGenerateDmaReq", "group__dspi__hal.html#gga3c93ad318e54430c1230fc1dfff06894a3bb79d3c56c6110432d38dddef054344", null ]
    ] ],
    [ "dspi_status_and_interrupt_request_t", "group__dspi__hal.html#ga4eede4254ab5c5dcc6455290794df4bb", [
      [ "kDspiTxComplete", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bba1250abd2c9532ec9024d4e4589b15c4e", null ],
      [ "kDspiTxAndRxStatus", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bba76ae26ce982dc760f34d43ef77aff4df", null ],
      [ "kDspiEndOfQueue", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bbae26e3bb587466c7075af063a5a342b68", null ],
      [ "kDspiTxFifoUnderflow", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bba9e1094f02be24573437b71f3c64a78cd", null ],
      [ "kDspiTxFifoFillRequest", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bbac47470dc6b1cd08d73107f4aedb1ffb0", null ],
      [ "kDspiRxFifoOverflow", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bbadd17f34827e8e5921d4c121569534239", null ],
      [ "kDspiRxFifoDrainRequest", "group__dspi__hal.html#gga4eede4254ab5c5dcc6455290794df4bba573f7989c51c8a6fe6a48dd6388b32d1", null ]
    ] ],
    [ "dspi_delay_type_t", "group__dspi__hal.html#ga1ca2fbee37b3cb046c075a7e765d64ed", [
      [ "kDspiPcsToSck", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64eda1b1297e9388df6c83b58ca3bbb3cb267", null ],
      [ "kDspiLastSckToPcs", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64edae7abf34372d1fd3044103519f52865c8", null ],
      [ "kDspiAfterTransfer", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64eda0993c646e3ceed8b6a36ae347592813f", null ]
    ] ],
    [ "DSPI_HAL_Init", "group__dspi__hal.html#gada8361e518c8f8953b10b7d3e42367a7", null ],
    [ "DSPI_HAL_Enable", "group__dspi__hal.html#gabeab4eaab7cab28b61e30321e42c00f4", null ],
    [ "DSPI_HAL_Disable", "group__dspi__hal.html#ga46268ea88ed50568f02921092f90f55f", null ],
    [ "DSPI_HAL_SetBaudRate", "group__dspi__hal.html#ga84a512e5496bb692b51b57f297607120", null ],
    [ "DSPI_HAL_SetBaudDivisors", "group__dspi__hal.html#ga9f78cf887bf4c2e72dd3df0f8385e145", null ],
    [ "DSPI_HAL_SetMasterSlaveMode", "group__dspi__hal.html#gaa156f593001954db6baf4560626e6331", null ],
    [ "DSPI_HAL_IsMaster", "group__dspi__hal.html#ga4fbf4373187899da765f2091537fffe4", null ],
    [ "DSPI_HAL_SetContinuousSckCmd", "group__dspi__hal.html#ga26ad866d6f08fe21e542963df3e10336", null ],
    [ "DSPI_HAL_SetRxFifoOverwriteCmd", "group__dspi__hal.html#ga175d5cc905d6fc07c18c3ba692ed631b", null ],
    [ "DSPI_HAL_SetPcsPolarityMode", "group__dspi__hal.html#gaa8068643578665a62ff0c1c8617e917d", null ],
    [ "DSPI_HAL_SetFifoCmd", "group__dspi__hal.html#ga5d78de342aff25aa82945f2f12ae9e82", null ],
    [ "DSPI_HAL_SetFlushFifoCmd", "group__dspi__hal.html#gae420fe7d40db48e7acf475ef48b71792", null ],
    [ "DSPI_HAL_SetDatainSamplepointMode", "group__dspi__hal.html#ga4d76a4cb126e20bd88776734ba098586", null ],
    [ "DSPI_HAL_StartTransfer", "group__dspi__hal.html#gabcd434cc7ddda751db9d37e5ed9f5a41", null ],
    [ "DSPI_HAL_StopTransfer", "group__dspi__hal.html#ga1cd4552ea3b37ea2b1687cea2f57fa9b", null ],
    [ "DSPI_HAL_SetDataFormat", "group__dspi__hal.html#ga72193bce80dd4a2bfa31d1c32229d3c4", null ],
    [ "DSPI_HAL_SetDelay", "group__dspi__hal.html#gad313e7ff8d5f062902273609e0fd7537", null ],
    [ "DSPI_HAL_CalculateDelay", "group__dspi__hal.html#ga53c1d50ec0f3564e023915fe52340775", null ],
    [ "DSPI_HAL_GetMasterPushrRegAddr", "group__dspi__hal.html#gaf759f6b8589d7175aa34c84aa3c4f53d", null ],
    [ "DSPI_HAL_GetSlavePushrRegAddr", "group__dspi__hal.html#gaa159e6cc0413d4843a1e5c33603919eb", null ],
    [ "DSPI_HAL_GetPoprRegAddr", "group__dspi__hal.html#ga2cee977284207b9973a4bcb12acd505f", null ],
    [ "DSPI_HAL_SetTxFifoFillDmaIntMode", "group__dspi__hal.html#gae9c95f4d97d04958f3053f93a70fc88b", null ],
    [ "DSPI_HAL_SetRxFifoDrainDmaIntMode", "group__dspi__hal.html#ga496731c74850fa5f167067acb6c5a751", null ],
    [ "DSPI_HAL_SetIntMode", "group__dspi__hal.html#ga8a41baf745fb07272df0b444d332725d", null ],
    [ "DSPI_HAL_GetIntMode", "group__dspi__hal.html#ga570720ddafc565c5d1e355f48cc2a5db", null ],
    [ "DSPI_HAL_GetStatusFlag", "group__dspi__hal.html#ga8848d2425c38dea9ade5008890299ba6", null ],
    [ "DSPI_HAL_ClearStatusFlag", "group__dspi__hal.html#gac7e8f82727c42657df31791653363ee5", null ],
    [ "DSPI_HAL_ReadData", "group__dspi__hal.html#ga97b910ee2255442ed14205b49dd70c4e", null ],
    [ "DSPI_HAL_WriteDataSlavemode", "group__dspi__hal.html#gade64488f1b635669ff6f99734428b79b", null ],
    [ "DSPI_HAL_WriteDataSlavemodeBlocking", "group__dspi__hal.html#gafcfecaca7a9e4a53abb52dad3e94b5c0", null ],
    [ "DSPI_HAL_WriteDataMastermode", "group__dspi__hal.html#gad3fb19e48a836fad61f9563dc1847d58", null ],
    [ "DSPI_HAL_WriteDataMastermodeBlocking", "group__dspi__hal.html#gae2aa144a61a4af16ae6e4a73dd4e18e3", null ],
    [ "DSPI_HAL_WriteCmdDataMastermode", "group__dspi__hal.html#gaf8e59fefebc2bc5cbabd6fb17602d1f8", null ],
    [ "DSPI_HAL_WriteCmdDataMastermodeBlocking", "group__dspi__hal.html#ga0f54d93cfb5d745ac5cd2fca56d4390f", null ],
    [ "DSPI_HAL_GetTransferCount", "group__dspi__hal.html#ga3d3d9f8e585b56b8f5b5a14c87102753", null ],
    [ "DSPI_HAL_PresetTransferCount", "group__dspi__hal.html#ga0a1b3d0256ff03152bd1ce7becb2455d", null ],
    [ "DSPI_HAL_GetFormattedCommand", "group__dspi__hal.html#gadec38038330bd8e8b503648c0834381b", null ]
];