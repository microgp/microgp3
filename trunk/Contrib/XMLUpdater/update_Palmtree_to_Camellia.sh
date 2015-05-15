#!/bin/bash

# TODO check arguments, show help

cwd=$(pwd)
DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
cd "$cwd"

orig="$1"

cp "$orig" "${orig}.bak"

ns=$(mktemp)
up=$(mktemp)

xsltproc -o "$ns" "$DIR/XSL/strip_namespace.xsl" "$orig"
xsltproc -o "$up" "$DIR/XSL/Palmtree_to_Camellia.xsl" "$ns"
xsltproc -o "$orig" "$DIR/XSL/restore_namespace.xsl" "$up"

rm "$up" "$ns"

