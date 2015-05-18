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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../platform/devices/MK64F12/linker/gcc/MK64FN1M0xxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../platform/devices/MK64F12/linker/gcc/MK64FN1M0xxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -lm  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -lm  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DARM_MATH_CM4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DARM_MATH_CM4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices/MK64F12/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices/MK64F12/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/sai)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/i2c)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices/MK64F12/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/devices/MK64F12/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/sai)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../platform/drivers/src/i2c)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(sai_demo 
    "${ProjDirPath}/../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../platform/devices/MK64F12/startup/gcc/startup_MK64F12.S"
    "${ProjDirPath}/../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../platform/devices/MK64F12/startup/system_MK64F12.c"
    "${ProjDirPath}/../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../platform/devices/startup.h"
    "${ProjDirPath}/../../../gpio_pins.c"
    "${ProjDirPath}/../../../gpio_pins.h"
    "${ProjDirPath}/../../../pin_mux.c"
    "${ProjDirPath}/../../../pin_mux.h"
    "${ProjDirPath}/../../../board.c"
    "${ProjDirPath}/../../../board.h"
    "${ProjDirPath}/../hardware_init.c"
    "${ProjDirPath}/../main.c"
    "${ProjDirPath}/../audio.c"
    "${ProjDirPath}/../audio.h"
    "${ProjDirPath}/../equalizer.c"
    "${ProjDirPath}/../equalizer.h"
    "${ProjDirPath}/../pcm_data.h"
    "${ProjDirPath}/../terminal_menu.c"
    "${ProjDirPath}/../terminal_menu.h"
    "${ProjDirPath}/../fsl_sgtl5000_driver.c"
    "${ProjDirPath}/../fsl_sgtl5000_driver.h"
    "${ProjDirPath}/../../../../../platform/composite/src/soundcard/fsl_soundcard.c"
    "${ProjDirPath}/../../../../../platform/composite/inc/fsl_soundcard.h"
    "${ProjDirPath}/../fsl_edma_irq.c"
    "${ProjDirPath}/../fsl_i2c_irq.c"
    "${ProjDirPath}/../fsl_sai_irq.c"
)
SET_TARGET_PROPERTIES(sai_demo PROPERTIES OUTPUT_NAME "sai_demo.elf")

TARGET_LINK_LIBRARIES(sai_demo -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(sai_demo ${ProjDirPath}/../../../../../lib/ksdk_platform_lib/armgcc/K64F12/debug/libksdk_platform.a)
    TARGET_LINK_LIBRARIES(sai_demo ${ProjDirPath}/../../../../../platform/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(sai_demo ${ProjDirPath}/../../../../../lib/ksdk_platform_lib/armgcc/K64F12/release/libksdk_platform.a)
    TARGET_LINK_LIBRARIES(sai_demo ${ProjDirPath}/../../../../../platform/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(sai_demo m)
TARGET_LINK_LIBRARIES(sai_demo c)
TARGET_LINK_LIBRARIES(sai_demo gcc)
TARGET_LINK_LIBRARIES(sai_demo nosys)
TARGET_LINK_LIBRARIES(sai_demo -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/sai_demo.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/sai_demo.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET sai_demo POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/sai_demo.elf ${EXECUTABLE_OUTPUT_PATH}/sai_demo.hex)
ADD_CUSTOM_COMMAND(TARGET sai_demo POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/sai_demo.elf ${EXECUTABLE_OUTPUT_PATH}/sai_demo.bin)
