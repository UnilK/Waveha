HEADIR := include
SRCDIR := src
LIBDIR := lib
TESTDIR := test
BUILDDIR := build
BINDIR := bin

EXEC := waveha
MAINAPP := main
TEST := test

CXX := g++
CXXFLAGS := -std=c++17 -g -Og -Wall
#ALTERNATIVE := -std=c++17 -O3 -funroll-loops -mavx2 -Wall
RELEASEFLAGS := -std=c++17 -O3 -funroll-loops -mavx2 -Wall



# gather the file names
HEADERS := $(shell find $(HEADIR) -type f -name *.h -o -name *.hpp)
INLINES := $(shell find $(SRCDIR) -type f -name *.inl)
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
$(BINDIR)/$(EXEC):  $(BUILDSTRUCT) $(BINDIR) $(OBJECTS) $(LLIB) $(BUILDDIR)/$(MAINAPP).o
	$(CXX) $(BUILDDIR)/$(MAINAPP).o $(OBJECTS) $(LLIB) $(LIB) -o $(BINDIR)/$(EXEC)
	@mkdir -p audio
	@mkdir -p sessions
	@mkdir -p stacks
	@mkdir -p stacks/sources
	@mkdir -p stacks/saves
	@ln -f $(BINDIR)/$(EXEC) $(EXEC)


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
depend: $(INLINES) $(SOURCES) $(SRCDIR)/$(MAINAPP).cpp $(TESTDIR)/$(TEST).cpp
	makedepend -Y$(HEADIR) $(LLIBHINCDEP) $^
	@sed -i -e "s/$(SRCDIR)\//$(BUILDDIR)\//g" Makefile
	@sed -i -e "s/$(TESTDIR)\//$(BUILDDIR)\//g" Makefile

# release build
.PHONY: release
release: $(SOURCES) $(HEADERS) $(SRCDIR)/$(MAINAPP).cpp
	$(CXX) $(RELEASEFLAGS) $(SOURCES) $(SRCDIR)/$(MAINAPP).cpp $(INC) $(LLIB) $(LIB) -o $(BINDIR)/$(EXEC)



# DO NOT DELETE THIS LINE -- make depend depends on it. 

build/wave/file.o: include/wave/file.h include/wave/source.h
build/wave/file.o: lib/Wstream/include/wstream/wstream.h include/wave/util.h
build/wave/cache.o: include/wave/cache.h include/wave/source.h
build/wave/cache.o: include/wave/audio.h lib/Wstream/include/wstream/wstream.h
build/wave/audio.o: include/wave/audio.h lib/Wstream/include/wstream/wstream.h
build/wave/buffer.o: include/wave/buffer.h include/wave/util.h
build/wave/mic.o: include/wave/mic.h include/wave/buffer.h include/wave/util.h
build/wave/source.o: include/wave/source.h
build/wave/sound.o: include/wave/sound.h include/wave/buffer.h
build/wave/sound.o: include/wave/source.h include/wave/util.h
build/wave/util.o: include/wave/util.h
build/change/matrix.o: include/change/matrix.h include/math/constants.h
build/change/pitch.o: include/change/pitch.h include/math/fft.h
build/change/pitch.o: include/math/ft.h include/math/constants.h
build/change/pitch.o: include/ml/stack.h include/ml/layer.h include/ui/fileio.h
build/change/pitch.o: include/ml/util.h include/ml/judge.h
build/app/audio.o: include/app/audio.h include/wave/source.h
build/app/audio.o: include/wave/cache.h include/wave/audio.h
build/app/audio.o: include/wave/file.h lib/Wstream/include/wstream/wstream.h
build/app/audio.o: include/ui/terminal.h include/ui/frame.h
build/app/audio.o: include/ui/borders.h include/ui/style.h include/ui/window.h
build/app/audio.o: include/app/session.h include/ui/fileio.h include/app/app.h
build/app/audio.o: include/ui/box.h include/ui/slider.h include/ui/stack.h
build/app/audio.o: include/ui/text.h include/ui/button.h include/app/tools.h
build/app/audio.o: lib/Wstream/include/wstream/constants.h
build/app/content.o: include/app/content.h include/ui/frame.h
build/app/content.o: include/ui/borders.h include/ui/style.h
build/app/content.o: include/ui/window.h include/app/session.h
build/app/content.o: include/ui/terminal.h include/ui/fileio.h
build/app/content.o: include/app/app.h include/ui/box.h include/ui/slider.h
build/app/content.o: include/ui/stack.h include/ui/text.h include/ui/button.h
build/app/content.o: include/app/tools.h include/tools/analyzer.h
build/app/content.o: include/app/audio.h include/wave/source.h
build/app/content.o: include/wave/cache.h include/wave/audio.h
build/app/content.o: include/wave/file.h lib/Wstream/include/wstream/wstream.h
build/app/content.o: include/tools/graph.h include/wave/sound.h
build/app/content.o: include/wave/buffer.h include/change/pitch.h
build/app/content.o: include/tools/meditor.h include/change/matrix.h
build/app/content.o: include/tools/recorder.h include/ui/clock.h
build/app/content.o: include/wave/mic.h include/tools/trainer.h
build/app/content.o: include/ml/stack.h include/ml/layer.h include/ml/util.h
build/app/content.o: include/ml/judge.h
build/app/slot.o: include/app/slot.h include/ui/box.h include/ui/slider.h
build/app/slot.o: include/ui/stack.h include/ui/frame.h include/ui/borders.h
build/app/slot.o: include/ui/style.h include/ui/window.h include/ui/text.h
build/app/slot.o: include/ui/button.h include/app/session.h
build/app/slot.o: include/ui/terminal.h include/ui/fileio.h include/app/tab.h
build/app/slot.o: include/app/layout.h include/app/app.h include/app/tools.h
build/app/slot.o: include/app/content.h
build/app/app.o: include/app/app.h include/ui/window.h include/ui/frame.h
build/app/app.o: include/ui/borders.h include/ui/style.h include/ui/box.h
build/app/app.o: include/ui/slider.h include/ui/stack.h include/ui/text.h
build/app/app.o: include/ui/button.h include/ui/terminal.h include/app/tools.h
build/app/app.o: include/app/audio.h include/wave/source.h include/wave/cache.h
build/app/app.o: include/wave/audio.h include/wave/file.h
build/app/app.o: lib/Wstream/include/wstream/wstream.h include/app/session.h
build/app/app.o: include/ui/fileio.h include/app/layout.h
build/app/app.o: include/app/creations.h
build/app/layout.o: include/app/layout.h include/ui/slider.h include/ui/stack.h
build/app/layout.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/app/layout.o: include/ui/window.h include/ui/text.h include/ui/box.h
build/app/layout.o: include/ui/button.h include/app/session.h
build/app/layout.o: include/ui/terminal.h include/ui/fileio.h include/app/app.h
build/app/layout.o: include/app/tools.h include/app/tab.h include/app/slot.h
build/app/tab.o: include/app/tab.h include/ui/box.h include/ui/slider.h
build/app/tab.o: include/ui/stack.h include/ui/frame.h include/ui/borders.h
build/app/tab.o: include/ui/style.h include/ui/window.h include/ui/text.h
build/app/tab.o: include/ui/button.h include/app/session.h
build/app/tab.o: include/ui/terminal.h include/ui/fileio.h include/app/slot.h
build/app/tab.o: include/app/layout.h include/app/app.h include/app/tools.h
build/app/session.o: include/app/session.h include/ui/terminal.h
build/app/session.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/app/session.o: include/ui/window.h include/ui/fileio.h include/app/app.h
build/app/session.o: include/ui/box.h include/ui/slider.h include/ui/stack.h
build/app/session.o: include/ui/text.h include/ui/button.h include/app/tools.h
build/app/session.o: include/app/audio.h include/wave/source.h
build/app/session.o: include/wave/cache.h include/wave/audio.h
build/app/session.o: include/wave/file.h lib/Wstream/include/wstream/wstream.h
build/app/session.o: include/app/layout.h include/app/creations.h
build/app/tools.o: include/app/tools.h include/ui/slider.h include/ui/stack.h
build/app/tools.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/app/tools.o: include/ui/window.h include/ui/text.h include/app/app.h
build/app/tools.o: include/ui/box.h include/ui/button.h include/ui/terminal.h
build/app/creations.o: include/app/creations.h include/ui/terminal.h
build/app/creations.o: include/ui/frame.h include/ui/borders.h
build/app/creations.o: include/ui/style.h include/ui/window.h
build/app/creations.o: include/app/session.h include/ui/fileio.h
build/app/creations.o: include/app/app.h include/ui/box.h include/ui/slider.h
build/app/creations.o: include/ui/stack.h include/ui/text.h include/ui/button.h
build/app/creations.o: include/app/tools.h include/ml/mnist.h
build/app/creations.o: include/ml/stack.h include/ml/layer.h include/ml/util.h
build/app/creations.o: include/ml/judge.h include/ml/waves.h
build/ml/roll.o: include/ml/roll.h include/ml/layer.h include/ui/fileio.h
build/ml/roll.o: include/ml/util.h
build/ml/factory.o: include/ml/factory.h include/ml/util.h include/ml/field.h
build/ml/factory.o: include/ml/layer.h include/ui/fileio.h include/ml/field.inl
build/ml/factory.o: include/ml/matrix.h include/ml/deloc.h include/ml/roll.h
build/ml/factory.o: include/ml/ft.h include/ml/judge.h include/ml/norm.h
build/ml/field.o: include/ml/field.h include/ml/layer.h include/ui/fileio.h
build/ml/field.o: include/ml/util.h include/ml/field.inl
build/ml/norm.o: include/ml/norm.h include/ml/layer.h include/ui/fileio.h
build/ml/norm.o: include/ml/util.h
build/ml/deloc.o: include/ml/deloc.h include/ml/layer.h include/ui/fileio.h
build/ml/deloc.o: include/ml/util.h
build/ml/util.o: include/ml/util.h
build/ml/matrix.o: include/ml/matrix.h include/ml/layer.h include/ui/fileio.h
build/ml/matrix.o: include/ml/util.h
build/ml/mnist.o: include/ml/mnist.h include/ui/fileio.h
build/ml/layer.o: include/ml/layer.h include/ui/fileio.h include/ml/util.h
build/ml/judge.o: include/ml/judge.h
build/ml/waves.o: include/ml/waves.h include/math/ft.h include/change/pitch.h
build/ml/waves.o: lib/Wstream/include/wstream/wstream.h include/ui/fileio.h
build/ml/ft.o: include/ml/ft.h include/ml/layer.h include/ui/fileio.h
build/ml/ft.o: include/ml/util.h include/math/ft.h
build/ml/stack.o: include/ml/stack.h include/ml/layer.h include/ui/fileio.h
build/ml/stack.o: include/ml/util.h include/ml/judge.h include/ml/factory.h
build/math/ft.o: include/math/ft.h include/math/constants.h
build/math/fft.o: include/math/fft.h include/math/constants.h
build/tools/recorder.o: include/tools/recorder.h include/app/content.h
build/tools/recorder.o: include/ui/frame.h include/ui/borders.h
build/tools/recorder.o: include/ui/style.h include/ui/window.h
build/tools/recorder.o: include/app/session.h include/ui/terminal.h
build/tools/recorder.o: include/ui/fileio.h include/ui/button.h
build/tools/recorder.o: include/ui/text.h include/ui/clock.h
build/tools/recorder.o: include/wave/cache.h include/wave/source.h
build/tools/recorder.o: include/wave/audio.h include/wave/mic.h
build/tools/recorder.o: include/wave/buffer.h include/wave/sound.h
build/tools/recorder.o: include/app/app.h include/ui/box.h include/ui/slider.h
build/tools/recorder.o: include/ui/stack.h include/app/tools.h
build/tools/recorder.o: include/app/audio.h include/wave/file.h
build/tools/recorder.o: lib/Wstream/include/wstream/wstream.h
build/tools/recorder.o: lib/Wstream/include/wstream/constants.h
build/tools/trainer.o: include/tools/trainer.h include/app/content.h
build/tools/trainer.o: include/ui/frame.h include/ui/borders.h
build/tools/trainer.o: include/ui/style.h include/ui/window.h
build/tools/trainer.o: include/app/session.h include/ui/terminal.h
build/tools/trainer.o: include/ui/fileio.h include/tools/graph.h
build/tools/trainer.o: include/ml/stack.h include/ml/layer.h include/ml/util.h
build/tools/trainer.o: include/ml/judge.h include/ui/slider.h
build/tools/trainer.o: include/ui/stack.h include/ui/text.h include/app/app.h
build/tools/trainer.o: include/ui/box.h include/ui/button.h include/app/tools.h
build/tools/trainer.o: include/app/creations.h include/ui/clock.h
build/tools/meditor.o: include/tools/meditor.h include/app/content.h
build/tools/meditor.o: include/ui/frame.h include/ui/borders.h
build/tools/meditor.o: include/ui/style.h include/ui/window.h
build/tools/meditor.o: include/app/session.h include/ui/terminal.h
build/tools/meditor.o: include/ui/fileio.h include/app/audio.h
build/tools/meditor.o: include/wave/source.h include/wave/cache.h
build/tools/meditor.o: include/wave/audio.h include/wave/file.h
build/tools/meditor.o: lib/Wstream/include/wstream/wstream.h
build/tools/meditor.o: include/tools/graph.h include/ui/slider.h
build/tools/meditor.o: include/ui/stack.h include/ui/text.h
build/tools/meditor.o: include/change/matrix.h include/app/app.h
build/tools/meditor.o: include/ui/box.h include/ui/button.h include/app/tools.h
build/tools/meditor.o: include/math/fft.h include/math/ft.h
build/tools/meditor.o: include/math/constants.h include/app/creations.h
build/tools/meditor.o: include/change/pitch.h
build/tools/analyzer.o: include/tools/analyzer.h include/ui/button.h
build/tools/analyzer.o: include/ui/text.h include/ui/frame.h
build/tools/analyzer.o: include/ui/borders.h include/ui/style.h
build/tools/analyzer.o: include/ui/window.h include/ui/slider.h
build/tools/analyzer.o: include/ui/stack.h include/ui/terminal.h
build/tools/analyzer.o: include/app/session.h include/ui/fileio.h
build/tools/analyzer.o: include/app/content.h include/app/audio.h
build/tools/analyzer.o: include/wave/source.h include/wave/cache.h
build/tools/analyzer.o: include/wave/audio.h include/wave/file.h
build/tools/analyzer.o: lib/Wstream/include/wstream/wstream.h
build/tools/analyzer.o: include/tools/graph.h include/wave/sound.h
build/tools/analyzer.o: include/wave/buffer.h include/change/pitch.h
build/tools/analyzer.o: include/app/app.h include/ui/box.h include/app/tools.h
build/tools/analyzer.o: include/app/creations.h include/math/fft.h
build/tools/analyzer.o: include/math/ft.h
build/tools/graph.o: include/tools/graph.h include/ui/frame.h
build/tools/graph.o: include/ui/borders.h include/ui/style.h
build/tools/graph.o: include/ui/window.h include/app/session.h
build/tools/graph.o: include/ui/terminal.h include/ui/fileio.h
build/tools/graph.o: include/app/app.h include/ui/box.h include/ui/slider.h
build/tools/graph.o: include/ui/stack.h include/ui/text.h include/ui/button.h
build/tools/graph.o: include/app/tools.h include/math/constants.h
build/ui/text.o: include/ui/text.h include/ui/frame.h include/ui/borders.h
build/ui/text.o: include/ui/style.h include/ui/window.h
build/ui/borders.o: include/ui/borders.h include/ui/style.h include/ui/frame.h
build/ui/borders.o: include/ui/window.h
build/ui/fileio.o: include/ui/fileio.h
build/ui/knob.o: include/ui/knob.h include/ui/frame.h include/ui/borders.h
build/ui/knob.o: include/ui/style.h include/ui/window.h
build/ui/style.o: include/ui/style.h
build/ui/scroll.o: include/ui/scroll.h include/ui/frame.h include/ui/borders.h
build/ui/scroll.o: include/ui/style.h include/ui/window.h
build/ui/button.o: include/ui/button.h include/ui/text.h include/ui/frame.h
build/ui/button.o: include/ui/borders.h include/ui/style.h include/ui/window.h
build/ui/window.o: include/ui/window.h include/ui/frame.h include/ui/borders.h
build/ui/window.o: include/ui/style.h include/ui/clock.h
build/ui/clock.o: include/ui/clock.h
build/ui/frame.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/ui/frame.o: include/ui/window.h
build/ui/slider.o: include/ui/slider.h include/ui/stack.h include/ui/frame.h
build/ui/slider.o: include/ui/borders.h include/ui/style.h include/ui/window.h
build/ui/slider.o: include/ui/text.h
build/ui/stack.o: include/ui/stack.h include/ui/frame.h include/ui/borders.h
build/ui/stack.o: include/ui/style.h include/ui/window.h
build/ui/terminal.o: include/ui/terminal.h include/ui/frame.h
build/ui/terminal.o: include/ui/borders.h include/ui/style.h
build/ui/terminal.o: include/ui/window.h
build/ui/box.o: include/ui/box.h include/ui/slider.h include/ui/stack.h
build/ui/box.o: include/ui/frame.h include/ui/borders.h include/ui/style.h
build/ui/box.o: include/ui/window.h include/ui/text.h include/ui/button.h
build/main.o: include/app/app.h include/ui/window.h include/ui/frame.h
build/main.o: include/ui/borders.h include/ui/style.h include/ui/box.h
build/main.o: include/ui/slider.h include/ui/stack.h include/ui/text.h
build/main.o: include/ui/button.h include/ui/terminal.h include/app/tools.h
