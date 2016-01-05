var group__dac__hal =
[
    [ "dac_converter_config_t", "group__dac__hal.html#structdac__converter__config__t", [
      [ "dacRefVoltSrc", "group__dac__hal.html#a2da3d4650eb7ab817dd4bcf24269149e", null ],
      [ "lowPowerEnable", "group__dac__hal.html#a54e288703a0d85685440cdaac18e3328", null ]
    ] ],
    [ "dac_buffer_config_t", "group__dac__hal.html#structdac__buffer__config__t", [
      [ "bufferEnable", "group__dac__hal.html#aff644bbb341bb739876d17844844aa19", null ],
      [ "triggerMode", "group__dac__hal.html#a97743774d8ad5c11491989f85f5f9ea4", null ],
      [ "idxStartIntEnable", "group__dac__hal.html#afe82e3c92be47d6f58249b61bd857934", null ],
      [ "idxUpperIntEnable", "group__dac__hal.html#a857eadef2264cd71118e3df1b24d1e4f", null ],
      [ "dmaEnable", "group__dac__hal.html#aeddeff8f774d70b4ef43e3f948758334", null ],
      [ "buffWorkMode", "group__dac__hal.html#ac2383a32867e1bd1f912398bdbb30f36", null ],
      [ "upperIdx", "group__dac__hal.html#a9dbe237304a8fda9f28168c6354458c8", null ]
    ] ],
    [ "dac_status_t", "group__dac__hal.html#ga48d16081f1ecddd960da4c00683f042e", [
      [ "kStatus_DAC_Success", "group__dac__hal.html#gga48d16081f1ecddd960da4c00683f042ea107d715e40013a36e3ef04024a883287", null ],
      [ "kStatus_DAC_InvalidArgument", "group__dac__hal.html#gga48d16081f1ecddd960da4c00683f042eacd62d1c11a4df0d745636ead838ee230", null ],
      [ "kStatus_DAC_Failed", "group__dac__hal.html#gga48d16081f1ecddd960da4c00683f042ea015a5b7abfb61d3b6f58329df578818e", null ]
    ] ],
    [ "dac_ref_volt_src_mode_t", "group__dac__hal.html#gada1092ae6bd28fbd661efb82e22f82fa", [
      [ "kDacRefVoltSrcOfVref1", "group__dac__hal.html#ggada1092ae6bd28fbd661efb82e22f82faa0fee11f625f37284ff247f194625c3be", null ],
      [ "kDacRefVoltSrcOfVref2", "group__dac__hal.html#ggada1092ae6bd28fbd661efb82e22f82faa7ab48789024769b3391f0f7112228c67", null ]
    ] ],
    [ "dac_trigger_mode_t", "group__dac__hal.html#gaa8a69135bac8ad1c692f5bb2de99a692", [
      [ "kDacTriggerByHardware", "group__dac__hal.html#ggaa8a69135bac8ad1c692f5bb2de99a692a5d7a824d79b072fde00e87a5bbc5ba4f", null ],
      [ "kDacTriggerBySoftware", "group__dac__hal.html#ggaa8a69135bac8ad1c692f5bb2de99a692a7295b1e04c139437fc16db750d969d96", null ]
    ] ],
    [ "dac_buff_watermark_mode_t", "group__dac__hal.html#gab1660a96e9f979836b3efe7835b5f68b", [
      [ "kDacBuffWatermarkFromUpperAs1Word", "group__dac__hal.html#ggab1660a96e9f979836b3efe7835b5f68bad18a75ca5ef7449e29a6ef368354595a", null ],
      [ "kDacBuffWatermarkFromUpperAs2Word", "group__dac__hal.html#ggab1660a96e9f979836b3efe7835b5f68ba5a346e793da6b1b526c0248c102a48d8", null ],
      [ "kDacBuffWatermarkFromUpperAs3Word", "group__dac__hal.html#ggab1660a96e9f979836b3efe7835b5f68bae6d8f7bbe66f6de9d0ec35a178232019", null ],
      [ "kDacBuffWatermarkFromUpperAs4Word", "group__dac__hal.html#ggab1660a96e9f979836b3efe7835b5f68baaf255f2adfa168542b40b15603c15029", null ]
    ] ],
    [ "dac_buff_work_mode_t", "group__dac__hal.html#gafa6e3d2b94da438b66774e5756d964a2", [
      [ "kDacBuffWorkAsNormalMode", "group__dac__hal.html#ggafa6e3d2b94da438b66774e5756d964a2aa2997592c55b3dbf49753be4e65cee77", null ]
    ] ],
    [ "DAC_HAL_Init", "group__dac__hal.html#ga538aa49a7b910961e6387674156e9f2b", null ],
    [ "DAC_HAL_ConfigConverter", "group__dac__hal.html#ga4820d24326079575143f952ba407c0f2", null ],
    [ "DAC_HAL_ConfigBuffer", "group__dac__hal.html#gaac8d7629f8759e0356523a07b7456df8", null ],
    [ "DAC_HAL_SetBuffValue", "group__dac__hal.html#ga00aaf1585f28f1f14372b85e09667194", null ],
    [ "DAC_HAL_ClearBuffIdxUpperFlag", "group__dac__hal.html#ga4ac2d449af584cc67f044a36cec2a3c4", null ],
    [ "DAC_HAL_GetBuffIdxUpperFlag", "group__dac__hal.html#ga9dc1b650a2b8d9cfbb6364e691185460", null ],
    [ "DAC_HAL_ClearBuffIdxStartFlag", "group__dac__hal.html#gaa0e46dd587bf5553ee2070ed353de71f", null ],
    [ "DAC_HAL_GetBuffIdxStartFlag", "group__dac__hal.html#ga0538fdb86640f2be2e7286c27091ea2c", null ],
    [ "DAC_HAL_Enable", "group__dac__hal.html#gab96827b83efdb832503cd5807ae8d2ef", null ],
    [ "DAC_HAL_Disable", "group__dac__hal.html#ga2540d291cc877e9190826c879312a674", null ],
    [ "DAC_HAL_SetSoftTriggerCmd", "group__dac__hal.html#ga93b96b5556b0a67afdc91212738ad4ee", null ],
    [ "DAC_HAL_SetBuffCurIdx", "group__dac__hal.html#ga2fe43a165cd83113bf73f62184fa8299", null ],
    [ "DAC_HAL_GetBuffCurIdx", "group__dac__hal.html#gafbf96f588975cab413a0df7c060d305c", null ]
];