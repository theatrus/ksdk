var group__flexcan__hal =
[
    [ "flexcan_id_table_t", "group__flexcan__hal.html#structflexcan__id__table__t", [
      [ "isRemoteFrame", "group__flexcan__hal.html#ac041d0f3158f82a9e457e0eb737acf2d", null ],
      [ "isExtendedFrame", "group__flexcan__hal.html#ab8d3bf0aea19d52ea329cba535ac7ec2", null ],
      [ "idFilter", "group__flexcan__hal.html#a67c50793275188512eacc58cd663ff74", null ]
    ] ],
    [ "flexcan_buserr_counter_t", "group__flexcan__hal.html#structflexcan__buserr__counter__t", [
      [ "txerr", "group__flexcan__hal.html#a2d19cb809ea29bf519f9ff8cf42d903a", null ],
      [ "rxerr", "group__flexcan__hal.html#a90356e9b1a25887c1697617f663951b8", null ]
    ] ],
    [ "flexcan_msgbuff_code_status_t", "group__flexcan__hal.html#structflexcan__msgbuff__code__status__t", [
      [ "code", "group__flexcan__hal.html#a1354cdfc8a258de98a3cb66a27e2af64", null ],
      [ "msgIdType", "group__flexcan__hal.html#ad9a92b05e49c0ba43aa710c214cf134a", null ],
      [ "dataLen", "group__flexcan__hal.html#a83b4eea1ecf1254bdb979fc264ed33c0", null ]
    ] ],
    [ "flexcan_msgbuff_t", "group__flexcan__hal.html#structflexcan__msgbuff__t", [
      [ "cs", "group__flexcan__hal.html#a259e57cb52f43fcfe798b548d89b5fd1", null ],
      [ "msgId", "group__flexcan__hal.html#a47b5f7a35a1138a9eff7c511b5107032", null ],
      [ "data", "group__flexcan__hal.html#a966dc0684997478c1b07404a818d9409", null ]
    ] ],
    [ "flexcan_time_segment_t", "group__flexcan__hal.html#structflexcan__time__segment__t", [
      [ "propSeg", "group__flexcan__hal.html#ada7cecba809570dd01004ac6ba0f99e0", null ],
      [ "phaseSeg1", "group__flexcan__hal.html#a31b1a0f1125d506c4fbad07daa47b586", null ],
      [ "phaseSeg2", "group__flexcan__hal.html#aa27fd9751a87a1256fa1e52a5d0235da", null ],
      [ "preDivider", "group__flexcan__hal.html#a73117c933577cab5ae8ae334bb77b53a", null ],
      [ "rJumpwidth", "group__flexcan__hal.html#a28c17f94ef533b8c0c778317d5dd58c1", null ]
    ] ],
    [ "_flexcan_constants", "group__flexcan__hal.html#gafad462423eb2c311872511a1e04b24c7", [
      [ "kFlexCanMessageSize", "group__flexcan__hal.html#ggafad462423eb2c311872511a1e04b24c7aedf5716dc3ff32341a2844a4c860c7fa", null ]
    ] ],
    [ "_flexcan_err_status", "group__flexcan__hal.html#gae6730702ce494b655c34176af6623698", [
      [ "kFlexCanRxWrn", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a991a0dea942fba4f1cd18cd90d23ca90", null ],
      [ "kFlexCanTxWrn", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a0f6373d5d06b3b5776bf59df7bf61f0f", null ],
      [ "kFlexCanStfErr", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698aa8cd12e22c79eb43bd88a38ccb897f68", null ],
      [ "kFlexCanFrmErr", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a4a12f496a21cd58fab724563bdf38f1f", null ],
      [ "kFlexCanCrcErr", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a79ea350d55382f2499737450c3c37ed3", null ],
      [ "kFlexCanAckErr", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a9ae59a58f12b0b48fad869024bae2a37", null ],
      [ "kFlexCanBit0Err", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698a29d50ad37583a93b335fdd1812f091f7", null ],
      [ "kFlexCanBit1Err", "group__flexcan__hal.html#ggae6730702ce494b655c34176af6623698abc78182d045ece8e92e0ad5b9186f823", null ]
    ] ],
    [ "flexcan_status_t", "group__flexcan__hal.html#ga290ed2ee4a53fadd566037835a80698a", null ],
    [ "flexcan_operation_modes_t", "group__flexcan__hal.html#gacd9117b0f08ffc73dccf0f4f4e2a3124", [
      [ "kFlexCanNormalMode", "group__flexcan__hal.html#ggacd9117b0f08ffc73dccf0f4f4e2a3124a5595a026371cf60ad2a93ab6c5cf9967", null ],
      [ "kFlexCanListenOnlyMode", "group__flexcan__hal.html#ggacd9117b0f08ffc73dccf0f4f4e2a3124ab3c7fb2669c21ca80d28e9a7d498995d", null ],
      [ "kFlexCanLoopBackMode", "group__flexcan__hal.html#ggacd9117b0f08ffc73dccf0f4f4e2a3124a50f866c066b9421f31a3bfa5f9928bc2", null ],
      [ "kFlexCanFreezeMode", "group__flexcan__hal.html#ggacd9117b0f08ffc73dccf0f4f4e2a3124a8e7b6ef1a3b7eef53c2fb28fac33aab9", null ],
      [ "kFlexCanDisableMode", "group__flexcan__hal.html#ggacd9117b0f08ffc73dccf0f4f4e2a3124a79751af02625d486ac5588594c06f2e6", null ]
    ] ],
    [ "flexcan_msgbuff_code_rx_t", "group__flexcan__hal.html#ga81099a001e6802a23b2f1789cf18331c", [
      [ "kFlexCanRXInactive", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331cae79f7a8814a2f3da57fdf88e322b4646", null ],
      [ "kFlexCanRXFull", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331cab57c848bc4e9a2c2035d5ee3486e8189", null ],
      [ "kFlexCanRXEmpty", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331ca0fad2cd0b7e92dacf156a746548a3252", null ],
      [ "kFlexCanRXOverrun", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331ca38ca0baf7c701cb7a62e0e11d635be92", null ],
      [ "kFlexCanRXBusy", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331cad464f279a72012da24f1035d098a8d65", null ],
      [ "kFlexCanRXRanswer", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331ca69b5606a9d2bc809eef98fd425a53769", null ],
      [ "kFlexCanRXNotUsed", "group__flexcan__hal.html#gga81099a001e6802a23b2f1789cf18331ca02f5b26368996e9169a996a0c28037cb", null ]
    ] ],
    [ "flexcan_msgbuff_code_tx_t", "group__flexcan__hal.html#ga9d1fe24a81323d8a15d5d9ca321e141e", [
      [ "kFlexCanTXInactive", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141eac4427c1782b7374ba06388765bf89455", null ],
      [ "kFlexCanTXAbort", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141ea58855e6db254d50fef4333a040504fcc", null ],
      [ "kFlexCanTXData", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141eadbe7020565a66ad236cdf407e0c26d1d", null ],
      [ "kFlexCanTXRemote", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141ea3f606841cd2ce544700de20a2143a2bd", null ],
      [ "kFlexCanTXTanswer", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141ead6744f4e33db49d26c1431f5305eb93a", null ],
      [ "kFlexCanTXNotUsed", "group__flexcan__hal.html#gga9d1fe24a81323d8a15d5d9ca321e141ea56145d6248abfe9e15731e36d720d5f3", null ]
    ] ],
    [ "flexcan_msgbuff_transmission_type_t", "group__flexcan__hal.html#ga8e266577f746708b31eda744bd557872", [
      [ "kFlexCanMBStatusTypeTX", "group__flexcan__hal.html#gga8e266577f746708b31eda744bd557872ad68ad7bc591a5b562a09304a5632c2ec", null ],
      [ "kFlexCanMBStatusTypeTXRemote", "group__flexcan__hal.html#gga8e266577f746708b31eda744bd557872a08e9a0ff91dd702286d053a96b565581", null ],
      [ "kFlexCanMBStatusTypeRX", "group__flexcan__hal.html#gga8e266577f746708b31eda744bd557872ad3f0090c8ec11b8862b08ac0a47e9f74", null ],
      [ "kFlexCanMBStatusTypeRXRemote", "group__flexcan__hal.html#gga8e266577f746708b31eda744bd557872a626f9fb091008406e3e7b9609f0f3c94", null ],
      [ "kFlexCanMBStatusTypeRXTXRemote", "group__flexcan__hal.html#gga8e266577f746708b31eda744bd557872adb56a396ca5770f7289d9e87556a6006", null ]
    ] ],
    [ "flexcan_rx_fifo_id_element_format_t", "group__flexcan__hal.html#ga98708675f8e72a66324ea232d7e74703", [
      [ "kFlexCanRxFifoIdElementFormatA", "group__flexcan__hal.html#gga98708675f8e72a66324ea232d7e74703a90157530b5106e3e36543fd37a0bf727", null ],
      [ "kFlexCanRxFifoIdElementFormatB", "group__flexcan__hal.html#gga98708675f8e72a66324ea232d7e74703acca3aadfed8397590d1454542471a9c5", null ],
      [ "kFlexCanRxFifoIdElementFormatC", "group__flexcan__hal.html#gga98708675f8e72a66324ea232d7e74703a990d5db0d351995e1480c605653b22b9", null ],
      [ "kFlexCanRxFifoIdElementFormatD", "group__flexcan__hal.html#gga98708675f8e72a66324ea232d7e74703aaeff7411116909c94f38a7ac21e49717", null ]
    ] ],
    [ "flexcan_rx_fifo_id_filter_num_t", "group__flexcan__hal.html#ga704b23b25bebbc1444bb6bcfcc738e6e", [
      [ "kFlexCanRxFifoIDFilters_8", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea11ec22ce3410f0877e8fe757cb48c2e0", null ],
      [ "kFlexCanRxFifoIDFilters_16", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea3c1df88b967496487735c8e151a5afb7", null ],
      [ "kFlexCanRxFifoIDFilters_24", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea3cb62f8f52dbbecd30ed064ac52c2131", null ],
      [ "kFlexCanRxFifoIDFilters_32", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea1088166a83f187f1a30377937281bf12", null ],
      [ "kFlexCanRxFifoIDFilters_40", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea490b73af047bed1f3be772a1b2e09be0", null ],
      [ "kFlexCanRxFifoIDFilters_48", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea63287a21b718c1538939c068f25c3588", null ],
      [ "kFlexCanRxFifoIDFilters_56", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea205307da0a3ddbd844135d259003bf7e", null ],
      [ "kFlexCanRxFifoIDFilters_64", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea7f889d5f5e8316978cc00f1fa93fdb7b", null ],
      [ "kFlexCanRxFifoIDFilters_72", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6eadc235ad211be5522270cd88288dfda18", null ],
      [ "kFlexCanRxFifoIDFilters_80", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea48c8dde89d2bdde5f968bc4baea82443", null ],
      [ "kFlexCanRxFifoIDFilters_88", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea283ca323bcba22c4733cf9dd6060fc7c", null ],
      [ "kFlexCanRxFifoIDFilters_96", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6eafb19562c744c178cbb0ec65b7374a336", null ],
      [ "kFlexCanRxFifoIDFilters_104", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea507a7162337a631391feaa8036737b95", null ],
      [ "kFlexCanRxFifoIDFilters_112", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea6a886cc844ea2e284b3a3508f12e0065", null ],
      [ "kFlexCanRxFifoIDFilters_120", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6eac5fc6bd6b6008ed50b8efbd21f935f79", null ],
      [ "kFlexCanRxFifoIDFilters_128", "group__flexcan__hal.html#gga704b23b25bebbc1444bb6bcfcc738e6ea402a3c5d104b72e626386be41bca11a3", null ]
    ] ],
    [ "flexcan_rx_mask_type_t", "group__flexcan__hal.html#ga33c957aea3f0f2351514142cceb110bb", [
      [ "kFlexCanRxMaskGlobal", "group__flexcan__hal.html#gga33c957aea3f0f2351514142cceb110bba35d2885be790a0b089850b874b47b974", null ],
      [ "kFlexCanRxMaskIndividual", "group__flexcan__hal.html#gga33c957aea3f0f2351514142cceb110bba1a7678154dbbb6eea9a5c2b821fd947e", null ]
    ] ],
    [ "flexcan_msgbuff_id_type_t", "group__flexcan__hal.html#ga47e92a1e50790879c679a6147640b81e", [
      [ "kFlexCanMsgIdStd", "group__flexcan__hal.html#gga47e92a1e50790879c679a6147640b81ea4759e1e19974a73f373bda741461994c", null ],
      [ "kFlexCanMsgIdExt", "group__flexcan__hal.html#gga47e92a1e50790879c679a6147640b81ea1e9b229db28a9d9a40aa3a268ac725e3", null ]
    ] ],
    [ "flexcan_clk_source_t", "group__flexcan__hal.html#ga34f6dee1ed25368ce7a6f3fc758e4c7c", [
      [ "kFlexCanClkSourceOsc", "group__flexcan__hal.html#gga34f6dee1ed25368ce7a6f3fc758e4c7cad2b202348bff242c69ced8321f9500e1", null ],
      [ "kFlexCanClkSourceIpbus", "group__flexcan__hal.html#gga34f6dee1ed25368ce7a6f3fc758e4c7cac71a415554e9d5e61f53ee9ec0787e79", null ]
    ] ],
    [ "flexcan_int_type_t", "group__flexcan__hal.html#ga17d4917b163c31255229e917a6cc51af", [
      [ "kFlexCanIntRxwarning", "group__flexcan__hal.html#gga17d4917b163c31255229e917a6cc51afac8c96a76ab226219d6ab8b721ee28ef5", null ],
      [ "kFlexCanIntTxwarning", "group__flexcan__hal.html#gga17d4917b163c31255229e917a6cc51afa34dfa672471c2ff9d87fa34ce8606871", null ],
      [ "kFlexCanIntErr", "group__flexcan__hal.html#gga17d4917b163c31255229e917a6cc51afadbe1c8d1f68886c2a82e19e49b5718e2", null ],
      [ "kFlexCanIntBusoff", "group__flexcan__hal.html#gga17d4917b163c31255229e917a6cc51afa7caac9bf22319d0d5de9ed4608acb3a1", null ],
      [ "kFlexCanIntWakeup", "group__flexcan__hal.html#gga17d4917b163c31255229e917a6cc51afaac511b7421a92fde538c5979d72fdd0b", null ]
    ] ],
    [ "FLEXCAN_HAL_Enable", "group__flexcan__hal.html#gaba55384e3d6972dc23acc751f048f3af", null ],
    [ "FLEXCAN_HAL_Disable", "group__flexcan__hal.html#ga0be2454afc20b715794b5d6deb47488a", null ],
    [ "FLEXCAN_HAL_SelectClock", "group__flexcan__hal.html#gacdc15d03a80cdcee2a52ed557097f31d", null ],
    [ "FLEXCAN_HAL_GetClock", "group__flexcan__hal.html#ga37fa2e76826b121530b3e0c4102189f7", null ],
    [ "FLEXCAN_HAL_Init", "group__flexcan__hal.html#ga3d732fffb05ee0e4b8465e81154a396f", null ],
    [ "FLEXCAN_HAL_SetTimeSegments", "group__flexcan__hal.html#ga8eb66af3b82a56efaa57d29cacccf258", null ],
    [ "FLEXCAN_HAL_GetTimeSegments", "group__flexcan__hal.html#gaedd55eb3aad420784142ccd53e288087", null ],
    [ "FLEXCAN_HAL_ExitFreezeMode", "group__flexcan__hal.html#gaed6c6c3ee6e0d60aadd6637b835cc1a3", null ],
    [ "FLEXCAN_HAL_EnterFreezeMode", "group__flexcan__hal.html#ga8f4bdb53b058742a64f1c1c3e60f173b", null ],
    [ "FLEXCAN_HAL_SetOperationMode", "group__flexcan__hal.html#ga8cfb6da7811b659425228bd30312fb95", null ],
    [ "FLEXCAN_HAL_ExitOperationMode", "group__flexcan__hal.html#ga4b7f46d75540183c435c2eab096abba2", null ],
    [ "FLEXCAN_HAL_SetTxMsgBuff", "group__flexcan__hal.html#ga9540fbed2f4d1872b57ca427d5f544b7", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuff", "group__flexcan__hal.html#gaa1950be2bc9bdda4711f7c2246fee61c", null ],
    [ "FLEXCAN_HAL_GetMsgBuff", "group__flexcan__hal.html#ga4d1ff22c6f0c62e489756d8481638431", null ],
    [ "FLEXCAN_HAL_LockRxMsgBuff", "group__flexcan__hal.html#ga43988e7ed9275cd5940dd74cd9d1cffb", null ],
    [ "FLEXCAN_HAL_UnlockRxMsgBuff", "group__flexcan__hal.html#gad5fb98937ab35f4fc2f88d5f1dbd4801", null ],
    [ "FLEXCAN_HAL_EnableRxFifo", "group__flexcan__hal.html#gaf17390c328c206e64b00f003c5a093ef", null ],
    [ "FLEXCAN_HAL_DisableRxFifo", "group__flexcan__hal.html#ga2e0c31305b33939b54c6c273f151baa5", null ],
    [ "FLEXCAN_HAL_SetRxFifoFilterNum", "group__flexcan__hal.html#ga76b333f79a9e4756d58b78ef5d8b4b9d", null ],
    [ "FLEXCAN_HAL_SetMaxMsgBuffNum", "group__flexcan__hal.html#ga4d7f7adc732683091c1abeff125c2a66", null ],
    [ "FLEXCAN_HAL_SetRxFifoFilter", "group__flexcan__hal.html#gaf342d08b31f862cbdafd06d43efa5509", null ],
    [ "FLEXCAN_HAL_ReadRxFifo", "group__flexcan__hal.html#gae370c5422baad0f2b8df2815cc6213c0", null ],
    [ "FLEXCAN_HAL_SetMsgBuffIntCmd", "group__flexcan__hal.html#ga74c81496fcf18880619c00fdae9927fd", null ],
    [ "FLEXCAN_HAL_SetErrIntCmd", "group__flexcan__hal.html#gab4c9c63bc1952b70edce61ca61f880df", null ],
    [ "FLEXCAN_HAL_GetFreezeAck", "group__flexcan__hal.html#gaa394831a488e298d30af4003090c9c77", null ],
    [ "FLEXCAN_HAL_GetMsgBuffIntStatusFlag", "group__flexcan__hal.html#ga58628a759e55d8544792bb91c74d34ab", null ],
    [ "FLEXCAN_HAL_GetAllMsgBuffIntStatusFlag", "group__flexcan__hal.html#gac15c6e2219a206c45af4461712fd7ce9", null ],
    [ "FLEXCAN_HAL_ClearMsgBuffIntStatusFlag", "group__flexcan__hal.html#ga2fcecd4ed3682e784f97780aebd86733", null ],
    [ "FLEXCAN_HAL_GetErrCounter", "group__flexcan__hal.html#ga9946dd213354355a32c39e246c11b723", null ],
    [ "FLEXCAN_HAL_GetErrStatus", "group__flexcan__hal.html#ga48c2cabc8d2b0a6dc5086dd072669fe8", null ],
    [ "FLEXCAN_HAL_ClearErrIntStatusFlag", "group__flexcan__hal.html#ga9e3b01cc007089cb7f7031771f2c20d4", null ],
    [ "FLEXCAN_HAL_SetRxMaskType", "group__flexcan__hal.html#gac3ab586fc618a17d05d87e3ab60a3d2f", null ],
    [ "FLEXCAN_HAL_SetRxFifoGlobalStdMask", "group__flexcan__hal.html#gae77f4f2ef35edf73f6960fcc70d8bae6", null ],
    [ "FLEXCAN_HAL_SetRxFifoGlobalExtMask", "group__flexcan__hal.html#ga0fe7ec8dac22b213a88a1d1e6845984c", null ],
    [ "FLEXCAN_HAL_SetRxIndividualStdMask", "group__flexcan__hal.html#gaf361415b1438b9be050fa9b955a5a303", null ],
    [ "FLEXCAN_HAL_SetRxIndividualExtMask", "group__flexcan__hal.html#gadf16dd598d7d6723540134691440bcf1", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuffGlobalStdMask", "group__flexcan__hal.html#gaae9ba832569734614ceb173684d360de", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuff14StdMask", "group__flexcan__hal.html#gaeb3d5acce38b679223781e161f8ca2fe", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuff15StdMask", "group__flexcan__hal.html#ga3db0b70bef88b3703031b5f794af6918", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuffGlobalExtMask", "group__flexcan__hal.html#ga117af31cd031cff395237db7879504eb", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuff14ExtMask", "group__flexcan__hal.html#ga07ba8981139d68e60666fa069a7a00a0", null ],
    [ "FLEXCAN_HAL_SetRxMsgBuff15ExtMask", "group__flexcan__hal.html#ga16e58ba2a6a6894e65acfd5041963526", null ],
    [ "FLEXCAN_HAL_GetRxFifoHitIdAcceptanceFilter", "group__flexcan__hal.html#ga4ab14555b727b07c37766ea830b23518", null ]
];