/*
    * Helper Library for ftrace
    * Originally written by: Harvey Phillips
    * Rewritten by: James Walker
    * License: GPL
*/

//Includes
#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "common.h"

//Some kernel version shims
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))
#include <linux/kprobes.h>
#define KPROBE_LOOKUP 1
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0))
#define FTRACE_REGS_REC 1
#endif

//Main functions and definitions
#define USE_FENTRY_OFFSET 0 //Prevent recursive loop by detection of return address (0) or by jumping over ftrace call (1)

#ifdef PTREGS_SYSCALL_STUBS
#define SYSCALL_NAME(name) ("__x64_" name)
#else
#define SYSCALL_NAME(name) (name)
#endif

#define HOOK(_name, _hook, _orig)   \
{                                   \
    .name = SYSCALL_NAME(_name),    \
    .function = (_hook),            \
    .original = (_orig),            \
}

#if !USE_FENTRY_OFFSET
#pragma GCC optimize("-fno-optimize-sibling-calls")
#define ADDRESS(addr) (addr);
#else
#define ADDRESS(addr) (addr+MCOUNT_ISN_SIZE)
#endif

#ifdef FTRACE_REGS_REC
#define FTRACE_OPS_FL_RECURSION_SAFE FTRACE_OPS_FL_RECURSION
#else
#define ftrace_regs pt_regs
static __always_inline struct pt_regs *ftrace_get_regs(struct ftrace_regs *regs) {
    return regs;
}
#endif

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

static int fh_resolve_hook_address(struct ftrace_hook *hook) {
    #ifdef KPROBE_LOOKUP
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    #endif
    hook->address = kallsyms_lookup_name(hook->name);
    if (!hook->address) {
        printk(KERN_DEBUG "fh_resolve_hook_address failed to resolve symbol: %s\n", hook->name);
        return -ENOENT;
    }

    *((unsigned long*) hook->original) = ADDRESS(hook->address);
    return 0;
};

static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct ftrace_regs *fregs) {
    struct pt_regs *regs = ftrace_get_regs(fregs);
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);
    #if !USE_FENTRY_OFFSET
    if (!within_module(parent_ip, THIS_MODULE))
        regs->ip = (unsigned long) hook->function;
    #else
    regs->ip = (unsigned long) hook->function;
    #endif
};

int fh_install_hook(struct ftrace_hook *hook) {
    int err = fh_resolve_hook_address(hook);
    if (err) return err;

    hook->ops.func = fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
        | FTRACE_OPS_FL_RECURSION |FTRACE_OPS_FL_IPMODIFY;
    
    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err) {
        printk(KERN_DEBUG "ftrace_set_filter_ip failed: %d\n", err);
        return err;
    }

    err = register_ftrace_function(&hook->ops);
    if (err) {
        printk(KERN_DEBUG "register_ftrace_function failed: %d\n", err);
        return err;
    }
    return 0;
};

void fh_remove_hook(struct ftrace_hook *hook) {
    int err = unregister_ftrace_function(&hook->ops);
    if (err) printk(KERN_DEBUG "unregister_ftrace_function failed: %d\n", err);
    
    err = ftrace_set_filter_ip(&hook->ops, hook->address, 1,0);
    if (err) printk(KERN_DEBUG "ftrace_set_filter_ip failed: %d\n", err);
};

int fh_install_hooks(struct ftrace_hook *hooks, size_t count) {
    int err;
    size_t i;

    for (i = 0; i < count; i++) {
        err = fh_install_hook(&hooks[i]);
        if (err) goto error;
    };

    return 0;

    error:
    while (i != 0) {
        fh_remove_hook(&hooks[--i]);
    }
    return err;
};

void fh_remove_hooks(struct ftrace_hook *hooks, size_t count) {
    size_t i;
    for (i = 0; i < count; i++)
        fh_remove_hook(&hooks[i]);
};
