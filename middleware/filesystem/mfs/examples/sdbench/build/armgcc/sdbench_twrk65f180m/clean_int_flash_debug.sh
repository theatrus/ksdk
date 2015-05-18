#!/usr/bin/env bash
make clean 
if [ "${1}" != "nopause" ]; then
read -p "Press any key to continue... " -n1 -s
fi
