CC=g++
CFLAGS=-Wall -std=c++17
BINDIR=bin
OBJDIR=obj
SRCDIR=src
OBJ=obj/test.o obj/Expression.o obj/NFA.o

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/tester: $(OBJ)
	$(CC) -o $@ $^

test: $(BINDIR)/tester
	$(BINDIR)/tester


clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(BINDIR)/*