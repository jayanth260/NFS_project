CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = C_main.c Read_write.c
OBJS = $(SRCS:.c=.o)

TARGET = client

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
