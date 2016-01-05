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
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m0plus  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -mcpu=cortex-m0plus  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MKL43Z256VLH4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSIII")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_KL43Z48M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MKL43Z256VLH4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSIII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_KL43Z48M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M0/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M0/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M0/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M0/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(power_manager_rtos_demo_ucosiii 
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M0/Generic/GNU/os_cpu.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M0/Generic/GNU/os_cpu_a.S"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M0/Generic/GNU/os_cpu_c.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M0/GNU/cpu_c.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M0/GNU/cpu_a.S"
    "${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup/gcc/startup_MKL43Z4.S"
    "${ProjDirPath}/../../adc16_lpm_callback.c"
    "${ProjDirPath}/../../adc16_temperature.c"
    "${ProjDirPath}/../../adc16_temperature.h"
    "${ProjDirPath}/../../fsl_adc_irq.c"
    "${ProjDirPath}/../../fsl_lptmr_irq.c"
    "${ProjDirPath}/../../fsl_rtc_irq.c"
    "${ProjDirPath}/../../lpm_rtos.h"
    "${ProjDirPath}/../../lptmr_setup.c"
    "${ProjDirPath}/../../lptmr_setup.h"
    "${ProjDirPath}/../../rtc_setup.c"
    "${ProjDirPath}/../../rtc_setup.h"
    "${ProjDirPath}/../../main.c"
    "${ProjDirPath}/../../../..//demo_apps/power_manager_rtos_demo/task_lpm.c"
    "${ProjDirPath}/../../../..//demo_apps/power_manager_rtos_demo/task_lpm.h"
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
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/config/os_app_hooks.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_core.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_dbg.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_flag.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_int.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_mem.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_msg.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_mutex.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_prio.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_q.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_sem.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_stat.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_task.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_tick.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_time.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_tmr.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_type.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_var.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSIII/src/uC-CPU/cpu_core.c"
    "${ProjDirPath}/../../../../../../platform/devices/MKL43Z4/startup/system_MKL43Z4.c"
    "${ProjDirPath}/../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../platform/devices/startup.h"
)
SET_TARGET_PROPERTIES(power_manager_rtos_demo_ucosiii PROPERTIES OUTPUT_NAME "power_manager_rtos_demo_ucosiii.elf")

TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii ${ProjDirPath}/../../../../../../lib/ksdk_ucosiii_lib/armgcc/KL43Z4/debug/libksdk_platform_ucosiii.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii ${ProjDirPath}/../../../../../../lib/ksdk_ucosiii_lib/armgcc/KL43Z4/release/libksdk_platform_ucosiii.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii m)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii c)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii gcc)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii nosys)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_ucosiii -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/power_manager_rtos_demo_ucosiii.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/power_manager_rtos_demo_ucosiii.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET power_manager_rtos_demo_ucosiii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_ucosiii.elf ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_ucosiii.hex)
ADD_CUSTOM_COMMAND(TARGET power_manager_rtos_demo_ucosiii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_ucosiii.elf ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_ucosiii.bin)
