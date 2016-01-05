var group__edma__driver =
[
    [ "edma_user_config_t", "group__edma__driver.html#structedma__user__config__t", [
      [ "chnArbitration", "group__edma__driver.html#af6a9367e435cdcc1a239a7ccbd38aa13", null ],
      [ "notHaltOnError", "group__edma__driver.html#a3d0df54ed2a5b6840db6a279d6ba9066", null ]
    ] ],
    [ "edma_chn_state_t", "group__edma__driver.html#structedma__chn__state__t", [
      [ "channel", "group__edma__driver.html#a36a488cb5646ae23e1cd27fa4c898f0c", null ],
      [ "callback", "group__edma__driver.html#abd9d3f19806732ca4e66028bd5145ed8", null ],
      [ "parameter", "group__edma__driver.html#aae506339c8a30928848bedfa85f05169", null ],
      [ "status", "group__edma__driver.html#a5399b860ee024d8a672d7e539109ae17", null ]
    ] ],
    [ "edma_scatter_gather_list_t", "group__edma__driver.html#structedma__scatter__gather__list__t", [
      [ "address", "group__edma__driver.html#aaef294d34d3639dc86866729b7ee7371", null ],
      [ "length", "group__edma__driver.html#adde676116023ca67b108ff22b777980f", null ]
    ] ],
    [ "edma_state_t", "group__edma__driver.html#structedma__state__t", [
      [ "chn", "group__edma__driver.html#a4cd4993b52b9f2d9e080571817e446c0", null ]
    ] ],
    [ "STCD_SIZE", "group__edma__driver.html#ga7f0960bc8b86ebdf70632f839e914832", null ],
    [ "VIRTUAL_CHN_TO_EDMA_MODULE_REGBASE", "group__edma__driver.html#ga2153971dae04c6beaa8f793ba70b6700", null ],
    [ "VIRTUAL_CHN_TO_EDMA_CHN", "group__edma__driver.html#ga92d18817e1451a961d81c8d4357501d6", null ],
    [ "VIRTUAL_CHN_TO_DMAMUX_MODULE_REGBASE", "group__edma__driver.html#ga5dea015770302df2f22a45a07104ec2e", null ],
    [ "VIRTUAL_CHN_TO_DMAMUX_CHN", "group__edma__driver.html#ga46e69802a3047dce07c1075d7049b3a3", null ],
    [ "edma_callback_t", "group__edma__driver.html#gab79e6ce110f65567abad8afb7dd94309", null ],
    [ "edma_chn_status_t", "group__edma__driver.html#ga10b6a19b129232cb5dbbb2b320c9c224", [
      [ "kEDMAChnNormal", "group__edma__driver.html#gga10b6a19b129232cb5dbbb2b320c9c224a048d8fdbd086fe00875c1c1eaa5776de", null ],
      [ "kEDMAChnIdle", "group__edma__driver.html#gga10b6a19b129232cb5dbbb2b320c9c224adf8d761a4dfef11c2c96503642e292d7", null ],
      [ "kEDMAChnError", "group__edma__driver.html#gga10b6a19b129232cb5dbbb2b320c9c224a6e48551773d676e0b0a654f49e8240c5", null ]
    ] ],
    [ "edma_chn_state_type_t", "group__edma__driver.html#gaf2953f24d4e77d2b48a81649b388e0c9", [
      [ "kEDMAInvalidChannel", "group__edma__driver.html#ggaf2953f24d4e77d2b48a81649b388e0c9a86bc626247a5202c28081d91d93fff07", null ],
      [ "kEDMAAnyChannel", "group__edma__driver.html#ggaf2953f24d4e77d2b48a81649b388e0c9a23ba047d4c39c222968ee427f13f0b30", null ]
    ] ],
    [ "edma_transfer_type_t", "group__edma__driver.html#ga7803399034b374663f76a589da7d8419", [
      [ "kEDMAPeripheralToMemory", "group__edma__driver.html#gga7803399034b374663f76a589da7d8419ae94ab12f5f1047bb1d6d7ac7138a0b76", null ],
      [ "kEDMAMemoryToPeripheral", "group__edma__driver.html#gga7803399034b374663f76a589da7d8419a7439f8629c412b13978697220830e862", null ],
      [ "kEDMAMemoryToMemory", "group__edma__driver.html#gga7803399034b374663f76a589da7d8419aefd101c1a4783b9efa6595cff7ddea7b", null ]
    ] ],
    [ "EDMA_DRV_Init", "group__edma__driver.html#ga7deb54a7e997d79de1ed7c4e85290688", null ],
    [ "EDMA_DRV_Deinit", "group__edma__driver.html#ga3a4960bfad9f247d14b59041415dbc3a", null ],
    [ "EDMA_DRV_RequestChannel", "group__edma__driver.html#ga447c2ba3ddc2f7539d9dcd643ee2cc6d", null ],
    [ "EDMA_DRV_ReleaseChannel", "group__edma__driver.html#ga20385ab2bc82696e334dabcb00e670c0", null ],
    [ "EDMA_DRV_PrepareDescriptorTransfer", "group__edma__driver.html#ga09a0300eac8bc8a616937b80cd98489e", null ],
    [ "EDMA_DRV_PrepareDescriptorScatterGather", "group__edma__driver.html#ga9e3d50233ea8b0e3879c4f51cd15d1c9", null ],
    [ "EDMA_DRV_PrepareDescriptorChannelLink", "group__edma__driver.html#gabaf1b34ac08208964eb0c9b6a73a4e79", null ],
    [ "EDMA_DRV_PrepareDescriptorMinorLink", "group__edma__driver.html#gac26313d1cbe28eb05345d53bcee61cb8", null ],
    [ "EDMA_DRV_TriggerChannelStart", "group__edma__driver.html#ga43a98473ced305ae2cba2d92e5ea11cf", null ],
    [ "EDMA_DRV_PushDescriptorToReg", "group__edma__driver.html#ga41390e32e4a85b91201830a84a5fcd60", null ],
    [ "EDMA_DRV_ConfigLoopTransfer", "group__edma__driver.html#ga2cbe648d04e75b237eaa21f68fafa11c", null ],
    [ "EDMA_DRV_ConfigScatterGatherTransfer", "group__edma__driver.html#ga33c27d831a0a4e6374076278b85d4a67", null ],
    [ "EDMA_DRV_StartChannel", "group__edma__driver.html#gae5dc17a0595a4e27b4877edea7549ece", null ],
    [ "EDMA_DRV_StopChannel", "group__edma__driver.html#gaf0b33587b9f7ab6321c5e390564d0aa7", null ],
    [ "EDMA_DRV_InstallCallback", "group__edma__driver.html#ga8bb06f10bb3021e9847cea985f787301", null ],
    [ "EDMA_DRV_IRQHandler", "group__edma__driver.html#ga64bd6fed67de6dfd3ac084643077b083", null ],
    [ "EDMA_DRV_ErrorIRQHandler", "group__edma__driver.html#gab93343849db92e0d475d731358c1b8c2", null ],
    [ "EDMA_DRV_GetChannelStatus", "group__edma__driver.html#gac2e2a0e878dee3214d75aae65b4371f7", null ],
    [ "EDMA_DRV_GetUnfinishedBytes", "group__edma__driver.html#ga36b79296fb955212cc3ed925da8627eb", null ],
    [ "EDMA_DRV_GetFinishedBytes", "group__edma__driver.html#ga88c4c3f2acddd4cfb242fcfb6aba073d", null ],
    [ "g_edmaBase", "group__edma__driver.html#ga2c21bae8d9d906f0d39b7b15ea4a8a27", null ],
    [ "g_dmamuxBase", "group__edma__driver.html#gab00c7d3bfc3e35cf417fd414a59b4910", null ],
    [ "g_edmaIrqId", "group__edma__driver.html#ga5a2307640d23c6b67da894bdd56b5ad8", null ],
    [ "g_edmaErrIrqId", "group__edma__driver.html#gac39c0a41538bf4c9f13ebea120c61717", null ]
];