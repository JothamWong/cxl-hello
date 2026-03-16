#!/bin/bash

echo "disable=3" > /proc/mtrr
echo "base=0x8000000000 size=0x8000000000 type=uncachable" > /proc/mtrr