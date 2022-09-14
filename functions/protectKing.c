#include "protectKing.h"

unsigned int targetFD = 0;
unsigned int targetPID = 0;

static char *duplicate_filename(const char __user *filename)
{
	char *kernel_filename;
	kernel_filename = kzalloc(4096, GFP_KERNEL);
	if (!kernel_filename) return NULL;

	if (strncpy_from_user(kernel_filename, filename, 4096) < 0) {
		kfree(kernel_filename);
		return NULL;
	}
	return kernel_filename;
}

#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long hookWrite(struct pt_regs *regs) {
	long ret;
	struct task_struct *task;
	task = current;
	int signum = SIGKILL;
	if (task->pid == targetPID)
	{
		if (regs->di == targetFD)
		{
			printk(KERN_INFO, "PID: %d tried to write to target file.\n", task->pid);
			struct kernel_siginfo info;
			memset(&info, 0, sizeof(struct kernel_siginfo));
			info.si_signo = signum;
			int ret = send_sig_info(signum, &info, task);
					if (ret < 0) printk(KERN_INFO, KERN_INFO "Error sending SIGKILL\n");
					else {
						printk(KERN_INFO, KERN_INFO "Sent SIGKILL successfully\n");
						return 0;
					}
		}
	}

	ret = ogWrite(regs);

	return ret;
}
#else
asmlinkage long hookWrite(unsigned int fd, const char __user *buf, size_t count) {
    long ret;
	struct task_struct *task;
	task = current;
	int signum = SIGKILL;
	if (task->pid == targetPID)
	{
		if (fd == targetFD)
		{
			printk(KERN_INFO, "PID: %d tried to write to target file.\n", task->pid);
			struct kernel_siginfo info;
			memset(&info, 0, sizeof(struct kernel_siginfo));
			info.si_signo = signum;
			int ret = send_sig_info(signum, &info, task);
					if (ret < 0) printk(KERN_INFO, KERN_INFO "Error sending SIGKILL\n");
					else {
						printk(KERN_INFO, KERN_INFO "Sent SIGKILL successfully\n");
						return 0;
					}
		}
	}

	ret = ogWrite(fd, buf, count);

	return ret;
}
#endif
#ifdef PTREGS_SYSCALL_STUBS
asmlinkage long hookOpenAt(struct pt_regs *regs) {
    long ret;
	char *buf;
	struct task_struct *task;
	task = current;

	buf = duplicate_filename((void*) regs->si);
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
	{
		printk(KERN_INFO "PID: %d opened the file\n", task->pid);
		kfree(buf);
		ret = ogOpenAt(regs);
		printk(KERN_INFO "FD: %ld\n", ret);
		targetFD = ret;
		targetPID = task->pid;
		return ret;
	}

	kfree(buf);
	ret = ogOpenAt(regs);
	return ret;
}
#else
asmlinkage long hookOpenAt(int dfd, const char __user *filename, int flags, umode_t mode){
    long ret;
	char *buf;
	struct task_struct *task;
	task = current;

	buf = duplicate_filename(filename);
	if (strncmp(buf, "/tmp/test.txt", 13) == 0)
	{
		printk(KERN_INFO "PID: %d opened the file\n", task->pid);
		kfree(buf);
		ret = ogOpenAt(dfd, filename, flags, mode);
		printk(KERN_INFO "FD: %ld\n", ret);
		targetFD = ret;
		targetPID = task->pid;
		return ret;
	}

	kfree(buf);
	ret = ogOpenAt(filename, flags, mode);
	return ret;
}
#endif
