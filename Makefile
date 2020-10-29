DPSDIR = ./include
SRCDIR = ./src
BINDIR = ./build

_DEPS = Errors Token Lexer Ast
_BINS = Errors Lexer Ast Main

CXX = g++-9
CXXFLAGS = -std=c++17 -I $(DPSDIR)
DBGFLAGS = -DDEBUG -g -Wall -Wextra -pedantic -fsanitize=address 
ifndef lvl
  lvl = 3
endif
OPTFLAGS = -O$(lvl)

TARGET = $(BINDIR)/compiler

.PHONY: dbg opt clean

dbg: CXXFLAGS += $(DBGFLAGS)
opt: CXXFLAGS += $(OPTFLAGS)
dbg opt: $(TARGET)

$(TARGET): $(patsubst %,$(BINDIR)/%.o,$(_BINS))
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(patsubst %,$(DPSDIR)/%.h,$(_DEPS))
	$(CXX) -c -o $@ $< $(CXXFLAGS)
clean:
	rm -f $(BINDIR)/*
