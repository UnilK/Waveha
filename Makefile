EXEC := bin/waveha
TEST := test
MAINAPP := main

INCDIR := include
SRCDIR := src
LIBDIR := lib
TESTDIR := test
BUILDDIR := build
BINDIR := bin

CXX := g++
#CXXFLAGS := -std=c++17 -g -Wall -fsanitize=leak
CXXFLAGS := -std=c++17 -O3 -Wall



INCLUDES := $(shell find $(INCDIR) -name *.h)
SOURCES := $(shell find $(SRCDIR) -name *.cpp -not -name *$(MAINAPP).cpp)
OBJECTS := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SOURCES:.cpp=.o))

SRCSTRUCT := $(shell find $(SRCDIR) -type d)
BUILDSTRUCT := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SRCSTRUCT))

LLIB := $(shell find $(LIBDIR) -name *.a)
LLIBH := $(shell find $(LIBDIR) -name *include -type d)
LLIBHINC := $(patsubst $(LIBDIR)%,-I $(LIBDIR)%, $(LLIBH))

INC := -I $(INCDIR) $(LLIBHINC)
LIB := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio



$(EXEC): $(BUILDSTRUCT) $(BINDIR) $(OBJECTS) $(BUILDDIR)/$(MAINAPP).o $(LLIB) $(INCLUDES)
	$(CXX) $(BUILDDIR)/$(MAINAPP).o $(OBJECTS) $(LLIB) $(LIB) -o $(EXEC)

#build OBJECTS
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

$(BUILDSTRUCT):
	@echo "mkdir -p $(BUILDSTRUCT)"
	@mkdir -p $(BUILDSTRUCT)

$(BINDIR):
	@echo "mkdir -p $(BINDIR)"
	@mkdir -p $(BINDIR)

$(BUILDDIR)/$(MAINAPP).o: $(SRCDIR)/$(MAINAPP).cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

.PHONY: test
test: $(EXEC)
	$(CXX) -c $(CXXFLAGS) $(INC) $(TESTDIR)/$(TEST).cpp -o $(BUILDDIR)/$(TEST).o
	$(CXX) $(OBJECTS) $(LLIB) $(LIB) $(BUILDDIR)/$(TEST).o -o $(BINDIR)/$(TEST)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
	rm -rf $(BINDIR)

