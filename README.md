# Systrace
Simple strace implementation in C.

# Build
```bash
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
Full RELRO      Canary found      NX enabled    PIE enabled     No RPATH   No RUNPATH   No Symbols	Yes	2		2		systrace
```
