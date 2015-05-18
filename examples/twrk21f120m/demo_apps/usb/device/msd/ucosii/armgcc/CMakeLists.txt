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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/linker/gcc/MK21FN1M0Axxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/linker/gcc/MK21FN1M0Axxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x4000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-format  -fno-strict-aliasing  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker --defsym=__ram_vector_table__=1  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mtune=cortex-m4  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x4000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -D_DEBUG=1")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK21FN1M0AVMC12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K21F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -D_DEBUG=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK21FN1M0AVMC12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K21F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSII")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/include/twrk21f120m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/include/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/include/twrk21f120m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/printer)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/composite/src/sdcard)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/adapter/sources)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/adapter/sources/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/include/twrk21f120m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/common)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/composite)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/include/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/device/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/hal)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/include/twrk21f120m)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/audio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/cdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/hid)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/hub)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/msd)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/phdc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/classes/printer)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/controller/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/host/sources/controller)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/driver/khci)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/driver/max3353/sdk)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../usb/usb_core/otg/sources/otg)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/uart)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/gpio)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/pit)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/src/edma)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/composite/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/composite/src/sdcard)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(dev_msd_disk_ucosii 
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.S"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_dbg.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/startup/gcc/startup_MK21FA12.S"
    "${ProjDirPath}/../../../../../../../../platform/composite/src/sdcard/fsl_sdhc_card.c"
    "${ProjDirPath}/../../../../../../../../platform/composite/inc/fsl_sdhc_card.h"
    "${ProjDirPath}/../../../../../../../../platform/drivers/src/sdhc/fsl_sdhc_irq.c"
    "${ProjDirPath}/../../disk.c"
    "${ProjDirPath}/../../disk.h"
    "${ProjDirPath}/../../usb_descriptor.c"
    "${ProjDirPath}/../../usb_descriptor.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/config/app_hooks.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_core.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_flag.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mbox.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mem.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_mutex.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_q.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_sem.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_task.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_time.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSII/src/uCOS-II/Source/os_tmr.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../../../platform/devices/MK21FA12/startup/system_MK21FA12.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/startup.h"
    "${ProjDirPath}/../../../../../../gpio_pins.c"
    "${ProjDirPath}/../../../../../../gpio_pins.h"
    "${ProjDirPath}/../../../../../../pin_mux.c"
    "${ProjDirPath}/../../../../../../pin_mux.h"
    "${ProjDirPath}/../../../../../../board.h"
    "${ProjDirPath}/../../../../../../board.c"
    "${ProjDirPath}/../../hardware_init.c"
)
SET_TARGET_PROPERTIES(dev_msd_disk_ucosii PROPERTIES OUTPUT_NAME "dev_msd_disk_ucosii.elf")

TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii ${ProjDirPath}/../../../../../../../../usb/usb_core/device/build/armgcc/usbd_sdk_twrk21f120m_ucosii/debug/libusbd_ucosii.a)
    TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii ${ProjDirPath}/../../../../../../../../lib/ksdk_ucosii_lib/armgcc/K21FA12/debug/libksdk_platform_ucosii.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii ${ProjDirPath}/../../../../../../../../usb/usb_core/device/build/armgcc/usbd_sdk_twrk21f120m_ucosii/release/libusbd_ucosii.a)
    TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii ${ProjDirPath}/../../../../../../../../lib/ksdk_ucosii_lib/armgcc/K21FA12/release/libksdk_platform_ucosii.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii m)
TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii c)
TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii gcc)
TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii nosys)
TARGET_LINK_LIBRARIES(dev_msd_disk_ucosii -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/dev_msd_disk_ucosii.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/dev_msd_disk_ucosii.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET dev_msd_disk_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/dev_msd_disk_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/dev_msd_disk_ucosii.hex)
ADD_CUSTOM_COMMAND(TARGET dev_msd_disk_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/dev_msd_disk_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/dev_msd_disk_ucosii.bin)
