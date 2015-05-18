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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/linker/gcc/MKL43Z256xxx4_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/linker/gcc/MKL43Z256xxx4_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -Xlinker -z -Xlinker muldefs  -Xlinker -static  -Xlinker -cref  -nostartfiles  -Xlinker --undefined=__isr_vector  -lc  -lgcc      -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -z -Xlinker muldefs  -Xlinker -static  -Xlinker -cref  -nostartfiles  -Xlinker --undefined=__isr_vector  -lc  -lgcc      -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -D_AEABI_LC_CTYPE=C")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -D__STRICT_ANSI__=1")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKL43Z256VLH4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_MQX")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFRDM_KL43Z")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFREEDOM")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D_AEABI_LC_CTYPE=C")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D__STRICT_ANSI__=1")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKL43Z256VLH4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_MQX")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFRDM_KL43Z")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFREEDOM")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug/mqx)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug/mqx_stdlib)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/mcu/MKL43Z4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/board/frdmkl43z)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/mpu)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_dummy)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_mem)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_null)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_pipe)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_tfs)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_tty)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release/mqx)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release/mqx_stdlib)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/mcu/MKL43Z4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/board/frdmkl43z)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/mpu)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/config/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_dummy)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_mem)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_null)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_pipe)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_serial)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_tfs)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/nio/drivers/nio_tty)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/src/i2c)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(i2c_rtos_mqx 
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp/bsp.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp/bsp_config.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp/init_bsp.c"
    "${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/include/mqx.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/mqx/source/bsp/mqx_main.c"
    "${ProjDirPath}/../../../../../../rtos/mqx/config/common/small_ram_config.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/config/common/verif_enabled_config.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/config/mcu/MKL43Z4/mqx_sdk_config.h"
    "${ProjDirPath}/../../../../../../rtos/mqx/config/board/frdmkl43z/user_config.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../gpio_pins.c"
    "${ProjDirPath}/../../../../gpio_pins.h"
    "${ProjDirPath}/../../../../pin_mux.c"
    "${ProjDirPath}/../../../../pin_mux.h"
    "${ProjDirPath}/../../../../board.c"
    "${ProjDirPath}/../../../../board.h"
    "${ProjDirPath}/../../hardware_init.c"
    "${ProjDirPath}/../../i2c_mqx_rtos_irq.c"
    "${ProjDirPath}/../../main.c"
    "${ProjDirPath}/../../task_master.c"
    "${ProjDirPath}/../../task_slave.c"
    "${ProjDirPath}/../../task_sample.c"
    "${ProjDirPath}/../../i2c_rtos.h"
)
SET_TARGET_PROPERTIES(i2c_rtos_mqx PROPERTIES OUTPUT_NAME "i2c_rtos_mqx.elf")

TARGET_LINK_LIBRARIES(i2c_rtos_mqx -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug/mqx/lib_mqx.a)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/debug/mqx_stdlib/lib_mqx_stdlib.a)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../lib/ksdk_mqx_lib/armgcc/KL43Z4/debug/libksdk_platform_mqx.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release/mqx/lib_mqx.a)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../rtos/mqx/lib/frdmkl43z.armgcc/release/mqx_stdlib/lib_mqx_stdlib.a)
    TARGET_LINK_LIBRARIES(i2c_rtos_mqx ${ProjDirPath}/../../../../../../lib/ksdk_mqx_lib/armgcc/KL43Z4/release/libksdk_platform_mqx.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(i2c_rtos_mqx m)
TARGET_LINK_LIBRARIES(i2c_rtos_mqx c)
TARGET_LINK_LIBRARIES(i2c_rtos_mqx gcc)
TARGET_LINK_LIBRARIES(i2c_rtos_mqx nosys)
TARGET_LINK_LIBRARIES(i2c_rtos_mqx -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/i2c_rtos_mqx.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/i2c_rtos_mqx.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET i2c_rtos_mqx POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_mqx.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_mqx.hex)
ADD_CUSTOM_COMMAND(TARGET i2c_rtos_mqx POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_mqx.elf ${EXECUTABLE_OUTPUT_PATH}/i2c_rtos_mqx.bin)
