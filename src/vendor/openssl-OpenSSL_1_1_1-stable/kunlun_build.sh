#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;
./config
make -j10;
cp ./*.a ${VENDOR_INSTALL_PREFIX}/lib
cp ./*.so* ${VENDOR_INSTALL_PREFIX}/lib
cp -r ./include/* ${VENDOR_INSTALL_PREFIX}/include

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libssl.a" "libssl.so" "libssl.so.1.1")
RESULT_FILES+=("libcrypto.a" "libcrypto.so" "libcrypto.so.1.1")
# function all
EXISTS
