CC = gcc
CFLAGS = -Wall -O3 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=3 -fstack-clash-protection -Ilib
DEBUGFLAGS = -g
LDFLAGS = -pie -z noexecstack -Wl,-z,relro,-z,now
TARGET = systrace
TARGET_DEBUG = systrace_debug
SRC = src/systrace.c
LIB = lib/syscalls.h

.DEFAULT_GOAL := help

help:
	@echo "Usage: make <target>"
	@echo "Targets:"
	@echo "  all      Build e release version"
	@echo "  debug    Build debug version of $(TARGET_DEBUG)"
	@echo "  format   Format source files using clang-format"
	@echo "  clean    Remove generated binaries"
	@echo "  fast     Clean, format, and build the release version"

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)
	strip $(TARGET)

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(SRC)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(LDFLAGS) -o $(TARGET_DEBUG) $(SRC)

format:
	clang-format -i $(SRC) $(LIB)

clean:
	rm -f $(TARGET) $(TARGET_DEBUG)

fast: clean format all

.PHONY: all debug format clean fast
