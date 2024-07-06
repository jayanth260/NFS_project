CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = SS_main.c read_accessPaths.c Read_write_file.c create_delete_S.c update_paths.c copy.c
OBJS = $(SRCS:.c=.o)

TARGET = SS

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
