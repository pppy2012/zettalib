#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;
sh ./config_brpc.sh --libs="${VENDOR_INSTALL_PREFIX}/lib ${VENDOR_INSTALL_PREFIX}/lib64 ${VENDOR_INSTALL_PREFIX}/bin" --headers="${VENDOR_INSTALL_PREFIX}/include"
make -j10;
cp -r ./output/bin/* ${VENDOR_INSTALL_PREFIX}/bin
cp -r ./output/include/* ${VENDOR_INSTALL_PREFIX}/include
cp -r ./output/lib/* ${VENDOR_INSTALL_PREFIX}/lib

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libbrpc.a")
# function all
EXISTS
