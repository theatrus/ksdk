var group__enet__driver =
[
    [ "enet_multicast_group_t", "group__enet__driver.html#structenet__multicast__group__t", [
      [ "groupAdddr", "group__enet__driver.html#a7c67b60ce9c5b7fe3a68464dbfbb76bf", null ],
      [ "hash", "group__enet__driver.html#a523fde871af8bc4783e242380ef45a76", null ],
      [ "next", "group__enet__driver.html#aa05b9846b92b77f2bdbec1c35b11b256", null ],
      [ "prv", "group__enet__driver.html#ad7e70b583da102e939b2068016e78da0", null ]
    ] ],
    [ "enet_ethernet_header_t", "group__enet__driver.html#structenet__ethernet__header__t", [
      [ "destAddr", "group__enet__driver.html#a31cfa9dac2cef300e42d5851f7e397d1", null ],
      [ "sourceAddr", "group__enet__driver.html#a16cddcbe9096194af8449b96af925a4c", null ],
      [ "type", "group__enet__driver.html#a72c1e58b7c5de7c6fd460ba73bc5fd59", null ]
    ] ],
    [ "enet_8021vlan_header_t", "group__enet__driver.html#structenet__8021vlan__header__t", [
      [ "destAddr", "group__enet__driver.html#a8e1b3eb0bfe156083c86a6fa0d8ba100", null ],
      [ "sourceAddr", "group__enet__driver.html#a69cc69b8bb3313873a4f7799c83eee4d", null ],
      [ "tpidtag", "group__enet__driver.html#a5b7f5391854aba0af5f73eee841e9910", null ],
      [ "othertag", "group__enet__driver.html#a7a30056c4ebd3b6a91ab42dd8008e3eb", null ],
      [ "type", "group__enet__driver.html#a6818e97a3dfe7f7cb122512a9de4b7b0", null ]
    ] ],
    [ "enet_buff_descrip_context_t", "group__enet__driver.html#structenet__buff__descrip__context__t", [
      [ "rxBdBasePtr", "group__enet__driver.html#aaec9b432cc5803459e9b6e7cf530af2a", null ],
      [ "rxBdCurPtr", "group__enet__driver.html#a5ce71fed83dc0fc27ae5413cd432e59c", null ],
      [ "rxBdDirtyPtr", "group__enet__driver.html#a940cc148e53999f807c6f63e26420fef", null ],
      [ "txBdBasePtr", "group__enet__driver.html#ad887d2cd740eaec3751e73ebc07a1248", null ],
      [ "txBdCurPtr", "group__enet__driver.html#a59eef19d31827a61914cad3c5bb1bb23", null ],
      [ "txBdDirtyPtr", "group__enet__driver.html#a018747445b156a1f22374858a8beae62", null ],
      [ "isTxBdFull", "group__enet__driver.html#aeffa52cb53fb5e99668a89c610b668ac", null ],
      [ "isRxBdFull", "group__enet__driver.html#a6b0bcba8d07d9ad62939bf7179278cbc", null ],
      [ "rxBuffSizeAlign", "group__enet__driver.html#a9cf03b8c80ab69962358dfa404af18f7", null ],
      [ "txBuffSizeAlign", "group__enet__driver.html#a896b42c7ff15cfb7d2282335270582d4", null ],
      [ "extRxBuffQue", "group__enet__driver.html#a2133a1b7811e7dde1c530c479fbdb9f1", null ],
      [ "extRxBuffNum", "group__enet__driver.html#ad02e49ad750c2218805fb9b08c3db9a8", null ]
    ] ],
    [ "enet_stats_t", "group__enet__driver.html#structenet__stats__t", [
      [ "statsRxTotal", "group__enet__driver.html#afd173d0913cec00caa0bab51f2886edb", null ],
      [ "statsTxTotal", "group__enet__driver.html#a9ce72a416d44a866f6602663e65dbbc3", null ]
    ] ],
    [ "enet_mac_packet_buffer_t", "group__enet__driver.html#structenet__mac__packet__buffer__t", [
      [ "data", "group__enet__driver.html#a2b8bf879850dd3429980700084e5d9c1", null ],
      [ "length", "group__enet__driver.html#a6c54ba8905a2cde0a35a334f02acc422", null ],
      [ "next", "group__enet__driver.html#a012f3b55a96d4e2996894532971b0489", null ]
    ] ],
    [ "enet_buff_config_t", "group__enet__driver.html#structenet__buff__config__t", [
      [ "rxBdNumber", "group__enet__driver.html#adc2043396f7eed3cbdf5a99e0203bdc3", null ],
      [ "txBdNumber", "group__enet__driver.html#aae8004affe100323de479861608799be", null ],
      [ "rxBuffSizeAlign", "group__enet__driver.html#af8aa3f85fd80d60a7bfbc35a8cd4f82a", null ],
      [ "txBuffSizeAlign", "group__enet__driver.html#a4044e9acd7523210c0c4ceda8a507ea3", null ],
      [ "rxBdPtrAlign", "group__enet__driver.html#a30847a912f3f31e6cdfcea910cd402e9", null ],
      [ "rxBufferAlign", "group__enet__driver.html#a33e7c3e75a08613866199d04368c3a56", null ],
      [ "txBdPtrAlign", "group__enet__driver.html#afe0f4acd07eb3bfd847085a336e4f9b3", null ],
      [ "txBufferAlign", "group__enet__driver.html#ab6ad77bb8c60eeb4bf7ceb59eddc6319", null ],
      [ "extRxBuffQue", "group__enet__driver.html#a3223b66056c97d66845408cff5ad8c53", null ],
      [ "extRxBuffNum", "group__enet__driver.html#ac45eed86d6fb96aba70dd6e406f0bf55", null ]
    ] ],
    [ "enet_dev_if_t", "group__enet__driver.html#structenet__dev__if__t", [
      [ "next", "group__enet__driver.html#ae52b9b6d1e9d71fb42f59958c83efbd5", null ],
      [ "netIfPrivate", "group__enet__driver.html#a4261887f2128f94bad2844a275ef0405", null ],
      [ "multiGroupPtr", "group__enet__driver.html#a7f377c420cb4a030133d70db1bf52014", null ],
      [ "deviceNumber", "group__enet__driver.html#acb618860e08266643a855eed896cf17c", null ],
      [ "macAddr", "group__enet__driver.html#a03c0e88d0dda623a0c92568987fbd981", null ],
      [ "phyAddr", "group__enet__driver.html#ae72b2cd34f72c19cf6afb9d8b3dd3834", null ],
      [ "isInitialized", "group__enet__driver.html#a7df4c8a979377e3b9dcace6e73edd846", null ],
      [ "maxFrameSize", "group__enet__driver.html#a614c2f10dd7c87b3168bace88e96b9f6", null ],
      [ "isVlanTagEnabled", "group__enet__driver.html#abdb5ae01a1232a200fd76d08446c5f65", null ],
      [ "isTxCrcEnable", "group__enet__driver.html#a5f0806b4789bc4c15205d0c3a5154acc", null ],
      [ "isRxCrcFwdEnable", "group__enet__driver.html#a07d98f2e4d5df9d1f53d67dec11349d4", null ],
      [ "bdContext", "group__enet__driver.html#af58c2bec84f431265b92706942a6bec6", null ],
      [ "stats", "group__enet__driver.html#af41eb2556d196f2410f3cd2bb957c130", null ],
      [ "enetNetifcall", "group__enet__driver.html#a804b6d083a1d267fd8966f969a5af157", null ],
      [ "enetContextSync", "group__enet__driver.html#acb148f7717729501df77413e665d1b11", null ]
    ] ],
    [ "enet_user_config_t", "group__enet__driver.html#structenet__user__config__t", [
      [ "macCfgPtr", "group__enet__driver.html#a1c3b1da5930dffe34f599e189050d0f4", null ],
      [ "buffCfgPtr", "group__enet__driver.html#a0c560f9cd602e93c3633b7a283b634e5", null ]
    ] ],
    [ "ENET_RECEIVE_ALL_INTERRUPT", "group__enet__driver.html#gaa41bd83c22bd49ef3849d887ae18d0fb", null ],
    [ "ENET_ENABLE_DETAIL_STATS", "group__enet__driver.html#ga5791c510ab826125090d1e5cde1d0d8c", null ],
    [ "ENET_HTONS", "group__enet__driver.html#ga701a204c0e6eb1b83b42e990afe07f99", null ],
    [ "ENET_ORIGINAL_CRC32", "group__enet__driver.html#gae815c7219e94a628cd67514f32fa01b3", null ],
    [ "ENET_CRC32_POLYNOMIC", "group__enet__driver.html#ga400521fbe89ace3e16c98fb77dc24227", null ],
    [ "enet_crc_parameter_t", "group__enet__driver.html#gac2dcdcac796b05a0b540ab01e633a82f", [
      [ "kEnetCrcOffset", "group__enet__driver.html#ggac2dcdcac796b05a0b540ab01e633a82faa31ca173ee4907b959fa5bcb94ea6839", null ],
      [ "kEnetCrcMask1", "group__enet__driver.html#ggac2dcdcac796b05a0b540ab01e633a82fadfdfb3545f26c29f0730fb878dcedc8e", null ]
    ] ],
    [ "enet_protocol_type_t", "group__enet__driver.html#ga9f0bc359ed9c0f3c7e75f9deb23247fd", [
      [ "kEnetProtocoll2ptpv2", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fda2ef2a3cc2cf675bb9d5c88fc666d568a", null ],
      [ "kEnetProtocolIpv4", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fdad4b2291e9a6627d80c73f6569571316b", null ],
      [ "kEnetProtocolIpv6", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fda2b14377531cb3bed4a2590838d94b3bf", null ],
      [ "kEnetProtocol8021QVlan", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fda7ab057789d10405f77f83dd2e2ff8b7f", null ],
      [ "kEnetPacketUdpVersion", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fda8f3929688b6c3f152fecd2a3f0bcad70", null ],
      [ "kEnetPacketIpv4Version", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fda0385f761876cedbac9dec06a06d6bb75", null ],
      [ "kEnetPacketIpv6Version", "group__enet__driver.html#gga9f0bc359ed9c0f3c7e75f9deb23247fdabae96470a87bab2fa899ee0d99894601", null ]
    ] ],
    [ "ENET_DRV_Init", "group__enet__driver.html#gaa9cd0b3c94ae273d6b86d729358aa8f2", null ],
    [ "ENET_DRV_Deinit", "group__enet__driver.html#ga4ef9aa6bd0c9fb49b16e196435f0dbf2", null ],
    [ "ENET_DRV_UpdateRxBuffDescrip", "group__enet__driver.html#ga0bffb6999dc620530758f6a04a7c2ff9", null ],
    [ "ENET_DRV_CleanupTxBuffDescrip", "group__enet__driver.html#gaa4530b05a9effec7c345d1b715357a98", null ],
    [ "ENET_DRV_IncrRxBuffDescripIndex", "group__enet__driver.html#gac767ede662a265b3b55d5fbea572bcbf", null ],
    [ "ENET_DRV_IncrTxBuffDescripIndex", "group__enet__driver.html#ga6ff74b97f56c58525b6897f80b1033ec", null ],
    [ "ENET_DRV_RxErrorStats", "group__enet__driver.html#gac3b5081f6975b7db79e075f6356cc65c", null ],
    [ "ENET_DRV_TxErrorStats", "group__enet__driver.html#gae6e3d9076962d24c0644c448c0df5bcf", null ],
    [ "ENET_DRV_ReceiveData", "group__enet__driver.html#ga10147ff58830521e11b7a103d157ef12", null ],
    [ "ENET_DRV_InstallNetIfCall", "group__enet__driver.html#gaea0239038ea002e73422284563bf287f", null ],
    [ "ENET_DRV_SendData", "group__enet__driver.html#gaa53d50bfeca66f73a5b58372067b3f83", null ],
    [ "ENET_DRV_RxIRQHandler", "group__enet__driver.html#gaf4317cea65d90102a58874ac81f9ca2a", null ],
    [ "ENET_DRV_TxIRQHandler", "group__enet__driver.html#gada821c44977d19fe39b4127df4fccd40", null ],
    [ "ENET_DRV_CalculateCrc32", "group__enet__driver.html#ga280de35d9bb05449d7a5128a2b6041f6", null ],
    [ "ENET_DRV_AddMulticastGroup", "group__enet__driver.html#ga1bcd951f584b9729f7abb8026bda6e4f", null ],
    [ "ENET_DRV_LeaveMulticastGroup", "group__enet__driver.html#ga6645a942648738602fffad804fd21cae", null ],
    [ "enet_mac_enqueue_buffer", "group__enet__driver.html#gaf91fc06ffd12417665373e2e5b77c614", null ],
    [ "enet_mac_dequeue_buffer", "group__enet__driver.html#ga81300d523660a5fdecbdc83c047ae142", null ],
    [ "g_enetBase", "group__enet__driver.html#gaf5188426b7119fef58b568abb95b9ca7", null ],
    [ "g_enetTxIrqId", "group__enet__driver.html#ga76029a20338e22644d9191e63d7a287d", null ]
];