# Description

Trace system calls of a given pid with ptrace(2) and waitpid(1).

# Architecture

```mermaid
graph TD
    Start([Start Program]) --> CheckArgs{Check if<br>arguments are valid}
    CheckArgs -- Invalid --> ExitInvalidArgs([Exit with usage message])
    CheckArgs -- Valid --> ParsePID[Parse PID from arguments]

    ParsePID --> SetupSignalHandler[Setup SIGINT signal handler]
    SetupSignalHandler --> AttachProcess[Attach to the process using ptrace]
    AttachProcess -- Failed --> ExitAttachError([Exit with error])

    AttachProcess -- Success --> WaitInitialStop[Wait for the process to stop]
    WaitInitialStop -- Failed --> ExitWaitError([Exit with error])
    WaitInitialStop -- Success --> MonitorSyscalls[Start monitoring syscalls]

    subgraph SyscallMonitoring[Syscall Monitoring Loop]
        MonitorSyscalls --> EnterSyscall[Request process to enter next syscall]
        EnterSyscall -- Failed --> ExitSyscallError([Exit with error])

        EnterSyscall -- Success --> WaitSyscallStop[Wait for syscall event]
        WaitSyscallStop -- Failed --> ExitWaitError

        WaitSyscallStop -- ProcessExited --> Break[Break the loop]
        WaitSyscallStop -- SyscallStop --> GetSyscallInfo[Get syscall info with ptrace]

        GetSyscallInfo -- Failed --> HandleSyscallError[Log error and exit if necessary]
        GetSyscallInfo -- Success --> PrintSyscallInfo[Print syscall name and arguments]

        PrintSyscallInfo --> ContinueSyscall[Request process to continue]
        ContinueSyscall -- Failed --> ExitContinueError([Exit with error])
        
        ContinueSyscall --> WaitSyscallStop2[Wait for syscall result]
        WaitSyscallStop2 -- Failed --> ExitWaitError
        WaitSyscallStop2 -- Success --> GetSyscallResult[Get syscall result with ptrace]
        GetSyscallResult -- Failed --> LogSyscallError[Log error and continue]

        GetSyscallResult -- Success --> PrintSyscallResult[Print syscall result]
        PrintSyscallResult --> EnterSyscall
    end

    MonitorSyscalls -- ProcessExited --> DetachProcess[Detach process using ptrace]
    DetachProcess -- Failed --> ExitDetachError([Exit with error])
    DetachProcess -- Success --> ExitSuccess([Exit Successfully])

    HandleSignal([Handle SIGINT]) --> DetachSignalProcess[Detach process using ptrace]
    DetachSignalProcess --> ExitOnSignal([Exit on signal])
```

# Build

```bash
Usage: make <target>
Targets:
  all      Build the release version of systrace
  debug    Build the debug version of systrace_debug
  format   Format source files using clang-format
  clean    Remove generated binaries
  fast     Clean, format, and build the release version
```

# Usage

```bash
Usage: ./systrace <pid>
```
