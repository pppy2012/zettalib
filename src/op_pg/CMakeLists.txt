file(GLOB OP_PG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB OP_PG_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(OP_PG_INSTALL_FIELS "${OP_PG_HEADER}" CACHE INTERNAL "VAR")
set(OP_PG_SOURCE_FIELS "${OP_PG_SRC}" CACHE INTERNAL "VAR")

add_library(op_pg OBJECT ${OP_PG_SRC} ${OP_PG_HEADER})
target_include_directories(op_pg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(op_pg PUBLIC 
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/vendor/vendor_output/include"
  )