#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <asm/set_memory.h> // Required for set_memory_uc / set_memory_wb

static unsigned long phys_start = 0;
static unsigned long phys_size  = 0;

module_param(phys_start, ulong, 0444);
MODULE_PARM_DESC(phys_start, "Physical start address (e.g. 0x4080000000)");

module_param(phys_size, ulong, 0444);
MODULE_PARM_DESC(phys_size, "Size of region in bytes (e.g. 0x10000000000)");

static int __init uc_map_init(void)
{
    unsigned long num_pages;
    void *vaddr;
    int ret;

    if (!phys_start || !phys_size) {
        pr_err("uc_map: phys_start and phys_size are required\n");
        return -EINVAL;
    }

    if (!PAGE_ALIGNED(phys_start) || !PAGE_ALIGNED(phys_size)) {
        pr_err("uc_map: phys_start and phys_size must be page-aligned\n");
        return -EINVAL;
    }

    vaddr = __va(phys_start);
    num_pages = phys_size >> PAGE_SHIFT;

    ret = set_memory_uc((unsigned long)vaddr, num_pages);
    if (ret) {
        pr_err("uc_map: set_memory_uc failed with error %d\n", ret);
        return ret;
    }

    pr_info("uc_map: Successfully changed [%016lx - %016lx] (%lu MB) to UC\n",
            phys_start, phys_start + phys_size - 1, phys_size >> 20);
            
    return 0;
}

static void __exit uc_map_exit(void)
{
    unsigned long num_pages = phys_size >> PAGE_SHIFT;
    void *vaddr = __va(phys_start);
    int ret;

    ret = set_memory_wb((unsigned long)vaddr, num_pages);
    if (ret) {
        pr_err("uc_map: WARNING - Failed to restore memory to WB (%d)\n", ret);
    } else {
        pr_info("uc_map: Restored region to WB and exiting\n");
    }
}

module_init(uc_map_init);
module_exit(uc_map_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Map a physical region as UC via PAT");