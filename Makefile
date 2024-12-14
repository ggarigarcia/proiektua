CC = gcc
CFLAGS = -g #include direktorioa espezifikatu, konpilazio informazioa lortu
SRC = $(wildcard src/*.c) 
OBJ = $(SRC:src/%.c=obj/%.o) #.c fitxategien .o bertsioak
BIN = bin/kernel

#default
all: obj $(BIN)

$(BIN): $(OBJ) #bin birkonpilatu egingo da OBJ lista aldatzen bada:
	$(CC) $(OBJ) -o $(BIN)

obj/%.o: src/%.c #.o bakoitza bere .c-a aldatzean birkonpilatu
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN) 10 1 2 1 2 2

debug: $(BIN)
	gdb ./$(BIN)

clean:
	rm -f obj/*.o
	rm -f $(BIN)
