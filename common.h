#include <linux/init.h>
#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/syscalls.h>
#include <linux/namei.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/signal.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/sched/signal.h>

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif
