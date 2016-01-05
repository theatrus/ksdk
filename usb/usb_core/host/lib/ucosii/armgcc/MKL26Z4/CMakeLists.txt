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
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -g  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99 ")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99 ")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -D_DEBUG=1")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKL26Z128VLH4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D_DEBUG=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKL26Z128VLH4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSII")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M0/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M0/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller/ehci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include/MKL26Z4)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M0/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M0/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../sources/controller/ehci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKL26Z4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../include/MKL26Z4)
ENDIF()

# ADD_LIBRARY
ADD_LIBRARY(KsdkUsbHostUcosiiLib STATIC
    "${ProjDirPath}/../../../../../../adapter/sources/adapter.h"
    "${ProjDirPath}/../../../../../../adapter/sources/adapter_types.h"
    "${ProjDirPath}/../../../../../../adapter/sources/sdk/adapter_cfg.h"
    "${ProjDirPath}/../../../../../../adapter/sources/sdk/adapter_sdk.c"
    "${ProjDirPath}/../../../../../../adapter/sources/sdk/adapter_sdk.h"
    "${ProjDirPath}/../../../../../include/compiler.h"
    "${ProjDirPath}/../../../../../include/types.h"
    "${ProjDirPath}/../../../../../include/usb.h"
    "${ProjDirPath}/../../../../../include/usb_desc.h"
    "${ProjDirPath}/../../../../../include/usb_error.h"
    "${ProjDirPath}/../../../../../include/usb_misc.h"
    "${ProjDirPath}/../../../../../include/usb_opt.h"
    "${ProjDirPath}/../../../../../include/usb_types.h"
    "${ProjDirPath}/../../../../include/usb_host_stack_interface.h"
    "${ProjDirPath}/../../../../sources/classes/common/usb_classes.c"
    "${ProjDirPath}/../../../../sources/classes/audio/usb_host_audio.c"
    "${ProjDirPath}/../../../../sources/classes/audio/usb_host_audio.h"
    "${ProjDirPath}/../../../../sources/classes/audio/usb_host_audio_intf.c"
    "${ProjDirPath}/../../../../sources/classes/cdc/usb_host_cdc.c"
    "${ProjDirPath}/../../../../sources/classes/cdc/usb_host_cdc.h"
    "${ProjDirPath}/../../../../sources/classes/cdc/usb_host_cdc_intf.c"
    "${ProjDirPath}/../../../../sources/classes/hid/usb_host_hid.c"
    "${ProjDirPath}/../../../../sources/classes/hid/usb_host_hid.h"
    "${ProjDirPath}/../../../../sources/classes/hub/usb_host_hub.c"
    "${ProjDirPath}/../../../../sources/classes/hub/usb_host_hub.h"
    "${ProjDirPath}/../../../../sources/classes/hub/usb_host_hub_prv.h"
    "${ProjDirPath}/../../../../sources/classes/hub/usb_host_hub_sm.c"
    "${ProjDirPath}/../../../../sources/classes/hub/usb_host_hub_sm.h"
    "${ProjDirPath}/../../../../sources/classes/msd/usb_host_msd_bo.c"
    "${ProjDirPath}/../../../../sources/classes/msd/usb_host_msd_bo.h"
    "${ProjDirPath}/../../../../sources/classes/msd/usb_host_msd_queue.c"
    "${ProjDirPath}/../../../../sources/classes/msd/usb_host_msd_ufi.c"
    "${ProjDirPath}/../../../../sources/classes/msd/usb_host_msd_ufi.h"
    "${ProjDirPath}/../../../../sources/classes/phdc/usb_host_phdc.c"
    "${ProjDirPath}/../../../../sources/classes/phdc/usb_host_phdc.h"
    "${ProjDirPath}/../../../../sources/controller/usb_host.c"
    "${ProjDirPath}/../../../../sources/controller/usb_host.h"
    "${ProjDirPath}/../../../../sources/controller/usb_host_ch9.c"
    "${ProjDirPath}/../../../../sources/controller/usb_host_ch9.h"
    "${ProjDirPath}/../../../../sources/controller/usb_host_common.c"
    "${ProjDirPath}/../../../../sources/controller/usb_host_common.h"
    "${ProjDirPath}/../../../../sources/controller/usb_host_dev_mng.c"
    "${ProjDirPath}/../../../../sources/controller/usb_host_dev_mng.h"
    "${ProjDirPath}/../../../../sources/controller/khci/host_khci_interface.c"
    "${ProjDirPath}/../../../../sources/controller/khci/khci.c"
    "${ProjDirPath}/../../../../sources/controller/khci/khci.h"
    "${ProjDirPath}/../../../../sources/controller/khci/khci_prv.h"
    "${ProjDirPath}/../../../../include/MKL26Z4/usb_host_config.h"
    "${ProjDirPath}/../../../../sources/bsp/MKL26Z4/usb_host_bsp.c"
)

SET_TARGET_PROPERTIES(KsdkUsbHostUcosiiLib PROPERTIES OUTPUT_NAME "libusbh_ucosii.a")
