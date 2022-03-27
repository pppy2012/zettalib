#/bin/bash
export GTAGSFORCECPP=1
rm GPATH GRTAGS GTAGS 
#rm tags
#ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .
gtags 
path=`pwd`
find ${path} -name "*.h*"  | xargs dirname |sed -e 's/^/-I/' | sort | uniq > .clang_complete
