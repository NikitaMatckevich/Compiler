SRCDIR = ../src
BINDIR = ../bin
LIBDIR = ../lib

_BINS = Lexer Ast Main
_LIBS = 

BINS = $(patsubst %,$(BINDIR)/%.o,$(_BINS))
LIBS = $(patsubst %,$(BINDIR)/%.o,$(_LIBS)) 

CXX = g++-9
CXXFLAGS = -std=c++17 -I $(SRCDIR)
DBGFLAGS = -DDEBUG -g -Wall -Wextra -pedantic -fsanitize=address
ifndef lvl
  lvl = 3
endif
OPTFLAGS = -O$(lvl)

target = $(BINDIR)/compiler.out

all: $(target)

dbg: CXXFLAGS += $(DBGFLAGS)
dbg: $(target)

opt: CXXFLAGS += $(OPTFLAGS)
opt: $(target)

$(target): $(BINS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(BINDIR)/*
