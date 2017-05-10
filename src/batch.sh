#!/bin/bash 

#for a in `find . \( -type f \! -path "*.svn*" \) -print`; do
#    echo $a
#    perl -pi.update -e 's|\(c\) 2002-20.. Giovanni Squillero|(c) 2002-2016 Giovanni Squillero|' $a
#done

for a in `find . \( -iname "*.cc" -o -iname "*.c" -o -iname "*.cpp" -o -iname "*.h" -o -iname "*.rc" -o -iname "*.txt" -o -iname "*.pl" -o -iname "*.sh" -o -iname Makefile -o -iname AUTHORS -o -iname COPYING -o -iname INSTALL -o -iname NEWS -o -iname "README*" \) -print`; do
    #echo $a
    dos2unix $a
    svn propset copyright "(c) 2002-2016 Giovanni Squillero <giovanni.squillero@polito.it>" $a
    svn propset license "GNU General Public License v3+ (see COPYING)" $a
    svn propset svn:keywords "Id Date Author Revision LastChangedDate" $a
    svn propset svn:eol-style native $a
    echo " "
done

for a in `find . \( -iname "*.pl" -o -iname "*.sh" \) -print`; do
    svn propset svn:executable yes $a
done

#for a in `find . \( -iname "*.cc" -o -iname "*.h" -o -iname "*.txt" -o -iname Makefile -o -iname CMakeLists.txt \) -print`; do
#    echo $a
#    ./UpdateTeam.sh $a
#done

for a in `find Samples \( -iname "*.cc" -o -iname "*.c" -o -iname "*.cpp" -o -iname "*.h" -o -iname "*.txt" -o -iname Makefile -o -iname AUTHORS -o -iname COPYING -o -iname INSTALL -o -iname NEWS -o -iname README \) -print`; do
    svn propdel copyright $a
    svn propset svn:eol-style native $a
    svn propset svn:keywords "Id Date Author Revision LastChangedDate" $a
    echo " "
done

for a in `find Contrib \( -iname "*.cc" -o -iname "*.h" -o -iname "*.txt" -o -iname Makefile -o -iname CMakeLists.txt -o -iname README \) -print`; do
    svn propdel copyright $a
done
