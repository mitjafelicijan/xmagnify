CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lX11 -lXfixes -lXrender

TARGET = xmagnify

all: $(TARGET)

$(TARGET): main.o
	$(CC) main.o -o $(TARGET) $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f main.o $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall