INCLUDE(CMakeForceCompiler)

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

 
# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 ")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99 ")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -D_DEBUG=1")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK65FN2M0VMI18")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_MQX")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DUSBCFG_OTG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D_DEBUG=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK65FN2M0VMI18")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_MQX")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DUSBCFG_OTG")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/mcu/MK65F18)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/include/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/max3353)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/include/twrk65f180m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/include/twrk65f180m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../examples/twrk65f180m)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/mcu/MK65F18)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/include/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/max3353)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK65F18)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../host/include/twrk65f180m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../device/include/twrk65f180m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../examples/twrk65f180m)
ENDIF()

# ADD_LIBRARY
ADD_LIBRARY(KsdkUsbOtgMqxLib STATIC
    "${ProjDirPath}/../../../../../adapter/sources/adapter.h"
    "${ProjDirPath}/../../../../../adapter/sources/adapter_types.h"
    "${ProjDirPath}/../../../../../adapter/sources/sdk/adapter_cfg.h"
    "${ProjDirPath}/../../../../../adapter/sources/sdk/adapter_sdk.c"
    "${ProjDirPath}/../../../../../adapter/sources/sdk/adapter_sdk.h"
    "${ProjDirPath}/../../../../include/compiler.h"
    "${ProjDirPath}/../../../../include/types.h"
    "${ProjDirPath}/../../../../include/usb.h"
    "${ProjDirPath}/../../../../include/usb_desc.h"
    "${ProjDirPath}/../../../../include/usb_error.h"
    "${ProjDirPath}/../../../../include/usb_misc.h"
    "${ProjDirPath}/../../../../include/usb_opt.h"
    "${ProjDirPath}/../../../../include/usb_types.h"
    "${ProjDirPath}/../../../../device/include/usb_device_stack_interface.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_audio.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_cdc.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_composite.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_hid.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_msc.h"
    "${ProjDirPath}/../../../../device/sources/classes/include/usb_class_phdc.h"
    "${ProjDirPath}/../../../../device/sources/classes/common/usb_class.c"
    "${ProjDirPath}/../../../../device/sources/classes/common/usb_class_internal.h"
    "${ProjDirPath}/../../../../device/sources/classes/audio/usb_audio.c"
    "${ProjDirPath}/../../../../device/sources/classes/audio/usb_audio.h"
    "${ProjDirPath}/../../../../device/sources/classes/cdc/usb_cdc.c"
    "${ProjDirPath}/../../../../device/sources/classes/cdc/usb_cdc.h"
    "${ProjDirPath}/../../../../device/sources/classes/composite/usb_composite.c"
    "${ProjDirPath}/../../../../device/sources/classes/composite/usb_composite.h"
    "${ProjDirPath}/../../../../device/sources/classes/hid/usb_hid.c"
    "${ProjDirPath}/../../../../device/sources/classes/hid/usb_hid.h"
    "${ProjDirPath}/../../../../device/sources/classes/msd/usb_msc.c"
    "${ProjDirPath}/../../../../device/sources/classes/msd/usb_msc.h"
    "${ProjDirPath}/../../../../device/sources/classes/msd/usb_msc_scsi.c"
    "${ProjDirPath}/../../../../device/sources/classes/msd/usb_msc_scsi.h"
    "${ProjDirPath}/../../../../device/sources/classes/phdc/usb_phdc.c"
    "${ProjDirPath}/../../../../device/sources/classes/phdc/usb_phdc.h"
    "${ProjDirPath}/../../../../device/sources/controller/usb_dev.c"
    "${ProjDirPath}/../../../../device/sources/controller/usb_dev.h"
    "${ProjDirPath}/../../../../device/sources/controller/usb_framework.c"
    "${ProjDirPath}/../../../../device/sources/controller/usb_framework.h"
    "${ProjDirPath}/../../../../device/sources/controller/khci/device_khci_interface.c"
    "${ProjDirPath}/../../../../device/sources/controller/khci/khci_dev.c"
    "${ProjDirPath}/../../../../device/sources/controller/khci/khci_dev.h"
    "${ProjDirPath}/../../../../device/sources/controller/khci/khci_dev_misc.h"
    "${ProjDirPath}/../../../../device/sources/controller/ehci/ehci_dev_main.c"
    "${ProjDirPath}/../../../../device/sources/controller/ehci/ehci_dev_main.h"
    "${ProjDirPath}/../../../../device/sources/controller/ehci/device_ehci_interface.c"
    "${ProjDirPath}/../../../../device/sources/controller/ehci/ehci_usbprv.h"
    "${ProjDirPath}/../../../../host/sources/classes/common/usb_classes.c"
    "${ProjDirPath}/../../../../host/sources/classes/audio/usb_host_audio.c"
    "${ProjDirPath}/../../../../host/sources/classes/audio/usb_host_audio.h"
    "${ProjDirPath}/../../../../host/sources/classes/audio/usb_host_audio_intf.c"
    "${ProjDirPath}/../../../../host/sources/classes/cdc/usb_host_cdc.c"
    "${ProjDirPath}/../../../../host/sources/classes/cdc/usb_host_cdc.h"
    "${ProjDirPath}/../../../../host/sources/classes/cdc/usb_host_cdc_intf.c"
    "${ProjDirPath}/../../../../host/sources/classes/hid/usb_host_hid.c"
    "${ProjDirPath}/../../../../host/sources/classes/hid/usb_host_hid.h"
    "${ProjDirPath}/../../../../host/sources/classes/hub/usb_host_hub.c"
    "${ProjDirPath}/../../../../host/sources/classes/hub/usb_host_hub.h"
    "${ProjDirPath}/../../../../host/sources/classes/hub/usb_host_hub_prv.h"
    "${ProjDirPath}/../../../../host/sources/classes/hub/usb_host_hub_sm.c"
    "${ProjDirPath}/../../../../host/sources/classes/hub/usb_host_hub_sm.h"
    "${ProjDirPath}/../../../../host/sources/classes/msd/usb_host_msd_bo.c"
    "${ProjDirPath}/../../../../host/sources/classes/msd/usb_host_msd_bo.h"
    "${ProjDirPath}/../../../../host/sources/classes/msd/usb_host_msd_queue.c"
    "${ProjDirPath}/../../../../host/sources/classes/msd/usb_host_msd_ufi.c"
    "${ProjDirPath}/../../../../host/sources/classes/msd/usb_host_msd_ufi.h"
    "${ProjDirPath}/../../../../host/sources/classes/phdc/usb_host_phdc.c"
    "${ProjDirPath}/../../../../host/sources/classes/phdc/usb_host_phdc.h"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host.c"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host.h"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_ch9.c"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_ch9.h"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_common.c"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_common.h"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_dev_mng.c"
    "${ProjDirPath}/../../../../host/sources/controller/usb_host_dev_mng.h"
    "${ProjDirPath}/../../../../host/sources/controller/khci/host_khci_interface.c"
    "${ProjDirPath}/../../../../host/sources/controller/khci/khci.c"
    "${ProjDirPath}/../../../../host/sources/controller/khci/khci.h"
    "${ProjDirPath}/../../../../host/sources/controller/khci/khci_prv.h"
    "${ProjDirPath}/../../../../host/sources/controller/ehci/ehci_host.c"
    "${ProjDirPath}/../../../../host/sources/controller/ehci/ehci_prv.h"
    "${ProjDirPath}/../../../../host/sources/controller/ehci/host_ehci_interface.c"
    "${ProjDirPath}/../../../sources/include/usb_otg_dev_api.h"
    "${ProjDirPath}/../../../sources/include/usb_otg_host_api.h"
    "${ProjDirPath}/../../../sources/include/usb_otg_main.h"
    "${ProjDirPath}/../../../sources/driver/khci/usb_otg_khci.c"
    "${ProjDirPath}/../../../sources/driver/khci/usb_otg_khci.h"
    "${ProjDirPath}/../../../sources/driver/usb_otg.c"
    "${ProjDirPath}/../../../sources/driver/usb_otg.h"
    "${ProjDirPath}/../../../sources/driver/max3353/max3353_interface.c"
    "${ProjDirPath}/../../../sources/driver/max3353/sdk/usb_otg_max3353.c"
    "${ProjDirPath}/../../../sources/driver/max3353/usb_otg_max3353.h"
    "${ProjDirPath}/../../../sources/otg/usb_otg_main.c"
    "${ProjDirPath}/../../../sources/otg/usb_otg_private.h"
    "${ProjDirPath}/../../../sources/otg/usb_otg_sm.h"
    "${ProjDirPath}/../../../sources/otg/usb_otg_sm_a.c"
    "${ProjDirPath}/../../../sources/otg/usb_otg_sm_b.c"
    "${ProjDirPath}/../../../sources/otg/usb_otg_sm_main.c"
    "${ProjDirPath}/../../../../device/include/twrk65f180m/usb_device_config.h"
    "${ProjDirPath}/../../../../device/sources/bsp/twrk65f180m/usb_dev_bsp.c"
    "${ProjDirPath}/../../../../host/include/twrk65f180m/usb_host_config.h"
    "${ProjDirPath}/../../../../host/sources/bsp/twrk65f180m/usb_host_bsp.c"
    "${ProjDirPath}/../../../sources/bsp/twrk65f180m/usb_otg_bsp.c"
    "${ProjDirPath}/../../../../../../examples/twrk65f180m/board.h"
    "${ProjDirPath}/../../../../../../examples/twrk65f180m/pin_mux.c"
    "${ProjDirPath}/../../../../../../examples/twrk65f180m/pin_mux.h"
)

SET_TARGET_PROPERTIES(KsdkUsbOtgMqxLib PROPERTIES OUTPUT_NAME "libusbotg_mqx.a")
