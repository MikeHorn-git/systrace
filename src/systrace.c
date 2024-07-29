#include "syscalls.h"
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_error(const char *message)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void print_syscall(long syscall_number)
{
    if (syscall_number >= 0 && syscall_number < sizeof(syscall_names) / sizeof(syscall_names[0]))
        printf("Syscall: %s (%ld)\n", syscall_names[syscall_number], syscall_number);
    else
        printf("Unknown syscall number: %ld\n", syscall_number);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);

    // Attach to the target process
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1)
        handle_error("ptrace attach");

    int status;
    struct user_regs_struct regs;

    // Wait for the process to stop
    if (waitpid(pid, &status, 0) == -1)
        handle_error("waitpid");

    // Optionally set options
    if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL) == -1)
        handle_error("ptrace setoptions");

    // Monitor syscalls
    while (1)
    {
        // Enter next system call
        if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1)
            handle_error("ptrace syscall");

        if (waitpid(pid, &status, 0) == -1)
            handle_error("waitpid");

        // Check if the process has exited
        if (WIFEXITED(status))
            break;

        // Check if the process has stopped because of a syscall
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
        {
            // Get syscall information
            if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1)
                handle_error("ptrace getregs");

            long syscall_number = regs.orig_rax;
            print_syscall(syscall_number);

            // Print system call arguments
            fprintf(stderr, "%ld(%ld, %ld, %ld, %ld, %ld, %ld)",
                    syscall_number,
                    (long)regs.rdi, (long)regs.rsi, (long)regs.rdx,
                    (long)regs.r10, (long)regs.r8, (long)regs.r9);

            // Continue to the next syscall
            if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1)
                handle_error("ptrace syscall");

            if (waitpid(pid, &status, 0) == -1)
                handle_error("waitpid");

            // Get syscall result
            if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
                fputs(" = ?\n", stderr);
                if (errno == ESRCH)
                    exit(regs.rdi); // system call was _exit(2) or similar
                handle_error("ptrace getregs");
            }

            // Print system call result
            fprintf(stderr, " = %ld\n", (long)regs.rax);
        }
    }

    // Detach from the process
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1)
        handle_error("ptrace detach");

    return 0;
}
