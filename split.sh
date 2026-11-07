#!/bin/bash
sudo daxctl disable-device dax0.0
sudo daxctl reconfigure-device -s 256G dax0.0
sudo daxctl enable-device dax0.0
sudo daxctl create-device --region=0 --size=256G --align=2M
sudo daxctl reconfigure-device --mode=system-ram dax0.1 --force
sudo daxctl list -u
