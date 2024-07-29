#include "syscalls.h"
#include <asm/unistd_64.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    struct user_regs_struct regs;
    int status;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid = atoi(argv[1]);

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach");
        return 1;
    }

    waitpid(pid, &status, 0);

    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        perror("ptrace getregs");
        return 1;
    }

    // Retrieve and print syscall name
    long syscall_number = regs.orig_rax;
    if (syscall_number >= 0 && syscall_number < sizeof(syscall_names) / sizeof(syscall_names[0])) {
        printf("%s (%ld)\n", syscall_names[syscall_number], syscall_number);
    } else {
        printf("Unknown system call number: %ld\n", syscall_number);
    }

    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
        perror("ptrace detach");
        return 1;
    }

    return 0;
}
