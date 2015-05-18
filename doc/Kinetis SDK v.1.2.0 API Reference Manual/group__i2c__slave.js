var group__i2c__slave =
[
    [ "i2c_slave_state_t", "group__i2c__slave.html#structi2c__slave__state__t", [
      [ "status", "group__i2c__slave.html#a6291a245e746dea479e64ece8ee5d811", null ],
      [ "txSize", "group__i2c__slave.html#aa3367ed49350ef7d850dd871316fcd9f", null ],
      [ "rxSize", "group__i2c__slave.html#a4d745537cff24f3be5e0dca01aa43817", null ],
      [ "txBuff", "group__i2c__slave.html#a016a0d3b5b61aac9840c659742269c4a", null ],
      [ "rxBuff", "group__i2c__slave.html#a442191c9061d0a87b8c0fa1a926ea644", null ],
      [ "isTxBusy", "group__i2c__slave.html#a4e97c88eb94bddfd1049b0dc3fd4ec73", null ],
      [ "isRxBusy", "group__i2c__slave.html#a718be9279d250f8f3e4169ad11aca378", null ],
      [ "isTxBlocking", "group__i2c__slave.html#a27309537093114c7ecd801aee1fc8881", null ],
      [ "isRxBlocking", "group__i2c__slave.html#ae03137ed1516985f08f0beb3c2659da0", null ],
      [ "irqEvent", "group__i2c__slave.html#a5fd2ed1576c02f494ac4fd0f27f2488a", null ],
      [ "slaveListening", "group__i2c__slave.html#a5c4c1818f0dfafe16075e7941fa8395c", null ],
      [ "slaveCallback", "group__i2c__slave.html#a9cec8d08fcc81220fbf4f1f0025d10c4", null ],
      [ "callbackParam", "group__i2c__slave.html#a3128e5a6880fef2f2b860c71b5c20210", null ]
    ] ],
    [ "i2c_slave_user_config_t", "group__i2c__slave.html#structi2c__slave__user__config__t", [
      [ "address", "group__i2c__slave.html#a5520ac66222fdb5dd1ea21d26f03e2c5", null ],
      [ "slaveListening", "group__i2c__slave.html#a1296c61219d3b12a32ad4e9ce5ba89ef", null ],
      [ "slaveCallback", "group__i2c__slave.html#aa385d0b6a5ccb0a46240f850d56c2c42", null ],
      [ "callbackParam", "group__i2c__slave.html#ad287ceb403f35c0f22f60c368ed0376e", null ]
    ] ],
    [ "i2c_slave_callback_t", "group__i2c__slave.html#ga481e039c264030d715a42d9891b18c6a", null ],
    [ "i2c_slave_event_t", "group__i2c__slave.html#gad8b613f0d4e9d1b9511ca0b6d6ab7e8c", [
      [ "kI2CSlaveTxReq", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca9690d644b639c1c3cf3935f98f04ea99", null ],
      [ "kI2CSlaveRxReq", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca960cd84894bc6c592163a972d293153e", null ],
      [ "kI2CSlaveTxNAK", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca534852aa1b53ae161c0eeb28ae58cb03", null ],
      [ "kI2CSlaveTxEmpty", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca6daa57cda7bfec5e3f8fbd2838ffd678", null ],
      [ "kI2CSlaveRxFull", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca2616401de807f4378eb5fe9bc785b036", null ],
      [ "kI2CSlaveAbort", "group__i2c__slave.html#ggad8b613f0d4e9d1b9511ca0b6d6ab7e8ca21cba2f1ec182fa92b8f5fa174e4144d", null ]
    ] ],
    [ "I2C_DRV_SlaveInit", "group__i2c__slave.html#ga1716284bfa9bd0d97dc97b94b0e93ef1", null ],
    [ "I2C_DRV_SlaveDeinit", "group__i2c__slave.html#gae4da11516818d197155078259aa13d3b", null ],
    [ "I2C_DRV_SlaveGetHandler", "group__i2c__slave.html#gaa6f5e2c32bb170b5c6b4cf2d930dc7f9", null ],
    [ "I2C_DRV_SlaveSendData", "group__i2c__slave.html#gad31681dae0361b046fb74b7defb6720a", null ],
    [ "I2C_DRV_SlaveSendDataBlocking", "group__i2c__slave.html#ga4c1dc62e265d9672189d4dc80efc6427", null ],
    [ "I2C_DRV_SlaveReceiveData", "group__i2c__slave.html#ga9a60e0a12994032cc0627fc03f2f01ee", null ],
    [ "I2C_DRV_SlaveReceiveDataBlocking", "group__i2c__slave.html#ga46881f8ec7e387f4ecc4e4b5243945ae", null ],
    [ "I2C_DRV_SlaveGetReceiveStatus", "group__i2c__slave.html#ga75018eda1efe28391bfd17913f594e0b", null ],
    [ "I2C_DRV_SlaveGetTransmitStatus", "group__i2c__slave.html#ga37a4aad40dfbcb9aa43533186b18fd66", null ],
    [ "I2C_DRV_SlaveAbortReceiveData", "group__i2c__slave.html#gabff1a7cfad477f1ba8f5bbebbabd63d1", null ],
    [ "I2C_DRV_SlaveAbortSendData", "group__i2c__slave.html#ga7f365e54dba34d6dda355aba95c879b8", null ],
    [ "I2C_DRV_SlaveIsBusBusy", "group__i2c__slave.html#gaa1690b4dca101c5d2a72dfa771a8c74c", null ],
    [ "I2C_DRV_SlaveSendDataPolling", "group__i2c__slave.html#gad645f516d1510cc97b5d00c947a6de3b", null ],
    [ "I2C_DRV_SlaveReceiveDataPolling", "group__i2c__slave.html#ga84374ce6400b160a1f37eb678ac8f8bb", null ],
    [ "I2C_DRV_SlaveIRQHandler", "group__i2c__slave.html#ga918c0aa35e371c87c3c8730c308882df", null ],
    [ "g_i2cBase", "group__i2c__slave.html#ga4e22d124919fe3a94a971f0fbd43374b", null ]
];