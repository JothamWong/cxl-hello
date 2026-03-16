#!/bin/bash

echo "disable=3" > /proc/mtrr
echo "base=0x4000000000 size=0x4000000000 type=write-back" > /proc/mtrr