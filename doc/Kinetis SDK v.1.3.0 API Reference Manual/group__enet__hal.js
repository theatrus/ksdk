var group__enet__hal =
[
    [ "enet_bd_struct_t", "group__enet__hal.html#structenet__bd__struct__t", [
      [ "length", "group__enet__hal.html#ab19ebc5b0b9870958ef77f752b38ec24", null ],
      [ "control", "group__enet__hal.html#aecd286f91bc04f42e9d6cb93c7196104", null ],
      [ "buffer", "group__enet__hal.html#a025b9202f56a16d48cebc761acb3f8ba", null ],
      [ "controlExtend0", "group__enet__hal.html#a1099a7d70fcbb69598d9d501832f1036", null ],
      [ "controlExtend1", "group__enet__hal.html#a24b27eebded26092018b5c04dd204d87", null ],
      [ "payloadCheckSum", "group__enet__hal.html#a8742bb01a4c687e8b357a99149097e00", null ],
      [ "headerLength", "group__enet__hal.html#a5ce70d2245f538b8c7fef1429d46d043", null ],
      [ "protocalTyte", "group__enet__hal.html#a789549ea96a45865035d67a6e06aac3b", null ],
      [ "controlExtend2", "group__enet__hal.html#ac8323fc3e3a12b299d2925358e4bf390", null ],
      [ "timestamp", "group__enet__hal.html#a9bc9f488c41158f5b3dd0afd4931ef44", null ]
    ] ],
    [ "enet_config_rmii_t", "group__enet__hal.html#structenet__config__rmii__t", [
      [ "mode", "group__enet__hal.html#ae3ddd9812145ee77293b76b061cb24cd", null ],
      [ "speed", "group__enet__hal.html#a90aa3224327a87ae7ae331d42e2a5832", null ],
      [ "duplex", "group__enet__hal.html#ad0f716b4aa133681cd0fa5114b13405e", null ],
      [ "isRxOnTxDisabled", "group__enet__hal.html#af3bf4c2882228b6494f9ba90bbcbf7ce", null ],
      [ "isLoopEnabled", "group__enet__hal.html#a1e588862dbedcb20f366195902b94397", null ]
    ] ],
    [ "enet_config_ptp_timer_t", "group__enet__hal.html#structenet__config__ptp__timer__t", [
      [ "isSlaveEnabled", "group__enet__hal.html#a4d87fc0da9a51f806e19b085a99af279", null ],
      [ "clockIncease", "group__enet__hal.html#a2a0a3bf06063adf0fe9bf147be7abb61", null ],
      [ "period", "group__enet__hal.html#a82587e9f4e3ce5e9833cd6934c3d821c", null ]
    ] ],
    [ "enet_config_tx_fifo_t", "group__enet__hal.html#structenet__config__tx__fifo__t", [
      [ "isStoreForwardEnabled", "group__enet__hal.html#a5296c6aa332be24441abef920203bfcf", null ],
      [ "txFifoWrite", "group__enet__hal.html#a0d04c7ccbd173ce3cfcf5a47ad1cb49f", null ],
      [ "txEmpty", "group__enet__hal.html#abc28b8bf317fb30e0630ca67d1422379", null ],
      [ "txAlmostEmpty", "group__enet__hal.html#a86b3d738ed2f6a7f672dd1921531248d", null ],
      [ "txAlmostFull", "group__enet__hal.html#a7da66f472f177fcc3b99caf7983be60a", null ]
    ] ],
    [ "enet_config_rx_fifo_t", "group__enet__hal.html#structenet__config__rx__fifo__t", [
      [ "rxFull", "group__enet__hal.html#af250c1d3b2f6f752e8a854c0044522ae", null ],
      [ "rxAlmostFull", "group__enet__hal.html#a4738433430cc525ddd17aa083c6f3776", null ],
      [ "rxEmpty", "group__enet__hal.html#abb902a57c04d48ad4bb3782f6506cf56", null ],
      [ "rxAlmostEmpty", "group__enet__hal.html#a228574905884ffee28ddcf59e418f9a9", null ]
    ] ],
    [ "enet_mib_rx_stat_t", "group__enet__hal.html#structenet__mib__rx__stat__t", [
      [ "rxPackets", "group__enet__hal.html#a666d80f29f5c3308396ade70de7aef07", null ],
      [ "rxBroadcastPackets", "group__enet__hal.html#a1e92c361ca304b5fd2588aeba17b3c17", null ],
      [ "rxMulticastPackets", "group__enet__hal.html#ac3af594cd39d5113c8d6370be3221873", null ],
      [ "rxCrcAlignErrorPackets", "group__enet__hal.html#aca764c4b0932635ec0443761554305a5", null ],
      [ "rxUnderSizeGoodPackets", "group__enet__hal.html#af924c1666398b92338e67b6fde8fd678", null ],
      [ "rxUnderSizeBadPackets", "group__enet__hal.html#ac14509a12bbd5cbb5fd4743dd00ca558", null ],
      [ "rxOverSizeGoodPackets", "group__enet__hal.html#ac0c8e7f0cff1e918111cfbf79270bc8b", null ],
      [ "rxOverSizeBadPackets", "group__enet__hal.html#a57d136a79dcb86edc8a33496c310f754", null ],
      [ "rxByte64Packets", "group__enet__hal.html#a592bb3fb4732d880dfc819dd31467c75", null ],
      [ "rxByte65to127Packets", "group__enet__hal.html#a8f427afe700a279b13b3f54dc18d2921", null ],
      [ "rxByte128to255Packets", "group__enet__hal.html#a944938e4ae48c931f4aafa2c86581cf2", null ],
      [ "rxByte256to511Packets", "group__enet__hal.html#a0f380cf9341e6910e816920bc0ebd184", null ],
      [ "rxByte512to1023Packets", "group__enet__hal.html#a26f2d4822396ae6fc51bdec4aaf0f47a", null ],
      [ "rxByte1024to2047Packets", "group__enet__hal.html#acd1f1c7e808906d7babd4ab787495546", null ],
      [ "rxByteOver2048Packets", "group__enet__hal.html#a2ed420e08cd49d573f14dfd5bfaf1c36", null ],
      [ "rxOctets", "group__enet__hal.html#ab0a2c9d020878d3996bc02482652f5b7", null ],
      [ "ieeeOctetsrxFrameOk", "group__enet__hal.html#a0f3ecbb317f023db6daf89e6707a7bf6", null ],
      [ "ieeerxFrameDrop", "group__enet__hal.html#aa5496ac27743a3ee690b137a301054ba", null ],
      [ "ieeerxFrameOk", "group__enet__hal.html#a25c06293f0250c378d4e835bd88bd3df", null ],
      [ "ieeerxFrameCrcErr", "group__enet__hal.html#a44ee64a870673eabf2f4ac11382903d3", null ],
      [ "ieeetxFrameAlignErr", "group__enet__hal.html#aa53ce54eb726f1e4a6ecbe8209859253", null ],
      [ "ieeetxFrameMacErr", "group__enet__hal.html#a0c10abc1a6e908d79ea2c591507a81b1", null ],
      [ "ieeetxFramePause", "group__enet__hal.html#a592126f0527d6c65275c200e4eef1c1a", null ]
    ] ],
    [ "enet_mib_tx_stat_t", "group__enet__hal.html#structenet__mib__tx__stat__t", [
      [ "txPackets", "group__enet__hal.html#ac55c2c37474c5ced323c9ec4ce16c7a9", null ],
      [ "txBroadcastPackets", "group__enet__hal.html#abd65b5cc459dba2e6d50e26bedad9b63", null ],
      [ "txMulticastPackets", "group__enet__hal.html#a0dc6b649c9c7796eff192f6b3a379d27", null ],
      [ "txCrcAlignErrorPackets", "group__enet__hal.html#a24113f5c75ea04ab4fb5e2497162c3f5", null ],
      [ "txUnderSizeGoodPackets", "group__enet__hal.html#add5ac06b76a0b2b29256a6577274f069", null ],
      [ "txUnderSizeBadPackets", "group__enet__hal.html#ab9e5195837b41e9621f88c8a49cc676d", null ],
      [ "txOverSizeGoodPackets", "group__enet__hal.html#a26eb21aace5feb21d56a5a0406992eec", null ],
      [ "txOverSizeBadPackets", "group__enet__hal.html#ae9ed2dc272cc0159faf7a4f99d084a91", null ],
      [ "txCollision", "group__enet__hal.html#ab364ccc6ae89b469750d35495775e3e7", null ],
      [ "txByte64Packets", "group__enet__hal.html#a72d7979920c267b71914947ee2ceabc0", null ],
      [ "txByte65to127Packets", "group__enet__hal.html#aef2eb3dc929520b8739d7b1f9aa8e9d9", null ],
      [ "txByte128to255Packets", "group__enet__hal.html#a391e8bab22c964733866c3a7fe55f23d", null ],
      [ "txByte256to511Packets", "group__enet__hal.html#ac0ae2a54114ad66025f1fe80cd30e11f", null ],
      [ "txByte512to1023Packets", "group__enet__hal.html#acf5fecf4c7cc9575f76f57c1747746c7", null ],
      [ "txByte1024to2047Packets", "group__enet__hal.html#aa888b1cb5fb0d00121fbc258ddc60e75", null ],
      [ "txByteOver2048Packets", "group__enet__hal.html#a979ee274e8c500129c8bc51360c0cdc8", null ],
      [ "txOctets", "group__enet__hal.html#ac119a028030cc4caad33c4079266bde0", null ],
      [ "ieeeOctetstxFrameOk", "group__enet__hal.html#afdea47113998c82724a8cdf1603782b9", null ],
      [ "ieeetxFrameOk", "group__enet__hal.html#afab10e262646f927851d470d426b0824", null ],
      [ "ieeetxFrameOneCollision", "group__enet__hal.html#ae195d87b3299162b80959e4c64c586c8", null ],
      [ "ieeetxFrameMultiCollison", "group__enet__hal.html#a073f981274c50f79344330eb965e0bfa", null ],
      [ "ieeetxFrameLateCollison", "group__enet__hal.html#ac2ed5a372cd1dcdb6e889362e90a0488", null ],
      [ "ieeetxFrmaeExcCollison", "group__enet__hal.html#a7bc94d0be969ddb4cf50cd1ae4b2650d", null ],
      [ "ieeetxFrameDelay", "group__enet__hal.html#aa5acd88afdd49733d8888723794159e2", null ],
      [ "ieeetxFrameMacErr", "group__enet__hal.html#a6b0f024c617c44e0b598048744b15671", null ],
      [ "ieeetxFrameCarrSenseErr", "group__enet__hal.html#a043c65292b3552238c4322b33b17b083", null ],
      [ "ieeetxFramePause", "group__enet__hal.html#a23dd0a2e3088dad67612ba8049c77114", null ]
    ] ],
    [ "enet_special_maccfg_t", "group__enet__hal.html#structenet__special__maccfg__t", [
      [ "rxMaxFrameLen", "group__enet__hal.html#ab46ffb74f7eb06ba8f3f024768a0672b", null ],
      [ "rxTruncLen", "group__enet__hal.html#a6b832c6920a58d902f0002b757481b44", null ],
      [ "txInterPacketGap", "group__enet__hal.html#ad325bfcf3e8c6c9d17442e6dd2e3b9ae", null ]
    ] ],
    [ "enet_mac_config_t", "group__enet__hal.html#structenet__mac__config__t", [
      [ "macMode", "group__enet__hal.html#a1eb76740d2f6e081f0c8b8aea7e1355a", null ],
      [ "macAddr", "group__enet__hal.html#abc9b5df2f2ab8a9fde8c1df274da03ab", null ],
      [ "rmiiCfgPtr", "group__enet__hal.html#af50aa9a4de9b64a9f32e5e30e9474b39", null ],
      [ "macCtlConfigure", "group__enet__hal.html#aa6e3856642413729bda87b43c45ac4a5", null ],
      [ "rxFifoPtr", "group__enet__hal.html#ae6c4ff5f602541e382bbb07252e5429c", null ],
      [ "txFifoPtr", "group__enet__hal.html#ac47a4fec5c6721f6ae1ba8f0f2e89992", null ],
      [ "rxAccelerCfg", "group__enet__hal.html#a34bd19cf375a91822cf20b700d5b5f21", null ],
      [ "txAccelerCfg", "group__enet__hal.html#ad8fc5134906c33a9f1af727ca679349f", null ],
      [ "pauseDuration", "group__enet__hal.html#a0634f56ae7263e72418e0ee54015a3ee", null ],
      [ "macSpecialCfg", "group__enet__hal.html#a4f1118f03769808237c7197e3302158f", null ]
    ] ],
    [ "enet_bd_config", "group__enet__hal.html#structenet__bd__config", [
      [ "txBds", "group__enet__hal.html#a929b68c162d868a824ee4b2053ee2b8d", null ],
      [ "txBuffer", "group__enet__hal.html#ab4bd479697f73abed4a7691c94472812", null ],
      [ "txBdNumber", "group__enet__hal.html#a055bf2183ce9ba2d8983012ac85c743f", null ],
      [ "txBuffSizeAlign", "group__enet__hal.html#aa0c10e7f3f2fbb1e1b102bc24e0b2e79", null ],
      [ "rxBds", "group__enet__hal.html#a2320a835e5d66030d77e3988d1cdc2de", null ],
      [ "rxBuffer", "group__enet__hal.html#abb0e7117e5cb88482f88c45a036aa395", null ],
      [ "rxBdNumber", "group__enet__hal.html#ad6087416aeb9a2d0db95f3aceb4c0717", null ],
      [ "rxBuffSizeAlign", "group__enet__hal.html#aba293411b0088fd50193bf3984786de4", null ]
    ] ],
    [ "enet_cur_status_t", "group__enet__hal.html#structenet__cur__status__t", [
      [ "rxStatic", "group__enet__hal.html#a1505a2dba40d0a8d37c62d094c30a8a3", null ],
      [ "txStatic", "group__enet__hal.html#ad4a050ba575112da87a6f23a5d1274ad", null ],
      [ "maxFrameLen", "group__enet__hal.html#a7c4d70802cb150139b08d9ae52ed4e26", null ],
      [ "statusFlags", "group__enet__hal.html#ad5f362ceb16d32dbe9ee7032d9f4a971", null ]
    ] ],
    [ "enet_bd_attr_t", "group__enet__hal.html#structenet__bd__attr__t", [
      [ "bdCtl", "group__enet__hal.html#adb6b2391606852b6914981cdb30ae444", null ],
      [ "rxBdExtCtl", "group__enet__hal.html#aa18223cb013f4dea19b2d8b68e753417", null ],
      [ "rxBdExtCtl1", "group__enet__hal.html#a75f053d0abde2a158ceadaac4600038b", null ],
      [ "rxBdExtCtl2", "group__enet__hal.html#a57fd37721df549c5172e8e369bdfb2a1", null ],
      [ "bdLen", "group__enet__hal.html#acc3658b2755e73461b95ec42783228ef", null ],
      [ "bdTimestamp", "group__enet__hal.html#a7fdb465379321eee7082ea30acda9d20", null ],
      [ "flags", "group__enet__hal.html#a5978e870c4c653c182ef241f542753ce", null ]
    ] ],
    [ "ENET_ALIGN", "group__enet__hal.html#ga49324c817c2810ff85f4dfe5490d18eb", null ],
    [ "BD_SHORTSWAP", "group__enet__hal.html#gabdf4a3efa8075addbedefdbbcbba135c", null ],
    [ "enet_status_t", "group__enet__hal.html#ga61dc76e4d0c7b7ce8d089037ae5c160c", [
      [ "kStatus_ENET_InvalidInput", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160caed5402747f7709e4edcbe1de9b37d932", null ],
      [ "kStatus_ENET_InvalidDevice", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca587792618de219204707f7f99ea9d818", null ],
      [ "kStatus_ENET_InitTimeout", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca98ff986433be5ffd751920e358593e71", null ],
      [ "kStatus_ENET_MemoryAllocateFail", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca945c26400d94b06a72e2a4f1db6c617e", null ],
      [ "kStatus_ENET_GetClockFreqFail", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cad334eaf438002b02965b16399d14b7be", null ],
      [ "kStatus_ENET_Initialized", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cab643fe9591acadc1e9d65f33885ab853", null ],
      [ "kStatus_ENET_Open", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca808ab007a23e3e0556f4db14da670317", null ],
      [ "kStatus_ENET_Close", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cab7838729e03f63733623b552218fcae8", null ],
      [ "kStatus_ENET_Layer2UnInitialized", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160caf2168764418fccae22f55f7dec22fde8", null ],
      [ "kStatus_ENET_Layer2OverLarge", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca5c25b169f5501fd892b3f803a97bbdd2", null ],
      [ "kStatus_ENET_Layer2BufferFull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca8fec4320bfe86e7d2d652054256728b1", null ],
      [ "kStatus_ENET_Layer2TypeError", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca31b04ec24fb36bffb177206cc1eeef61", null ],
      [ "kStatus_ENET_PtpringBufferFull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160caf0d7474499289ed65b1578ef316d963c", null ],
      [ "kStatus_ENET_PtpringBufferEmpty", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca2df0c7e76d5eadb47eac8cd54ab27bb6", null ],
      [ "kStatus_ENET_SMIUninitialized", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca4f62cf68221538c64fac40f438c7ff8a", null ],
      [ "kStatus_ENET_SMIVisitTimeout", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca62fad2b391258e84e58529b5b32e38fe", null ],
      [ "kStatus_ENET_RxbdInvalid", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160caf9fed305e0d1330d0dacd9ad417ed416", null ],
      [ "kStatus_ENET_RxbdEmpty", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca63839eca4ff4a23bc05504f1878fc4be", null ],
      [ "kStatus_ENET_RxbdTrunc", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cad5b4c7526aa49d83fe0e927dcbc36bd9", null ],
      [ "kStatus_ENET_RxbdError", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca93b806aa35ae91e2024915852ff46ccc", null ],
      [ "kStatus_ENET_RxBdFull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca100f18c37ff58509137726fbf56ac817", null ],
      [ "kStatus_ENET_SmallRxBuffSize", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cabc51ab555499d66c267fcbce895892e8", null ],
      [ "kStatus_ENET_NoEnoughRxBuffers", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cacab4dea6425fc8f0d5203975c4ebfa22", null ],
      [ "kStatus_ENET_LargeBufferFull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca9d1733982d51cb730bf78f46ace1d0aa", null ],
      [ "kStatus_ENET_TxLarge", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca77250d2c40809272a2614075dbebf219", null ],
      [ "kStatus_ENET_TxbdFull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca0755fd921cd6307d0d0daf67e8d0c6c2", null ],
      [ "kStatus_ENET_TxbdNull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca06b114ce4f655a4d9d164754894264ba", null ],
      [ "kStatus_ENET_TxBufferNull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca62d8716171953de0a62522271498e174", null ],
      [ "kStatus_ENET_NoRxBufferLeft", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca2c1f51fd70e7715f0184754fa77616ae", null ],
      [ "kStatus_ENET_UnknownCommand", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca76c5b1837efcfca3a1c4e24cc785da1a", null ],
      [ "kStatus_ENET_TimeOut", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca0fe00b08edab48ec4d3dc893baa3fa46", null ],
      [ "kStatus_ENET_MulticastPointerNull", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca1bdc4e9ad74a7da1db8164f0562e0b83", null ],
      [ "kStatus_ENET_NoMulticastAddr", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160ca5d2320ff068e1b657575669f26d7c4a5", null ],
      [ "kStatus_ENET_AlreadyAddedMulticast", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cab1a6e5243ba2693ecf67003a51575491", null ],
      [ "kStatus_ENET_PHYAutoDiscoverFail", "group__enet__hal.html#gga61dc76e4d0c7b7ce8d089037ae5c160cafab25af1957cce29d7f54b1da0215290", null ]
    ] ],
    [ "enet_rx_bd_control_status_t", "group__enet__hal.html#gaaecaf56644e78cce816b9610825b8493", [
      [ "kEnetRxBdEmpty", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a17942af84bade8d36f4e25908d1298cc", null ],
      [ "kEnetRxBdRxSoftOwner1", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a9c28f72c6cb706d33d40bfef4a946779", null ],
      [ "kEnetRxBdWrap", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a9ffa107370c8db848708dfe3b1063a98", null ],
      [ "kEnetRxBdRxSoftOwner2", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493ae0ff21d31ac278ac27f3efa4d2b04dde", null ],
      [ "kEnetRxBdLast", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a1561114470dae7f56b95675b4f14f7b3", null ],
      [ "kEnetRxBdMiss", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a09570729a0571f25d70ce1d85625c7d8", null ],
      [ "kEnetRxBdBroadCast", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a0d4ea47a8f00ffc45455dee0b0579d1b", null ],
      [ "kEnetRxBdMultiCast", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493ab454c4f020616097dcce0d97d49a5c44", null ],
      [ "kEnetRxBdLengthViolation", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493aa2936b889fd36fd622ae167e946d3346", null ],
      [ "kEnetRxBdNoOctet", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a651d8f7af70b271b520ef612cd262b43", null ],
      [ "kEnetRxBdCrc", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493aeebd73c85d31fa9bfd9a89f458f660d9", null ],
      [ "kEnetRxBdOverRun", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493ad1f39fb172a636cc1b85944a7c30ae84", null ],
      [ "kEnetRxBdTrunc", "group__enet__hal.html#ggaaecaf56644e78cce816b9610825b8493a473205ffdb6e228d3f8670e73cf85622", null ]
    ] ],
    [ "enet_rx_bd_control_extend0_t", "group__enet__hal.html#ga7069c75b56924456d4bdb3c7ec86bb3a", [
      [ "kEnetRxBdIpv4", "group__enet__hal.html#gga7069c75b56924456d4bdb3c7ec86bb3aaac7cc5302df27f5891339dd92797c1a2", null ],
      [ "kEnetRxBdIpv6", "group__enet__hal.html#gga7069c75b56924456d4bdb3c7ec86bb3aac7d39644b0a147a14489afe562535f3e", null ],
      [ "kEnetRxBdVlan", "group__enet__hal.html#gga7069c75b56924456d4bdb3c7ec86bb3aaa50349fc3dd9584c0956aa73467d704f", null ],
      [ "kEnetRxBdProtocolChecksumErr", "group__enet__hal.html#gga7069c75b56924456d4bdb3c7ec86bb3aa9b4d919b4e1c693b4454f182dc821ced", null ],
      [ "kEnetRxBdIpHeaderChecksumErr", "group__enet__hal.html#gga7069c75b56924456d4bdb3c7ec86bb3aa45b71ba4bbe00078a71ddd09bd21486f", null ]
    ] ],
    [ "enet_rx_bd_control_extend1_t", "group__enet__hal.html#gaa7f62acf5254675237d286c677cced10", [
      [ "kEnetRxBdIntrrupt", "group__enet__hal.html#ggaa7f62acf5254675237d286c677cced10ab953051269703f2ef38f1ec67cd46a34", null ],
      [ "kEnetRxBdUnicast", "group__enet__hal.html#ggaa7f62acf5254675237d286c677cced10a8c1c42d5db6b125042a5fba01fbf9f5f", null ],
      [ "kEnetRxBdCollision", "group__enet__hal.html#ggaa7f62acf5254675237d286c677cced10a5f4bdd24e31c2abe9c77a59f84cf2876", null ],
      [ "kEnetRxBdPhyErr", "group__enet__hal.html#ggaa7f62acf5254675237d286c677cced10aab289d5f1b1ff6d0a87cce595b97524f", null ],
      [ "kEnetRxBdMacErr", "group__enet__hal.html#ggaa7f62acf5254675237d286c677cced10ae687e0c0040dc269866e4ffa03358ca4", null ]
    ] ],
    [ "enet_tx_bd_control_status_t", "group__enet__hal.html#gac927d989b03928bd15f1d6e88e157962", [
      [ "kEnetTxBdReady", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962ad6ba63b7317d0d4a2f641b76199e680b", null ],
      [ "kEnetTxBdTxSoftOwner1", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962aa451851904d19741d28407e6a720d593", null ],
      [ "kEnetTxBdWrap", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962a80b686bed11127a27b4b9546da806110", null ],
      [ "kEnetTxBdTxSoftOwner2", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962a67f9de131c6fae04b38cfd65d1c06b71", null ],
      [ "kEnetTxBdLast", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962af0bd99ebb4d1784ad17a36081dcdbbf9", null ],
      [ "kEnetTxBdTransmitCrc", "group__enet__hal.html#ggac927d989b03928bd15f1d6e88e157962aa8b14584473924aa6f5a6e2af4b4f8a0", null ]
    ] ],
    [ "enet_tx_bd_control_extend0_t", "group__enet__hal.html#ga722ec151b798e6ba3eb7eba4b56d5cfb", [
      [ "kEnetTxBdTxErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfba6fe87a4b8b0787b9f5bfe06e791cb5dc", null ],
      [ "kEnetTxBdTxUnderFlowErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfba96372bbda198682bcf0c169b8b4455c5", null ],
      [ "kEnetTxBdExcessCollisionErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfbafb26a6bc1b13e237934cefbab7d020b5", null ],
      [ "kEnetTxBdTxFrameErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfbad5f48efb753a58b97290218fd98c076d", null ],
      [ "kEnetTxBdLatecollisionErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfbad63ec922e5132d03faa94e8bfc360e8d", null ],
      [ "kEnetTxBdOverFlowErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfbaec8c6bee0d3257b8a39c36ed95c1a862", null ],
      [ "kEnetTxTimestampErr", "group__enet__hal.html#gga722ec151b798e6ba3eb7eba4b56d5cfba61e8213bdc5e3689317e8f0c77a2b260", null ]
    ] ],
    [ "enet_tx_bd_control_extend1_t", "group__enet__hal.html#ga8ad69dce730d314c7fc8b3c38ca2f8f0", [
      [ "kEnetTxBdTxInterrupt", "group__enet__hal.html#gga8ad69dce730d314c7fc8b3c38ca2f8f0afe74ca52b81d81a02f2ced61003daf5e", null ],
      [ "kEnetTxBdTimeStamp", "group__enet__hal.html#gga8ad69dce730d314c7fc8b3c38ca2f8f0a33e4cebf1aae4c8931ec213ea7af6dee", null ]
    ] ],
    [ "enet_constant_parameter_t", "group__enet__hal.html#ga0e9e201d974aa0bdd83ced18e9baea8f", [
      [ "kEnetMacAddrLen", "group__enet__hal.html#gga0e9e201d974aa0bdd83ced18e9baea8fa6046aea2209d6ff07c301e2f8893067a", null ],
      [ "kEnetHashValMask", "group__enet__hal.html#gga0e9e201d974aa0bdd83ced18e9baea8fade635c7bbba3ec49b9573286212485d9", null ],
      [ "kEnetMinBuffSize", "group__enet__hal.html#gga0e9e201d974aa0bdd83ced18e9baea8fa2558341b88b8d95a7d572d43c62ff300", null ],
      [ "kEnetMaxTimeout", "group__enet__hal.html#gga0e9e201d974aa0bdd83ced18e9baea8fa9a187f4e111225ca14bb55bff4815811", null ],
      [ "kEnetMdcFreq", "group__enet__hal.html#gga0e9e201d974aa0bdd83ced18e9baea8fa3deb4a08f9df237e569273569f1c2cbf", null ]
    ] ],
    [ "enet_fifo_configure_t", "group__enet__hal.html#ga38d55287830c74c423a16ab28c733dc1", [
      [ "kEnetMinTxFifoAlmostFull", "group__enet__hal.html#gga38d55287830c74c423a16ab28c733dc1ab35b2618b41de8e0bfa34ae19ae5ff9c", null ],
      [ "kEnetMinFifoAlmostEmpty", "group__enet__hal.html#gga38d55287830c74c423a16ab28c733dc1a45f00b6a6d4616c616bd2e5209158cc8", null ],
      [ "kEnetDefaultTxFifoAlmostFull", "group__enet__hal.html#gga38d55287830c74c423a16ab28c733dc1a8cb7bb0a6e49f718dd4aa9ce9e259ec6", null ]
    ] ],
    [ "enet_mac_operate_mode_t", "group__enet__hal.html#ga3ef7a6c057f63bf13453cf1f532e8dd2", [
      [ "kEnetMacNormalMode", "group__enet__hal.html#gga3ef7a6c057f63bf13453cf1f532e8dd2ab1607ba9866d2f07624de3a5d48997d1", null ],
      [ "kEnetMacSleepMode", "group__enet__hal.html#gga3ef7a6c057f63bf13453cf1f532e8dd2a65258b92923112267faa9cc17e608317", null ]
    ] ],
    [ "enet_config_rmii_mode_t", "group__enet__hal.html#gafc2cc72e1af78f5aa16674131499ff07", [
      [ "kEnetCfgMii", "group__enet__hal.html#ggafc2cc72e1af78f5aa16674131499ff07a6753e3f42941038cb430114cc04dfbc1", null ],
      [ "kEnetCfgRmii", "group__enet__hal.html#ggafc2cc72e1af78f5aa16674131499ff07a2c6cd0df0e8fa377feb5e5e84abcc151", null ]
    ] ],
    [ "enet_config_speed_t", "group__enet__hal.html#ga90f9d3632ef612f4a518088a694a5e3e", [
      [ "kEnetCfgSpeed100M", "group__enet__hal.html#gga90f9d3632ef612f4a518088a694a5e3ea24d62fa9a7c32ff39b03b12bf4402f4f", null ],
      [ "kEnetCfgSpeed10M", "group__enet__hal.html#gga90f9d3632ef612f4a518088a694a5e3ea5bf1fd2e594dec086dc4a7b982460efc", null ]
    ] ],
    [ "enet_config_duplex_t", "group__enet__hal.html#gacc4ca3757251081fb2aa637b13a97b4b", [
      [ "kEnetCfgHalfDuplex", "group__enet__hal.html#ggacc4ca3757251081fb2aa637b13a97b4ba52f2999d88782c9a53354ad95f0f71cb", null ],
      [ "kEnetCfgFullDuplex", "group__enet__hal.html#ggacc4ca3757251081fb2aa637b13a97b4ba063f7eea16f6cea27abbf0b002cc86aa", null ]
    ] ],
    [ "enet_mii_write_t", "group__enet__hal.html#ga0efd18bb4f4b754039fc30c440e38b02", [
      [ "kEnetWriteNoCompliant", "group__enet__hal.html#gga0efd18bb4f4b754039fc30c440e38b02a071aad90c6f364585ea9b9ac7ac976ea", null ],
      [ "kEnetWriteValidFrame", "group__enet__hal.html#gga0efd18bb4f4b754039fc30c440e38b02a92934397c4fd57911d41492a139ad6a1", null ]
    ] ],
    [ "enet_mii_read_t", "group__enet__hal.html#ga9c0264e7da8e7d0a3ead667d3a7b1a89", [
      [ "kEnetReadValidFrame", "group__enet__hal.html#gga9c0264e7da8e7d0a3ead667d3a7b1a89a6b19b0254c0b11aee1b3877753761905", null ],
      [ "kEnetReadNoCompliant", "group__enet__hal.html#gga9c0264e7da8e7d0a3ead667d3a7b1a89a24a9f97502a4811a4a8342b7a731401b", null ]
    ] ],
    [ "enet_special_address_filter_t", "group__enet__hal.html#ga69bbc285332a8e642111b4abae4c890c", [
      [ "kEnetSpecialAddressInit", "group__enet__hal.html#gga69bbc285332a8e642111b4abae4c890ca1255645aeec763bc6cb8be1f07d91e64", null ],
      [ "kEnetSpecialAddressEnable", "group__enet__hal.html#gga69bbc285332a8e642111b4abae4c890ca264c1ce2b705134ff69c227899f490eb", null ],
      [ "kEnetSpecialAddressDisable", "group__enet__hal.html#gga69bbc285332a8e642111b4abae4c890ca3db2df7fceb88c77a41cc01f80bf8276", null ]
    ] ],
    [ "enet_timer_channel_t", "group__enet__hal.html#ga5990bf42d6a5a95052c1200beadddc2c", [
      [ "kEnetTimerChannel1", "group__enet__hal.html#gga5990bf42d6a5a95052c1200beadddc2ca2eb7095345cae053eab9ebd07860742c", null ],
      [ "kEnetTimerChannel2", "group__enet__hal.html#gga5990bf42d6a5a95052c1200beadddc2ca3891fd3a7ad3b67f35b0dd99868b03f2", null ],
      [ "kEnetTimerChannel3", "group__enet__hal.html#gga5990bf42d6a5a95052c1200beadddc2ca918b58b1b1a8b42e092c18b1bb708c1a", null ],
      [ "kEnetTimerChannel4", "group__enet__hal.html#gga5990bf42d6a5a95052c1200beadddc2ca9891f4496250983472a8a9a154b0d242", null ]
    ] ],
    [ "enet_timer_channel_mode_t", "group__enet__hal.html#ga5ecdc252559025df704a07bb9453c9e9", [
      [ "kEnetChannelDisable", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9aeee7a9ee62bc50ade797545c5221f09f", null ],
      [ "kEnetChannelRisingCapture", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a8bdb1fb4ba9b5888b6824e7b84a7c066", null ],
      [ "kEnetChannelFallingCapture", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9ac14af6586ecc07ec73d7ff9201ac1a29", null ],
      [ "kEnetChannelBothCapture", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9ab46fc5a03c8a1b589426021fdb0685ee", null ],
      [ "kEnetChannelSoftCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a8dfb0724dbe2c0d93456a1aacc91a98f", null ],
      [ "kEnetChannelToggleCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a9c62e8689b45eae5e62b181e333e94a1", null ],
      [ "kEnetChannelClearCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a5c0bb050ff22c490511f2bec98f65d3c", null ],
      [ "kEnetChannelSetCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9ad144627e784f61e170bb1461ea6a7244", null ],
      [ "kEnetChannelClearCompareSetOverflow", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a31c50de4a0f70b8fdb370576bd5f0aff", null ],
      [ "kEnetChannelSetCompareClearOverflow", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a75edd090db67ef990efb255cd766649d", null ],
      [ "kEnetChannelPulseLowonCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9a29728b5854627088d6b4ca968584fef2", null ],
      [ "kEnetChannelPulseHighonCompare", "group__enet__hal.html#gga5ecdc252559025df704a07bb9453c9e9ae83d3f6d1ab5a6dbf38f601d8516fae3", null ]
    ] ],
    [ "enet_interrupt_request_t", "group__enet__hal.html#ga849ae37d9db7202edcec94e310b63481", [
      [ "kEnetBabrInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481aa502d77a8a8fad30e0b73cca3df02082", null ],
      [ "kEnetBabtInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481aa10d5c05e48ed9c482b3dc15a2c3991f", null ],
      [ "kEnetGraceStopInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481aa92b71c1ae0de09fc6ec0ccd077fbe4e", null ],
      [ "kEnetTxFrameInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481ac5752ccd614dda5bd694abde3eec5497", null ],
      [ "kEnetTxByteInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481abb97997f0f82b63383d83e9d6f27a5f2", null ],
      [ "kEnetRxFrameInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a457aa3155b96a7f7fa0d05a021ca93a9", null ],
      [ "kEnetRxByteInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a8aac2950a75d8778b282b71ecac1fbfd", null ],
      [ "kEnetMiiInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481af79fec08f7a0b8573f5278af4d705805", null ],
      [ "kEnetEBusERInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a144736452d2075a38b942830ea3f01df", null ],
      [ "kEnetLateCollisionInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a5fbd71cd8c33b15896068f791750addc", null ],
      [ "kEnetRetryLimitInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a80814a8a12e4f063fbf1762d77fe6e26", null ],
      [ "kEnetUnderrunInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a11b0480f2f13e5282e3522716fe264b1", null ],
      [ "kEnetPayloadRxInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481aa2e48d44d47e91bea5353b0fb0aef5dc", null ],
      [ "kEnetWakeupInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481ae5f3b22e033392e927d0d58d0027c8d5", null ],
      [ "kEnetTsAvailInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a3868d71c81eef37c68d2301e5da3f663", null ],
      [ "kEnetTsTimerInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481a4207a99d4ec78fcb2a489152798aa298", null ],
      [ "kEnetAllInterrupt", "group__enet__hal.html#gga849ae37d9db7202edcec94e310b63481ac3546575279c631eb6b44655aa4715d9", null ]
    ] ],
    [ "enet_irq_number_t", "group__enet__hal.html#ga188b39e059d6e9fcc6e4908413e1eff3", [
      [ "kEnetTsTimerNumber", "group__enet__hal.html#gga188b39e059d6e9fcc6e4908413e1eff3ac7e44458d4bbc2a23458708b57d9c85c", null ],
      [ "kEnetReceiveNumber", "group__enet__hal.html#gga188b39e059d6e9fcc6e4908413e1eff3aedf1e65edd70ab60e971a62001bff29a", null ],
      [ "kEnetTransmitNumber", "group__enet__hal.html#gga188b39e059d6e9fcc6e4908413e1eff3a43c26aa13caff2d28a62fe1ebeb9aa30", null ],
      [ "kEnetMiiErrorNumber", "group__enet__hal.html#gga188b39e059d6e9fcc6e4908413e1eff3a0ab4ed62dc73a04eab6f309c572e2c1b", null ]
    ] ],
    [ "enet_frame_max_t", "group__enet__hal.html#gacc88529688c625d67ba84c5e1dbe273a", [
      [ "kEnetNsecOneSec", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa795a264aa9bae01dbf09d0930a80bc38", null ],
      [ "kEnetMaxFrameSize", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aaf383e7bd05019bc27865dd15eed5e213", null ],
      [ "kEnetMaxFrameVlanSize", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aab3c73c6a827f9d6eec390f1bdd8479bf", null ],
      [ "kEnetMaxFrameDateSize", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa87c3bc176281c7da2224e0b0bf20fe38", null ],
      [ "kEnetDefaultTruncLen", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa0f87d184fa1af33e5aeb90b272ce28c6", null ],
      [ "kEnetDefaultIpg", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa7a30a729be5212d6ed5a78da38a3d03d", null ],
      [ "kEnetMaxValidTxIpg", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa9ca9ed9f8369e91424d19c11ddee8fc1", null ],
      [ "kEnetMinValidTxIpg", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa4881e16b7dd05d31f6a172fade792e30", null ],
      [ "kEnetMaxMdioHoldCycle", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aad831a81dfd5be6cb01e867b0bb8438ae", null ],
      [ "kEnetMaxFrameBdNumbers", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa2ceebde5de66829971ef3928be0a51f3", null ],
      [ "kEnetFrameFcsLen", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa17253e9ee760d2845c488b289a96422f", null ],
      [ "kEnetEthernetHeadLen", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aaef03975d9867b3d0fa39a588eb649d67", null ],
      [ "kEnetEthernetVlanHeadLen", "group__enet__hal.html#ggacc88529688c625d67ba84c5e1dbe273aa95fafbcf61e7e85f6535afd6f667ab52", null ]
    ] ],
    [ "enet_txaccelerator_config_t", "group__enet__hal.html#ga0fa587d1b52c143a17de1043582a567b", [
      [ "kEnetTxAccelisShift16Enabled", "group__enet__hal.html#gga0fa587d1b52c143a17de1043582a567baf89d29fde37214301cc7c849d8f75d24", null ],
      [ "kEnetTxAccelIpCheckEnabled", "group__enet__hal.html#gga0fa587d1b52c143a17de1043582a567baf5b18ff0ca770924d8ec7cf7ccf4cded", null ],
      [ "kEnetTxAccelProtoCheckEnabled", "group__enet__hal.html#gga0fa587d1b52c143a17de1043582a567bae8498916bfe9a71a549b4da6180152f8", null ]
    ] ],
    [ "enet_rxaccelerator_config_t", "group__enet__hal.html#ga93306e69aa85d75c08630eaaebbf48b3", [
      [ "kEnetRxAccelPadRemoveEnabled", "group__enet__hal.html#gga93306e69aa85d75c08630eaaebbf48b3acf24b5e73c4a12c583b7d4a70f5adb47", null ],
      [ "kEnetRxAccelIpCheckEnabled", "group__enet__hal.html#gga93306e69aa85d75c08630eaaebbf48b3a847369e18a0d6250455eb8e1f8d943ce", null ],
      [ "kEnetRxAccelProtoCheckEnabled", "group__enet__hal.html#gga93306e69aa85d75c08630eaaebbf48b3af75089c12f5ad6cfb3ce25782baf0cae", null ],
      [ "kEnetRxAccelMacCheckEnabled", "group__enet__hal.html#gga93306e69aa85d75c08630eaaebbf48b3ab86343c8d974722584810ea8fe6867a2", null ],
      [ "kEnetRxAccelisShift16Enabled", "group__enet__hal.html#gga93306e69aa85d75c08630eaaebbf48b3a1054d079bb21fefd29b0b468f28d94e2", null ]
    ] ],
    [ "enet_mac_control_flag_t", "group__enet__hal.html#ga16892e12fab260a803adc80450555c95", [
      [ "kEnetStopModeEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a50f2f4c8f06a6e4853e4dd128a564f1d", null ],
      [ "kEnetPayloadlenCheckEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95aac5f5d947ddb380d66cddc9dca54e04b", null ],
      [ "kEnetRxFlowControlEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ac75aa041395eb8f3afb46fc91d26b753", null ],
      [ "kEnetRxCrcFwdEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a65e953c9350ba50041990f2d45b22d60", null ],
      [ "kEnetRxPauseFwdEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a3bb3fbdd8b831f6006e0a5ec32cb7b33", null ],
      [ "kEnetRxPadRemoveEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95adb4d988a5b93e02c4ae2f13bc0bb5df3", null ],
      [ "kEnetRxBcRejectEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a305f1b4430defedf74558b862885e590", null ],
      [ "kEnetRxPromiscuousEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95aec382ad86ff56e6561852f30a3b0e2ab", null ],
      [ "kEnetTxCrcFwdEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a42fe160374aba8ce7f278bea29fceb0b", null ],
      [ "kEnetTxCrcBdEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a44e22be983e7cc1a2090de465cdd06d2", null ],
      [ "kEnetMacAddrInsert", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ac3ab057959af8de7cbb18045cd725157", null ],
      [ "kEnetTxAccelEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ae3662161a47cd4a529800963935f0d7b", null ],
      [ "kEnetRxAccelEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ac91ad8e420b348b2e95826ae18c8e1b3", null ],
      [ "kEnetStoreAndFwdEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a1a9a4dac6e96b7f4633e3c423a595f67", null ],
      [ "kEnetMacMibEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ac5e81fbdfda948fd46a3681eb29e2d91", null ],
      [ "kEnetSMIPreambleDisable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ab8856a5b718bf22fc25b83fbbd1774d5", null ],
      [ "kEnetVlanTagEnabled", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95a919a303d818cbc42699bd363bc242925", null ],
      [ "kEnetMacEnhancedEnable", "group__enet__hal.html#gga16892e12fab260a803adc80450555c95ac88ff8f722392a8a926f5cd1ce11d00e", null ]
    ] ],
    [ "enet_en_dynamical_act_t", "group__enet__hal.html#ga0025fc69273206ef530443f932fc8227", [
      [ "kEnGraceSendStop", "group__enet__hal.html#gga0025fc69273206ef530443f932fc8227a26f87c06b75de8944b74422eb7959568", null ],
      [ "kEnSendPauseFrame", "group__enet__hal.html#gga0025fc69273206ef530443f932fc8227aeb069b869b4a45712ae4223d4004a829", null ],
      [ "kEnClearMibCounter", "group__enet__hal.html#gga0025fc69273206ef530443f932fc8227a9010ad99bec8f0a7ce0a607822f6c90e", null ]
    ] ],
    [ "ENET_HAL_Init", "group__enet__hal.html#ga72cfbac7a6330f587e54f98e121d90cb", null ],
    [ "ENET_HAL_Config", "group__enet__hal.html#gacc70509c6aa5a2fb8d0f5b6d15479a7a", null ],
    [ "ENET_HAL_GetStatus", "group__enet__hal.html#ga17b328204be5464227390ec46e2e8f0a", null ],
    [ "ENET_HAL_SetMulticastAddrHash", "group__enet__hal.html#ga5283aa8134797ddc92d61eacce8a6cb2", null ],
    [ "ENET_HAL_GetBufDescripAttr", "group__enet__hal.html#ga65cf8b918ab641fff2e85179ec6202de", null ],
    [ "ENET_HAL_GetBuffDescripData", "group__enet__hal.html#ga6e34e70e40e54054ac25cd7ac1a09fbc", null ],
    [ "ENET_HAL_ClrRxBdAfterHandled", "group__enet__hal.html#ga181fb64fdd93868b5b237c5e98000560", null ],
    [ "ENET_HAL_SetTxBdBeforeSend", "group__enet__hal.html#gaf4d723ef25192d32f7068f2ab71e6200", null ],
    [ "ENET_HAL_ClrTxBdAfterSend", "group__enet__hal.html#ga7403d4bb8491dac823dbe0cbfb91c7ed", null ],
    [ "ENET_HAL_SetRxBdActive", "group__enet__hal.html#gace0cbb055d701ab0e14508b63c763aea", null ],
    [ "ENET_HAL_SetTxBdActive", "group__enet__hal.html#ga52198e5257b7cbf182b66ac208fdf0dc", null ],
    [ "ENET_HAL_SetRMIIMode", "group__enet__hal.html#gabaca6b6958d3ce7328e0b18df3e1dad6", null ],
    [ "ENET_HAL_GetSMIData", "group__enet__hal.html#ga4da9f643a10423dd022d1e1b4296cb7a", null ],
    [ "ENET_HAL_SetSMIRead", "group__enet__hal.html#ga3145ae67097a7ef33bb6630001031080", null ],
    [ "ENET_HAL_SetSMIWrite", "group__enet__hal.html#gab096df03a03eab45c2d3095ca1ae8542", null ],
    [ "ENET_HAL_EnDynamicalAct", "group__enet__hal.html#ga4a9826823c03497f6a58ce321de101c2", null ],
    [ "ENET_HAL_Enable", "group__enet__hal.html#gae4cfae314ab0566addb6f24766135969", null ],
    [ "ENET_HAL_Disable", "group__enet__hal.html#gacd92c913326fd767e0278b296cc4eab8", null ],
    [ "ENET_HAL_SetIntMode", "group__enet__hal.html#gaa39b7f962c4475acc4de704b69e95e9f", null ],
    [ "ENET_HAL_ClearIntStatusFlag", "group__enet__hal.html#ga0d223867769d61133bb9e946a37b4918", null ],
    [ "ENET_HAL_GetIntStatusFlag", "group__enet__hal.html#ga445d7040a5c8bc8be0a91376262b02a7", null ],
    [ "ENET_HAL_Start1588Timer", "group__enet__hal.html#ga5801b521174f42982c93c0f92f4b668e", null ],
    [ "ENET_HAL_Stop1588Timer", "group__enet__hal.html#ga2701e04ab98917ce39e8852dfa3a64ef", null ],
    [ "ENET_HAL_Adjust1588Timer", "group__enet__hal.html#ga2e58ed646c99f32defc64230d26238ab", null ],
    [ "ENET_HAL_Set1588TimerNewTime", "group__enet__hal.html#ga226809727d2ef67da983c5d2d8bba776", null ],
    [ "ENET_HAL_Get1588TimerCurrentTime", "group__enet__hal.html#gacccad78db1a67209b9379e643b0c1698", null ],
    [ "ENET_HAL_Get1588TimerChnStatus", "group__enet__hal.html#gaca7812272d7f510ca7cbc6d82d19492b", null ],
    [ "ENET_HAL_Rst1588TimerCmpValAndClrFlag", "group__enet__hal.html#gac9310bcdbbcbf677d3d748a9e4ea111e", null ]
];