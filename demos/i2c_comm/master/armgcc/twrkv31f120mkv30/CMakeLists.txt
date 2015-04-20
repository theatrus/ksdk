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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../platform/linker/MKV30F12810/gcc/MKV30F128xxx10_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../platform/linker/MKV30F12810/gcc/MKV30F128xxx10_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x400  -Xlinker --defsym=__heap_size__=0x400")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x400  -Xlinker --defsym=__heap_size__=0x400")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKV30F128VLH10")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_KV30F100M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKV30F128VLH10")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_KV30F100M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/startup/MKV30F12810)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../boards/twrkv31f120mkv30)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/startup/MKV30F12810)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../boards/twrkv31f120mkv30)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(i2c_comm_mstr 
    "${ProjDirPath}/../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../platform/startup/MKV30F12810/gcc/startup_MKV30F12810.S"
    "${ProjDirPath}/../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../src/i2c_comm_master.c"
    "${ProjDirPath}/../../../../../platform/drivers/src/i2c/fsl_i2c_irq.c"
    "${ProjDirPath}/../../../../../platform/startup/MKV30F12810/system_MKV30F12810.c"
    "${ProjDirPath}/../../../../../platform/startup/MKV30F12810/system_MKV30F12810.h"
    "${ProjDirPath}/../../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/gpio_pins.c"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/gpio_pins.h"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/pin_mux.c"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/pin_mux.h"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/board.h"
    "${ProjDirPath}/../../../../../boards/twrkv31f120mkv30/hardware_init.c"
)
SET_TARGET_PROPERTIES(i2c_comm_mstr PROPERTIES OUTPUT_NAME "i2c_comm_mstr.elf")

TARGET_LINK_LIBRARIES(i2c_comm_mstr -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(i2c_comm_mstr ${ProjDirPath}/../../../../../lib/ksdk_platform_lib/armgcc/KV30F12810/debug/libksdk_platform.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(i2c_comm_mstr ${ProjDirPath}/../../../../../lib/ksdk_platform_lib/armgcc/KV30F12810/release/libksdk_platform.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(i2c_comm_mstr m)
TARGET_LINK_LIBRARIES(i2c_comm_mstr c)
TARGET_LINK_LIBRARIES(i2c_comm_mstr gcc)
TARGET_LINK_LIBRARIES(i2c_comm_mstr nosys)
TARGET_LINK_LIBRARIES(i2c_comm_mstr -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/i2c_comm_mstr.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/i2c_comm_mstr.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET i2c_comm_mstr POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/i2c_comm_mstr.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_comm_mstr.hex)
ADD_CUSTOM_COMMAND(TARGET i2c_comm_mstr POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/i2c_comm_mstr.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_comm_mstr.bin)
