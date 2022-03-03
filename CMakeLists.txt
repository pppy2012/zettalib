cmake_minimum_required(VERSION 3.0.0)
project(zettalib VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-fPIC)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
add_definitions(-DZETTA_LIB_COMPILER)

set(CMAKE_BUILD_TYPE "Debug")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -DENABLE_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -DENABLE_DEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O2")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -Wall -O2")
endif()

add_subdirectory(src/op_mysql)
add_subdirectory(src/zthread)
add_subdirectory(src/procenv)
add_subdirectory(src/error)
add_subdirectory(src/tool_func)


configure_file(src/sys_config.h.in sys_config.h)
add_library(zettalib STATIC src/sys_config.cc)

target_include_directories(zettalib PUBLIC
 "${PROJECT_BINARY_DIR}"
 "${PROJECT_SOURCE_DIR}/src"
 "${PROJECT_SOURCE_DIR}/src/vendor/include"
)
link_directories(${CMAKE_SOURCE_DIR}/src/vendor/lib)
target_link_libraries(zettalib op_mysql procenv error zthread tool_func)

set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/output")

INSTALL(TARGETS zettalib ARCHIVE DESTINATION lib)
INSTALL(FILES 
  "${ERRORCUP_INSTALL_FIELS}" 
  "${OP_MYSQL_INSTALL_FIELS}" 
  "${PROJECT_SOURCE_DIR}/src/procenv/biodirectpopen.h"
  "${PROJECT_SOURCE_DIR}/src/procenv/proc_env.h"
  "${ZTHREAD_INSTALL_FIELS}" 
  "${TOOL_FUNC_INSTALL_FIELS}" 
  DESTINATION include/zettalib)
