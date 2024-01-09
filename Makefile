CC = gcc
CFLAGS = -std=gnu99

SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = movies

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(TARGET) $(OBJECTS)