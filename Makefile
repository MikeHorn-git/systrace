CC = gcc
CFLAGS = -Wall -O3 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=3 -fstack-clash-protection -Ilib
DEBUGFLAGS = -g
LDFLAGS = -pie -z noexecstack -Wl,-z,relro,-z,now
TARGET = systrace
TARGET_DEBUG = systrace_debug
SRC = src/systrace.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)
	strip $(TARGET)

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(SRC)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(LDFLAGS) -o $(TARGET_DEBUG) $(SRC)

clean:
	rm -f $(TARGET) $(TARGET_DEBUG)

.PHONY: all clean debug
