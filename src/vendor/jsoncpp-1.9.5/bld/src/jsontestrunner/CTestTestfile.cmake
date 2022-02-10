# CMake generated Testfile for 
# Source directory: /home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner
# Build directory: /home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/bld/src/jsontestrunner
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(jsoncpp_readerwriter "/usr/bin/python3.6" "-B" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/runjsontests.py" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/bld/bin/jsontestrunner_exe" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter PROPERTIES  WORKING_DIRECTORY "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/CMakeLists.txt;43;add_test;/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/CMakeLists.txt;0;")
add_test(jsoncpp_readerwriter_json_checker "/usr/bin/python3.6" "-B" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/runjsontests.py" "--with-json-checker" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/bld/bin/jsontestrunner_exe" "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter_json_checker PROPERTIES  WORKING_DIRECTORY "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/CMakeLists.txt;47;add_test;/home/summerxwu/code/kunlun/zettalib/vendor/jsoncpp-1.9.5/src/jsontestrunner/CMakeLists.txt;0;")
