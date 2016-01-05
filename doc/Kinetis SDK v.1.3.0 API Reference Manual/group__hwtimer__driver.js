var group__hwtimer__driver =
[
    [ "Hardware Timer Manager Driver", "group__htwdr.html", null ],
    [ "Hardware Timer Manager Interrupt handlers", "group__htwint.html", null ],
    [ "hwtimer_ptr_t", "group__hwtimer__driver.html#structhwtimer__t", [
      [ "devif", "group__hwtimer__driver.html#afffe82c211eee9d28d8f63c085a39c20", null ],
      [ "clockFreq", "group__hwtimer__driver.html#aa481ebb29646c49900ecf192b0b048c6", null ],
      [ "divider", "group__hwtimer__driver.html#a277dc8b9a3b1564132ce6d02b290ef04", null ],
      [ "modulo", "group__hwtimer__driver.html#a1b0ab0e78d5dab868f9201183fe043d6", null ],
      [ "ticks", "group__hwtimer__driver.html#a3456c66f0e9163666d9f663f8b78e5de", null ],
      [ "callbackFunc", "group__hwtimer__driver.html#ac99fe6467c61b3ce902215a5607e1ab8", null ],
      [ "callbackData", "group__hwtimer__driver.html#a042ba6400361d7f5847babade2a92fd1", null ],
      [ "callbackPending", "group__hwtimer__driver.html#af563299d66d8499c9f674f1a273521df", null ],
      [ "callbackBlocked", "group__hwtimer__driver.html#aa02508bbc8b02dd0b8b7bf9cfaf5a2b1", null ],
      [ "llContext", "group__hwtimer__driver.html#a924a7e09ca71276ea7bbb261f7007e8b", null ]
    ] ],
    [ "hwtimer_time_ptr_t", "group__hwtimer__driver.html#structhwtimer__time__t", [
      [ "ticks", "group__hwtimer__driver.html#a6626df25af1912496d69ddf4b66d2404", null ],
      [ "subTicks", "group__hwtimer__driver.html#a1fe906ddc9f9fc67b9d2717f5405471e", null ]
    ] ],
    [ "hwtimer_devif_ptr_t", "group__hwtimer__driver.html#structhwtimer__devif__t", [
      [ "init", "group__hwtimer__driver.html#a0633c7d14f0f21dd3516612cd1f8cff8", null ],
      [ "deinit", "group__hwtimer__driver.html#a80209ec17acfe69070a120ee0980639f", null ],
      [ "setDiv", "group__hwtimer__driver.html#a188da1ee3da0ccd10b6c16e3ca2a9bd6", null ],
      [ "start", "group__hwtimer__driver.html#acb914db6066801a9b3b1c662bad22390", null ],
      [ "stop", "group__hwtimer__driver.html#a37f83f93af9fa9ca1084aafc5abffc16", null ],
      [ "getTime", "group__hwtimer__driver.html#a8931f3a30085997e46dad750ed0ea77c", null ]
    ] ],
    [ "HWTIMER_LL_CONTEXT_LEN", "group__hwtimer__driver.html#ga3882a451eb1a46e250596f0fbd986dd9", null ],
    [ "hwtimer_callback_t", "group__hwtimer__driver.html#ga7e51f117931cdedb8cfdb3a2e877d16f", null ],
    [ "hwtimer_devif_init_t", "group__hwtimer__driver.html#gaa545dba18643418617c10f510d4911e1", null ],
    [ "hwtimer_devif_deinit_t", "group__hwtimer__driver.html#ga1e7965fe7414b5688bcfd11f21700237", null ],
    [ "hwtimer_devif_set_div_t", "group__hwtimer__driver.html#ga972f9d3576776d94d39dd7bbfd209829", null ],
    [ "hwtimer_devif_start_t", "group__hwtimer__driver.html#ga8ff0d8deb3b20d40f27db91f45a48ba0", null ],
    [ "hwtimer_devif_stop_t", "group__hwtimer__driver.html#gac6bbc126c66409fbd26ef2a12996751a", null ],
    [ "hwtimer_devif_reset_t", "group__hwtimer__driver.html#gaff40aabcd469fb2306eed492795ce340", null ],
    [ "hwtimer_devif_get_time_t", "group__hwtimer__driver.html#ga19420dd514cd52410043552c47617be7", null ],
    [ "_hwtimer_error_code_t", "group__hwtimer__driver.html#ga86e47170dc8b2304d3f7506a096202a5", [
      [ "kHwtimerSuccess", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5ad3c188c724a7879626c88d35aa3eda5a", null ],
      [ "kHwtimerInvalidInput", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5a6bc656084d8236efce398d729ed3522e", null ],
      [ "kHwtimerInvalidPointer", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5ad770b7cc292d5a997fd5b5524bca2176", null ],
      [ "kHwtimerClockManagerError", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5a6f63344a33c28b43a7affa95a3e9f79c", null ],
      [ "kHwtimerRegisterHandlerError", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5a4fbf1501ea6cca3f607365011ae8ab0f", null ],
      [ "kHwtimerUnknown", "group__hwtimer__driver.html#gga86e47170dc8b2304d3f7506a096202a5a4f3b71bf306eba7acbd71a469d360dcd", null ]
    ] ],
    [ "HWTIMER_SYS_Init", "group__hwtimer__driver.html#gaa7262c800be740c26ea5adef2b82f1f0", null ],
    [ "HWTIMER_SYS_Deinit", "group__hwtimer__driver.html#ga1a55521d87a4f1e66590931f7767f75c", null ],
    [ "HWTIMER_SYS_SetPeriod", "group__hwtimer__driver.html#gacef4f93dff6e95727fa496bf83a7d6c2", null ],
    [ "HWTIMER_SYS_GetPeriod", "group__hwtimer__driver.html#gaebd67e7129964c8d80dfc895e21ed563", null ],
    [ "HWTIMER_SYS_Start", "group__hwtimer__driver.html#ga9e312e6f46a44adb2126d60007caa586", null ],
    [ "HWTIMER_SYS_Stop", "group__hwtimer__driver.html#gadfa995724176e8de14b7535bd0856af4", null ],
    [ "HWTIMER_SYS_GetModulo", "group__hwtimer__driver.html#gaf2a3604bcf65a9e3415ef0206f939c08", null ],
    [ "HWTIMER_SYS_GetTime", "group__hwtimer__driver.html#gaddc5bb3aeb845c47d6f9e1afd923ec11", null ],
    [ "HWTIMER_SYS_GetTicks", "group__hwtimer__driver.html#gae8722987a12a0513f0c55c26bc943f18", null ],
    [ "HWTIMER_SYS_RegisterCallback", "group__hwtimer__driver.html#ga5ddc607d2dec5dc858ecfb22351d9a7d", null ],
    [ "HWTIMER_SYS_BlockCallback", "group__hwtimer__driver.html#ga57143ec3e22d72fa5d3f1b8a54093dce", null ],
    [ "HWTIMER_SYS_UnblockCallback", "group__hwtimer__driver.html#ga60f6f44965fa29cfd42051fb563951a0", null ],
    [ "HWTIMER_SYS_CancelCallback", "group__hwtimer__driver.html#ga8d1cde6c269e1bb321bffa5c7ad06c7f", null ]
];