#!/usr/bin/env bash
(cd  ../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk60d100m && source build_debug.sh nopause) 
(cd  ../../../../../../../filesystem/mfs/build/armgcc/mfs_twrk60d100m && source build_debug.sh nopause) 
(cd  ../../../../../build/armgcc/rtcs_twrk60d100m && source build_debug.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk60d100m && source build_debug.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk60d100m && source build_debug.sh nopause) 
(cd  ../../../../../../../../lib/ksdk_mqx_lib/armgcc/K60D10 && source build_debug.sh nopause) 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash debug" -G "Unix Makefiles" 
make all 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
