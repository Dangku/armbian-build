#!/bin/bash

eval "kernel_args=($(cat /proc/cmdline))"
for arg in "${kernel_args[@]}"; do
    #echo "${arg}"
    case "${arg}" in
	    board=*)
		#echo "${arg#board=}"
		BOARD=${arg#board=}
		;;
    esac
done

echo board=$BOARD
export BOARD=$BOARD
