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

# DEBUG LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../../../platform/linker/MK60D10/gcc/MK60DN512xxx10_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../../../platform/linker/MK60D10/gcc/MK60DN512xxx10_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x1000  -Xlinker --defsym=__heap_size__=0x0400")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x1000  -Xlinker --defsym=__heap_size__=0x0400")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K60D100M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCLOCK_SETUP=4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K60D100M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCLOCK_SETUP=4")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/startup/MK60D10)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/generator)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/virtual_com)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../composite/hid_audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../composite/msd_cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/hid_keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/hid_mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/cdc/cdc_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/cdc/cdc_serial/sdk/serial_bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/keyboard+mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/msd/msd_cmd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/phdc/11073Manager_Demo)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../otg/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/printer)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/startup/MK60D10)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/generator)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/virtual_com)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../composite/hid_audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../composite/msd_cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/hid_keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/hid_mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/cdc/cdc_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/cdc/cdc_serial/sdk/serial_bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/keyboard+mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/msd/msd_cmd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../host/phdc/11073Manager_Demo)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../otg/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include/twrk60d100m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/classes/printer)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/otg/sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(dev_phdc_weighscale_bm 
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/MK60D10/gcc/startup_MK60D10.S"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../../../platform/startup/MK60D10/system_MK60D10.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/gpio_pins.c"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/gpio_pins.h"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/pin_mux.c"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/pin_mux.h"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/board.h"
    "${ProjDirPath}/../../../../../../../../boards/twrk60d100m/hardware_init.c"
    "${ProjDirPath}/../../../ieee11073_nom_codes.h"
    "${ProjDirPath}/../../../ieee11073_phd_types.h"
    "${ProjDirPath}/../../../phd_com_model.c"
    "${ProjDirPath}/../../../phd_com_model.h"
    "${ProjDirPath}/../../../usb_descriptor.c"
    "${ProjDirPath}/../../../usb_descriptor.h"
    "${ProjDirPath}/../../../weighscale.c"
    "${ProjDirPath}/../../../weighscale.h"
)
SET_TARGET_PROPERTIES(dev_phdc_weighscale_bm PROPERTIES OUTPUT_NAME "dev_phdc_weighscale_bm.elf")

TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm ${ProjDirPath}/../../../../../../../usb_core/device/build/armgcc/usbd_sdk_twrk60d100m_bm/debug/libusbd_bm.a)
    TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm ${ProjDirPath}/../../../../../../../../lib/ksdk_platform_lib/armgcc/K60D10/debug/libksdk_platform.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm ${ProjDirPath}/../../../../../../../usb_core/device/build/armgcc/usbd_sdk_twrk60d100m_bm/release/libusbd_bm.a)
    TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm ${ProjDirPath}/../../../../../../../../lib/ksdk_platform_lib/armgcc/K60D10/release/libksdk_platform.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm m)
TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm c)
TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm gcc)
TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm nosys)
TARGET_LINK_LIBRARIES(dev_phdc_weighscale_bm -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/dev_phdc_weighscale_bm.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/dev_phdc_weighscale_bm.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET dev_phdc_weighscale_bm POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/dev_phdc_weighscale_bm.elf ${EXECUTABLE_OUTPUT_PATH}/dev_phdc_weighscale_bm.hex)
ADD_CUSTOM_COMMAND(TARGET dev_phdc_weighscale_bm POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/dev_phdc_weighscale_bm.elf ${EXECUTABLE_OUTPUT_PATH}/dev_phdc_weighscale_bm.bin)
