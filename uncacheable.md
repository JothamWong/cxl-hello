# notes for uncacheable

There seems to be 2 approaches to setting uncacheability:

1. mtrr
2. pat

mtrr approach is very hard, the starting PA and the size need to meet the mtrr requirements. I tried with the xconn switch poolmap, but the bios seems to map the pa contiguously after the node 1 system ram so tough luck

pat approach seems more feasible. i think the kernel module devdax_uc should work fine


The dax driver itself needs to be unbinded before doing the PAT remapping with devdax_uc
I noticed that there were 2 entries for the PAT entries with the module, and 1 after
I suspect that the bricking is probably because of aliasing rules


## Current Steps

0. Find the address of dax

This can be done with `grep dax /proc/iomem`

1. Unbind dax driver

`echo dax0.0 > /sys/bus/dax/drivers/device_dax/unbind`

2. Run the kmodule

See the uc readme

3. Run the pat fiter and dmesg | tail to verify for no errors

Right now I see 1 entry with the kmod alone

`python3 pat_filter.py 0x4080000000 0x1407fffffff`
`dmesg | tail`

Should see the following:

`PAT: [mem 0x0000004080000000-0x0000014080000000] uncached-minus`

4. Write to /dev/mem and read from /dev/mem now

Map to /dev/mem instead of /dev/dax0.0

<!-- 
4. Bind dax driver back

Now should see 2 entries in PAT

```
root@pxe:~/jotham/cxl-hello# python3 pat_filter.py 0x4080000000 0x1407fffffff
PAT: [mem 0x0000004080000000-0x0000014080000000] uncached-minus
PAT: [mem 0x0000004080000000-0x0000014080000000] uncached-minus
```

Now the thing is idk if the 2 entries bricks the device or not.

From https://www.landley.net/kdocs/ols/2008/ols2008v2-pages-135-144.pdf it seems like having overlapping is actually undefined behavior of sorts. -->