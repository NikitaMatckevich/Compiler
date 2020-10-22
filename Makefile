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

TARGET = $(BINDIR)/compiler

dbg: CXXFLAGS += $(DBGFLAGS)
opt: CXXFLAGS += $(OPTFLAGS)
all dbg opt: $(TARGET)

$(TARGET): $(BINS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BINDIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(BINDIR)/*
