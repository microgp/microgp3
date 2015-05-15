#!/bin/bash
##############################################################################
#               * Giovanni Squillero, Ph.D.                                  #
#    /\   /\    * Politecnico di Torino - Dip. Automatica e Informatica      #
#   /     \     * Cso Duca degli Abruzzi 24 / I-10129 TORINO / ITALY         #
#   \ \    \    *                                                            #
#    \/.  \/.   * Tel: +39-011-564.7186  /  Fax: +39-011-564.7099            #
#               *                                                            #
#               * email: giovanni.squillero@polito.it                        #
#      (!)      * www  : http://www.cad.polito.it/staff/squillero/           #
##############################################################################

tmp=/tmp/ugp3.$$
pwd=$PWD
version=`./GetVersion.pl`

if [[ $(svn status Samples | grep ^M) ]]; then
    echo Local modifications in Samples: a commit is needed...
    svn status | grep ^M
fi

echo -n Invoking make for MicroGP v$version \(`./GetVersion.pl -n`\)...
cmake -DCMAKE_BUILD_TYPE=XWIN32 . >/dev/null 2>&1
make -j8 >/dev/null 2>&1
echo ' done.'

echo Cleaning Samples...
svn-clean Samples

echo Copying files to $tmp...
mkdir $tmp
find . -iname "*.exe" -exec echo "    {}" \; -exec cp {} $tmp \;
cp NEWS* AUTHORS COPYING README* $tmp
cp -pr Samples $tmp
find $tmp -name .svn -exec rm -rf {} \;
touch ${tmp}/00_THIS_IS_MicroGP_v${version}
find $tmp -type f -exec unix2dos {} \;

echo -n Stripping \'n packing \(hold on\)...
cd $tmp;
for f in *.exe; do 
    i586-mingw32msvc-strip $f
    upx --force -qqq $f &
    #upx -qqq $f &
done
wait
echo ' done.'

echo UPX compression report
upx -qq -l *.exe
echo ' '

tarball=ugp3-${version}-win32.zip
echo Creating archive...
7z -tzip -mx9 a $tarball *
cp $tarball $pwd

echo -n Switching back to normal build...
cd $pwd
cmake -DCMAKE_BUILD_TYPE=RELEASE . >/dev/null 2>&1
echo ' done.'

echo -n Cleaning up...
rm -Rf $tmp

echo win32/$tarball >LATEST_WIN32
./GetVersion.pl -D >LATEST_WIN32_DESCRIPTION

echo ' done.'

if [ "$1" == "commit" ]; then
    echo " " 
    echo "Sending files to sourceforge..." 
    scp LATEST_WIN32* ${USER},ugp3@web.sourceforge.net:/home/project-web/ugp3/htdocs
    scp $tarball ${USER},ugp3@frs.sourceforge.net:/home/pfs/project/u/ug/ugp3/win32
fi
