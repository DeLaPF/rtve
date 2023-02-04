mkdir -p unixBuild
cd unixBuild
cmake -S ../ -B . -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_EXPORT_COMPILE_COMMANDS=1
rm ../compile_commands.json && mv compile_commands.json ..
make
cd ..
