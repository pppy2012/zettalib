#!/bin/sh

cd ../
source ./common.sh
cd -

rm -r build;
mkdir build && cd build;
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX} -DWITH_SSL=${VENDOR_INSTALL_PREFIX}
cd libmariadb/libmariadb
make -j10;
mv ./libmariadb* ${VENDOR_INSTALL_PREFIX}/lib
cd - # back to build
tar xzf ../preserved_header.tgz -C ${VENDOR_INSTALL_PREFIX}/include

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libmariadb.so")
RESULT_FILES+=("libmariadb.a")
# function all
EXISTS

