CC = gcc
CFLAGS = -Wall -Wextra

TARGET1 = obj/F4server
SRCS1 = src/F4server.c src/errExit.c src/fifo.c src/sem.c src/server.c src/shm.c
OBJS1 = $(SRCS1:.c=.o)

TARGET2 = obj/F4client
SRCS2 = src/F4client.c src/errExit.c src/fifo.c src/sem.c src/client.c src/shm.c
OBJS2 = $(SRCS2:.c=.o)

.PHONY: all clean

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS1) $(OBJS2)


