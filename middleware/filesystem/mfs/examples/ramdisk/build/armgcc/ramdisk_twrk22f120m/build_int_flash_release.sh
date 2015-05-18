#!/usr/bin/env bash
(cd  ../../../../../../../../rtos/mqx/mqx/build/armgcc/mqx_twrk22f120m && source build_release.sh nopause) 
(cd  ../../../../../build/armgcc/mfs_twrk22f120m && source build_release.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/nshell/build/armgcc/nshell_twrk22f120m && source build_release.sh nopause) 
(cd  ../../../../../../../../rtos/mqx/mqx_stdlib/build/armgcc/mqx_stdlib_twrk22f120m && source build_release.sh nopause) 
(cd  ../../../../../../../../lib/ksdk_mqx_lib/armgcc/K22F51212 && source build_release.sh nopause) 
cmake -DCMAKE_TOOLCHAIN_FILE=armgcc.cmake -DCMAKE_BUILD_TYPE="int flash release" -G "Unix Makefiles" 
make all 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
