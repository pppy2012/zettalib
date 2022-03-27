#!/bin/sh

cd ../
source ./common.sh
cd -

rm -r build;
mkdir build && cd build;
cmake ../ -DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}
make -j10;make install;

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libgflags.a")
RESULT_FILES+=("libgflags_nothreads.a")
# function all
EXISTS

