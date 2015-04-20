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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../platform/linker/MK64F12/gcc/MK64FN1M0xxx12_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0xa000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0xa000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK64FN1M0VMD12")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_FREE_RTOS")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K64F120M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K64F12/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K64F12/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/port)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/port/arch)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv6)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv6/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/netif)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/posix)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/common/phyksz8041)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/twrk64f120m)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K64F12/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/config/K64F12/gcc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/port)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/port/arch)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv6)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv6/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/netif)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include/posix)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/FreeRTOS/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/common/phyksz8041)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup/MK64F12)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/twrk64f120m)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(lwip_ping_demo_freertos 
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/port.c"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portasm.S"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portmacro.h"
    "${ProjDirPath}/../../../../../../rtos/FreeRTOS/port/gcc/portTicks.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../platform/startup/MK64F12/gcc/startup_MK64F12.S"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/arch/cc.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/arch/perf.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/ethernetif.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/lwipopts.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/sys_arch.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/port/lwip_fsl_irq.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/api_lib.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/api_msg.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/err.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/netbuf.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/netdb.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/netifapi.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/sockets.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/api/tcpip.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/def.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/dhcp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/dns.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/init.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/mem.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/memp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/netif.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/pbuf.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/raw.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/stats.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/sys.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/tcp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/tcp_in.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/tcp_out.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/lwip_timers.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/udp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/autoip.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/icmp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/igmp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/inet.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/inet_chksum.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/ip.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/ip_addr.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/ipv4/ip_frag.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/asn1_dec.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/asn1_enc.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/mib2.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/mib_structs.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/msg_in.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/core/snmp/msg_out.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/autoip.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/icmp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/igmp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/inet.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/inet_chksum.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/ip.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/ip_addr.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/ipv4/lwip/ip_frag.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/api.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/api_msg.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/arch.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/debug.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/def.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/dhcp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/dns.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/err.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/init.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/mem.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/memp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/memp_std.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/netbuf.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/netdb.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/netif.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/netifapi.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/opt.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/pbuf.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/raw.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/sio.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/snmp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/snmp_asn1.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/snmp_msg.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/snmp_structs.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/sockets.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/stats.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/sys.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/tcp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/tcpip.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/tcp_impl.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/timers.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/lwip/udp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/netif/etharp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/netif/ppp_oe.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/netif/slipif.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/posix/netdb.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/include/posix/sys/socket.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/etharp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/slipif.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/auth.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/auth.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/chap.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/chap.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/chpms.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/chpms.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/fsm.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/fsm.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ipcp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ipcp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/lcp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/lcp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/magic.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/magic.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/md5.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/md5.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/pap.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/pap.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ppp.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ppp.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/pppdebug.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ppp_impl.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/ppp_oe.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/randm.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/randm.h"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/vj.c"
    "${ProjDirPath}/../../../../../../tcpip/lwip/src/netif/ppp/vj.h"
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
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../boards/common/phyksz8041/fsl_phy_driver.c"
    "${ProjDirPath}/../../../src/ping.c"
    "${ProjDirPath}/../../../../../../platform/startup/MK64F12/system_MK64F12.c"
    "${ProjDirPath}/../../../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/gpio_pins.c"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/gpio_pins.h"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/pin_mux.c"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/pin_mux.h"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/board.h"
    "${ProjDirPath}/../../../../../../boards/twrk64f120m/hardware_init.c"
)
SET_TARGET_PROPERTIES(lwip_ping_demo_freertos PROPERTIES OUTPUT_NAME "lwip_ping_demo_freertos.elf")

TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos ${ProjDirPath}/../../../../../../lib/ksdk_freertos_lib/armgcc/K64F12/debug/libksdk_platform_freertos.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos ${ProjDirPath}/../../../../../../lib/ksdk_freertos_lib/armgcc/K64F12/release/libksdk_platform_freertos.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos m)
TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos c)
TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos gcc)
TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos nosys)
TARGET_LINK_LIBRARIES(lwip_ping_demo_freertos -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/lwip_ping_demo_freertos.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/lwip_ping_demo_freertos.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET lwip_ping_demo_freertos POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_freertos.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_freertos.hex)
ADD_CUSTOM_COMMAND(TARGET lwip_ping_demo_freertos POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_freertos.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_freertos.bin)
