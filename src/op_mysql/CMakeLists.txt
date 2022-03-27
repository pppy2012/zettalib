file(GLOB OP_MYSQL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB OP_MYSQL_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(OP_MYSQL_INSTALL_FIELS "${OP_MYSQL_HEADER}" CACHE INTERNAL "VAR")
set(OP_MYSQL_SOURCE_FIELS "${OP_MYSQL_SRC}" CACHE INTERNAL "VAR")

add_library(op_mysql OBJECT ${OP_MYSQL_SRC} ${OP_MYSQL_HEADER})
target_include_directories(op_mysql INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(op_mysql PUBLIC 
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/vendor/vendor_output/include"
  )
