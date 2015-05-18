#!/usr/bin/env bash
(./build_int_flash_debug.sh nopause) 
(./build_int_flash_release.sh nopause) 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
