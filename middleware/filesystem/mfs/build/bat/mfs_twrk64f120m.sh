#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"

if [ "${TOOL}" = "armgcc" ]; then
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/."
cp -f "${ROOTDIR}/source/include/mfs_cnfg.h" "${OUTPUTDIR}/mfs_cnfg.h"
cp -f "${ROOTDIR}/source/include/mfs_rev.h" "${OUTPUTDIR}/mfs_rev.h"
cp -f "${ROOTDIR}/source/include/sh_mfs.h" "${OUTPUTDIR}/sh_mfs.h"
cp -f "${ROOTDIR}/source/include/mfs.h" "${OUTPUTDIR}/mfs.h"
cp -f "${ROOTDIR}/source/include/part_mgr.h" "${OUTPUTDIR}/part_mgr.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_esdhc/esdhc.h" "${OUTPUTDIR}/esdhc.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_sdcard/sdcard_esdhc/sdcard_esdhc.h" "${OUTPUTDIR}/sdcard_esdhc.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_sdcard/sdcard.h" "${OUTPUTDIR}/sdcard.h"
cp -f "${ROOTDIR}/source/mfs_usb/mfs_usb.h" "${OUTPUTDIR}/mfs_usb.h"
cp -f "${ROOTDIR}/../../../rtos/mqx/nshell/source/include/sh_mfs.h" "${OUTPUTDIR}/sh_mfs.h"
:
fi


if [ "${TOOL}" = "kds" ]; then
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/."
cp -f "${ROOTDIR}/source/include/mfs_cnfg.h" "${OUTPUTDIR}/mfs_cnfg.h"
cp -f "${ROOTDIR}/source/include/mfs_rev.h" "${OUTPUTDIR}/mfs_rev.h"
cp -f "${ROOTDIR}/source/include/sh_mfs.h" "${OUTPUTDIR}/sh_mfs.h"
cp -f "${ROOTDIR}/source/include/mfs.h" "${OUTPUTDIR}/mfs.h"
cp -f "${ROOTDIR}/source/include/part_mgr.h" "${OUTPUTDIR}/part_mgr.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_esdhc/esdhc.h" "${OUTPUTDIR}/esdhc.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_sdcard/sdcard_esdhc/sdcard_esdhc.h" "${OUTPUTDIR}/sdcard_esdhc.h"
cp -f "${ROOTDIR}/source/mfs_sdcard/nio_sdcard/sdcard.h" "${OUTPUTDIR}/sdcard.h"
cp -f "${ROOTDIR}/source/mfs_usb/mfs_usb.h" "${OUTPUTDIR}/mfs_usb.h"
cp -f "${ROOTDIR}/../../../rtos/mqx/nshell/source/include/sh_mfs.h" "${OUTPUTDIR}/sh_mfs.h"
:
fi




