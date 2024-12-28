CC = gcc
CFLAGS = -g 
SRC = $(wildcard src/*.c) 
OBJDIR = obj
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o) #.c fitxategien .o bertsioak
BINDIR = bin
BIN = $(BINDIR)/kernel

#default
all: $(OBJDIR) $(BIN)

$(BIN): $(OBJ) #bin birkonpilatu egingo da OBJ aldatzen bada:
	$(CC) $(OBJ) -o $(BIN)

$(OBJDIR)/%.o: src/%.c #.o bakoitza bere .c-a aldatzean birkonpilatu
	$(CC) $(CFLAGS) -c $< -o $@

# HAU ALDATU NAHI DEN BALIOAK SARTUZ
run: $(BIN)
	./$(BIN) 10 1 1 2 2 2 1

debug: $(BIN)
	gdb ./$(BIN)

clean:
	rm -f $(OBJDIR)/*
	rm -f $(BINDIR)/*
