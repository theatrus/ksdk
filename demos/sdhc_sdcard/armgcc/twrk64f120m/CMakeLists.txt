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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../boards/twrk64f120m)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../boards/twrk64f120m)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(sdhc_sdcard 
    "${ProjDirPath}/../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../platform/startup/MK64F12/gcc/startup_MK64F12.S"
    "${ProjDirPath}/../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../platform/composite/src/sdcard/fsl_sdhc_card.c"
    "${ProjDirPath}/../../../../platform/composite/inc/fsl_sdhc_card.h"
    "${ProjDirPath}/../../src/main.c"
    "${ProjDirPath}/../../src/sdhc_sdcard.h"
    "${ProjDirPath}/../../src/fsl_gpio_irq.c"
    "${ProjDirPath}/../../../../platform/drivers/src/sdhc/fsl_sdhc_irq.c"
    "${ProjDirPath}/../../../../platform/startup/MK64F12/system_MK64F12.c"
    "${ProjDirPath}/../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../boards/twrk64f120m/gpio_pins.c"
    "${ProjDirPath}/../../../../boards/twrk64f120m/gpio_pins.h"
    "${ProjDirPath}/../../../../boards/twrk64f120m/pin_mux.c"
    "${ProjDirPath}/../../../../boards/twrk64f120m/pin_mux.h"
    "${ProjDirPath}/../../../../boards/twrk64f120m/board.h"
    "${ProjDirPath}/../../../../boards/twrk64f120m/hardware_init.c"
)
SET_TARGET_PROPERTIES(sdhc_sdcard PROPERTIES OUTPUT_NAME "sdhc_sdcard.elf")

TARGET_LINK_LIBRARIES(sdhc_sdcard -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(sdhc_sdcard ${ProjDirPath}/../../../../lib/ksdk_platform_lib/armgcc/K64F12/debug/libksdk_platform.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(sdhc_sdcard ${ProjDirPath}/../../../../lib/ksdk_platform_lib/armgcc/K64F12/release/libksdk_platform.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(sdhc_sdcard m)
TARGET_LINK_LIBRARIES(sdhc_sdcard c)
TARGET_LINK_LIBRARIES(sdhc_sdcard gcc)
TARGET_LINK_LIBRARIES(sdhc_sdcard nosys)
TARGET_LINK_LIBRARIES(sdhc_sdcard -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/sdhc_sdcard.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/sdhc_sdcard.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET sdhc_sdcard POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/sdhc_sdcard.elf ${EXECUTABLE_OUTPUT_PATH}/sdhc_sdcard.hex)
ADD_CUSTOM_COMMAND(TARGET sdhc_sdcard POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/sdhc_sdcard.elf ${EXECUTABLE_OUTPUT_PATH}/sdhc_sdcard.bin)
