#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;make -j10;
cp ./src/interfaces/libpq/libpq.* ${VENDOR_INSTALL_PREFIX}/lib
tar xzf ./preserved_header.tgz -C ${VENDOR_INSTALL_PREFIX}/include 

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libpq.a")
RESULT_FILES+=("libpq.so")
# function all
EXISTS

