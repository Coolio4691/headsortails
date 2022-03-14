mkdir -p build
cd build
cmake ..
make
echo "" # create new line just for looks
./HeadsOrTails $@ 
cd ..