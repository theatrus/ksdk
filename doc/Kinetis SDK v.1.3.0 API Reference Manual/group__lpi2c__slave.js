var group__lpi2c__slave =
[
    [ "lpi2c_slave_state_t", "group__lpi2c__slave.html#structlpi2c__slave__state__t", [
      [ "status", "group__lpi2c__slave.html#a4209e664a3c46298abb3f22eaca696ab", null ],
      [ "txSize", "group__lpi2c__slave.html#af49985c9b7a55fea146072ff7d1826d3", null ],
      [ "rxSize", "group__lpi2c__slave.html#a23177143908c8bfc9ad8ef8370cf5487", null ],
      [ "txBuff", "group__lpi2c__slave.html#af8599bda1f45fa4846e89db37c58c4b7", null ],
      [ "rxBuff", "group__lpi2c__slave.html#a13d05cfb82ad59c1ed94e018fce098c3", null ],
      [ "isTxBusy", "group__lpi2c__slave.html#a390547d824100d73ea29edfaa7f21493", null ],
      [ "isRxBusy", "group__lpi2c__slave.html#a24ef7feaffc86111b99edef34880d1cb", null ],
      [ "isTxBlocking", "group__lpi2c__slave.html#a1fe24cfaae462521cae85911b2197900", null ],
      [ "isRxBlocking", "group__lpi2c__slave.html#a5f93dbe5796c0bc27e437421798d122b", null ],
      [ "irqEvent", "group__lpi2c__slave.html#ab945cde78e9f27ac04ea0aae272f8873", null ],
      [ "slaveListening", "group__lpi2c__slave.html#a871c631f610573e5ecfe69675692fc5a", null ],
      [ "slaveCallback", "group__lpi2c__slave.html#a97ab056c4ddb6623024ab523ced5c685", null ],
      [ "callbackParam", "group__lpi2c__slave.html#a5ef4c3fba32757ba744507cd85e039cb", null ]
    ] ],
    [ "lpi2c_slave_user_config_t", "group__lpi2c__slave.html#structlpi2c__slave__user__config__t", [
      [ "address", "group__lpi2c__slave.html#a7b7d9d605586b9c21ef9a35cdc0c63c7", null ],
      [ "slaveListening", "group__lpi2c__slave.html#ae1b141e5f0113268f1e1c8e59bb32e22", null ],
      [ "slaveCallback", "group__lpi2c__slave.html#a94f831838d308f18c00718a751cf9209", null ],
      [ "callbackParam", "group__lpi2c__slave.html#a1c7330a0a8bb6cda552347120e0e5239", null ]
    ] ],
    [ "lpi2c_slave_callback_t", "group__lpi2c__slave.html#gae8ab0beb618f427185e230f5dcf8c5d3", null ],
    [ "lpi2c_slave_event_t", "group__lpi2c__slave.html#gaba2d0b048d546f1b03711cfb66cef79b", [
      [ "kLPI2CSlaveTxReq", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79bafe75a077f9a7db5e0fa7ba6a527eb77d", null ],
      [ "kLPI2CSlaveRxReq", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79bae21b9463e00ec6d437804864fb8efd0b", null ],
      [ "kLPI2CSlaveTxNAK", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79ba3121620b7f400eddd0b063290035f0d9", null ],
      [ "kLPI2CSlaveTxEmpty", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79baf2ebb6677eaa76de087c78c4728ae448", null ],
      [ "kLPI2CSlaveRxFull", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79ba96fd02e1fc60a9883408a15dfff9c2ea", null ],
      [ "kLPI2CSlaveAbort", "group__lpi2c__slave.html#ggaba2d0b048d546f1b03711cfb66cef79ba8d82880a6cfc1447733431b6134cd233", null ]
    ] ],
    [ "LPI2C_DRV_SlaveInit", "group__lpi2c__slave.html#gaccd961ad313b4684566ec78d24b11f72", null ],
    [ "LPI2C_DRV_SlaveDeinit", "group__lpi2c__slave.html#ga00bc908d0abf4c018331ce2228c5047c", null ],
    [ "LPI2C_DRV_SlaveGetHandler", "group__lpi2c__slave.html#ga6a24ca22578a01a0377174c1ee6673c9", null ],
    [ "LPI2C_DRV_SlaveSendData", "group__lpi2c__slave.html#gaddca5823d3cddf575c4bd72f4fe3ef46", null ],
    [ "LPI2C_DRV_SlaveSendDataBlocking", "group__lpi2c__slave.html#gaad6b53727765a8ebbbf38e310470b664", null ],
    [ "LPI2C_DRV_SlaveReceiveData", "group__lpi2c__slave.html#ga2115be40beed926a63f3c82f03250cf9", null ],
    [ "LPI2C_DRV_SlaveReceiveDataBlocking", "group__lpi2c__slave.html#ga435f7e26a2186b674de0cbda64c9c681", null ],
    [ "LPI2C_DRV_SlaveGetReceiveStatus", "group__lpi2c__slave.html#gad731ffaf55341f507f5199a2117c71d3", null ],
    [ "LPI2C_DRV_SlaveGetTransmitStatus", "group__lpi2c__slave.html#ga5cab7560796617f0b0bd8f7f0e18ca98", null ],
    [ "LPI2C_DRV_SlaveAbortReceiveData", "group__lpi2c__slave.html#gadd7c3e21db38ff25d8cfbe3aa3d368e6", null ],
    [ "LPI2C_DRV_SlaveAbortSendData", "group__lpi2c__slave.html#ga46c68780460f11b9d841bd76c7a7b613", null ],
    [ "LPI2C_DRV_SlaveIsBusBusy", "group__lpi2c__slave.html#gacb2e7313560f2ba39841b65b640fa82c", null ],
    [ "LPI2C_DRV_SlaveSendDataPolling", "group__lpi2c__slave.html#gacf9e6e8da80847ccc808bb863b829d97", null ],
    [ "LPI2C_DRV_SlaveReceiveDataPolling", "group__lpi2c__slave.html#ga4c3a84a75166a97e2d0097584f6ad042", null ],
    [ "g_lpi2cBase", "group__lpi2c__slave.html#gaebe70dbb1ff646f8e2614c9f8366dbc5", null ]
];