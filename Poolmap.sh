#!/bin/bash

echo "Turn off cxl switch" 
bmc_fc3 apollo_power_control off
sleep 5
echo "Turn on cxl switch"
bmc_fc3 apollo_power_control on
sleep 5
echo "Call xconn-control probe"
xconn-control probe

# --- PORT 8 (G1) CONFIGURATION ---
echo "Set up Downstream Port 18 to Endpoint port 8 (G1) - 1TB Aligned"
xconn-control poolmap 2  128 0 8    0x10000000000 0 
xconn-control poolmap 18 128 0 8    0x12000000000 1
xconn-control poolmap 4  128 0 8    0x14000000000 2 
xconn-control poolmap 20 128 0 8    0x16000000000 3 
xconn-control poolmap 5  128 0 8    0x18000000000 4
xconn-control poolmap 21 128 0 8    0x1A000000000 5 
xconn-control poolmap 6  128 0 8    0x1C000000000 6
xconn-control poolmap 22 128 0 8    0x1E000000000 7
xconn-control sethdmsize 8 1024 

# --- PORT 9 (G2) CONFIGURATION ---
echo "Set up Downstream Port 18 to Endpoint port 9 (G2) - 1TB Aligned"
xconn-control poolmap 2  128 0 9    0x10000000000 0 
xconn-control poolmap 18 128 0 9    0x12000000000 1
xconn-control poolmap 4  128 0 9    0x14000000000 2 
xconn-control poolmap 20 128 0 9    0x16000000000 3 
xconn-control poolmap 5  128 0 9    0x18000000000 4
xconn-control poolmap 21 128 0 9    0x1A000000000 5 
xconn-control poolmap 6  128 0 9    0x1C000000000 6
xconn-control poolmap 22 128 0 9    0x1E000000000 7
xconn-control sethdmsize 9 1024 

# --- PORT 10 (G3) CONFIGURATION ---
echo "Set up Downstream Port 2 to Endpoint port 10 (G3) - 1TB Aligned"
xconn-control poolmap 2  128 0 10   0x10000000000 0 
xconn-control poolmap 18 128 0 10   0x12000000000 1
xconn-control poolmap 4  128 0 10   0x14000000000 2 
xconn-control poolmap 20 128 0 10   0x16000000000 3 
xconn-control poolmap 5  128 0 10   0x18000000000 4
xconn-control poolmap 21 128 0 10   0x1A000000000 5 
xconn-control poolmap 6  128 0 10   0x1C000000000 6
xconn-control poolmap 22 128 0 10   0x1E000000000 7
xconn-control sethdmsize 10 1024 

# --- PORT 11 (G4) CONFIGURATION ---
echo "Set up Downstream Port 2 to Endpoint port 11 (G4) - 1TB Aligned"
xconn-control poolmap 2  128 0 11   0x10000000000 0 
xconn-control poolmap 18 128 0 11   0x12000000000 1
xconn-control poolmap 4  128 0 11   0x14000000000 2 
xconn-control poolmap 20 128 0 11   0x16000000000 3 
xconn-control poolmap 5  128 0 11   0x18000000000 4
xconn-control poolmap 21 128 0 11   0x1A000000000 5 
xconn-control poolmap 6  128 0 11   0x1C000000000 6
xconn-control poolmap 22 128 0 11   0x1E000000000 7
xconn-control sethdmsize 11 1024 