#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;
./autogen.sh
./configure --prefix=${VENDOR_INSTALL_PREFIX}
make -j10 ;make install;

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libprotobuf-lite.a")
RESULT_FILES+=("libprotobuf-lite.la")
RESULT_FILES+=("libprotobuf-lite.so")
RESULT_FILES+=("libprotobuf-lite.so.15")
RESULT_FILES+=("libprotobuf-lite.so.15.0.0")
RESULT_FILES+=("libprotobuf.a")
RESULT_FILES+=("libprotobuf.la")
RESULT_FILES+=("libprotobuf.so")
RESULT_FILES+=("libprotobuf.so.15")
RESULT_FILES+=("libprotobuf.so.15.0.0")
RESULT_FILES+=("libprotoc.a")
RESULT_FILES+=("libprotoc.la")
RESULT_FILES+=("libprotoc.so")
RESULT_FILES+=("libprotoc.so.15")
RESULT_FILES+=("libprotoc.so.15.0.0")
# function all
EXISTS

