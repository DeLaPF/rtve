:: Build Debug
if not exist debug mkdir debug
cd debug
cmake -S ../ -B . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
mingw32-make.exe
cd ..

:: Build Release
if not exist release mkdir release
cd release
cmake -S ../ -B . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make.exe
cd ..

