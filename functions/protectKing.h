#include "../common.h"

#ifdef PTREGS_SYSCALL_STUBS
extern asmlinkage long (*ogWrite)(struct pt_regs *regs);
asmlinkage long hookWrite(struct pt_regs *regs);
extern asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long hookOpenAt(struct pt_regs *regs);
#else
extern asmlinkage long (*ogWrite)(unsigned int fd, const char __user *buf, size_t count);
asmlinkage long hookWrite(unsigned int fd, const char __user *buf, size_t count);
extern asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode);
#endif
