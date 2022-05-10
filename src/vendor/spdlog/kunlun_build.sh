#!/bin/sh

cd ../
source ./common.sh
cd -

rm -r build;
mkdir build && cd build;
cmake ../ -DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}
make -j10;make install;
cp -r ${VENDOR_INSTALL_PREFIX}/lib64/* ${VENDOR_INSTALL_PREFIX}/lib
rm -r ${VENDOR_INSTALL_PREFIX}/lib64


