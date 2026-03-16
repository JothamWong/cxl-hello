# UC kmod

make
sudo insmod devdax_uc phys_start=0x4080000000 phys_size=0x800000000
sudo rmmod devdax_uc
make clean