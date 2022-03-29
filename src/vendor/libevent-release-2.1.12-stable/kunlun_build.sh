#!/bin/sh

cd ../
source ./common.sh
cd -

make clean;
./autogen.sh
./configure --prefix=${VENDOR_INSTALL_PREFIX} --includedir=${VENDOR_INSTALL_PREFIX}/include/libevent
make -j10 ;make install;

# validity checking
echo "=================================="
echo "Check build ${dir}"
echo "=================================="
sleep 1
RESULT_FILES=("libevent-2.1.so.7")
RESULT_FILES+=("libevent-2.1.so.7.0.1")
RESULT_FILES+=("libevent.a")
RESULT_FILES+=("libevent_core-2.1.so.7")
RESULT_FILES+=("libevent_core-2.1.so.7.0.1")
RESULT_FILES+=("libevent_core.a")
RESULT_FILES+=("libevent_core.la")
RESULT_FILES+=("libevent_core.so")
RESULT_FILES+=("libevent_extra-2.1.so.7")
RESULT_FILES+=("libevent_extra-2.1.so.7.0.1")
RESULT_FILES+=("libevent_extra.a")
RESULT_FILES+=("libevent_extra.la")
RESULT_FILES+=("libevent_extra.so")
RESULT_FILES+=("libevent.la")
RESULT_FILES+=("libevent_openssl-2.1.so.7")
RESULT_FILES+=("libevent_openssl-2.1.so.7.0.1")
RESULT_FILES+=("libevent_openssl.a")
RESULT_FILES+=("libevent_openssl.la")
RESULT_FILES+=("libevent_openssl.so")
RESULT_FILES+=("libevent_pthreads-2.1.so.7")
RESULT_FILES+=("libevent_pthreads-2.1.so.7.0.1")
RESULT_FILES+=("libevent_pthreads.a")
RESULT_FILES+=("libevent_pthreads.la")
RESULT_FILES+=("libevent_pthreads.so")
RESULT_FILES+=("libevent.so")
# function all
EXISTS

