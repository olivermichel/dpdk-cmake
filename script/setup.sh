#!/usr/bin/env bash

[ "$EUID" -ne 0 ] && echo >&2 "Please run as superuser" && exit 1;
[ -z "$RTE_SDK" ] && echo >&2 "RTE_SDK environment variable empty" && exit 1;
[ -z "$RTE_TARGET" ] && echo >&2 "RTE_TARGET environment variable empty" && exit 1;

echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
echo 0 > /proc/sys/kernel/randomize_va_space

modprobe uio
modprobe hwmon
insmod ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko
insmod ${RTE_SDK}/${RTE_TARGET}/kmod/igb_uio.ko
