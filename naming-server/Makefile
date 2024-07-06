CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = NM_main.c create_delete.c book_keeping.c Search.c Redundunt.c
OBJS = $(SRCS:.c=.o)

TARGET = NM

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
