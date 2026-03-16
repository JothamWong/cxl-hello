#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/set_memory.h>

static unsigned long phys_start = 0;
static unsigned long phys_size  = 0;

module_param(phys_start, ulong, 0444);
MODULE_PARM_DESC(phys_start, "Physical start address (e.g. 0x4080000000)");

module_param(phys_size, ulong, 0444);
MODULE_PARM_DESC(phys_size, "Size of region in bytes (e.g. 0x800000000)");

static void __iomem *mapped_region;

static int __init uc_map_init(void)
{
    unsigned long n_pages;

    if (!phys_start || !phys_size) {
        pr_err("uc_map: phys_start and phys_size are required\n");
        return -EINVAL;
    }

    if (!PAGE_ALIGNED(phys_start) || !PAGE_ALIGNED(phys_size)) {
        pr_err("uc_map: phys_start and phys_size must be page-aligned (multiple of 0x%lx)\n",
               PAGE_SIZE);
        return -EINVAL;
    }

    n_pages = phys_size >> PAGE_SHIFT;

    mapped_region = ioremap(phys_start, phys_size);
    if (!mapped_region) {
        pr_err("uc_map: ioremap failed for [%016lx - %016lx]\n",
               phys_start, phys_start + phys_size - 1);
        return -ENOMEM;
    }

    if (set_memory_uc((unsigned long)mapped_region, n_pages)) {
        pr_err("uc_map: set_memory_uc failed\n");
        iounmap(mapped_region);
        return -EIO;
    }

    pr_info("uc_map: mapped phys [%016lx - %016lx] (%lu MB) as UC at vaddr %px\n",
            phys_start, phys_start + phys_size - 1,
            phys_size >> 20, mapped_region);
    return 0;
}

static void __exit uc_map_exit(void)
{
    unsigned long n_pages = phys_size >> PAGE_SHIFT;

    set_memory_wb((unsigned long)mapped_region, n_pages);
    iounmap(mapped_region);
    pr_info("uc_map: unmapped [%016lx - %016lx] and restored to WB\n",
            phys_start, phys_start + phys_size - 1);
}

module_init(uc_map_init);
module_exit(uc_map_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Map a physical region as UC via PAT");