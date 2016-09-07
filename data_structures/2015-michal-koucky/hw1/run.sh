#cd cmake
#rm -rf CMakeCache.txt  CMakeFiles  cmake_install.cmake  data.out  gen-data  Makefile  solution
#cmake .
#make

time ./solution -t "$1" -i "$1"data.txt -o "$1"data.out
time ./solution -t "$1" -i "$1"data.txt -o "$1"data.out --text
time ./solution -t "$1" -i "$1"data.txt -o "$1"data.out --no-reductions


