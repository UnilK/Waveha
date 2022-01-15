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

build/wave/file.o: include/wave/file.h include/wave/source.h
build/wave/file.o: lib/Wstream/include/wstream/wstream.h include/wave/util.h
build/wave/cache.o: include/wave/cache.h include/wave/source.h
build/wave/cache.o: lib/Wstream/include/wstream/wstream.h
build/wave/buffer.o: include/wave/buffer.h include/wave/util.h
build/wave/mic.o: include/wave/mic.h include/wave/buffer.h include/wave/util.h
build/wave/source.o: include/wave/source.h
build/wave/sound.o: include/wave/sound.h include/wave/buffer.h
build/wave/sound.o: include/wave/source.h include/wave/util.h
build/wave/util.o: include/wave/util.h
build/app/app.o: include/app/app.h include/ui/window.h include/ui/frame.h
build/app/app.o: include/ui/borders.h include/ui/style.h include/ui/clock.h
build/app/app.o: include/app/tab.h include/app/resizableFrame.h
build/app/app.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/analyzer.o: include/app/analyzer.h include/app/box.h
build/app/analyzer.o: include/ui/button.h include/ui/text.h include/ui/frame.h
build/app/analyzer.o: include/ui/borders.h include/ui/style.h
build/app/analyzer.o: include/ui/window.h include/ui/clock.h
build/app/analyzer.o: include/app/resizableFrame.h include/app/tab.h
build/app/analyzer.o: include/app/graph.h include/wave/source.h
build/app/analyzer.o: include/changer/pitch.h include/app/app.h
build/app/analyzer.o: include/math/fft.h
build/app/tab.o: include/app/tab.h include/app/resizableFrame.h
build/app/tab.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/app/tab.o: include/ui/window.h include/ui/clock.h include/app/box.h
build/app/tab.o: include/ui/button.h include/ui/text.h
build/app/graph.o: include/app/graph.h include/ui/frame.h include/ui/borders.h
build/app/graph.o: include/ui/style.h include/ui/window.h include/ui/clock.h
build/app/graph.o: include/math/constants.hpp
build/app/box.o: include/app/box.h include/ui/button.h include/ui/text.h
build/app/box.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/app/box.o: include/ui/window.h include/ui/clock.h
build/app/box.o: include/app/resizableFrame.h include/app/tab.h
build/app/resizableFrame.o: include/app/resizableFrame.h include/ui/frame.h
build/app/resizableFrame.o: include/ui/borders.h include/ui/style.h
build/app/resizableFrame.o: include/ui/window.h include/ui/clock.h
build/math/ft.o: include/math/ft.h include/math/constants.hpp
build/math/fft.o: include/math/fft.h include/math/constants.hpp
build/ui/text.o: include/ui/text.h include/ui/frame.h include/ui/borders.h
build/ui/text.o: include/ui/style.h include/ui/window.h include/ui/clock.h
build/ui/borders.o: include/ui/borders.h include/ui/style.h include/ui/frame.h
build/ui/borders.o: include/ui/window.h include/ui/clock.h
build/ui/knob.o: include/ui/knob.h include/ui/frame.h include/ui/borders.h
build/ui/knob.o: include/ui/style.h include/ui/window.h include/ui/clock.h
build/ui/style.o: include/ui/style.h
build/ui/button.o: include/ui/button.h include/ui/text.h include/ui/frame.h
build/ui/button.o: include/ui/borders.h include/ui/style.h include/ui/window.h
build/ui/button.o: include/ui/clock.h
build/ui/window.o: include/ui/window.h include/ui/frame.h include/ui/borders.h
build/ui/window.o: include/ui/style.h include/ui/clock.h
build/ui/clock.o: include/ui/clock.h
build/ui/frame.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/ui/frame.o: include/ui/window.h include/ui/clock.h
build/changer/pitch.o: include/changer/pitch.h include/math/fft.h
build/main.o: include/app/app.h include/ui/window.h include/ui/frame.h
build/main.o: include/ui/borders.h include/ui/style.h include/ui/clock.h
build/main.o: include/app/tab.h include/app/resizableFrame.h include/app/box.h
build/main.o: include/ui/button.h include/ui/text.h
