# Polaris

# Build
Begin by creating a new folder called build using `mkdir build`. Then `cd build`. This project
uses CMake. In my case, I want it to make me Mingw-Makefiles so I will run `cmake .. -G MinGW Makefiles`. 
If you want to check if your platform is supported, run `cmake --help`. In this case with MinGW I will then
just run `mingw32-make` and will finally have the executable. A simple test would be `./polaris "../tests/basic.pol"` 
or you can run `ctest`. To specifiy a specific build, add the option `-DCMAKE_BUILD_TYPE=Debug` or `-DCMAKE_BUID_TYPE=Release`.
