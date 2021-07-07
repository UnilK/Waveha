HEADIR := include
SRCDIR := src
LIBDIR := lib
TESTDIR := test
BUILDDIR := build
BINDIR := bin

EXEC := $(BINDIR)/waveha
MAINAPP := main
TEST := test

CXX := g++
CXXFLAGS := -std=c++17 -g -Og -Wall
#CXXFLAGS := -std=c++17 -O3 -Wall



# gather the file names
HEADERS := $(shell find $(HEADIR) -type f -name *.h -o -name *.hpp)
SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SOURCES:.cpp=.o))

# mirror the source directory structure to the build directory.
SRCSTRUCT := $(shell find $(SRCDIR) -type d)
BUILDSTRUCT := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SRCSTRUCT))

# gather the local libraries (archives)
LLIB := $(shell find $(LIBDIR) -type f -name *.a)
LLIBH := $(shell find $(LIBDIR) -type d -name *include)
LLIBHINC := $(patsubst $(LIBDIR)%,-I $(LIBDIR)%, $(LLIBH))

INC := -I $(HEADIR) $(LLIBHINC)
LIB := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio



# link the executable
$(EXEC): $(BUILDSTRUCT) $(BINDIR) $(OBJECTS) $(LLIB)
	$(CXX) $(OBJECTS) $(LLIB) $(LIB) -o $(EXEC)

# compile the source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

# create build directory structure
$(BUILDSTRUCT):
	@echo "mkdir -p $(BUILDSTRUCT)"
	@mkdir -p $(BUILDSTRUCT)

# create directory for binaries
$(BINDIR):
	@echo "mkdir -p $(BINDIR)"
	@mkdir -p $(BINDIR)

# compile test(s)
.PHONY: test
test: $(EXEC)
	$(CXX) -c $(CXXFLAGS) $(INC) $(TESTDIR)/$(TEST).cpp -o $(BUILDDIR)/$(TEST).o
	$(CXX) $(BUILDDIR)/$(TEST).o $(OBJECTS) $(LLIB) $(LIB) -o $(BINDIR)/$(TEST)

# nuke the builds and bins
.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
	rm -rf $(BINDIR)

# lazy header dependency generation
.PHONY: depend
depend: $(SOURCES) $(SRCDIR)/$(MAINAPP).cpp $(TESTDIR)/$(TEST).cpp
	makedepend -Y$(HEADIR) $^
	@sed -i -e "s/$(SRCDIR)\//$(BUILDDIR)\//g" Makefile
	@sed -i -e "s/$(TESTDIR)\//$(BUILDDIR)\//g" Makefile

# DO NOT DELETE THIS LINE -- make depend depends on it. 

build/wave/voiceTransform.o: include/wave/voiceTransform.h include/wave/vocal.h
build/wave/voiceTransform.o: include/wave/vocalTransform.h include/math/FFT.h
build/wave/voiceTransform.o: include/math/FT.h include/math/constants.h
build/wave/vocalTransform.o: include/wave/vocalTransform.h include/wave/vocal.h
build/wave/vocal.o: include/wave/vocal.h
build/math/FFT.o: include/math/FFT.h include/math/constants.h
build/math/FIR.o: include/math/FIR.h
build/math/sincFIR.o: include/math/sincFIR.h include/math/FIR.h
build/math/sincFIR.o: include/math/constants.h
build/math/FT.o: include/math/FT.h include/math/constants.h
build/main.o: include/math/FT.h include/math/FFT.h
build/main.o: include/wave/voiceTransform.h include/wave/vocal.h
build/main.o: include/wave/vocalTransform.h
