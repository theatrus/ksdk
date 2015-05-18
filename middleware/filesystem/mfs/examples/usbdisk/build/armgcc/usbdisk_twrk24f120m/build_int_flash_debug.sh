#!/usr/bin/env bash
(cd  ../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk24f120m && source build_debug.sh nopause) 
(cd  ../../../../../build/armgcc/mfs_twrk24f120m && source build_debug.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk24f120m && source build_debug.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk24f120m && source build_debug.sh nopause) 
(cd  ../../../../../../../../lib/ksdk_mqx_lib/armgcc/K24F25612 && source build_debug.sh nopause) 
(cd  ../../../../../../../../usb/usb_core/host/build/armgcc/usbh_sdk_twrk24f120m_mqx && source build_debug.sh nopause) 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash debug" -G "Unix Makefiles" 
make all 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
