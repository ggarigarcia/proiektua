CC = gcc
CFLAGS = -g 
SRC = $(wildcard src/*.c) 
HDR = $(wildcard src/*.h)
OBJDIR = obj
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o) #.c fitxategien .o bertsioak
BINDIR = bin
BIN = $(BINDIR)/kernel

#default
all: $(OBJDIR) $(BIN)

$(BIN): $(OBJ) #bin birkonpilatu egingo da OBJ aldatzen bada:
	$(CC) $(OBJ) -o $(BIN)

$(OBJDIR)/%.o: src/%.c $(HDR) #.o bakoitza bere .c-a (edo .h) aldatzean birkonpilatu
	$(CC) $(CFLAGS) -c $< -o $@

# HAU ALDATU NAHI DEN BALIOAK SARTUZ
run: $(BIN)
	./$(BIN) 10 1 1 1 1 1 4 25

debug: $(BIN)
	gdb ./$(BIN)

clean:
	rm -f $(OBJDIR)/*
	rm -f $(BINDIR)/*
