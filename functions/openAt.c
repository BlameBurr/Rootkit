#include "openAt.h"

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long hookOpenAt(struct pt_regs *regs) {
	char *buf = kzalloc(4096, GFP_KERNEL);
	long ret = ogOpenAt(regs);
	int flags = regs->dx;
	if (!buf) return 0;

	if (strncpy_from_user(buf, (char *)regs->si, 4096) < 0) {
		kfree(buf);
		return 0;
	}
	
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
		if ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR || (flags & O_APPEND) == O_APPEND) ret = -1;

	kfree(buf);
	return ret;
}
#else
asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode){
	char *buf = kzalloc(4096, GFP_KERNEL);
	long ret = ogOpenAt(dfd, filename, flags, mode);
	if (!buf) return 0;

	if (strncpy_from_user(buf, filename, 4096) < 0) {
		kfree(buf);
		return 0;
	}
	
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
		if ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR || (flags & O_APPEND) == O_APPEND) ret = -1;

	kfree(buf);
	return ret;
}
#endif
