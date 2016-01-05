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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../platform/devices/MK21DA5/linker/gcc/MK21DN512Axxx5_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../platform/devices/MK21DA5/linker/gcc/MK21DN512Axxx5_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x2000  -Xlinker --defsym=__ram_vector_table__=1")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK21DN512AVMC5")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K21D50M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK21DN512AVMC5")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K21D50M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K21DA5/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K21DA5/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK21DA5/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK21DA5/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K21DA5/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K21DA5/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK21DA5/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/devices/MK21DA5/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../..)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(power_manager_rtos_demo_freertos 
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/port.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portasm.S"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portmacro.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portTicks.h"
    "${ProjDirPath}/../../../../../../platform/devices/MK21DA5/startup/gcc/startup_MK21DA5.S"
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
    "${ProjDirPath}/../../task_lpm.c"
    "${ProjDirPath}/../../task_lpm.h"
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
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/croutine.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/FreeRTOS.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/list.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/mpu_wrappers.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/portable.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/projdefs.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/semphr.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/StackMacros.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/task.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/include/timers.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/croutine.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/heap_1.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/heap_2.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/heap_3.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/heap_4.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/list.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/queue.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/tasks.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/timers.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/src/event_groups.c"
    "${ProjDirPath}/../../../../../../platform/devices/MK21DA5/startup/system_MK21DA5.c"
    "${ProjDirPath}/../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../platform/devices/startup.h"
)
SET_TARGET_PROPERTIES(power_manager_rtos_demo_freertos PROPERTIES OUTPUT_NAME "power_manager_rtos_demo_freertos.elf")

TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos ${ProjDirPath}/../../../../../../lib/ksdk_freertos_lib/armgcc/K21DA5/debug/libksdk_platform_freertos.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos ${ProjDirPath}/../../../../../../lib/ksdk_freertos_lib/armgcc/K21DA5/release/libksdk_platform_freertos.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos m)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos c)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos gcc)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos nosys)
TARGET_LINK_LIBRARIES(power_manager_rtos_demo_freertos -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/power_manager_rtos_demo_freertos.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/power_manager_rtos_demo_freertos.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET power_manager_rtos_demo_freertos POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_freertos.elf ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_freertos.hex)
ADD_CUSTOM_COMMAND(TARGET power_manager_rtos_demo_freertos POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_freertos.elf ${EXECUTABLE_OUTPUT_PATH}/power_manager_rtos_demo_freertos.bin)
