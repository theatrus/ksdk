INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)

# this file was generated
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(rtcs)


SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)


ENABLE_LANGUAGE(ASM)
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)


# -----------------------------------------------
# common settings
# -----------------------------------------------

# common asm flags
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfloat-abi=hard")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=fpv4-sp-d16")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Xassembler --no-warn")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -std=gnu99")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wall")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mthumb")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -gdwarf-2")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -gstrict-dwarf")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -g2")

# common asm defines
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DCPU_MK64FN1M0VMD12=1")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DFSL_RTOS_MQX=1")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DMQX_USE_IO_OLD=0")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DPLATFORM_SDK_ENABLED=1")

# common c compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-2")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gstrict-dwarf")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g2")

# common c compiler defines
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCPU_MK64FN1M0VMD12=1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFSL_RTOS_MQX=1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMQX_USE_IO_OLD=0")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_SDK_ENABLED=1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_AEABI_LC_CTYPE=C")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__STRICT_ANSI__=1")


IF(CMAKE_BUILD_TYPE MATCHES "debug")
# -----------------------------------------------
# special settings for 'debug'
# -----------------------------------------------

#   asm defines
    SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -D_DEBUG=1")

#   c compiler flags
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

#   c compiler defines
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEBUG=1")

    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK64F12/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK64F12/startup")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/phyksz8041")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/board/twrk64f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/mcu/MK64F12")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/cpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/common")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/tad")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx_stdlib/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio/fs")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/bsp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../examples/twrk64f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/CMSIS/Include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/hal/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/enet")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/mpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/uart")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/utilities/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../filesystem/mfs/source/mfs_sdcard/nio_esdhc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/osa/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/system/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../filesystem/mfs/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../security/wolfssl/source/wolfssl")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../security/wolfssl/source")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk64f120m.armgcc/debug/rtcs)
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES "release")
# -----------------------------------------------
# special settings for 'release'
# -----------------------------------------------

#   asm defines
    SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DNDEBUG=1")

#   c compiler flags
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")

#   c compiler defines
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG=1")

    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK64F12/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK64F12/startup")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/phyksz8041")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/board/twrk64f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/mcu/MK64F12")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/cpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/common")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/tad")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx_stdlib/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio/fs")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/bsp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../examples/twrk64f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/CMSIS/Include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/hal/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/enet")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/mpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/uart")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/utilities/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../filesystem/mfs/source/mfs_sdcard/nio_esdhc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/osa/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/system/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../filesystem/mfs/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../security/wolfssl/source/wolfssl")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../security/wolfssl/source")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk64f120m.armgcc/release/rtcs)
ENDIF()


ADD_LIBRARY(lib_rtcs.a STATIC
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/phyksz8041/fsl_phy_driver.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/phyksz8041/fsl_phy_driver.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/fsl_enet_rtcs_adapter.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/port/fsl_enet_rtcs_adapter.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/dhcpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/dnscln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/echocln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/echosrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpc_cmd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpclnt.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_cmd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_cmd_file.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_cmd_transfer.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_msg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/ftpsrv_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv_script.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv_ws.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/httpsrv_ws_api.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/llmnrsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/modem.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/modem_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_base64.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_sha1.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_smtp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_ssl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_util.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/rtcs_utf8.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/sntp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/socknio.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetcln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetcln_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetcln_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetio.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetsrv_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/telnetsrv_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftp_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpcln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpcln_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpcln_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpsrv_supp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/apps/tftpsrv_task.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/arpcmd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/getaddrinfo.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/getnameinfo.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ifbootp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ifdhcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ifppp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ifstat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/iftraps.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/iftunnel.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/inet.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/inet_ntop.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/inet_pton.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ipcfg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ipcfg6.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/iwcfg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ping.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/ping6.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcs.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcs_hosts.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcs_if.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcscmd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcsdela.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcsglob.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcsinit.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcslog0.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcslog1.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/rtcsrand.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_close.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_dgram.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_options.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_select.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_stream.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/sock_util.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/soigmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/soip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/if/solink.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/arp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/arp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/arpif.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/asn1.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/bootp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ccp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/chap.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/checksum.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcp6.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcpcln6.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcpcln6_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcpsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dhcpuprv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dnscln.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/dns_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/echocln.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/fcs16.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ftpc.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ftpsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ftpsrv_msg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ftpsrv_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv_supp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv_ws_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv_script.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/httpsrv_ws.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/icmp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/icmp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/iwcfg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/llmnrsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rip_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_base64.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_sha1.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_user_config.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_utf8.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/igmp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip_if.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ipcfg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ipcfg_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip-e.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ipcp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ipradix.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/iptunnel.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/lcp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/md5.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/nat.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/nat_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/nat_session.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/natvers.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/pap.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ppp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ppp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/pppfsm.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ppphdlc.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rip.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/route.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_sock.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_ssl.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_err.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_stats.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_if.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_hosts.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_assert.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/echosrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_in.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_smtp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs_util.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcs25x.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcscfg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcslog.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcspcb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcsrtos.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcstime.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/rtcsvers.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/snmp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/snmpcfg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/socket.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tcp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tcp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tcpip.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/telnet.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/telnetcln.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/telnetcln_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/telnetsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/telnetsrv_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tftp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tftpcln.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tftpcln_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tftpsrv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/tftpsrv_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ticker.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/udp.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/udp_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/icmp6.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/icmp6_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip6.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip6_if.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip6_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/ip6_trace.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/nd6.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/mld.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/addrinfo.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media/arp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media/ip-e.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media/ipcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media/ipcpfsm.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/media/proxyarp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/alggen.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/dnat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/ifdnat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/ifnat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/nat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/nat_apply.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/natalg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/natdata.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/natevent.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/natftp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/nattftp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/nat/sonat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcsdate.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcsmsec.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcsmsg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcspart.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcssec.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/os/mqx/rtcstask.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/ccp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/chap.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/gen16.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/genmd5.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/lcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/md5.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/md5step.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/pap.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/pktrx.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/pkttx.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/ppp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/pppfsm.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/pppglob.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/ppp/ppphdlc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_arpdisp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_clock.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_echo.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_echosrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_enet.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ftpc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ftpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_gate.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_getname.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_getroute.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ghbn.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ipconfig.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_iwconfig.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_llmnrsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_nat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_nstat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ping.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_ppp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_rtcs.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_sendto.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_smtp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_snmpd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_telnetcln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_telnetsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_tftpcln.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/rtcs/sh_tftpsrv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/1213.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/mib.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/mqxmib.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/snmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/trap_receivers.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/snmp/traps.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/bootcomm.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/bootinit.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/bootp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/checksum.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/dhcpclnt.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/dhcputil.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/icmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/igmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/igmputil.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/ip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/ip_reasm.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/ip_if.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/iplocal.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/ipradix.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/iproute.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/iptunnel.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/iputil.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/rip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/route.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/rtcspcb.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcp_buf.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcp_clos.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcp_rcv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcp_send.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/tcpip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/ticker.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/timer.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/tcpip/udp.c"
)


IF(CMAKE_BUILD_TYPE MATCHES "debug")
    ADD_CUSTOM_COMMAND(TARGET lib_rtcs.a POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../bat/rtcs_twrk64f120m.sh ${CMAKE_CURRENT_SOURCE_DIR}/../../../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk64f120m.armgcc/debug/rtcs armgcc)
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES "release")
    ADD_CUSTOM_COMMAND(TARGET lib_rtcs.a POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../bat/rtcs_twrk64f120m.sh ${CMAKE_CURRENT_SOURCE_DIR}/../../../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk64f120m.armgcc/release/rtcs armgcc)
ENDIF()


