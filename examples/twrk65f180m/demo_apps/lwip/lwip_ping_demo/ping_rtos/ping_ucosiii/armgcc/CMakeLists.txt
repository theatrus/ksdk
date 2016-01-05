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
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/linker/gcc/MK65FN2M0xxx18_flash.ld  -static")

# RELEASE LINK FILE
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/linker/gcc/MK65FN2M0xxx18_flash.ld  -static")

# DEBUG ASM FLAGS
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG C FLAGS
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g  -O0  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# DEBUG LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x8000")

# RELEASE ASM FLAGS
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE C FLAGS
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os  -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  -mthumb  -MMD  -MP  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mapcs  -std=gnu99")

# RELEASE LD FLAGS
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=fpv4-sp-d16  --specs=nano.specs  -lm  -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -mthumb  -mapcs  -Xlinker --gc-sections  -Xlinker -static  -Xlinker -z  -Xlinker muldefs  -Xlinker --defsym=__stack_size__=0x2000  -Xlinker --defsym=__heap_size__=0x8000")

# ASM MACRO
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}  -DDEBUG")

# C MACRO
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DCPU_MK65FN2M0VMI18")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DFSL_RTOS_UCOSIII")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTWR_K65F180M")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DTOWER")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DCPU_MK65FN2M0VMI18")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DFSL_RTOS_UCOSIII")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTWR_K65F180M")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -DTOWER")

# CXX MACRO

# INCLUDE_DIRECTORIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/arch)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv6)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv6/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/netif)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/posix)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../..)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../..)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/osa/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/CMSIS/Include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/startup)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/hal/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/drivers/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/system/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/arch)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv6)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv6/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/netif)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/posix)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../platform/utilities/inc)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/config)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-LIB)
    INCLUDE_DIRECTORIES(${ProjDirPath}/../../../../../..)
ENDIF()

# ADD_EXECUTABLE
ADD_EXECUTABLE(lwip_ping_demo_ucosiii 
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_misc_utilities.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.S"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU/cpu_c.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU/cpu_a.S"
    "${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/startup/gcc/startup_MK65F18.S"
    "${ProjDirPath}/../../fsl_phy_driver.c"
    "${ProjDirPath}/../../main.c"
    "${ProjDirPath}/../../hardware_init.c"
    "${ProjDirPath}/../../../../../../gpio_pins.c"
    "${ProjDirPath}/../../../../../../gpio_pins.h"
    "${ProjDirPath}/../../../../../../pin_mux.c"
    "${ProjDirPath}/../../../../../../pin_mux.h"
    "${ProjDirPath}/../../../../../../board.c"
    "${ProjDirPath}/../../../../../../board.h"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/fsl_debug_console.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/inc/fsl_debug_console.h"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.c"
    "${ProjDirPath}/../../../../../../../../platform/utilities/src/print_scan.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/arch/cc.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/arch/perf.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/ethernetif.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/lwipopts.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/sys_arch.c"
    "${ProjDirPath}/../../../../../../../../platform/system/src/hwtimer/fsl_hwtimer_pit_irq.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/port/lwip_fsl_irq.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/api_lib.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/api_msg.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/err.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/netbuf.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/netdb.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/netifapi.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/sockets.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/api/tcpip.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/def.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/dhcp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/dns.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/init.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/mem.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/memp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/netif.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/pbuf.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/raw.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/stats.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/sys.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/tcp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/tcp_in.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/tcp_out.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/lwip_timers.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/udp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/autoip.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/icmp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/igmp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/inet.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/inet_chksum.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/ip.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/ip_addr.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/ipv4/ip_frag.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/asn1_dec.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/asn1_enc.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/mib2.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/mib_structs.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/msg_in.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/core/snmp/msg_out.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/autoip.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/icmp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/igmp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/inet.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/inet_chksum.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/ip.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/ip_addr.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/ipv4/lwip/ip_frag.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/api.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/api_msg.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/arch.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/debug.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/def.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/dhcp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/dns.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/err.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/init.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/mem.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/memp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/memp_std.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/netbuf.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/netdb.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/netif.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/netifapi.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/opt.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/pbuf.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/raw.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/sio.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/snmp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/snmp_asn1.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/snmp_msg.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/snmp_structs.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/sockets.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/stats.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/sys.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/tcp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/tcpip.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/tcp_impl.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/timers.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/lwip/udp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/netif/etharp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/netif/ppp_oe.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/netif/slipif.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/posix/netdb.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/include/posix/sys/socket.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/etharp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/slipif.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/auth.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/auth.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/chap.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/chap.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/chpms.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/chpms.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/fsm.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/fsm.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ipcp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ipcp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/lcp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/lcp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/magic.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/magic.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/md5.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/md5.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/pap.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/pap.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ppp.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ppp.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/pppdebug.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ppp_impl.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/ppp_oe.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/randm.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/randm.h"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/vj.c"
    "${ProjDirPath}/../../../../../../../../middleware/tcpip/lwip/src/netif/ppp/vj.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/config/os_app_hooks.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_core.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_dbg.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_flag.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_int.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_mem.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_msg.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_mutex.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_prio.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_q.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_sem.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_stat.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_task.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_tick.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_time.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_tmr.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_type.h"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uCOS-III/Source/os_var.c"
    "${ProjDirPath}/../../../../../../../../rtos/uCOSIII/src/uC-CPU/cpu_core.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/MK65F18/startup/system_MK65F18.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/startup.c"
    "${ProjDirPath}/../../../../../../../../platform/devices/startup.h"
)
SET_TARGET_PROPERTIES(lwip_ping_demo_ucosiii PROPERTIES OUTPUT_NAME "lwip_ping_demo_ucosiii.elf")

TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii -Wl,--start-group)
# LIBRARIES
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii ${ProjDirPath}/../../../../../../../../lib/ksdk_ucosiii_lib/armgcc/K65F18/debug/libksdk_platform_ucosiii.a)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii ${ProjDirPath}/../../../../../../../../lib/ksdk_ucosiii_lib/armgcc/K65F18/release/libksdk_platform_ucosiii.a)
ENDIF()

# SYSTEM LIBRARIES
TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii m)
TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii c)
TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii gcc)
TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii nosys)
TARGET_LINK_LIBRARIES(lwip_ping_demo_ucosiii -Wl,--end-group)

# MAP FILE
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}  -Xlinker -Map=debug/lwip_ping_demo_ucosiii.map")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  -Xlinker -Map=release/lwip_ping_demo_ucosiii.map")

# BIN AND HEX
ADD_CUSTOM_COMMAND(TARGET lwip_ping_demo_ucosiii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_ucosiii.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_ucosiii.hex)
ADD_CUSTOM_COMMAND(TARGET lwip_ping_demo_ucosiii POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_ucosiii.elf ${EXECUTABLE_OUTPUT_PATH}/lwip_ping_demo_ucosiii.bin)
