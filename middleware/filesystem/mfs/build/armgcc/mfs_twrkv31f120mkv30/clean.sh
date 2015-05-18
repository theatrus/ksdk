#!/usr/bin/env bash
rm -rf CMakeFiles Makefile cmake_install.cmake CMakeCache.txt *.map
rm -rf "debug" 
rm -rf "release" 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
