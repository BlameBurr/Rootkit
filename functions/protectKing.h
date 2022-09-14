#include "../common.h"

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*ogWrite)(struct pt_regs *regs);
asmlinkage long hookWrite(struct pt_regs *regs);
static asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long hookOpenAt(struct pt_regs *regs);
#else
static asmlinkage long (*ogWrite)(unsigned int fd, const char __user *buf, size_t count);
asmlinkage long hookWrite(unsigned int fd, const char __user *buf, size_t count);
static asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode);
#endif