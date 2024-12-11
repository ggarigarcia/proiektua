CC = gcc
CFLAGS = -Iinclude -g #include direktorioa espezifikatu, konpilazio informazioa lortu
SRC = $(wildcard src/*.c) #src/scheduler.c src/prozesu-sortzaile.c
OBJDIR = obj
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o) #src-ko fitxategi guztiak .o-ra bihurtu obj karpetan
BIN = bin/kernel

#default
all: $(OBJDIR) $(BIN)

$(BIN): $(OBJ) #bin birkonpilatu egingo da OBJ lista aldatzen bada:
	$(CC) $(OBJ) -o $(BIN)

$(OBJDIR)/%.o: src/%.c #.o bakoitza bere .c-a aldatzean birkonpilatu
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN) 10 1 2 1 2 2

debug: $(BIN)
	gdb ./$(BIN) 10 1 2 1 2 2

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BIN)
	rm src/*.exe
