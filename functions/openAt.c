#include "openAt.h"

short closeProcess = 0;

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long (*ogClose)(unsigned int fd);
asmlinkage long hookOpenAt(struct pt_regs *regs) {
	char *buf = kzalloc(4096, GFP_KERNEL);
	long ret = 0;
	int flags = regs->dx;
	if (!buf) return ret;

	if ((strncpy_from_user(buf, (char *)regs->si, 4096) > 0) && (strncmp(buf, "/tmp/test.txt", 13) == 0))
		if ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR || (flags & O_APPEND) == O_APPEND) ret = -1;

	kfree(buf);
	
	if ((ret == -1) && (closeProcess == 1)) ogClose(0);
	if (ret == 0) ret = ogOpenAt(regs);

	return ret;
}
#else
asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long (*ogClose)(unsigned int fd);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode){
	char *buf = kzalloc(4096, GFP_KERNEL);
	long ret = 0;
	if (!buf) return ret;

	if ((strncpy_from_user(buf, filename, 4096) > 0) && (strncmp(buf, "/tmp/test.txt", 13) == 0))
		if ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR || (flags & O_APPEND) == O_APPEND) ret = -1;

	kfree(buf);
	
	if ((ret == -1) && (closeProcess == 1)) ogClose(0);
	if (ret == 0) ret = ogOpenAt(dfd, filename, flags, mode);

	return ret;
}
#endif
