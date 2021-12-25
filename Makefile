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

build/wave/freeAudio.o: include/wave/freeAudio.h include/wave/readableAudio.h
build/wave/freeAudio.o: include/wave/audioBuffer.h
build/wave/audioBuffer.o: include/wave/audioBuffer.h
build/wave/audioBuffer.o: include/wave/typeConverter.h
build/wave/audioFile.o: include/wave/audioFile.h
build/wave/audioFile.o: lib/Wstream/include/wstream/wstream.h
build/wave/audioFile.o: include/wave/typeConverter.h
build/wave/recordableAudio.o: include/wave/recordableAudio.h
build/wave/recordableAudio.o: include/wave/audioBuffer.h
build/wave/recordableAudio.o: include/wave/typeConverter.h
build/wave/typeConverter.o: include/wave/typeConverter.h
build/wave/readableAudio.o: include/wave/readableAudio.h
build/wave/readableAudio.o: include/wave/audioBuffer.h
build/wave/readableAudio.o: include/wave/typeConverter.h
build/app/mainFrame.o: include/app/mainFrame.h include/ui/frame.h
build/app/mainFrame.o: include/ui/core.h include/ui/window.h include/ui/clock.h
build/app/mainFrame.o: include/ui/command.h include/ui/style.h
build/app/app.o: include/app/app.h include/ui/core.h include/ui/window.h
build/app/app.o: include/ui/clock.h include/ui/frame.h include/ui/command.h
build/app/app.o: include/ui/style.h include/app/mainFrame.h
build/app/commands.o: include/app/app.h include/ui/core.h include/ui/window.h
build/app/commands.o: include/ui/clock.h include/ui/frame.h
build/app/commands.o: include/ui/command.h include/ui/style.h
build/app/commands.o: include/app/mainFrame.h
build/math/FFT.o: include/math/FFT.h include/math/constants.hpp
build/math/FT.o: include/math/FT.h include/math/constants.hpp
build/ui/text.o: include/ui/text.h include/ui/frame.h include/ui/core.h
build/ui/text.o: include/ui/window.h include/ui/clock.h include/ui/command.h
build/ui/text.o: include/ui/style.h
build/ui/style.o: include/ui/style.h
build/ui/button.o: include/ui/button.h include/ui/frame.h include/ui/core.h
build/ui/button.o: include/ui/window.h include/ui/clock.h include/ui/command.h
build/ui/button.o: include/ui/style.h
build/ui/command.o: include/ui/command.h
build/ui/window.o: include/ui/window.h include/ui/clock.h include/ui/core.h
build/ui/window.o: include/ui/style.h include/ui/command.h include/ui/frame.h
build/ui/clock.o: include/ui/clock.h
build/ui/frame.o: include/ui/frame.h include/ui/core.h include/ui/window.h
build/ui/frame.o: include/ui/clock.h include/ui/command.h include/ui/style.h
build/ui/graph.o: include/ui/graph.h include/ui/frame.h include/ui/core.h
build/ui/graph.o: include/ui/window.h include/ui/clock.h include/ui/command.h
build/ui/graph.o: include/ui/style.h
build/ui/core.o: include/ui/core.h include/ui/window.h include/ui/clock.h
build/ui/core.o: include/ui/frame.h include/ui/command.h include/ui/style.h
build/main.o: include/app/app.h include/ui/core.h include/ui/window.h
build/main.o: include/ui/clock.h include/ui/frame.h include/ui/command.h
build/main.o: include/ui/style.h include/app/mainFrame.h
build/main.o: include/wave/audioFile.h lib/Wstream/include/wstream/wstream.h
build/test.o: include/math/FFT.h include/math/FT.h
