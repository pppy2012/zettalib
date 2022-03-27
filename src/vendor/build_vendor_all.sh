#!/bin/sh

source ./common.sh
CURPATH=`pwd`
BUILD_LOG_PATH="${CURPATH}/build_log"
rm -r ${BUILD_LOG_PATH}
mkdir -p ${BUILD_LOG_PATH}
rm -r ${CURPATH}/vendor_output
for dir in ${VENDOR_LIST[@]}
do
    echo "=================================="
    echo "Start build ${dir}"
    echo "=================================="
    sleep 2
    cd ${dir} && sh ./kunlun_build.sh > ${BUILD_LOG_PATH}/${dir}_build_log 2>&1
    if [ $? == 0 ]
    then
      echo "=================================="
      echo "Finish build ${dir}"
      echo "=================================="
      cd ${CURPATH}
      sleep 1
    else
      echo "=================================="
      echo "Build ${dir} faild"
      echo "=================================="
      echo "Check detail in output log ${BUILD_LOG_PATH}/${dir}_build_log"
      cd ${CURPATH}
    fi
done 
