#!/bin/bash

# Script to automate all steps

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <phys_start> <phys_size>"
    echo "Example: $0 0x4080000000 0x8000000000"
    exit 1
fi

PHYS_START=$1
PHYS_SIZE=$2

echo "unbinding dax0.0"
echo dax0.0 > /sys/bus/dax/drivers/device_dax/unbind

echo "Set up uc module"
pushd uc
make
insmod devdax_uc.ko phys_start="$PHYS_START" phys_size="$PHYS_SIZE"
popd