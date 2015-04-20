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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../platform/linker/MK60D10/gcc/MK60DN512xxx10_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../platform/linker/MK60D10/gcc/MK60DN512xxx10_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x8000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=soft  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x8000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K60D100M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK60DN512VMD10")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K60D100M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/common/phyksz8041)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup/MK60D10)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/twrk60d100m)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU/ARM-Cortex-M4/GNU)
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
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../rtos/uCOSII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/common/phyksz8041)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/CMSIS/Include/device)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/startup/MK60D10)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../boards/twrk60d100m)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(lwip_udpecho_demo_ucosii 
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu.h"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.S"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.c"
    "${ProjDirPath}/../../../../../../rtos/uCOSII/src/uCOS-II/Ports/ARM-Cortex-M4/Generic/GNU/os_dbg.c"
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../platform/startup/MK60D10/gcc/startup_MK60D10.S"
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
    "${ProjDirPath}/../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../boards/common/phyksz8041/fsl_phy_driver.c"
    "${ProjDirPath}/../../../src/udpecho.c"
    "${ProjDirPath}/../../../../../../platform/startup/MK60D10/system_MK60D10.c"
    "${ProjDirPath}/../../../../../../platform/startup/startup.c"
    "${ProjDirPath}/../../../../../../platform/startup/startup.h"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/gpio_pins.c"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/gpio_pins.h"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/pin_mux.c"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/pin_mux.h"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/board.h"
    "${ProjDirPath}/../../../../../../boards/twrk60d100m/hardware_init.c"
)
SET_TARGET_PROPERTIES(lwip_udpecho_demo_ucosii PROPERTIES OUTPUT_NAME "lwip_udpecho_demo_ucosii.elf")

TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii ${ProjDirPath}/../../../../../../lib/ksdk_ucosii_lib/armgcc/K60D10/debug/libksdk_platform_ucosii.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii ${ProjDirPath}/../../../../../../lib/ksdk_ucosii_lib/armgcc/K60D10/release/libksdk_platform_ucosii.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii m)
TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii c)
TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii gcc)
TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii nosys)
TARGET_LINK_LIBRARIES(lwip_udpecho_demo_ucosii -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/lwip_udpecho_demo_ucosii.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/lwip_udpecho_demo_ucosii.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET lwip_udpecho_demo_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/lwip_udpecho_demo_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_udpecho_demo_ucosii.hex)
ADD_CUSTOM_COMMAND(TARGET lwip_udpecho_demo_ucosii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/lwip_udpecho_demo_ucosii.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_udpecho_demo_ucosii.bin)
