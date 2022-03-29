#!/bin/sh

VENDOR_PATH=`pwd`
VENDOR_INSTALL_PREFIX="${VENDOR_PATH}/vendor_output"

VENDOR_LIST=("gflags")
VENDOR_LIST+=("leveldb-1.20")
VENDOR_LIST+=("protobuf-3.5.0")
VENDOR_LIST+=("jsoncpp-1.9.5")
VENDOR_LIST+=("openssl-OpenSSL_1_1_1-stable")
VENDOR_LIST+=("incubator-brpc")
VENDOR_LIST+=("braft")
VENDOR_LIST+=("mariadb-10.6.7")
VENDOR_LIST+=("postgresql-11.15")
VENDOR_LIST+=("rapidjson-1.1.0")
VENDOR_LIST+=("libevent-release-2.1.12-stable")

function EXISTS() (
for file in ${RESULT_FILES[@]}
do
  test -e ${VENDOR_INSTALL_PREFIX}/lib/${file}
  if [ $? == 0 ]
  then
    echo "$file founded"
  else
    echo "$file missing"
    echo "$file should appear under ${VENDOR_INSTALL_PREFIX}/lib directory"
    return -1
  fi
done
)
