var group__mpu__hal =
[
    [ "mpu_hardware_info_t", "group__mpu__hal.html#structmpu__hardware__info__t", [
      [ "kMPUHardwareRevisionLevel", "group__mpu__hal.html#a0a082133386f9dfbdf21b7a05b8cf9fb", null ],
      [ "kMPUSupportSlavePortsNum", "group__mpu__hal.html#a1f58ab9aa30622f7454571a83cbceb86", null ],
      [ "kMPUSupportRegionsNum", "group__mpu__hal.html#accd2f4915f3de9c2f84ed658aad6febc", null ]
    ] ],
    [ "mpu_access_err_info_t", "group__mpu__hal.html#structmpu__access__err__info__t", [
      [ "master", "group__mpu__hal.html#a3258336ea58b3ffd9954544080bfc153", null ],
      [ "attributes", "group__mpu__hal.html#ab3ed10ced06d1599aa69d1a36961888a", null ],
      [ "accessType", "group__mpu__hal.html#a3924a882672ce6f2395d56e71511b809", null ],
      [ "accessCtr", "group__mpu__hal.html#ad520e4547e1c9bd5c8f77d111dcdb3c4", null ],
      [ "addr", "group__mpu__hal.html#a70ac376fcaf3bfbef5ee2c1a844e0a14", null ],
      [ "slavePort", "group__mpu__hal.html#a0ce59dcc62bf2933178a96c76c1ecb3a", null ]
    ] ],
    [ "mpu_low_masters_access_rights_t", "group__mpu__hal.html#structmpu__low__masters__access__rights__t", [
      [ "superAccessRights", "group__mpu__hal.html#acc063832a1b685fbb0b4c3f2078a1635", null ],
      [ "userAccessRights", "group__mpu__hal.html#a157d968fa12be1922bdc1034030a8e55", null ]
    ] ],
    [ "mpu_high_masters_access_rights_t", "group__mpu__hal.html#structmpu__high__masters__access__rights__t", [
      [ "kMPUWriteEnable", "group__mpu__hal.html#a0cfa0742e372bc51c407113f648c06b6", null ],
      [ "kMPUReadEnable", "group__mpu__hal.html#ae3f36f2257e6eadc5a53ee297857b0d9", null ]
    ] ],
    [ "mpu_region_config_t", "group__mpu__hal.html#structmpu__region__config__t", [
      [ "regionNum", "group__mpu__hal.html#a88fa583a682f8b35c3a4b4a339ec3869", null ],
      [ "startAddr", "group__mpu__hal.html#a3c87d7842845e008c0a66c83f0d60473", null ],
      [ "endAddr", "group__mpu__hal.html#a198c3ed494747998131f7a30911622d8", null ],
      [ "accessRights1", "group__mpu__hal.html#ab4d7cbafdf7fd410f4daae42eff326e5", null ],
      [ "accessRights2", "group__mpu__hal.html#abcbd17475e907234fef5de73717be3eb", null ],
      [ "regionEnable", "group__mpu__hal.html#a0a56f82f73479ebbac1b4e043199bb3a", null ]
    ] ],
    [ "mpu_region_num_t", "group__mpu__hal.html#ga79c9fc12716568a2aa818f3b7f1ae1d2", null ],
    [ "mpu_region_total_num_t", "group__mpu__hal.html#ga22906b087e45d639905ecb796af01bb7", [
      [ "kMPU8Regions", "group__mpu__hal.html#gga22906b087e45d639905ecb796af01bb7a2079cabecc126e150c3cecd02700689c", null ],
      [ "kMPU12Regions", "group__mpu__hal.html#gga22906b087e45d639905ecb796af01bb7a2df5623d1d92a72a6508ea21caf30286", null ],
      [ "kMPU16Regions", "group__mpu__hal.html#gga22906b087e45d639905ecb796af01bb7a870441b08becc274c2334f0d70d9fde7", null ]
    ] ],
    [ "mpu_err_access_type_t", "group__mpu__hal.html#ga688f512b6643c1861646c5e3fb944fb5", [
      [ "kMPUErrTypeRead", "group__mpu__hal.html#gga688f512b6643c1861646c5e3fb944fb5a5d35cfb0d74bc557ad3df18a09fe26f2", null ],
      [ "kMPUErrTypeWrite", "group__mpu__hal.html#gga688f512b6643c1861646c5e3fb944fb5aceb72014f55d726e5dc582eae4022c89", null ]
    ] ],
    [ "mpu_err_attributes_t", "group__mpu__hal.html#ga8643ef655a2714210f68b1b7b183fdbb", [
      [ "kMPUInstructionAccessInUserMode", "group__mpu__hal.html#gga8643ef655a2714210f68b1b7b183fdbbaf76b80b4e2a2293a8acc9bf692ac58a4", null ],
      [ "kMPUDataAccessInUserMode", "group__mpu__hal.html#gga8643ef655a2714210f68b1b7b183fdbba67e27c793c0dd4e28b9aa7ffa2b1fe5c", null ],
      [ "kMPUInstructionAccessInSupervisorMode", "group__mpu__hal.html#gga8643ef655a2714210f68b1b7b183fdbba1260418c24890a80f045dcf211220288", null ],
      [ "kMPUDataAccessInSupervisorMode", "group__mpu__hal.html#gga8643ef655a2714210f68b1b7b183fdbba3f87d3021ad2e1a45f449a7628dd99f6", null ]
    ] ],
    [ "mpu_access_mode_t", "group__mpu__hal.html#ga809008bc6edaa9cd59323e2d62ac9a46", [
      [ "kMPUAccessInUserMode", "group__mpu__hal.html#gga809008bc6edaa9cd59323e2d62ac9a46ac618ffe033dbc6bc1beaf21f144852d2", null ],
      [ "kMPUAccessInSupervisorMode", "group__mpu__hal.html#gga809008bc6edaa9cd59323e2d62ac9a46adaacfb0beeeaf50c7cd748e860ab0d41", null ]
    ] ],
    [ "mpu_master_t", "group__mpu__hal.html#ga56305e68140c25fbc03bc6dd240b650d", [
      [ "kMPUMaster0", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650da9632f2b9dcdfdd42ab457ac6b043667f", null ],
      [ "kMPUMaster1", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650daefd2807fe36794c5216c5a5e7b4923a4", null ],
      [ "kMPUMaster2", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650da689bea6622f7b281e7d4c7c160ed0e31", null ],
      [ "kMPUMaster3", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650da15fcd39616a53aa720748c9be516df57", null ],
      [ "kMPUMaster4", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650dad06d002dd9cdc84457d6a2f0dd5936b2", null ],
      [ "kMPUMaster5", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650da70aad908a5d03d0678a5eab9aa51f019", null ],
      [ "kMPUMaster6", "group__mpu__hal.html#gga56305e68140c25fbc03bc6dd240b650da2fefc0eb4f0769b4a996ab25a6e074ca", null ]
    ] ],
    [ "mpu_err_access_ctr_t", "group__mpu__hal.html#ga426c1f6a55d2586672b880f820972c94", [
      [ "kMPUNoRegionHit", "group__mpu__hal.html#gga426c1f6a55d2586672b880f820972c94ae6bfc0ac0640684308ed584f3f0d9ce0", null ],
      [ "kMPUNoneOverlappRegion", "group__mpu__hal.html#gga426c1f6a55d2586672b880f820972c94a4ee70a1486c0e72b53689696986a59c8", null ],
      [ "kMPUOverlappRegion", "group__mpu__hal.html#gga426c1f6a55d2586672b880f820972c94a9544296562f4678e558d7829b9875b60", null ]
    ] ],
    [ "mpu_supervisor_access_rights_t", "group__mpu__hal.html#ga55d1dc350059dad9011157b7ca807751", [
      [ "kMPUSupervisorReadWriteExecute", "group__mpu__hal.html#gga55d1dc350059dad9011157b7ca807751a729f188092e37cde8fe4edc9d5c05629", null ],
      [ "kMPUSupervisorReadExecute", "group__mpu__hal.html#gga55d1dc350059dad9011157b7ca807751a162a344cf15c6a7dc691d9ea27514428", null ],
      [ "kMPUSupervisorReadWrite", "group__mpu__hal.html#gga55d1dc350059dad9011157b7ca807751afa00cde0c2dc07a98a9c6a06dc892db3", null ],
      [ "kMPUSupervisorEqualToUsermode", "group__mpu__hal.html#gga55d1dc350059dad9011157b7ca807751a2f6e9300b3dfa7db2704e84403988511", null ]
    ] ],
    [ "mpu_user_access_rights_t", "group__mpu__hal.html#gaa0dbe6632c1bf7e38ac7b77337192161", [
      [ "kMPUUserNoAccessRights", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161ada5f83fb1df09f8bbc61f403ee171954", null ],
      [ "kMPUUserExecute", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a9f14b723a51988cebb39e450b3f928c5", null ],
      [ "kMPUUserWrite", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a15c1e0e0310ec55147b4be1a394e10c5", null ],
      [ "kMPUUserWriteExecute", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a6ea98e2ac101995e09475cc72b31fccc", null ],
      [ "kMPUUserRead", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a6d6b4cc879036d43c587734caab7d606", null ],
      [ "kMPUUserReadExecute", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a3928d5e623d1747fe3657542f35854bc", null ],
      [ "kMPUUserReadWrite", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161ade21d6c2ae1b0d7e63d428a327a359e5", null ],
      [ "kMPUUserReadWriteExecute", "group__mpu__hal.html#ggaa0dbe6632c1bf7e38ac7b77337192161a19ead9f5f64ad1b9bde4cf4765833e7e", null ]
    ] ],
    [ "mpu_status_t", "group__mpu__hal.html#ga74cd828a30b1ada088753a0f3ce64d0f", [
      [ "kStatus_MPU_Success", "group__mpu__hal.html#gga74cd828a30b1ada088753a0f3ce64d0faa2918f0459122e7d8f1db4d19d989314", null ],
      [ "kStatus_MPU_Fail", "group__mpu__hal.html#gga74cd828a30b1ada088753a0f3ce64d0fa60ebe138c34ee1a45e39f9301e6d7ca1", null ],
      [ "kStatus_MPU_NotInitlialized", "group__mpu__hal.html#gga74cd828a30b1ada088753a0f3ce64d0fa83934a59464053bbcccb718a52ad9f00", null ],
      [ "kStatus_MPU_NullArgument", "group__mpu__hal.html#gga74cd828a30b1ada088753a0f3ce64d0fa45b828a501443ccf28bb2367d14e7444", null ]
    ] ],
    [ "MPU_HAL_Enable", "group__mpu__hal.html#ga2edb9a30729f74ed3f61f9708bc79306", null ],
    [ "MPU_HAL_Disable", "group__mpu__hal.html#ga7c5ae95a2c4b2a62bae711dab1136127", null ],
    [ "MPU_HAL_IsEnable", "group__mpu__hal.html#ga19d0088a8ebdd79c1204f58166586806", null ],
    [ "MPU_HAL_GetHardwareInfo", "group__mpu__hal.html#ga0747495e69e5843b8a012ec1274af3b2", null ],
    [ "MPU_HAL_GetDetailErrorAccessInfo", "group__mpu__hal.html#gaba4e6bd9e49b6342c84e360016606a3e", null ],
    [ "MPU_HAL_SetRegionAddr", "group__mpu__hal.html#gacc1c74ced31d7a0d538849d0923f4e8d", null ],
    [ "MPU_HAL_SetLowMasterAccessRights", "group__mpu__hal.html#gace80c700aad64315ae611c4cd2fb74a2", null ],
    [ "MPU_HAL_SetHighMasterAccessRights", "group__mpu__hal.html#gaed43744f32a926330eebb46604f6d2e5", null ],
    [ "MPU_HAL_SetRegionValidCmd", "group__mpu__hal.html#gae2f361e00375a4ff83e059f444ad4bda", null ],
    [ "MPU_HAL_SetLowMasterAccessRightsByAlternateReg", "group__mpu__hal.html#gaf56941ccae53ac24b3b10d70dbd6f05c", null ],
    [ "MPU_HAL_SetHighMasterAccessRightsByAlternateReg", "group__mpu__hal.html#ga1818b084a05692b773815071f9f4d797", null ],
    [ "MPU_HAL_SetRegionConfig", "group__mpu__hal.html#gacfd8b18f7f8a0b7faf8d59064f83a41c", null ],
    [ "MPU_HAL_Init", "group__mpu__hal.html#ga12bda8f5c08542629be2a51568ed596d", null ]
];