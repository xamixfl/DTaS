CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I. -Ibst -Iavl -Ihashtable -Igraphviz
LDFLAGS = -lm

# Находим все .c файлы рекурсивно
SRCS = $(wildcard *.c) \
       $(wildcard bst/*.c) \
       $(wildcard avl/*.c) \
       $(wildcard hashtable/*.c) \
       $(wildcard graphviz/*.c)

# Преобразуем .c в .o файлы в корневой папке
OBJS = $(notdir $(SRCS:.c=.o))
TARGET = lab7

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Общее правило для компиляции .c файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: bst/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: avl/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: hashtable/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: graphviz/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.dot *.png

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run