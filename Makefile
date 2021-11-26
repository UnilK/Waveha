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
SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp -not -name $(MAINAPP).cpp)
OBJECTS := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SOURCES:.cpp=.o))

# mirror the source directory structure to the build directory.
SRCSTRUCT := $(shell find $(SRCDIR) -type d)
BUILDSTRUCT := $(patsubst $(SRCDIR)%,$(BUILDDIR)%, $(SRCSTRUCT))

# gather the local libraries (archives)
LLIB := $(shell find $(LIBDIR) -type f -name *.a)
LLIBH := $(shell find $(LIBDIR) -type d -name *include)
LLIBHINC := $(patsubst $(LIBDIR)%,-I $(LIBDIR)%, $(LLIBH))

INC := -I $(HEADIR) $(LLIBHINC)
LIB := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lpthread



# link the executable
$(EXEC):  $(BUILDSTRUCT) $(BINDIR) $(OBJECTS) $(LLIB) $(BUILDDIR)/$(MAINAPP).o
	$(CXX) $(BUILDDIR)/$(MAINAPP).o $(OBJECTS) $(LLIB) $(LIB) -o $(EXEC)
	@mkdir -p audio

# compile the source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

# build the file that include main separately
$(BUILDDIR)/$(MAINAPP).o: $(SRCDIR)/$(MAINAPP).cpp
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

LLIBHINCDEP := $(patsubst $(LIBDIR)%,-I$(LIBDIR)%, $(LLIBH))

.PHONY: depend
depend: $(SOURCES) $(SRCDIR)/$(MAINAPP).cpp $(TESTDIR)/$(TEST).cpp
	makedepend -Y$(HEADIR) $(LLIBHINCDEP) $^
	@sed -i -e "s/$(SRCDIR)\//$(BUILDDIR)\//g" Makefile
	@sed -i -e "s/$(TESTDIR)\//$(BUILDDIR)\//g" Makefile

# DO NOT DELETE THIS LINE -- make depend depends on it. 

build/wave/pitchHandler.o: include/wave/pitchHandler.h include/math/FFT.h
build/wave/waveTransform.o: include/wave/waveTransform.h include/wave/vocal.h
build/wave/waveTransform.o: include/wave/vocalTransform.h include/math/FFT.h
build/wave/waveTransform.o: include/math/FT.h include/math/constants.hpp
build/wave/waveTransform.o: include/math/sincFIR.h include/math/FIR.h
build/wave/vocalTransform.o: include/wave/vocalTransform.h include/wave/vocal.h
build/wave/audioClassifier.o: include/wave/audioClassifier.h
build/wave/vocal.o: include/wave/vocal.h include/math/constants.hpp
build/app/mainFrame.o: include/app/mainFrame.h include/ui/frame.h
build/app/mainFrame.o: include/ui/core.h include/ui/window.h include/ui/clock.h
build/app/mainFrame.o: include/ui/style.h
build/app/app.o: include/app/app.h include/ui/core.h include/ui/window.h
build/app/app.o: include/ui/clock.h include/ui/frame.h include/ui/style.h
build/app/app.o: include/app/mainFrame.h
build/math/FFT.o: include/math/FFT.h include/math/constants.hpp
build/math/FIR.o: include/math/FIR.h
build/math/sincFIR.o: include/math/sincFIR.h include/math/FIR.h
build/math/sincFIR.o: include/math/constants.hpp
build/math/FT.o: include/math/FT.h include/math/constants.hpp
build/ui/text.o: include/ui/text.h include/ui/frame.h include/ui/core.h
build/ui/text.o: include/ui/window.h include/ui/clock.h include/ui/style.h
build/ui/style.o: include/ui/style.h
build/ui/button.o: include/ui/button.h include/ui/frame.h include/ui/core.h
build/ui/button.o: include/ui/window.h include/ui/clock.h include/ui/style.h
build/ui/window.o: include/ui/window.h include/ui/clock.h include/ui/core.h
build/ui/window.o: include/ui/style.h include/ui/frame.h
build/ui/clock.o: include/ui/clock.h
build/ui/frame.o: include/ui/frame.h include/ui/core.h include/ui/window.h
build/ui/frame.o: include/ui/clock.h include/ui/style.h
build/ui/core.o: include/ui/core.h include/ui/window.h include/ui/clock.h
build/ui/core.o: include/ui/frame.h include/ui/style.h
build/main.o: include/app/app.h include/ui/core.h include/ui/window.h
build/main.o: include/ui/clock.h include/ui/frame.h include/ui/style.h
build/main.o: include/app/mainFrame.h
build/test.o: lib/Wavestream/include/wavestream.h include/math/FFT.h
build/test.o: include/math/sincFIR.h include/math/FIR.h
build/test.o: include/wave/waveTransform.h include/wave/vocal.h
build/test.o: include/wave/vocalTransform.h include/math/FT.h
