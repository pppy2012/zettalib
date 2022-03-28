# Introduction

Zettalib is the common library used by kunlun distributed database moudle. Excepte the utility object, Zettalib also contains the third party dependence.

# Building from the source

## Building Vendor

A shell script is provided called build_vendor_all.sh laying under src/vendor directorys.

Just Run the script to build the vendor moudles.

> cd src/vendor && sh ./build_vendor_all.sh

All the output will be placed under src/vendor/vendor_output directory.
The compiling log will be redirected to src/vendor/build_log directory.

## Building Zettalib

Use cmake to build. Typically one should create a build directory in source root directory, call it 'build', then in build, do

> cmake .. && make -j8

The install_prefix is build/output by default. You can change it by specify the -DCMAKE_INSTALL_PREFIX= prefix

# Usage

Zettalib and its vendor dependence is used by cluster_mgr and node_mgr, which is the cluster managing moudle and server node managing moudle respectivly.

To compile the cluster_mgr and node_mgr, Zettalib should be compiled successfully by the using the instructions metioned above.

The result paths include 'build/output' (zettalib default output prefix) and 'src/vendor/vendor_output' (vendor dependence compiled result install prefix) should be used by cluster_mgr and node_mgr to fill the library_path and include_path.
