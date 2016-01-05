#!/usr/bin/env bash
(./build_debug.sh nopause) 
(./build_release.sh nopause) 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
