CC = gcc
CFLAGS = -Wall -O3 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=3 -fstack-clash-protection -Ilib
DEBUGFLAGS = -g
LDFLAGS = -pie -z noexecstack -Wl,-z,relro,-z,now
TARGET = systrace
TARGET_DEBUG = systrace_debug
SRC = src/systrace.c
LIB = lib/syscalls.h

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
