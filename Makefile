CC = gcc
FLAGS = -o
SRC = main.c
TARGET = main
OUTPUT = main

#defektuz
all: $(TARGET)

#exekutagarria lortu
$(TARGET): $(SRC)
	$(CC) $(SRC) $(FLAGS) $(OUTPUT)

#exekutagarria martxan jarri
run: $(TARGET)
	./$(TARGET) 10 1 2

clean: 
	rm $(OUTPUT)

