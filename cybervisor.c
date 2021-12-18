#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/cpufeature.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yair Koskas");
MODULE_DESCRIPTION("Hypervisor from scratch, for fun n education");

#define CPUID_FEAT_ECX_VMX 1 << 5
#define CR4_ENABLE_VMX 13
#define CR4_VMXE _BITUL(CR4_ENABLE_VMX)

int vmxSupport(void) {
    int cpuid = 0;
	__asm__(
		"mov $1, %%eax;"
		"cpuid;"
		"mov %%ecx, %0;" : "=r" (cpuid) : : "rax"
	);
	
    // vmx support lies in the fifth bit of cpuid now
    return cpuid | CPUID_FEAT_ECX_VMX;

}

int getVMXOperation(void) {
	unsigned long cr4;
	__asm__ __volatile__("mov %%cr4, %0", "=r" (cr4) : : "memory");
	cr4 |= CR4_VMXE;
	__asm__ __volatile__("mov %0, %%cr4", : : "r" (cr4) : "memory");
}

int __init visor_start(void) {
	if (!vmxSupport()) {
        printk(KERN_INFO "VMX support wasn't detected, Exiting\n");
        return 0;
    } else {
    	printk(KERN_INFO "VMX support detected, Loading Cybervisor\n");
	}

    return 0;
}

static void __exit visor_exit(void) {
	printk(KERN_INFO "Unloading Cybervisor\n");
}

module_init(visor_start);
module_exit(visor_exit);
