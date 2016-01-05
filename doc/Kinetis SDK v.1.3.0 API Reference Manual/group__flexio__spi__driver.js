var group__flexio__spi__driver =
[
    [ "flexio_spi_state_t", "group__flexio__spi__driver.html#structflexio__spi__state__t", [
      [ "isTxBusy", "group__flexio__spi__driver.html#ae4a6b0230e4803d09ce86482d8cfa50d", null ],
      [ "isRxBusy", "group__flexio__spi__driver.html#a2ebe9be7cfc1c437265702f685a54bcd", null ],
      [ "isXBusy", "group__flexio__spi__driver.html#aac4cf10d2a8adab7db874d8812426180", null ],
      [ "isTxBlocking", "group__flexio__spi__driver.html#a56476a1c4d515efadf4ff0ddfaa2bd00", null ],
      [ "isRxBlocking", "group__flexio__spi__driver.html#a6dab7f177abc9001fe8612b4250fc979", null ],
      [ "isXBlocking", "group__flexio__spi__driver.html#a77f156d6b87d3872d085def176b4d4d3", null ],
      [ "txIrqSync", "group__flexio__spi__driver.html#a0a9265cd377f7878c319c6a2a0d0f09d", null ],
      [ "rxIrqSync", "group__flexio__spi__driver.html#af15a379d4ec1b183c50a2cbbc9c1a8ec", null ],
      [ "xIrqSync", "group__flexio__spi__driver.html#a6b5899595e6bc7c1087d2ba354247d3a", null ],
      [ "rxCallback", "group__flexio__spi__driver.html#ab6a1d1504a8ac29bd98df664607c3f10", null ],
      [ "rxCallbackParam", "group__flexio__spi__driver.html#a600eec89acd6b23ad46981bed38de0cd", null ],
      [ "isTxUseDma", "group__flexio__spi__driver.html#ac5bc41036fdbd2e5ba1b612f4de82fd2", null ],
      [ "isRxUseDma", "group__flexio__spi__driver.html#a4c6cfe750b296b30ee7c53e003e3de63", null ],
      [ "dmaSpiTx", "group__flexio__spi__driver.html#a771b6c6aa1e27c67255ede6e0d712d52", null ],
      [ "dmaSpiRx", "group__flexio__spi__driver.html#a8192ca800759da46054ff0a7e379ec2d", null ]
    ] ],
    [ "flexio_spi_hwconfig_t", "group__flexio__spi__driver.html#structflexio__spi__hwconfig__t", [
      [ "sdoPinIdx", "group__flexio__spi__driver.html#a6fcafafa1bc1b1eb4b67a61b2d39c7c3", null ],
      [ "sdiPinIdx", "group__flexio__spi__driver.html#a71116e8a3b03d6eea69ec1353926cf92", null ],
      [ "sclkPinIdx", "group__flexio__spi__driver.html#a8df06e84baca0eed69629da6b97f6cf4", null ],
      [ "csnPinIdx", "group__flexio__spi__driver.html#adeaabbe728296c9db2d8bd8343c9c44e", null ],
      [ "shifterIdx", "group__flexio__spi__driver.html#af22ff51f2b1661f2411fe46d7acbd94d", null ],
      [ "timerIdx", "group__flexio__spi__driver.html#a9765116c7f661e13a056fe0d5df6c154", null ]
    ] ],
    [ "flexio_spi_userconfig_t", "group__flexio__spi__driver.html#structflexio__spi__userconfig__t", [
      [ "spiMode", "group__flexio__spi__driver.html#aaadb4e1810c5e8f2c9af48fe004a1f8e", null ],
      [ "baudRate", "group__flexio__spi__driver.html#aae7a4b5ade2c6d1a2c868c694f203e39", null ],
      [ "clkPhase", "group__flexio__spi__driver.html#a23bc56cac63b4a8f24e71cfcab8071cd", null ],
      [ "dataSize", "group__flexio__spi__driver.html#a02f0f62ab7e71ea8969a6df234f95d81", null ],
      [ "bitDirection", "group__flexio__spi__driver.html#a38acc9f28dbcdedc94770c0efade008e", null ],
      [ "spiHwConfig", "group__flexio__spi__driver.html#a6591d2c43451e21d574ddb372fae4d38", null ]
    ] ],
    [ "flexio_spi_rx_callback_t", "group__flexio__spi__driver.html#ga2c8ac59f0a896d4fd588e7881ddb2946", null ],
    [ "flexio_spi_status_t", "group__flexio__spi__driver.html#gaf2721f77a5577d24b64411a86c0490c6", null ],
    [ "flexio_spi_master_slave_mode_t", "group__flexio__spi__driver.html#ga5b2bf6ed17e498ee4691137895ec0be1", [
      [ "kFlexIOSpiMaster", "group__flexio__spi__driver.html#gga5b2bf6ed17e498ee4691137895ec0be1aa396416e2f74ddf7832520c0baca8633", null ],
      [ "kFlexIOSpiSlave", "group__flexio__spi__driver.html#gga5b2bf6ed17e498ee4691137895ec0be1a852ff088d819e2c2654b3632b13c6c1e", null ]
    ] ],
    [ "flexio_spi_shift_direction_t", "group__flexio__spi__driver.html#ga25594db149526d67b23d92724487e87f", [
      [ "kFlexIOSpiMsbFirst", "group__flexio__spi__driver.html#gga25594db149526d67b23d92724487e87fa84359f3b93ba2e3083f3b24566f275fe", null ],
      [ "kFlexIOSpiLsbFirst", "group__flexio__spi__driver.html#gga25594db149526d67b23d92724487e87fa3e1396388d905fa37c40d67a3a9f6e54", null ]
    ] ],
    [ "flexio_spi_clock_phase_t", "group__flexio__spi__driver.html#gac059394d6eabe169d3a1ee8a8b6f7edd", [
      [ "kFlexIOSpiClockPhase_FirstEdge", "group__flexio__spi__driver.html#ggac059394d6eabe169d3a1ee8a8b6f7edda422631f18458dd5d42c47a159bc9344f", null ],
      [ "kFlexIOSpiClockPhase_SecondEdge", "group__flexio__spi__driver.html#ggac059394d6eabe169d3a1ee8a8b6f7edda23816505d8bcb5eea6bfbc6879e1e15b", null ]
    ] ],
    [ "flexio_spi_data_bitcount_mode_t", "group__flexio__spi__driver.html#gae4e518a5b077982fca7b6bf1025c7aaf", [
      [ "kFlexIOSpi8BitMode", "group__flexio__spi__driver.html#ggae4e518a5b077982fca7b6bf1025c7aafabbdc8880ede508bf84c461262216d3d3", null ],
      [ "kFlexIOSpi16BitMode", "group__flexio__spi__driver.html#ggae4e518a5b077982fca7b6bf1025c7aafadf4986705f46fe51eaf84aca0978f0ae", null ]
    ] ],
    [ "FLEXIO_SPI_DRV_Init", "group__flexio__spi__driver.html#gabf61aad1d2a5ec0485c1342cc854ebf9", null ],
    [ "FLEXIO_SPI_DRV_Deinit", "group__flexio__spi__driver.html#gae18c77babc9b0dfc204d2baaf977f25a", null ],
    [ "FLEXIO_SPI_DRV_SendDataBlocking", "group__flexio__spi__driver.html#gaf23ea8afbaa1375ee4fd66fa96ea1f18", null ],
    [ "FLEXIO_SPI_DRV_SendData", "group__flexio__spi__driver.html#ga2c87a2473aea4f5806f4acb4ea089fa2", null ],
    [ "FLEXIO_SPI_DRV_GetTransmitStatus", "group__flexio__spi__driver.html#ga35919a9d64ed5d714080397504613589", null ],
    [ "FLEXIO_SPI_DRV_AbortSendingData", "group__flexio__spi__driver.html#gafb6d5807f38f827b11ddbd9e44acc7f4", null ],
    [ "FLEXIO_SPI_DRV_ReceiveDataBlocking", "group__flexio__spi__driver.html#ga288c92900f50765b11721c3af30f83bc", null ],
    [ "FLEXIO_SPI_DRV_ReceiveData", "group__flexio__spi__driver.html#ga2ea759f7e078717203138b2f2d2f45a8", null ],
    [ "FLEXIO_SPI_DRV_GetReceiveStatus", "group__flexio__spi__driver.html#gada0772656e5e9b3d89d8ee76a8d96de4", null ],
    [ "FLEXIO_SPI_DRV_AbortReceivingData", "group__flexio__spi__driver.html#gaeaf879ed0c5d89453d7090ebefa43a3c", null ],
    [ "FLEXIO_SPI_DRV_TransferDataBlocking", "group__flexio__spi__driver.html#gaa8a68e4010aa1fed3296b63feaf070ee", null ],
    [ "FLEXIO_SPI_DRV_TransferData", "group__flexio__spi__driver.html#ga8d808c479551c0bf37690a7f893c3867", null ],
    [ "FLEXIO_SPI_DRV_TX_IRQHandler", "group__flexio__spi__driver.html#gac3331a6f8058eb95f9843cc059ee13db", null ],
    [ "FLEXIO_SPI_DRV_RX_IRQHandler", "group__flexio__spi__driver.html#ga3d46b29fb409f53a44654cd018fabf5a", null ],
    [ "FLEXIO_SPI_DRV_EdmaSendDataBlocking", "group__flexio__spi__driver.html#ga9e8252cc6d1bb043621dda9fe2963f8c", null ],
    [ "FLEXIO_SPI_DRV_EdmaSendData", "group__flexio__spi__driver.html#gad409616a034658dc2e3b4e30b3dac7fe", null ],
    [ "FLEXIO_SPI_DRV_EdmaGetTransmitStatus", "group__flexio__spi__driver.html#ga0ece9c9548a8a026d2266707d5d95c1e", null ],
    [ "FLEXIO_SPI_DRV_EdmaAbortSendingData", "group__flexio__spi__driver.html#gaf95cfd4a3969084c6d233d26a3742632", null ],
    [ "FLEXIO_SPI_DRV_EdmaReceiveDataBlocking", "group__flexio__spi__driver.html#ga9817d863bba5a68924cf2f1fff24a00b", null ],
    [ "FLEXIO_SPI_DRV_EdmaReceiveData", "group__flexio__spi__driver.html#ga1806e60b0f77a0f5bd879fbb041cfcd7", null ],
    [ "FLEXIO_SPI_DRV_EdmaGetReceiveStatus", "group__flexio__spi__driver.html#gaac977ab7a35d63e39c5d46a1393b93cf", null ],
    [ "FLEXIO_SPI_DRV_EdmaAbortReceivingData", "group__flexio__spi__driver.html#ga0a4918e0a59bc4f11f89ab1e64717ab1", null ],
    [ "FLEXIO_SPI_DRV_EdmaTransferDataBlocking", "group__flexio__spi__driver.html#gab0b8b1ceca2e8970c422ce46a0b442c7", null ],
    [ "FLEXIO_SPI_DRV_EdmaTransferData", "group__flexio__spi__driver.html#gaf40f029a1d289d083abb6b01126b559e", null ],
    [ "FLEXIO_SPI_DRV_DmaSendDataBlocking", "group__flexio__spi__driver.html#ga218329e088c02f72914d8ee84cdf1b47", null ],
    [ "FLEXIO_SPI_DRV_DmaSendData", "group__flexio__spi__driver.html#gab7262f639a03f62055b86233cc79233a", null ],
    [ "FLEXIO_SPI_DRV_DmaGetTransmitStatus", "group__flexio__spi__driver.html#gad052722c7b17a16f90aa8bb36a2d878d", null ],
    [ "FLEXIO_SPI_DRV_DmaAbortSendingData", "group__flexio__spi__driver.html#ga289c1463ff720bb18625f4d7edb780f2", null ],
    [ "FLEXIO_SPI_DRV_DmaReceiveDataBlocking", "group__flexio__spi__driver.html#gaddb546f281d2cf4120c298d5de13af25", null ],
    [ "FLEXIO_SPI_DRV_DmaReceiveData", "group__flexio__spi__driver.html#ga10d3a55bb2e9110a7c94681b98567e45", null ],
    [ "FLEXIO_SPI_DRV_DmaGetReceiveStatus", "group__flexio__spi__driver.html#gac4bead0f8260d4a142db165a1bb03324", null ],
    [ "FLEXIO_SPI_DRV_AbortDmaReceivingData", "group__flexio__spi__driver.html#gafa9f6a24c637bb2a5944d7be77aba378", null ],
    [ "FLEXIO_SPI_DRV_DmaTransferDataBlocking", "group__flexio__spi__driver.html#ga7f0129cafb8a805e1fd9f8a60d9a1c06", null ],
    [ "FLEXIO_SPI_DRV_DmaTransferData", "group__flexio__spi__driver.html#ga94177c55cb25ac1991701d4bcb11e873", null ]
];