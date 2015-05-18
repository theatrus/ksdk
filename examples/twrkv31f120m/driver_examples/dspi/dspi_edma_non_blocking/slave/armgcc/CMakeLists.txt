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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/linker/gcc/MKV31F512xxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/linker/gcc/MKV31F512xxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKV31F512VLL12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_KV31F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKV31F512VLL12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_KV31F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/dspi)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../platform/drivers/src/dspi)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(dspi_edma_non_blocking_example_slave 
    "${ProjDirPath}/../../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/startup/gcc/startup_MKV31F51212.S"
    "${ProjDirPath}/../../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/startup/system_MKV31F51212.c"
    "${ProjDirPath}/../../../../../../../platform/devices/MKV31F51212/startup/system_MKV31F51212.h"
    "${ProjDirPath}/../../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../../platform/devices/startup.h"
    "${ProjDirPath}/../../../../../gpio_pins.c"
    "${ProjDirPath}/../../../../../gpio_pins.h"
    "${ProjDirPath}/../../../../../pin_mux.c"
    "${ProjDirPath}/../../../../../pin_mux.h"
    "${ProjDirPath}/../../../../../board.c"
    "${ProjDirPath}/../../../../../board.h"
    "${ProjDirPath}/../hardware_init.c"
    "${ProjDirPath}/../main.c"
    "${ProjDirPath}/../fsl_dspi_edma_irq.c"
    "${ProjDirPath}/../fsl_edma_irq.c"
)
SET_TARGET_PROPERTIES(dspi_edma_non_blocking_example_slave PROPERTIES OUTPUT_NAME "dspi_edma_non_blocking_example_slave.elf")

TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave ${ProjDirPath}/../../../../../../../lib/ksdk_platform_lib/armgcc/KV31F51212/debug/libksdk_platform.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave ${ProjDirPath}/../../../../../../../lib/ksdk_platform_lib/armgcc/KV31F51212/release/libksdk_platform.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave m)
TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave c)
TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave gcc)
TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave nosys)
TARGET_LINK_LIBRARIES(dspi_edma_non_blocking_example_slave -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/dspi_edma_non_blocking_example_slave.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/dspi_edma_non_blocking_example_slave.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET dspi_edma_non_blocking_example_slave POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/dspi_edma_non_blocking_example_slave.elf ${EXECUTABLE_OUTPUT_PATH}/dspi_edma_non_blocking_example_slave.hex)
ADD_CUSTOM_COMMAND(TARGET dspi_edma_non_blocking_example_slave POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/dspi_edma_non_blocking_example_slave.elf ${EXECUTABLE_OUTPUT_PATH}/dspi_edma_non_blocking_example_slave.bin)
