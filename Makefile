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
	@mkdir -p sessions

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

build/wave/audioBuffer.o: include/wave/audioBuffer.h
build/wave/audioBuffer.o: include/wave/typeConverter.h
build/wave/audioFile.o: include/wave/audioFile.h include/wave/readableAudio.h
build/wave/audioFile.o: include/wave/audioBuffer.h
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
build/app/mainFrame.o: include/ui/core.h include/ui/window.h
build/app/mainFrame.o: include/ui/command.h include/ui/clock.h
build/app/mainFrame.o: include/ui/style.h include/ui/borders.h
build/app/mainFrame.o: include/app/tab.h include/app/resizableFrame.h
build/app/mainFrame.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/app.o: include/app/app.h include/ui/core.h include/ui/window.h
build/app/app.o: include/ui/frame.h include/ui/command.h include/ui/borders.h
build/app/app.o: include/ui/clock.h include/ui/style.h include/app/mainFrame.h
build/app/app.o: include/app/tab.h include/app/resizableFrame.h
build/app/app.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/analyzer.o: include/app/analyzer.h include/app/box.h
build/app/analyzer.o: include/ui/button.h include/ui/text.h include/ui/frame.h
build/app/analyzer.o: include/ui/core.h include/ui/window.h
build/app/analyzer.o: include/ui/command.h include/ui/clock.h
build/app/analyzer.o: include/ui/style.h include/ui/borders.h
build/app/analyzer.o: include/app/resizableFrame.h include/app/tab.h
build/app/analyzer.o: include/app/graph.h include/wave/audioFile.h
build/app/analyzer.o: include/wave/readableAudio.h include/wave/audioBuffer.h
build/app/analyzer.o: lib/Wstream/include/wstream/wstream.h
build/app/tab.o: include/app/tab.h include/app/resizableFrame.h
build/app/tab.o: include/ui/frame.h include/ui/core.h include/ui/window.h
build/app/tab.o: include/ui/command.h include/ui/clock.h include/ui/style.h
build/app/tab.o: include/ui/borders.h include/app/box.h include/ui/button.h
build/app/tab.o: include/ui/text.h
build/app/commands.o: include/app/app.h include/ui/core.h include/ui/window.h
build/app/commands.o: include/ui/frame.h include/ui/command.h
build/app/commands.o: include/ui/borders.h include/ui/clock.h
build/app/commands.o: include/ui/style.h include/app/mainFrame.h
build/app/commands.o: include/app/tab.h include/app/resizableFrame.h
build/app/commands.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/graph.o: include/app/graph.h include/ui/frame.h include/ui/core.h
build/app/graph.o: include/ui/window.h include/ui/command.h include/ui/clock.h
build/app/graph.o: include/ui/style.h include/ui/borders.h
build/app/box.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/box.o: include/ui/frame.h include/ui/core.h include/ui/window.h
build/app/box.o: include/ui/command.h include/ui/clock.h include/ui/style.h
build/app/box.o: include/ui/borders.h include/app/resizableFrame.h
build/app/box.o: include/app/tab.h
build/app/resizableFrame.o: include/app/resizableFrame.h include/ui/frame.h
build/app/resizableFrame.o: include/ui/core.h include/ui/window.h
build/app/resizableFrame.o: include/ui/command.h include/ui/clock.h
build/app/resizableFrame.o: include/ui/style.h include/ui/borders.h
build/math/FFT.o: include/math/FFT.h include/math/constants.hpp
build/math/FT.o: include/math/FT.h include/math/constants.hpp
build/ui/text.o: include/ui/text.h include/ui/frame.h include/ui/core.h
build/ui/text.o: include/ui/window.h include/ui/command.h include/ui/clock.h
build/ui/text.o: include/ui/style.h include/ui/borders.h
build/ui/borders.o: include/ui/borders.h include/ui/frame.h include/ui/core.h
build/ui/borders.o: include/ui/window.h include/ui/command.h include/ui/clock.h
build/ui/borders.o: include/ui/style.h
build/ui/style.o: include/ui/style.h
build/ui/button.o: include/ui/button.h include/ui/text.h include/ui/frame.h
build/ui/button.o: include/ui/core.h include/ui/window.h include/ui/command.h
build/ui/button.o: include/ui/clock.h include/ui/style.h include/ui/borders.h
build/ui/command.o: include/ui/command.h
build/ui/window.o: include/ui/window.h include/ui/core.h include/ui/style.h
build/ui/window.o: include/ui/clock.h include/ui/command.h include/ui/frame.h
build/ui/window.o: include/ui/borders.h
build/ui/clock.o: include/ui/clock.h
build/ui/frame.o: include/ui/frame.h include/ui/core.h include/ui/window.h
build/ui/frame.o: include/ui/command.h include/ui/clock.h include/ui/style.h
build/ui/frame.o: include/ui/borders.h
build/ui/core.o: include/ui/core.h include/ui/window.h include/ui/frame.h
build/ui/core.o: include/ui/command.h include/ui/borders.h include/ui/clock.h
build/ui/core.o: include/ui/style.h
build/main.o: include/app/app.h include/ui/core.h include/ui/window.h
build/main.o: include/ui/frame.h include/ui/command.h include/ui/borders.h
build/main.o: include/ui/clock.h include/ui/style.h include/app/mainFrame.h
build/main.o: include/app/tab.h include/app/resizableFrame.h include/app/box.h
build/main.o: include/ui/button.h include/ui/text.h
build/test.o: include/math/FFT.h include/math/FT.h
