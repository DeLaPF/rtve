:: Build Release
if not exist winBuild mkdir winBuild
cd winBuild
cmake -S ../ -B . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make.exe
cd ..

