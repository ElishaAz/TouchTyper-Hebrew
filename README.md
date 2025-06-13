A Hebrew Touch-Typing Tutor, based on https://github.com/siddharthroy12/TouchTyper

# Build Instructions

## Building for Web

**You need to install Emscripten** [Download Here!](https://emscripten.org/docs/getting_started/downloads.html)

1. Clone the repository using Git. Execute this in a terminal: `git clone https://github.com/ElishaAz/TouchTyper-Hebrew`
2. Cd into TouchTyper. `cd TouchTyper`
3. Install the raylib git submodule using `git submodule update --init --recursive --depth=1`
4. Make a build folder. Your builds will go here. `mkdir build; cd build`
5. Setup cmake `emcmake cmake -S .. -D CMAKE_BUILD_TYPE=Release`
6. Run `cmake --build .` to compile the project
7. Run a local web server and open the TouchTyper.html

## Building (Linux/macOS)

1. Clone the repository using `git clone https://github.com/ElishaAz/TouchTyper-Hebrew`
2. Move to the TouchTyper directory
`cd TouchTyper`
3. Install the raylib git submodule using `git submodule update --init --recursive --depth=1`
4. Make a build folder and cd to it with `mkdir build && cd build`
5. Setup cmake `cmake .. -DCMAKE_BUILD_TYPE=Release`
6. Run `make` to compile the project
7. Make the binary executable `chmod +x TouchTyper`
8. Run the binary with `./TouchTyper`

## Building (Windows, MinGW)

**You need to install MinGW. Select make and developer tools in the installer** [Download MinGW here!](https://sourceforge.net/projects/mingw/)

1. Clone the repository using Git. Execute this in a terminal: `git clone https://github.com/ElishaAz/TouchTyper-Hebrew`
2. Cd into TouchTyper. `cd TouchTyper`
3. Install the raylib git submodule using `git submodule update --init --recursive --depth=1`
4. Make a build folder. Your builds will go here. `mkdir build; cd build`
5. Setup CMake. **Make sure to define MinGW Makefiles if you are using MinGW!** `cmake .. -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"`
6. Make sure you have mingw32-make. If you do, just run it in the build directory and it will start compiling everything. It won't take long, and once it's finished, then
7. Run TouchTyper.exe! You have just compiled TouchTyper for Windows using MinGW.

## Building (Windows, MSVC)

You need to have Visual Studio 2019 (other versions aren't tested, please tell us if it works for you) with C++ Development selected in the installer.

1. Clone the repository using Git. Execute this in a terminal: `git clone https://github.com/ElishaAz/TouchTyper-Hebrew`
2. Cd into TouchTyper. `cd TouchTyper`
3. Install the raylib git submodule using `git submodule update --init --recursive --depth=1`
4. Make a build folder. Your builds will go here. `mkdir build; cd build`
5. Setup CMake. `cmake .. -DCMAKE_BUILD_TYPE=Release`
6. Let's build the project! Run `cmake --build .`
7. Go into Debug, your build of TouchTyper is there. You have now compiled TouchTyper for Windows using MSVC.
