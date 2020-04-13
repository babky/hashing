#!/bin/bash

set -e
DIR=`dirname "$0"`
DIR=`realpath "${DIR}"`
CMAKE=${CMAKE:-cmake}

cd "${DIR}/.."
if [ ! -f vcpkg/vcpkg ]
then
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
else
  cd vcpkg
fi

# Quality enforcement
./vcpkg install gtest
./vcpkg install glog

# Libraries
./vcpkg install boost-date-time
./vcpkg install boost-log
./vcpkg install boost-program-options
./vcpkg install boost-random
./vcpkg install boost-serialization
./vcpkg install boost-system
./vcpkg install boost-thread
./vcpkg install boost-ublas

cd "${DIR}/.."
if [ ! -d "paradiseo" ]
then
  # git clone git://scm.gforge.inria.fr/paradiseo/paradiseo.git 
  git clone git@github.com:babky/paradiseo.git
  cd paradiseo
  git checkout fix-compilation-issues
  mkdir build
  $CMAKE .. -DINSTALL_TYPE=full
  make
fi 

cd "${DIR}"
$CMAKE \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MODULE_PATH="${DIR}/../paradiseo/cmake/module" \
  -DPARADISEO_ROOT="${DIR}/../paradiseo" \
  -DCMAKE_TOOLCHAIN_FILE="`pwd`/../vcpkg/scripts/buildsystems/vcpkg.cmake" \
  -DBOOST_ROOT='${DIR}/../vcpkg/installed/x64-linux/' .
