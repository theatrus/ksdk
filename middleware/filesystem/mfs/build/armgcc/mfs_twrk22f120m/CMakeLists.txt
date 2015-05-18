INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)

# this file was generated
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(mfs)


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
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DCPU_MK22FN512VDC12=1")
SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DFSL_RTOS_MQX=1")
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
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCPU_MK22FN512VDC12=1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFSL_RTOS_MQX=1")
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
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/sources/classes/msd")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/sources/classes/hub")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/adapter/sources")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/adapter/sources/sdk")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/include/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/cpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/common")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/tad")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx_stdlib/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio/fs")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/bsp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/board/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/mcu/MK22F51212")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../examples/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK22F51212/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK22F51212/startup")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/CMSIS/Include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/hal/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/mpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/uart")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/utilities/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/mfs_sdcard/nio_esdhc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/osa/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/system/inc")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk22f120m.armgcc/debug/mfs)
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
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/sources/classes/msd")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/sources/classes/hub")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/adapter/sources")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/adapter/sources/sdk")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../usb/usb_core/host/include/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/psp/cortex_m/cpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/common")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/tad")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx_stdlib/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/nio/fs")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/mqx/source/bsp")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/board/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/config/mcu/MK22F51212")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../examples/twrk22f120m")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK22F51212/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices/MK22F51212/startup")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/devices")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/CMSIS/Include")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/hal/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/mpu")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/drivers/src/uart")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/utilities/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../source/mfs_sdcard/nio_esdhc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/osa/inc")
    INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../platform/system/inc")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk22f120m.armgcc/release/mfs)
ENDIF()


ADD_LIBRARY(lib_mfs.a STATIC
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/mfs_cnfg.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/mfs_prv.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/mfs_rev.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/sh_mfs.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/mfs.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/include/part_mgr.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/mfs_usb/mfs_usb.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/mfs_usb/mfs_usb.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_cache.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_cd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_compare.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_copy.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_create.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_del.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_df.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_di.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_dir.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_dirent.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_disect.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_flush.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_format.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_mkdir.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_pwd.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_read.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_readtest.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_rename.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_rmdir.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_type.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_write.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/mfs/sh_writetest.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../rtos/mqx/nshell/source/include/sh_mfs.h"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/dir_read.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_attribute.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_cache.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_cluster.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_delete.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_dir_entry.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_dir_chain.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_directory.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_fat.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_find.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_format.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_flush.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_globals.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_handle.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_chain.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_ioctl.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_lfn.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_misc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_mount.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_open.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_parse.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_read.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_rename.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_rw.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_search.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_time.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_unicode.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_valdev.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_write.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/part_mgr.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../source/generic/mfs_nio.c"
)


IF(CMAKE_BUILD_TYPE MATCHES "debug")
    ADD_CUSTOM_COMMAND(TARGET lib_mfs.a POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../bat/mfs_twrk22f120m.sh ${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk22f120m.armgcc/debug/mfs armgcc)
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES "release")
    ADD_CUSTOM_COMMAND(TARGET lib_mfs.a POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../bat/mfs_twrk22f120m.sh ${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/twrk22f120m.armgcc/release/mfs armgcc)
ENDIF()


