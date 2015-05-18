#!/usr/bin/env bash
rm -rf CMakeFiles Makefile cmake_install.cmake CMakeCache.txt *.map
rm -rf "int flash debug" 
rm -rf "int flash release" 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
