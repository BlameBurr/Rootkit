#include "../common.h"
char *getcwd(struct task_struct *task);
#ifdef PTREGS_SYSCALL_STUBS
extern asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long hookOpenAt(struct pt_regs *regs);
#else
extern asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode);
#endif
