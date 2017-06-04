#!/bin/sh

echo "Do not forget to remove line 158 in FindParadiseo.cmake which caused problems."
cmake -DCMAKE_MODULE_PATH=~/source/paradiseo/cmake/module/ -DPARADISEO_ROOT=~/source/paradiseo/ -DBUILD_DIR=build -DCMAKE_BUILD_TYPE=Release .

