# Sources
SRCDIR = ../src
BINDIR = ../bin
LIBDIR = ../lib

_BINS = Lexer Ast Main
_LIBS = 

BINS = $(patsubst %,$(BINDIR)/%.o,$(_BINS))
LIBS = $(patsubst %,$(BINDIR)/%.o,$(_LIBS)) 

# Compiler flags
CXX = g++-9
CXXFLAGS = -g -Wall -Wextra -pedantic -fsanitize=address -std=c++17 -I $(SRCDIR)

$(BINDIR)/compiler.out: $(BINS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(BINDIR)/*
