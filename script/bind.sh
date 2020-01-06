#!/usr/bin/env bash

[ "$EUID" -ne 0 ] && echo >&2 "Please run as superuser" && exit 1;
[ -z "$RTE_SDK" ] && echo >&2 "RTE_SDK environment variable empty" && exit 1;
[ -z $1 ] && echo >&2 "usage: bind.sh <device-name>" && exit 1;

ifconfig $1 down
${RTE_SDK}/usertools/dpdk-devbind.py --bind=igb_uio $1
