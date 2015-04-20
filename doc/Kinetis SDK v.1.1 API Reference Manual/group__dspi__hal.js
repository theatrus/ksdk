var group__dspi__hal =
[
    [ "fsl_dspi_hal.h", "fsl__dspi__hal_8h.html", null ],
    [ "dspi_data_format_config_t", "group__dspi__hal.html#structdspi__data__format__config__t", [
      [ "bitsPerFrame", "group__dspi__hal.html#a99d8d0dec68d5d2c1de35f8bb8b66a75", null ],
      [ "clkPolarity", "group__dspi__hal.html#a8926c27980536d08b063f984cc5dc751", null ],
      [ "clkPhase", "group__dspi__hal.html#ad751cbe15bfe01f02088e2a36059774a", null ],
      [ "direction", "group__dspi__hal.html#a7b5fc2246586c461ebd0325bba2e3113", null ]
    ] ],
    [ "dspi_slave_config_t", "group__dspi__hal.html#structdspi__slave__config__t", [
      [ "isEnabled", "group__dspi__hal.html#a0455c10258d06e9b23b72b45064b26cc", null ],
      [ "dataConfig", "group__dspi__hal.html#a390dbd59c3f76acb1071799e66548e25", null ],
      [ "isTxFifoDisabled", "group__dspi__hal.html#a8f74003d1d513f571b237dde6c53d3be", null ],
      [ "isRxFifoDisabled", "group__dspi__hal.html#a7baead2ba09f2b30d71c51ad447ab0d6", null ]
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
    [ "dspi_fifo_t", "group__dspi__hal.html#ga317e18cc3009527f76f2a2e2fd969073", [
      [ "kDspiTxFifo", "group__dspi__hal.html#gga317e18cc3009527f76f2a2e2fd969073a1bc4ddf8090ce2f77222df8b019b77de", null ],
      [ "kDspiRxFifo", "group__dspi__hal.html#gga317e18cc3009527f76f2a2e2fd969073a17a12e128e1f01f8024a4de10e7e09d5", null ]
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
    [ "dspi_fifo_counter_pointer_t", "group__dspi__hal.html#gab262d5ca72da94b034fe18d7d84ffa96", [
      [ "kDspiRxFifoPointer", "group__dspi__hal.html#ggab262d5ca72da94b034fe18d7d84ffa96ac6931fb9e3b476e6fd89401b83ac3067", null ],
      [ "kDspiRxFifoCounter", "group__dspi__hal.html#ggab262d5ca72da94b034fe18d7d84ffa96acf7633495ed23f4216d761ae1be426a3", null ],
      [ "kDspiTxFifoPointer", "group__dspi__hal.html#ggab262d5ca72da94b034fe18d7d84ffa96ae0c9617e48e3fb684e9358ada3925af3", null ],
      [ "kDspiTxFifoCounter", "group__dspi__hal.html#ggab262d5ca72da94b034fe18d7d84ffa96a4f5aa8b944ed48d343f58caf405d4b2c", null ]
    ] ],
    [ "dspi_delay_type_t", "group__dspi__hal.html#ga1ca2fbee37b3cb046c075a7e765d64ed", [
      [ "kDspiPcsToSck", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64eda1b1297e9388df6c83b58ca3bbb3cb267", null ],
      [ "kDspiLastSckToPcs", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64edae7abf34372d1fd3044103519f52865c8", null ],
      [ "kDspiAfterTransfer", "group__dspi__hal.html#gga1ca2fbee37b3cb046c075a7e765d64eda0993c646e3ceed8b6a36ae347592813f", null ]
    ] ],
    [ "DSPI_HAL_Init", "group__dspi__hal.html#gacd6eeaedf410e9e1f41b5b33649e2f53", null ],
    [ "DSPI_HAL_Enable", "group__dspi__hal.html#ga584b151f7cac3dff32ad3ea2443f64c5", null ],
    [ "DSPI_HAL_Disable", "group__dspi__hal.html#ga7415446346130ae59e919c892e938d7e", null ],
    [ "DSPI_HAL_SetBaudRate", "group__dspi__hal.html#ga3884240ccf644a9e72baa9bddaf879a9", null ],
    [ "DSPI_HAL_SetBaudDivisors", "group__dspi__hal.html#ga50dd7a2c010f8075d8781a5f11840687", null ],
    [ "DSPI_HAL_SetMasterSlaveMode", "group__dspi__hal.html#gaccbf146a1f41b2f98a6a6b6bf6a92f5f", null ],
    [ "DSPI_HAL_IsMaster", "group__dspi__hal.html#ga33c73312d54d89851f107da174bbcf00", null ],
    [ "DSPI_HAL_SetContinuousSckCmd", "group__dspi__hal.html#ga5e28abe7e0835cbcf45d7d1619933974", null ],
    [ "DSPI_HAL_SetModifiedTimingFormatCmd", "group__dspi__hal.html#gafea455e047a2083e5d4ed49bbafadb89", null ],
    [ "DSPI_HAL_SetRxFifoOverwriteCmd", "group__dspi__hal.html#ga33aaa13008b3e4f27998d38fa23ce6f1", null ],
    [ "DSPI_HAL_SetPcsPolarityMode", "group__dspi__hal.html#ga6204761965cfc6d7ad5fe88ff4529162", null ],
    [ "DSPI_HAL_SetFifoCmd", "group__dspi__hal.html#ga83363cb5c1c771b1dadfbb4e6a5b84f9", null ],
    [ "DSPI_HAL_SetFlushFifoCmd", "group__dspi__hal.html#ga4cc1d890bdeca0e0dd2b289d124e78b5", null ],
    [ "DSPI_HAL_SetDatainSamplepointMode", "group__dspi__hal.html#gacca76efc7cba29e69b79f18033b4126d", null ],
    [ "DSPI_HAL_StartTransfer", "group__dspi__hal.html#ga1e1253aa52573b11b7711884d82404c9", null ],
    [ "DSPI_HAL_StopTransfer", "group__dspi__hal.html#gab7ee51278faa1f58253a2e4c57d40fb3", null ],
    [ "DSPI_HAL_SetDataFormat", "group__dspi__hal.html#ga349aed7131a754766b0375ba2028daf9", null ],
    [ "DSPI_HAL_SetDelay", "group__dspi__hal.html#ga777d6d624b0d638b3ec3e4532e26e2ad", null ],
    [ "DSPI_HAL_CalculateDelay", "group__dspi__hal.html#ga93d0ac8aa458b6cce74efd6d7eb7f3c7", null ],
    [ "DSPI_HAL_GetMasterPushrRegAddr", "group__dspi__hal.html#ga3a946e271bf2f6bcbed00ccb2bd51143", null ],
    [ "DSPI_HAL_GetSlavePushrRegAddr", "group__dspi__hal.html#ga68a357a35083644c47d120e045411875", null ],
    [ "DSPI_HAL_GetPoprRegAddr", "group__dspi__hal.html#gaa9b29a554501b3d5a3b28079b4b84de7", null ],
    [ "DSPI_HAL_SetDozemodeCmd", "group__dspi__hal.html#gabc432d90aa9f7fc642b996d536236d39", null ],
    [ "DSPI_HAL_SetTxFifoFillDmaIntMode", "group__dspi__hal.html#ga39e1c8668d664e897a0eba1efa99a79b", null ],
    [ "DSPI_HAL_SetRxFifoDrainDmaIntMode", "group__dspi__hal.html#ga3ae755726cfcdb0e1b1dd97124ddf70c", null ],
    [ "DSPI_HAL_SetIntMode", "group__dspi__hal.html#ga33301419ad92394cc238fc4ae5c020b9", null ],
    [ "DSPI_HAL_GetIntMode", "group__dspi__hal.html#gae6269adfc55b9491fadc421f9379b09d", null ],
    [ "DSPI_HAL_GetStatusFlag", "group__dspi__hal.html#ga8c42322c55e7d9d318b581fc981c89c6", null ],
    [ "DSPI_HAL_ClearStatusFlag", "group__dspi__hal.html#gaae9d325c7fe89f3400c7046e17b87d40", null ],
    [ "DSPI_HAL_GetFifoCountOrPtr", "group__dspi__hal.html#ga43e7e74f0bd1578ae29db9dd5ca11089", null ],
    [ "DSPI_HAL_ReadData", "group__dspi__hal.html#ga770e395a5fd02dacb50302d3f02ff19f", null ],
    [ "DSPI_HAL_WriteDataSlavemode", "group__dspi__hal.html#gaed35722a337d7b03bed85a1875ff7f90", null ],
    [ "DSPI_HAL_WriteDataSlavemodeBlocking", "group__dspi__hal.html#gab59fd8cb39b575f28e4e245e45b4881d", null ],
    [ "DSPI_HAL_WriteDataMastermode", "group__dspi__hal.html#gab0c96db580d27200d4afc44383f52d73", null ],
    [ "DSPI_HAL_WriteDataMastermodeBlocking", "group__dspi__hal.html#ga59b29aae7daf75de09c4a8e6b637bd7c", null ],
    [ "DSPI_HAL_WriteCmdDataMastermode", "group__dspi__hal.html#gaf2bccb53a90191f3b2a683e15bb2be7d", null ],
    [ "DSPI_HAL_WriteCmdDataMastermodeBlocking", "group__dspi__hal.html#ga798ae83d7a4866fcd1b0bf9f87b82f98", null ],
    [ "DSPI_HAL_GetTransferCount", "group__dspi__hal.html#ga4cd40267bbafecca951fb100861caf18", null ],
    [ "DSPI_HAL_PresetTransferCount", "group__dspi__hal.html#ga952a0fe945b922e0866cef728c34bf11", null ],
    [ "DSPI_HAL_GetFormattedCommand", "group__dspi__hal.html#ga427da7ba440de1c66e41f13e38d0934b", null ],
    [ "DSPI_HAL_GetFifoData", "group__dspi__hal.html#ga524e117873f9f93c2118f0175b9f6cc5", null ],
    [ "DSPI_HAL_SetHaltInDebugmodeCmd", "group__dspi__hal.html#ga1da774b2fa7db1ad928a56506bc95259", null ]
];