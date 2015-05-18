var group__dma__hal =
[
    [ "dma_channel_link_config_t", "group__dma__hal.html#structdma__channel__link__config__t", [
      [ "linkType", "group__dma__hal.html#a377415cfa6e618ecfb2ba154e828cf8b", null ],
      [ "channel1", "group__dma__hal.html#afc71a80292db667048cb89cf952aa04a", null ],
      [ "channel2", "group__dma__hal.html#ab708f793951a1729ff8687adb8dcf696", null ]
    ] ],
    [ "dma_error_status_t", "group__dma__hal.html#structdma__error__status__t", [
      [ "dmaBytesToBeTransffered", "group__dma__hal.html#a06e0915aea24357f22652651ebcf1251", null ],
      [ "dmaTransDone", "group__dma__hal.html#a2a40d5e48a90ad6e6d34e798dbcc503d", null ],
      [ "dmaBusy", "group__dma__hal.html#abc51bc25f2494c506b82b1c01e2a1a7f", null ],
      [ "dmaPendingRequest", "group__dma__hal.html#a1b34caaf231e55e92625dfa98cd1f27c", null ],
      [ "dmaDestBusError", "group__dma__hal.html#afb2a7fed8074d40618d00acd583dd0d5", null ],
      [ "dmaSourceBusError", "group__dma__hal.html#ae14cf14a47d6971f33a585da9504ee57", null ],
      [ "dmaConfigError", "group__dma__hal.html#ad3de4ab47a69f5e187772b4ae2a50347", null ]
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
    [ "DMA_HAL_Init", "group__dma__hal.html#gaa0c10c99592c98f399ed2d8b5150ffc8", null ],
    [ "DMA_HAL_ConfigTransfer", "group__dma__hal.html#gace2cc571a02106e94c0d8f97185a25e8", null ],
    [ "DMA_HAL_SetSourceAddr", "group__dma__hal.html#gaffa20f14ef4542ae4cee7cdb296f65a7", null ],
    [ "DMA_HAL_SetDestAddr", "group__dma__hal.html#ga19d1f97c0285fbb9e80dcc325880a54f", null ],
    [ "DMA_HAL_SetTransferCount", "group__dma__hal.html#gab141f15bfe0629672a7662f5e1f3a38b", null ],
    [ "DMA_HAL_GetUnfinishedByte", "group__dma__hal.html#ga345f43d9ce32db026b000b72cce385e0", null ],
    [ "DMA_HAL_SetIntCmd", "group__dma__hal.html#gac42fac902d02a4cb3dd36c48f5775594", null ],
    [ "DMA_HAL_SetCycleStealCmd", "group__dma__hal.html#gadf882b0e590cdd544d4bea8986f81469", null ],
    [ "DMA_HAL_SetAutoAlignCmd", "group__dma__hal.html#gaccccffd3bcbd634db8def6ad7e9cad1b", null ],
    [ "DMA_HAL_SetAsyncDmaRequestCmd", "group__dma__hal.html#ga53bffbde9cf45b2024fb679218c77a84", null ],
    [ "DMA_HAL_SetSourceIncrementCmd", "group__dma__hal.html#ga206fd2c228ec3bd40fe84be283b13706", null ],
    [ "DMA_HAL_SetDestIncrementCmd", "group__dma__hal.html#ga13746c7ecf8aefee4832f775283fcb87", null ],
    [ "DMA_HAL_SetSourceTransferSize", "group__dma__hal.html#ga98a247575a005cf85c0999ddee47a82d", null ],
    [ "DMA_HAL_SetDestTransferSize", "group__dma__hal.html#gadce58dc6436a126c13f7d5f90b22da87", null ],
    [ "DMA_HAL_SetTriggerStartCmd", "group__dma__hal.html#gaa514d4d14362eb4413b215740637361a", null ],
    [ "DMA_HAL_SetSourceModulo", "group__dma__hal.html#ga6f352066c7d7c7e394422c6477a8560e", null ],
    [ "DMA_HAL_SetDestModulo", "group__dma__hal.html#ga86a5ccea8e9493b46a601695b479b556", null ],
    [ "DMA_HAL_SetDmaRequestCmd", "group__dma__hal.html#ga7b23035daf1b5b9bea7038cb33fa0ddf", null ],
    [ "DMA_HAL_SetDisableRequestAfterDoneCmd", "group__dma__hal.html#gaa5afc383c43ceafe5b54ae895f216c03", null ],
    [ "DMA_HAL_SetChanLink", "group__dma__hal.html#ga46ca3629aa08ffef8e89fb3769571983", null ],
    [ "DMA_HAL_ClearStatus", "group__dma__hal.html#ga6c8bf1ca070c2531bd75e10b45565b13", null ],
    [ "DMA_HAL_GetStatus", "group__dma__hal.html#gabf6ed6faa3275054716412952c391bae", null ]
];