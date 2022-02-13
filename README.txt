A personal audio research program.

This project uses SFML

The developent stack is GNU tools (gcc, gdb, make and make depend),
vim and some simple custom scripts.

To compile & run from a fresh clone on linux:

1. Download SFML if it's not already installed. 

sudo apt-get install libsfml-dev

Or use whatever the package manager is that you are using.
The official SFML website has tutorials on installation.

2. Update submodules

git submodule init
git submodule update

3. Compile submodules

Currently the project has only one submodule: Wstream.
go to it's root directory lib/Wstream and run make

cd lib/Wstream
make

4. Compile the project

run make in the project's root directory

cd -
make

5. Run the program

./waveha

