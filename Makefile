CXX := g++
EXEC := bin/waveha
TEST := test

TESTDIR := test
SRCDIR := src
BUILDDIR := build
BINDIR := bin
LIBDIR := lib

SOURCES := $(shell find $(SRCDIR) -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SOURCES:.cpp=.o))

SRCSTRUCT := $(shell find $(SRCDIR) -type d)
BUILDSTRUCT := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SRCSTRUCT))

LLIB := $(shell find $(LIBDIR) -name *.a)
LLIBH := $(shell find $(LIBDIR) -name *include -type d)
LLIBHINC := $(patsubst $(LIBDIR)%,-I $(LIBDIR)%, $(LLIBH))

INC := -I include $(LLIBHINC)
LIB := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

CXXFLAGS := -std=c++17 -g -Wall -fsanitize=leak
#CXXFLAGS := -std=c++17 -O2 -Wall

$(EXEC): $(BUILDSTRUCT) $(BINDIR) $(OBJECTS) $(LLIB)
	$(CXX) $(OBJECTS) $(LLIB) $(LIB) -o $(EXEC)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

$(BUILDSTRUCT):
	@echo "mkdir -p $(BUILDSTRUCT)"
	@mkdir -p $(BUILDSTRUCT)

$(BINDIR):
	@echo "mkdir -p $(BINDIR)"
	@mkdir -p $(BINDIR)

.PHONY: test
test: $(EXEC)
	$(CXX) -c $(CXXFLAGS) $(INC) $(TESTDIR)/$(TEST).cpp -o $(BUILDDIR)/$(TEST).o
	$(CXX) $(OBJECTS) $(BUILDDIR)/$(TEST).o -o $(BINDIR)/$(TEST)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
	rm -rf $(BINDIR)

