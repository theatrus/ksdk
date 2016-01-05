var group__tsi__driver =
[
    [ "tsi_user_config_t", "group__tsi__driver.html#structtsi__user__config__t", [
      [ "config", "group__tsi__driver.html#ac06cfdf8d812c700ab637da3339bdddf", null ],
      [ "pCallBackFunc", "group__tsi__driver.html#aea54b7507dfc9e332622e2a44f94daed", null ],
      [ "usrData", "group__tsi__driver.html#a2085aae0f7ae35a02cd7ad7d8b9e886c", null ]
    ] ],
    [ "tsi_operation_mode_t", "group__tsi__driver.html#structtsi__operation__mode__t", [
      [ "enabledElectrodes", "group__tsi__driver.html#ab66c441a8faea7a6199294cad8c771dd", null ],
      [ "config", "group__tsi__driver.html#a8cddbc1039612e84875d283b6cded424", null ]
    ] ],
    [ "tsi_state_t", "group__tsi__driver.html#structtsi__state__t", [
      [ "status", "group__tsi__driver.html#ae19ca1a08e421fa65658cead53c49e86", null ],
      [ "pCallBackFunc", "group__tsi__driver.html#aa7c406ef379aa3e8fea5aede456db957", null ],
      [ "usrData", "group__tsi__driver.html#a9e451154d8ee74b74a583a6394652814", null ],
      [ "irqSync", "group__tsi__driver.html#a395ec00d39eab4ce12e52af5bda848f7", null ],
      [ "lock", "group__tsi__driver.html#ac1fd33b7ecbdf7a44907271f189a6a7f", null ],
      [ "lockChangeMode", "group__tsi__driver.html#a9fd679ccd03afe5c4477c1762c0b2d27", null ],
      [ "isBlockingMeasure", "group__tsi__driver.html#af8e3ba257449ab95172cd51394ba18be", null ],
      [ "opMode", "group__tsi__driver.html#a51856ae9c21d087c3a26016a93b5ee35", null ],
      [ "opModesData", "group__tsi__driver.html#a2b73eb0598f27f5f63df3225f98009ee", null ],
      [ "counters", "group__tsi__driver.html#ab4d1fd6cec6aaffc677c2c405a209998", null ]
    ] ],
    [ "tsi_callback_t", "group__tsi__driver.html#ga06c8a253cee00f407105143acbc36c36", null ],
    [ "tsi_modes_t", "group__tsi__driver.html#ga7a1c44ef526eb168efeb5325c4206202", [
      [ "tsi_OpModeNormal", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202a8476ba7726647ce5db603baf04cc51c6", null ],
      [ "tsi_OpModeProximity", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202adb05a5ac75ffb68159dc8ad0fe4b8d16", null ],
      [ "tsi_OpModeLowPower", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202af218dcc0bee90e41180b5c58f3348d90", null ],
      [ "tsi_OpModeNoise", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202a58b84285be55eef1195e1b382a13758a", null ],
      [ "tsi_OpModeCnt", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202ad60d8a0aea71a33bd97ffd1369aca8f0", null ],
      [ "tsi_OpModeNoChange", "group__tsi__driver.html#gga7a1c44ef526eb168efeb5325c4206202afebbf66d4297f33475b1a0e09c64d7f0", null ]
    ] ],
    [ "TSI_DRV_Init", "group__tsi__driver.html#gadae9cf629a994a96fe9be0bb83992b6d", null ],
    [ "TSI_DRV_DeInit", "group__tsi__driver.html#ga84ed5339149e76ba1a63415e512e7aa3", null ],
    [ "TSI_DRV_EnableElectrode", "group__tsi__driver.html#ga009f75442d87b335b74a809835506f8c", null ],
    [ "TSI_DRV_GetEnabledElectrodes", "group__tsi__driver.html#gafd1a247495cb551edbc6a010424e314c", null ],
    [ "TSI_DRV_Measure", "group__tsi__driver.html#ga86b101dbf4486218e09e5967fb1e7f5e", null ],
    [ "TSI_DRV_MeasureBlocking", "group__tsi__driver.html#ga1d93f36fde89f95dd8d38aa4896ae96f", null ],
    [ "TSI_DRV_AbortMeasure", "group__tsi__driver.html#ga7b27646aaa2fc1218cb0ccbac8d9b509", null ],
    [ "TSI_DRV_GetCounter", "group__tsi__driver.html#ga74907730315ef4cbd3a7e53c9d194aab", null ],
    [ "TSI_DRV_GetStatus", "group__tsi__driver.html#gaac8a3969f549f2ba2d1c745f3e7e3f5e", null ],
    [ "TSI_DRV_EnableLowPower", "group__tsi__driver.html#ga4b364e0c04feb37e50f3f14fa7c31240", null ],
    [ "TSI_DRV_DisableLowPower", "group__tsi__driver.html#ga0e9ef33fe2d4d2ff0c5cd4d0a43ed87e", null ],
    [ "TSI_DRV_Recalibrate", "group__tsi__driver.html#gae32c63bb4f527cc68adc39ccc958149d", null ],
    [ "TSI_DRV_SetCallBackFunc", "group__tsi__driver.html#ga575bd79753dccf3f4e50e46706f91c7f", null ],
    [ "TSI_DRV_ChangeMode", "group__tsi__driver.html#ga56ad2cc9672ca83cdcf40d7204b08077", null ],
    [ "TSI_DRV_GetMode", "group__tsi__driver.html#ga3595af3dd86d57eced36b49a1f9ff447", null ],
    [ "TSI_DRV_LoadConfiguration", "group__tsi__driver.html#gaba6898571a90849c5033a516d4323c75", null ],
    [ "TSI_DRV_SaveConfiguration", "group__tsi__driver.html#gac3adb8d865736779f037c0d84b540417", null ],
    [ "g_tsiBase", "group__tsi__driver.html#ga567d8769cc27b126e0e155b92b6de588", null ],
    [ "g_tsiIrqId", "group__tsi__driver.html#gac407efd6876a4040954c3bf056e661ab", null ],
    [ "g_tsiStatePtr", "group__tsi__driver.html#ga9436b458136ebfec9c31cac0e3257879", null ]
];