CC = gcc

CFLAGS = -Wall -Wextra -pthread -g -Iinclude

TARGET = simulation

# Source files
SRCS = main.c \
       src/simulation.c \
       src/queue.c \
       src/counter.c \
       src/balancer.c \
       src/supervisor.c

# Object files (keeps them in same folders)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile rule (works with src/ paths)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.o src/*.o $(TARGET)