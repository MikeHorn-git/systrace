# Description
Trace system calls of a given pid with ptrace(2) and waitpid(1).

# Installation
```bash
git clone https://github.com/MikeHorn-git/systrace.git
cd systrace
make
```

# Usage
```bash
Usage: ./systrace <pid>
```

# Security
```bash
checksec --file=systrace
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH	Symbols		FORTIFY	Fortified	Fortifiable	FILE
Full RELRO      Canary found      NX enabled    PIE enabled     No RPATH   No RUNPATH   No Symbols	Yes	1		1		systrace
```
