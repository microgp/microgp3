#!/bin/bash

echo "Generating doc files"
find Libs -name "*.h" -print -exec docify {} {}.doc++ \;
find Frontends -name "*.h" -print -exec docify {} {}.doc++ \;
doc++ --all --filenames  --full-toc --dir Docs/Html `find . -name "*.doc++" -print`
find . -name "*.doc++" -exec rm {} \;
