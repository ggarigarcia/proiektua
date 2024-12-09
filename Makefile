CC = gcc
FLAGS = -o
SRC = 1zatia.c
TARGET = 1zatia
OUTPUT = 1zatia

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

