#!/bin/sh

cd ../
source ./common.sh
cd -


make clean;
autoreconf -ivf
./configure --prefix=${VENDOR_INSTALL_PREFIX} --disable-openssl
make -j10;
make install

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libevent.a")
RESULT_FILES+=("libevent.so")
# function all
EXISTS

