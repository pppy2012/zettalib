#include "op_log.h"

using namespace kunlun;

int main(int argc, char** argv) {
    Op_Log::getInstance()->init("./test", "op_test", 10);

    for(int i = 0; i<10000; i++) 
        KLOG_INFO("***********************this is info log record, =========param: {}", i);

    return 0;
}