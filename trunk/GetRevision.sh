#!/bin/sh

REV=`svn info | awk '/Revision.?:/ { print $NF }'`
echo $REV
