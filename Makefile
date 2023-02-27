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
RELEASEFLAGS := -std=c++17 -O3 -flto -funroll-loops -mavx2 -Wall



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

build/designb/wavelet.o: include/designb/wavelet.h include/designb/common.h
build/designb/splitter.o: include/designb/splitter.h include/designb/common.h
build/designb/splitter.o: include/designb/math.h
build/designb/common.o: include/designb/common.h
build/designb/math.o: include/designb/math.h
build/designd/common.o: include/designd/common.h
build/designd/pacer.o: include/designd/pacer.h include/designd/common.h
build/designd/pacer.o: include/designd/math.h
build/designd/resampler.o: include/designd/resampler.h
build/designd/math.o: include/designd/math.h
build/designd/painter.o: include/designd/painter.h include/designd/common.h
build/designd/painter.o: include/designd/math.h
build/designd/equalizer.o: include/designd/equalizer.h include/designd/common.h
build/designd/equalizer.o: include/designd/math.h
build/designd/binder.o: include/designd/binder.h
build/designd/knot.o: include/designd/knot.h include/designd/pacer.h
build/designd/knot.o: include/designd/resampler.h include/designd/painter.h
build/designd/knot.o: include/designd/equalizer.h include/designd/binder.h
build/designc/common.o: include/designc/common.h
build/designc/pacer.o: include/designc/pacer.h include/designc/common.h
build/designc/pacer.o: include/designc/math.h
build/designc/resampler.o: include/designc/resampler.h
build/designc/math.o: include/designc/math.h
build/designc/painter.o: include/designc/painter.h include/designc/common.h
build/designc/painter.o: include/designc/math.h
build/designc/equalizer.o: include/designc/equalizer.h include/designc/common.h
build/designc/equalizer.o: include/designc/math.h
build/designc/binder.o: include/designc/binder.h
build/designc/knot.o: include/designc/knot.h include/designc/pacer.h
build/designc/knot.o: include/designc/resampler.h include/designc/painter.h
build/designc/knot.o: include/designc/equalizer.h include/designc/binder.h
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
build/change/pitcher.o: include/change/pitcher.h include/math/sinc.h
build/change/changer2.o: include/change/changer2.h include/change/util.h
build/change/changer2.o: include/change/pitch.h include/math/constants.h
build/change/changer2.o: include/math/fft.h include/math/ft.h
build/change/phaser3.o: include/change/phaser3.h include/math/constants.h
build/change/phaser3.o: include/math/fft.h
build/change/pitcher2.o: include/change/pitcher2.h include/math/constants.h
build/change/changer3.o: include/change/changer3.h include/math/constants.h
build/change/changer3.o: include/math/fft.h
build/change/util.o: include/change/util.h
build/change/matrix.o: include/change/matrix.h include/math/constants.h
build/change/phaser2.o: include/change/phaser2.h include/math/fft.h
build/change/phaser2.o: include/change/util.h
build/change/detector2.o: include/change/detector2.h include/change/pitch.h
build/change/detector2.o: include/math/constants.h include/math/fft.h
build/change/detector2.o: include/math/ft.h
build/change/detector.o: include/change/detector.h include/math/fft.h
build/change/detector.o: include/math/ft.h
build/change/resample.o: include/change/resample.h include/math/constants.h
build/change/changer1.o: include/change/changer1.h include/change/util.h
build/change/changer1.o: include/math/ft.h include/math/constants.h
build/change/phaser.o: include/change/phaser.h include/math/fft.h
build/change/phaser.o: include/math/constants.h
build/change/tests.o: include/change/tests.h include/change/pitcher2.h
build/change/tests.o: include/change/phaser.h include/change/phaser2.h
build/change/tests.o: include/change/phaser4.h include/math/fft.h
build/change/tests.o: include/math/constants.h include/change/util.h
build/change/tests.o: include/change/pitch.h include/designc/knot.h
build/change/tests.o: include/designc/pacer.h include/designc/resampler.h
build/change/tests.o: include/designc/painter.h include/designc/equalizer.h
build/change/tests.o: include/designc/binder.h include/designa/math.h
build/change/tests.o: include/designd/knot.h include/designd/pacer.h
build/change/tests.o: include/designd/resampler.h include/designd/painter.h
build/change/tests.o: include/designd/equalizer.h include/designd/binder.h
build/change/pitch.o: include/change/pitch.h include/math/fft.h
build/change/pitch.o: include/math/ft.h include/math/sinc.h
build/change/pitch.o: include/math/constants.h include/ml/stack.h
build/change/pitch.o: include/ml/layer.h include/ui/fileio.h include/ml/util.h
build/change/pitch.o: include/ml/judge.h include/ml/db.h
build/change/pitch.o: include/change/detector2.h include/change/pitcher2.h
build/change/pitch.o: include/change/phaser2.h include/change/phaser.h
build/change/pitch.o: include/change/util.h include/change/tests.h
build/change/pitch.o: include/designa/math.h include/designa/color.h
build/change/pitch.o: include/designb/common.h include/designb/splitter.h
build/change/pitch.o: include/designb/wavelet.h include/designb/math.h
build/change/pitch.o: include/designc/knot.h include/designc/pacer.h
build/change/pitch.o: include/designc/resampler.h include/designc/painter.h
build/change/pitch.o: include/designc/equalizer.h include/designc/binder.h
build/change/phaser4.o: include/change/phaser4.h include/math/fft.h
build/change/pitcher3.o: include/change/pitcher3.h include/math/constants.h
build/change/pitcher3.o: include/math/fft.h include/change/resample.h
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
build/app/content.o: include/ml/judge.h include/ml/db.h include/tools/canvas.h
build/app/content.o: include/tools/labler.h
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
build/app/creations.o: include/app/tools.h include/ml/mnist.h include/ml/db.h
build/app/creations.o: include/ml/stack.h include/ml/layer.h include/ml/util.h
build/app/creations.o: include/ml/judge.h include/ml/waves.h
build/ml/roll.o: include/ml/roll.h include/ml/layer.h include/ui/fileio.h
build/ml/roll.o: include/ml/util.h
build/ml/factory.o: include/ml/factory.h include/ml/util.h include/ml/layer.h
build/ml/factory.o: include/ui/fileio.h include/ml/field.h include/ml/field.inl
build/ml/factory.o: include/ml/matrix.h include/ml/deloc.h include/ml/roll.h
build/ml/factory.o: include/ml/ft.h include/ml/judge.h include/ml/norm.h
build/ml/factory.o: include/ml/reblock.h include/ml/multiply.h
build/ml/factory.o: include/ml/phase.h include/ml/control.h
build/ml/field.o: include/ml/field.h include/ml/layer.h include/ui/fileio.h
build/ml/field.o: include/ml/util.h include/ml/field.inl
build/ml/norm.o: include/ml/norm.h include/ml/layer.h include/ui/fileio.h
build/ml/norm.o: include/ml/util.h
build/ml/deloc.o: include/ml/deloc.h include/ml/layer.h include/ui/fileio.h
build/ml/deloc.o: include/ml/util.h
build/ml/phase.o: include/ml/phase.h include/ml/layer.h include/ui/fileio.h
build/ml/phase.o: include/ml/util.h
build/ml/util.o: include/ml/util.h
build/ml/control.o: include/ml/control.h include/ml/layer.h include/ui/fileio.h
build/ml/control.o: include/ml/util.h
build/ml/matrix.o: include/ml/matrix.h include/ml/layer.h include/ui/fileio.h
build/ml/matrix.o: include/ml/util.h
build/ml/mnist.o: include/ml/mnist.h include/ml/db.h include/ui/fileio.h
build/ml/mnist.o: include/math/fft.h include/ml/util.h
build/ml/layer.o: include/ml/layer.h include/ui/fileio.h include/ml/util.h
build/ml/judge.o: include/ml/judge.h
build/ml/multiply.o: include/ml/multiply.h include/ml/layer.h
build/ml/multiply.o: include/ui/fileio.h include/ml/util.h
build/ml/mlml.o: include/ml/mlml.h include/ml/stack.h include/ml/layer.h
build/ml/mlml.o: include/ui/fileio.h include/ml/util.h include/ml/judge.h
build/ml/mlml.o: include/ml/db.h
build/ml/waves.o: include/ml/waves.h include/ml/db.h include/ui/fileio.h
build/ml/waves.o: include/math/ft.h include/change/pitch.h
build/ml/waves.o: lib/Wstream/include/wstream/wstream.h
build/ml/waves.o: include/change/detector.h include/math/constants.h
build/ml/waves.o: include/ml/util.h
build/ml/ft.o: include/ml/ft.h include/ml/layer.h include/ui/fileio.h
build/ml/ft.o: include/ml/util.h include/math/ft.h
build/ml/stack.o: include/ml/stack.h include/ml/layer.h include/ui/fileio.h
build/ml/stack.o: include/ml/util.h include/ml/judge.h include/ml/db.h
build/ml/stack.o: include/ml/factory.h
build/ml/db.o: include/ml/db.h include/ml/util.h
build/ml/reblock.o: include/ml/reblock.h include/ml/layer.h include/ui/fileio.h
build/ml/reblock.o: include/ml/util.h
build/math/sinc.o: include/math/sinc.h include/math/constants.h
build/math/ft.o: include/math/ft.h include/math/constants.h
build/math/fft.o: include/math/fft.h include/math/constants.h
build/math/windows.o: include/math/windows.h include/math/constants.h
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
build/tools/trainer.o: include/ml/judge.h include/ml/db.h include/ui/slider.h
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
build/tools/analyzer.o: include/math/ft.h include/math/sinc.h
build/tools/analyzer.o: include/ml/waves.h include/ml/db.h
build/tools/analyzer.o: include/math/constants.h include/change/tests.h
build/tools/graph.o: include/tools/graph.h include/ui/frame.h
build/tools/graph.o: include/ui/borders.h include/ui/style.h
build/tools/graph.o: include/ui/window.h include/app/session.h
build/tools/graph.o: include/ui/terminal.h include/ui/fileio.h
build/tools/graph.o: include/app/app.h include/ui/box.h include/ui/slider.h
build/tools/graph.o: include/ui/stack.h include/ui/text.h include/ui/button.h
build/tools/graph.o: include/app/tools.h include/math/constants.h
build/tools/labler.o: include/tools/labler.h include/app/content.h
build/tools/labler.o: include/ui/frame.h include/ui/borders.h
build/tools/labler.o: include/ui/style.h include/ui/window.h
build/tools/labler.o: include/app/session.h include/ui/terminal.h
build/tools/labler.o: include/ui/fileio.h include/app/audio.h
build/tools/labler.o: include/wave/source.h include/wave/cache.h
build/tools/labler.o: include/wave/audio.h include/wave/file.h
build/tools/labler.o: lib/Wstream/include/wstream/wstream.h
build/tools/labler.o: include/tools/graph.h include/ui/slider.h
build/tools/labler.o: include/ui/stack.h include/ui/text.h include/ui/button.h
build/tools/labler.o: include/wave/sound.h include/wave/buffer.h
build/tools/labler.o: include/app/app.h include/ui/box.h include/app/tools.h
build/tools/labler.o: include/app/creations.h include/ml/waves.h
build/tools/labler.o: include/ml/db.h include/math/ft.h
build/tools/canvas.o: include/tools/canvas.h include/app/content.h
build/tools/canvas.o: include/ui/frame.h include/ui/borders.h
build/tools/canvas.o: include/ui/style.h include/ui/window.h
build/tools/canvas.o: include/app/session.h include/ui/terminal.h
build/tools/canvas.o: include/ui/fileio.h include/tools/graph.h
build/tools/canvas.o: include/ui/slider.h include/ui/stack.h include/ui/text.h
build/tools/canvas.o: include/app/app.h include/ui/box.h include/ui/button.h
build/tools/canvas.o: include/app/tools.h include/math/constants.h
build/tools/canvas.o: include/app/audio.h include/wave/source.h
build/tools/canvas.o: include/wave/cache.h include/wave/audio.h
build/tools/canvas.o: include/wave/file.h lib/Wstream/include/wstream/wstream.h
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
build/designa/pacer.o: include/designa/pacer.h include/designa/math.h
build/designa/scolor.o: include/designa/scolor.h include/designa/color.h
build/designa/scolor.o: include/designa/math.h
build/designa/ftip.o: include/designa/ftip.h
build/designa/math.o: include/designa/math.h
build/designa/color.o: include/designa/color.h include/designa/math.h
build/designa/equalizer.o: include/designa/equalizer.h include/designa/math.h
build/designa/binder.o: include/designa/binder.h
build/designa/knot.o: include/designa/knot.h include/designa/pacer.h
build/designa/knot.o: include/designa/ftip.h include/designa/scolor.h
build/designa/knot.o: include/designa/equalizer.h include/designa/binder.h
build/designa/knot.o: include/designa/math.h
build/main.o: include/app/app.h include/ui/window.h include/ui/frame.h
build/main.o: include/ui/borders.h include/ui/style.h include/ui/box.h
build/main.o: include/ui/slider.h include/ui/stack.h include/ui/text.h
build/main.o: include/ui/button.h include/ui/terminal.h include/app/tools.h
