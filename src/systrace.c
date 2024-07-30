#include "syscalls.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t pid;

void handle_error(const char *message) {
  fprintf(stderr, "%s: %s\n", message, strerror(errno));
  exit(EXIT_FAILURE);
}

void print_syscall(long syscall_number, const struct user_regs_struct *regs) {
  fprintf(stderr, "%s(%ld, %ld, %ld, %ld, %ld, %ld, %ld)",
          syscall_names[syscall_number], syscall_number, (long)regs->rdi,
          (long)regs->rsi, (long)regs->rdx, (long)regs->r10, (long)regs->r8,
          (long)regs->r9);
}

void print_content(long syscall_content, const struct user_regs_struct *regs) {
  if (syscall_content < 0) {
    fprintf(stderr, " = %ld %s\n", syscall_content, strerror(-syscall_content));
  } else {
    fprintf(stderr, " = %ld\n", syscall_content);
  }
}

void handle_signal(int signal) {
  if (signal == SIGINT) {
    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
      fprintf(stderr, "systrace: Process %d detached.\n", pid);
    } else {
      perror("ptrace detach");
    }
    exit(EXIT_SUCCESS);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
    return 1;
  }

  pid = atoi(argv[1]);

  // Set up signal handler for SIGINT
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa)); // Ensure sa is zeroed out
  sa.sa_handler = handle_signal;
  sa.sa_flags = 0; // or SA_RESTART if desired
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1)
    handle_error("ptrace attach");

  fprintf(stderr, "systrace: Process %d attached.\n", pid);

  int status;
  struct user_regs_struct regs;

  if (waitpid(pid, &status, 0) == -1)
    handle_error("waitpid");

  // Monitor syscalls
  while (1) {
    // Enter next system call
    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1)
      handle_error("ptrace syscall");

    // Wait for the next event from the child process
    if (waitpid(pid, &status, 0) == -1)
      handle_error("waitpid");

    // Check if the process has exited
    if (WIFEXITED(status))
      break;

    // Check if the process has stopped because of a syscall
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
      // Get syscall information
      if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1)
        handle_error("ptrace getregs");

      print_syscall((long)regs.orig_rax, &regs);

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
      print_content((long)regs.rax, &regs);
    }
  }

  // Detach from the process
  if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1)
    handle_error("ptrace detach");

  return 0;
}
