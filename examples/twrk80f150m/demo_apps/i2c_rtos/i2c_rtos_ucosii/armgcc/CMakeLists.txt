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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../platform/devices/MK80F25615/linker/gcc/MK80FN256xxx15_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../platform/devices/MK80F25615/linker/gcc/MK80FN256xxx15_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK80FN256VDC15")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K80F150M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK80FN256VDC15")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K80F150M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK80F25615/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK80F25615/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK80F25615/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK80F25615/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(i2c_rtos_ucosii 
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.S"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_dbg.c"
    "${ProjDirPath}/../../../../../../platform/devices/MK80F25615/startup/gcc/startup_MK80F25615.S"
    "${ProjDirPath}/../../fsl_i2c_irq.c"
    "${ProjDirPath}/../../main.c"
    "${ProjDirPath}/../../task_master.c"
    "${ProjDirPath}/../../task_slave.c"
    "${ProjDirPath}/../../task_sample.c"
    "${ProjDirPath}/../../i2c_rtos.h"
    "${ProjDirPath}/../../hardware_init.c"
    "${ProjDirPath}/../../../../gpio_pins.c"
    "${ProjDirPath}/../../../../gpio_pins.h"
    "${ProjDirPath}/../../../../pin_mux.c"
    "${ProjDirPath}/../../../../pin_mux.h"
    "${ProjDirPath}/../../../../board.c"
    "${ProjDirPath}/../../../../board.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/config/app_hooks.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_core.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_flag.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mbox.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mem.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mutex.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_q.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_sem.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_task.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_time.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_tmr.c"
    "${ProjDirPath}/../../../../../../platform/devices/MK80F25615/startup/system_MK80F25615.c"
    "${ProjDirPath}/../../../../../../platform/devices/MK80F25615/startup/system_MK80F25615.h"
    "${ProjDirPath}/../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../platform/devices/startup.h"
)
SET_TARGET_PROPERTIES(i2c_rtos_ucosii PROPERTIES OUTPUT_NAME "i2c_rtos_ucosii.elf")

TARGET_LINK_LIBRARIES(i2c_rtos_ucosii -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(i2c_rtos_ucosii ${ProjDirPath}/../../../../../../lib/ksdk_ucosii_lib/armgcc/K80F25615/debug/libksdk_platform_ucosii.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(i2c_rtos_ucosii ${ProjDirPath}/../../../../../../lib/ksdk_ucosii_lib/armgcc/K80F25615/release/libksdk_platform_ucosii.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(i2c_rtos_ucosii m)
TARGET_LINK_LIBRARIES(i2c_rtos_ucosii c)
TARGET_LINK_LIBRARIES(i2c_rtos_ucosii gcc)
TARGET_LINK_LIBRARIES(i2c_rtos_ucosii nosys)
TARGET_LINK_LIBRARIES(i2c_rtos_ucosii -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/i2c_rtos_ucosii.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/i2c_rtos_ucosii.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET i2c_rtos_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_ucosii.hex)
ADD_CUSTOM_COMMAND(TARGET i2c_rtos_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_ucosii.bin)
