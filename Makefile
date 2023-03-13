CC=g++
CFLAGS=-Wall -std=c++17
BINDIR=bin
OBJDIR=obj
SRCDIR=src
OBJ=obj/Expression.o obj/NFA.o obj/DFA.o
DEBUG=0

ifeq ($(DEBUG), 1) 
	CFLAGS=-Wall -std=c++17 -DDEBUG
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/tester: $(OBJ) $(OBJDIR)/analyzer.o
	$(CC) -o $@ $^

test: $(BINDIR)/tester
	$(BINDIR)/tester

build: $(BINDIR)/Simulator

clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(BINDIR)/*