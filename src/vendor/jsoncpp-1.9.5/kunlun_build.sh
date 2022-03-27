#!/bin/sh

cd ../
source ./common.sh
cd -

rm -r build;
mkdir build && cd build;
cmake ../ -DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}
make -j10;make install;
mv ${VENDOR_INSTALL_PREFIX}/lib64/*.a ${VENDOR_INSTALL_PREFIX}/lib
mv ${VENDOR_INSTALL_PREFIX}/lib64/*.so* ${VENDOR_INSTALL_PREFIX}/lib
rm -r ${VENDOR_INSTALL_PREFIX}/lib64

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=`ls ${VENDOR_INSTALL_PREFIX}/lib/ | grep jsoncpp`
# function all
EXISTS
