#!/bin/sh

cd ../
source ./common.sh
cd -

rm -r build;
mkdir build && cd build;
cmake ../ -DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX} -DCMAKE_INCLUDE_PATH=${VENDOR_INSTALL_PREFIX}/include -DCMAKE_LIBRARY_PATH="${VENDOR_INSTALL_PREFIX}/lib"
make -j10;make install;


# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libbraft.a")
RESULT_FILES+=("libbraft.so")
# function all
EXISTS

