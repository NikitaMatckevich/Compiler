SRCDIR = ../src
BINDIR = ../bin
LIBDIR = ../lib

_BINS = Lexer Ast Main
_LIBS = 

BINS = $(patsubst %,$(BINDIR)/%.o,$(_BINS))
LIBS = $(patsubst %,$(BINDIR)/%.o,$(_LIBS)) 


# Compiler flags
CXX = g++-9
CXXFLAGS = -std=c++17 -I $(SRCDIR)
DBGFLAGS = -DDEBUG -g -Wall -Wextra -pedantic -fsanitize=address

ifndef lvl
  lvl = 3
endif
OPTFLAGS = -O$(lvl)

all: $(BINDIR)/compiler.out

dbg: CXXFLAGS += $(DBGFLAGS)
dbg: $(BINDIR)/compiler.out

opt: CXXFLAGS += $(OPTFLAGS)
opt: $(BINDIR)/compiler.out

$(BINDIR)/compiler.out: $(BINS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(BINDIR)/*
