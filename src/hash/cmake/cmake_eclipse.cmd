rem cmake -G "Eclipse CDT4 - MinGW Makefiles" -D "BOOST_ROOT=D:\source\boost_1_59_0" . 

rem For MSYS2 do the following
rem Add MSYS2\usr\bin and MSYS2\mingw64\bin to PATH (install mingw, boost, make via pacman)
rem Generate project using CDT4 Unix Makefiles
rem Set the proper BOOST_ROOT (download boost and use a local copy)
rem Use local cmake, i.e. download and unpacke windows cmake
rem the Final command may look like the following line

C:\Programs\cmake-3.7.0-rc1-win64-x64\bin\cmake -G "Eclipse CDT4 - Unix Makefiles" -D "BOOST_ROOT=C:\Users\babky\Documents\source\boost_1_62_0" . 
