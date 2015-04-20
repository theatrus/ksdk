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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x4000  -Xlinker --defsym=__heap_size__=0x8000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x4000  -Xlinker --defsym=__heap_size__=0x8000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFRDM_K64F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFREEDOM")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFRDM_K64F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFREEDOM")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/frdmk64f)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/audio/generator)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/cdc/virtual_com)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/composite/hid_audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/composite/msd_cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/hid/hid_keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/hid/hid_mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/phdc/weighscale)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/cdc_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/cdc_serial/sdk/serial_bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/keyboard+mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../phdc/11073Manager_Demo)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../otg/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include/frdmk64f)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include/frdmk64f)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/common/sgtl5000)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/sai)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/i2c)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/frdmk64f)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/audio/generator)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/cdc/virtual_com)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/composite/hid_audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/composite/msd_cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/hid/hid_keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/hid/hid_mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../device/phdc/weighscale)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../audio/speaker)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/cdc_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../cdc/cdc_serial/sdk/serial_bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/keyboard)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/keyboard+mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../phdc/11073Manager_Demo)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../otg/hid/mouse)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/device/include/frdmk64f)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../usb_core/host/include/frdmk64f)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../boards/common/sgtl5000)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/sai)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/i2c)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(host_msd_cmd_bm 
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/MK64F12/gcc/startup_MK64F12.S"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../../../platform/startup/MK64F12/system_MK64F12.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/gpio_pins.c"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/gpio_pins.h"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/pin_mux.c"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/pin_mux.h"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/board.h"
    "${ProjDirPath}/../../../../../../../../boards/frdmk64f/hardware_init.c"
    "${ProjDirPath}/../../../msd_commands.c"
    "${ProjDirPath}/../../../msd_commands.h"
)
SET_TARGET_PROPERTIES(host_msd_cmd_bm PROPERTIES OUTPUT_NAME "host_msd_cmd_bm.elf")

TARGET_LINK_LIBRARIES(host_msd_cmd_bm -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(host_msd_cmd_bm ${ProjDirPath}/../../../../../../../usb_core/host/build/armgcc/usbh_sdk_frdmk64f_bm/debug/libusbh_bm.a)
    TARGET_LINK_LIBRARIES(host_msd_cmd_bm ${ProjDirPath}/../../../../../../../../lib/ksdk_platform_lib/armgcc/K64F12/debug/libksdk_platform.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(host_msd_cmd_bm ${ProjDirPath}/../../../../../../../usb_core/host/build/armgcc/usbh_sdk_frdmk64f_bm/release/libusbh_bm.a)
    TARGET_LINK_LIBRARIES(host_msd_cmd_bm ${ProjDirPath}/../../../../../../../../lib/ksdk_platform_lib/armgcc/K64F12/release/libksdk_platform.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(host_msd_cmd_bm m)
TARGET_LINK_LIBRARIES(host_msd_cmd_bm c)
TARGET_LINK_LIBRARIES(host_msd_cmd_bm gcc)
TARGET_LINK_LIBRARIES(host_msd_cmd_bm nosys)
TARGET_LINK_LIBRARIES(host_msd_cmd_bm -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/host_msd_cmd_bm.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/host_msd_cmd_bm.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET host_msd_cmd_bm POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/host_msd_cmd_bm.elf ${EXECUTABLE_OUTPUT_PATH}/host_msd_cmd_bm.hex)
ADD_CUSTOM_COMMAND(TARGET host_msd_cmd_bm POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/host_msd_cmd_bm.elf ${EXECUTABLE_OUTPUT_PATH}/host_msd_cmd_bm.bin)
