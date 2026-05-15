CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700
TARGET := proc_switch_viewer

.PHONY: all clean run

all: $(TARGET)

$(TARGET): proc_switch_viewer.c
	$(CC) $(CFLAGS) -o $(TARGET) proc_switch_viewer.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
