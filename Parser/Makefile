# Makefile for compiling and running P1.y and P1.l

# Compiler
CC = g++

# Bison and Flex
BISON = bison
FLEX = flex

# Output executable name
OUTPUT = graphGen

# Compiler flags
CFLAGS = -O3 -std=c++20

# Name of File
NAME = regex
INP = inp.txt
OUT = out.txt

all: $(OUTPUT)

$(OUTPUT): $(NAME).tab.c lex.yy.c
	$(CC) -o $(OUTPUT) $(CFLAGS) $(NAME).tab.c lex.yy.c NFA.cpp

$(NAME).tab.c: $(NAME).y
	$(BISON) -d $(NAME).y

lex.yy.c: $(NAME).l
	$(FLEX) $(NAME).l 

clean:
	rm -f $(OUTPUT) $(NAME).tab.c $(NAME).tab.h lex.yy.c

run: $(OUTPUT)
	./$(OUTPUT) < $(INP) > $(OUT)

clear:
	rm -f $(OUTPUT) $(NAME).tab.c $(NAME).tab.h lex.yy.c
	@clear

fresh: clear all

freshRun: fresh run

.PHONY: all clean run
