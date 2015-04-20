var group__dma__hal =
[
    [ "dma_channel_link_config_t", "group__dma__hal.html#structdma__channel__link__config__t", [
      [ "linkType", "group__dma__hal.html#a377415cfa6e618ecfb2ba154e828cf8b", null ],
      [ "channel1", "group__dma__hal.html#afc71a80292db667048cb89cf952aa04a", null ],
      [ "channel2", "group__dma__hal.html#ab708f793951a1729ff8687adb8dcf696", null ]
    ] ],
    [ "dma_error_status_t", "group__dma__hal.html#uniondma__error__status__t", [
      [ "dmaBytesToBeTransffered", "group__dma__hal.html#a06e0915aea24357f22652651ebcf1251", null ],
      [ "dmaTransDone", "group__dma__hal.html#a645caae035d7e0fd34205ed8bf87f373", null ],
      [ "dmaBusy", "group__dma__hal.html#a242225b9abbfc88537fd48b96a348421", null ],
      [ "dmaPendingRequest", "group__dma__hal.html#afd8c2adbb9bfd2240ba2ff7eb4202f5d", null ],
      [ "_reserved1", "group__dma__hal.html#ad71be49ba6627a9cf80451b8dcc49582", null ],
      [ "dmaDestBusError", "group__dma__hal.html#a19ebd768a096dbd46fe59b2c42885c33", null ],
      [ "dmaSourceBusError", "group__dma__hal.html#a4a1c93a284488f85ad50ac9bc043dd5b", null ],
      [ "dmaConfigError", "group__dma__hal.html#a6176ffc807ed223179dcd339e66e2efe", null ],
      [ "_reserved0", "group__dma__hal.html#a23b04ea195b29b7343fd31d928b9e886", null ],
      [ "u", "group__dma__hal.html#a81e4b438cbd3fcdb3a6eabf15b46df36", null ],
      [ "b", "group__dma__hal.html#ab9a33bc3ca3dd16f0918d3bc26ac0438", null ]
    ] ],
    [ "dma_status_t", "group__dma__hal.html#ga5bdf9db6442e395a98b093485a7ce5b5", [
      [ "kStatus_DMA_InvalidArgument", "group__dma__hal.html#gga5bdf9db6442e395a98b093485a7ce5b5aa8a17be8607f18acf4ba27ff341d306d", null ],
      [ "kStatus_DMA_Fail", "group__dma__hal.html#gga5bdf9db6442e395a98b093485a7ce5b5abba01b7b5b79c4ef83a606018de551bd", null ]
    ] ],
    [ "dma_transfer_size_t", "group__dma__hal.html#ga4f754951efc1486472c14f009a02ac47", [
      [ "kDmaTransfersize32bits", "group__dma__hal.html#gga4f754951efc1486472c14f009a02ac47a9e777a68c71445888b02cd0bca7f2c95", null ],
      [ "kDmaTransfersize8bits", "group__dma__hal.html#gga4f754951efc1486472c14f009a02ac47aa33e3fd0c1a3e148e5ee616baf5c5d3f", null ],
      [ "kDmaTransfersize16bits", "group__dma__hal.html#gga4f754951efc1486472c14f009a02ac47a507fa4d0c85a5109433c4043f0d7e6fd", null ]
    ] ],
    [ "dma_modulo_t", "group__dma__hal.html#gac76d0efebec5b4912a0156f9d7d1c3b2", null ],
    [ "dma_channel_link_type_t", "group__dma__hal.html#ga4ff91ace4e5146d14cd505235a2c618e", [
      [ "kDmaChannelLinkDisable", "group__dma__hal.html#gga4ff91ace4e5146d14cd505235a2c618eab708548f1367c9e0c2e50b60e98ddac4", null ],
      [ "kDmaChannelLinkChan1AndChan2", "group__dma__hal.html#gga4ff91ace4e5146d14cd505235a2c618eadbdb4a514b0c47981977b7ffc7b2ce89", null ],
      [ "kDmaChannelLinkChan1", "group__dma__hal.html#gga4ff91ace4e5146d14cd505235a2c618ea39ba5f7aa26fcae98537709af35a2040", null ],
      [ "kDmaChannelLinkChan1AfterBCR0", "group__dma__hal.html#gga4ff91ace4e5146d14cd505235a2c618eaf16e20cb620c629f89d03831e7e3fedb", null ]
    ] ],
    [ "dma_transfer_type_t", "group__dma__hal.html#ga9cb7087af6efc80106c1033f80d60219", [
      [ "kDmaPeripheralToMemory", "group__dma__hal.html#gga9cb7087af6efc80106c1033f80d60219a8d348688389648b2f07486db0342cf36", null ],
      [ "kDmaMemoryToPeripheral", "group__dma__hal.html#gga9cb7087af6efc80106c1033f80d60219aa6471688bbf3f78bf4d048c3a8c06c99", null ],
      [ "kDmaMemoryToMemory", "group__dma__hal.html#gga9cb7087af6efc80106c1033f80d60219aff39b9c9f7450a5f608b901a0cd5c535", null ],
      [ "kDmaPeripheralToPeripheral", "group__dma__hal.html#gga9cb7087af6efc80106c1033f80d60219aa468fc60f806d615ad49185ab7703236", null ]
    ] ],
    [ "DMA_HAL_Init", "group__dma__hal.html#ga91ee4727a58f369e26fec78f028dd46e", null ],
    [ "DMA_HAL_ConfigTransfer", "group__dma__hal.html#gaec4a66f935a9fab426653d69df9a7534", null ],
    [ "DMA_HAL_SetSourceAddr", "group__dma__hal.html#gacb5021e2a5c5a3f20364dab2ffb2fa1a", null ],
    [ "DMA_HAL_SetDestAddr", "group__dma__hal.html#gaecc66846842a4b191f7a359b347354c3", null ],
    [ "DMA_HAL_SetTransferCount", "group__dma__hal.html#ga79be0f640a7be54c657fcb3e085a2b02", null ],
    [ "DMA_HAL_GetUnfinishedByte", "group__dma__hal.html#gad70d34913b8e60ef21aa9f61b15c32fb", null ],
    [ "DMA_HAL_SetIntCmd", "group__dma__hal.html#ga8ed27ea4f67ca1ecf480dc59b7ca0297", null ],
    [ "DMA_HAL_SetCycleStealCmd", "group__dma__hal.html#ga6d423d24b1bceeedc047ee9a0f8c334d", null ],
    [ "DMA_HAL_SetAutoAlignCmd", "group__dma__hal.html#ga8a158ffd6ef3c941f59186b07052a495", null ],
    [ "DMA_HAL_SetAsyncDmaRequestCmd", "group__dma__hal.html#ga3dace339055dc3db136c3d0dbc08ecaf", null ],
    [ "DMA_HAL_SetSourceIncrementCmd", "group__dma__hal.html#gabedb846e4b07728bce803f18cdad9754", null ],
    [ "DMA_HAL_SetDestIncrementCmd", "group__dma__hal.html#gaf78dddb7310211fac86a79305c968878", null ],
    [ "DMA_HAL_SetSourceTransferSize", "group__dma__hal.html#ga571eb07abf67df9a8bc6c3155c7e46ee", null ],
    [ "DMA_HAL_SetDestTransferSize", "group__dma__hal.html#gac9db6851844bf405ac73bfc6894083f5", null ],
    [ "DMA_HAL_SetTriggerStartCmd", "group__dma__hal.html#ga84753b4f4c1c3dc20ed9cdaddd73daed", null ],
    [ "DMA_HAL_SetSourceModulo", "group__dma__hal.html#gae37869c221b86e738c0d412c6c7636fb", null ],
    [ "DMA_HAL_SetDestModulo", "group__dma__hal.html#ga27b829175e3b02f7f4b8bcd98eb5a989", null ],
    [ "DMA_HAL_SetDmaRequestCmd", "group__dma__hal.html#ga61b776dcc14e50990b5077e83c8d1fa8", null ],
    [ "DMA_HAL_SetDisableRequestAfterDoneCmd", "group__dma__hal.html#ga2fd6d06d48b47f61099dc9a409039806", null ],
    [ "DMA_HAL_SetChanLink", "group__dma__hal.html#gab95117cb6c693631b23af3e2cd6f9428", null ],
    [ "DMA_HAL_ClearStatus", "group__dma__hal.html#ga48ac5604fea36acaf5d0a4033a861efa", null ],
    [ "DMA_HAL_GetStatus", "group__dma__hal.html#gac4b3137f13499cb36248b38e09a28b03", null ]
];