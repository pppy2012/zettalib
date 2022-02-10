# This script recursively copies all ReST documents from the source directory to
# the binary directory. CMAKE_CURRENT_SOURCE_DIR and SOURCES_DIR are substituted
# upon the cmake stage. The script is executed upon the make stage to ensure
# that the binary sources directory is always up to date.

file(GLOB SOURCES
  RELATIVE "/home/summerxwu/code/kunlun/zettalib/vendor/mydumper/docs"
  "/home/summerxwu/code/kunlun/zettalib/vendor/mydumper/docs/*.rst"
)
foreach(source ${SOURCES})
  configure_file(
    "/home/summerxwu/code/kunlun/zettalib/vendor/mydumper/docs/${source}"
    "/home/summerxwu/code/kunlun/zettalib/vendor/mydumper/build/docs/_sources/${source}"
    COPYONLY
    )
endforeach(source)
