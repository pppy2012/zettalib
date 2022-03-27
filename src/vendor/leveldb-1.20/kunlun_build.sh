#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;
make -j10;
cp out-static/*.a ${VENDOR_INSTALL_PREFIX}/lib
cp -r include/* ${VENDOR_INSTALL_PREFIX}/include

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libleveldb.a")
# function all
EXISTS
