#!/usr/bin/perl -w-         # -*- perl -*-
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

use File::Copy;
$TMP = "/tmp/ugp3.$$";
$PWD = `pwd` and chomp $PWD;

undef $/;

open H, "CMakeLists.txt" or die "Yeuch!";
$_ = <H>;
close H;

$VersionName = $1 if m/VERSION_NAME\s+"(\w+)"/; 
$VersionName_ = $VersionName; $VersionName_ =~ tr| |_|;
$Version = $1 if m/VERSION\s+"(\d+)"/;
$SubVersion = $1 if m/SUBVERSION\s+"(\d+)"/;
$Revision = $1 if `./GetRevision.sh` =~ m/(\d+)/;

print "Building tarball for MicroGP++ v3.${Version}.${SubVersion}_${Revision} \"${VersionName}\"\n";
print "\n";
#print "Compiling docs\n";
#`./MakeDocs.sh`;

@FILE_LIST = (
    'AUTHORS',
    'COPYING');

push @FILE_LIST, <INSTALL*>;
push @FILE_LIST, <README*>;
push @FILE_LIST, <NEWS*>;
push @FILE_LIST, <CHANGELOG*>;

push @FILE_LIST, <CMakeLists.txt>;
push @FILE_LIST, <ugp3_config.h>;

push @FILE_LIST, <Frontends/CMakeLists.txt>;
push @FILE_LIST, <Frontends/*/CMakeLists.txt>;
push @FILE_LIST, <Frontends/*/*.h>;
push @FILE_LIST, <Frontends/*/*.cc>;
push @FILE_LIST, <Frontends/*/*.cpp>;
push @FILE_LIST, <Frontends/*/*.rc>;
push @FILE_LIST, <Frontends/*/*.pro>;
push @FILE_LIST, <Frontends/*/*.ui>;

push @FILE_LIST, <Libs/CMakeLists.txt>;
push @FILE_LIST, <Libs/*/CMakeLists.txt>;
push @FILE_LIST, <Libs/*/*.h>;
push @FILE_LIST, <Libs/*/*.cc>;
push @FILE_LIST, <Libs/*/*/*.cc>;
push @FILE_LIST, <Libs/*/*/*.h>;

push @FILE_LIST, <Contrib/CMakeLists.txt>;
#foreach $dir (qw(ugp3-constraints ugp3-extractor ugp3-population ugp3-settings)) {
foreach $dir (qw(ugp3-extractor)) {
    push @FILE_LIST, <Contrib/${dir}/CMakeLists.txt>;
    push @FILE_LIST, <Contrib/${dir}/*.h>;
    push @FILE_LIST, <Contrib/${dir}/*.c>;
    push @FILE_LIST, <Contrib/${dir}/*.cc>;
    push @FILE_LIST, <Contrib/${dir}/*.pl>;
}
push @FILE_LIST, <Contrib/Scripts/*.sh>;
push @FILE_LIST, <Contrib/Scripts/*.pl>;
push @FILE_LIST, <Contrib/Scripts/*.pm>;

#push @FILE_LIST, <Docs/*.*>;
#push @FILE_LIST, <Docs/Html/*>;
#push @FILE_LIST, <Docs/Xml/Schemas/*>;
#push @FILE_LIST, <Docs/Xml/Styles/*>;
#push @FILE_LIST, <Docs/Xml/Transforms/*>;

# SAMPLES!!!!
#push @FILE_LIST, <Samples/README>;
#push @FILE_LIST, <Samples/OneMax/onemax.constraints.xml>;
#push @FILE_LIST, <Samples/OneMax/onemax.fitness-script.pl>;
#push @FILE_LIST, <Samples/OneMax/onemax.population.settings.xml>;
#push @FILE_LIST, <Samples/OneMax/ugp3.settings.xml>;
push @FILE_LIST, <Samples/*>;
push @FILE_LIST, <Samples/*/*>;
push @FILE_LIST, <Samples/*/*/*>;
push @FILE_LIST, <Samples/README>;
push @FILE_LIST, <Samples/README>;

`mkdir $TMP`;
$tarball = "ugp3-v3.${Version}.${SubVersion}_${Revision}";
print "Copying files\n";
for (@FILE_LIST) {
    $file = "$tarball/$_";
    ($dir) = $file =~ m|(.*)/[^/]|o;
    print "Creating $TMP/$dir\n" unless -d "$TMP/$dir";
    mkdir "$TMP/$dir" unless -d "$TMP/$dir";
    #copy "$_", "$TMP/$file"; # yeuch perl replacement do NOT preserve flags!
    `cp -p "$_" "$TMP/$file"`;
    #print "Copying $TMP/$file\n";
}

open H, ">>$TMP/$tarball/ugp3_config.h" or die;
print H "
#ifdef UGP3_VERSION_NAME
#undef UGP3_VERSION_NAME
#define UGP3_VERSION_NAME \"${VersionName}\"
#endif

#ifdef UGP3_VERSION
#undef UGP3_VERSION
#define UGP3_VERSION \"${Version}\"
#endif

#ifdef UGP3_SUBVERSION
#undef UGP3_SUBVERSION
#define UGP3_SUBVERSION \"${SubVersion}\"
#endif

#ifdef UGP3_REVISION
#undef UGP3_REVISION
#define UGP3_REVISION \"${Revision}\"
#endif
";

chdir $TMP;
`touch ${tarball}/00_THIS_IS_MicroGP_v3.${Version}.${SubVersion}_${Revision}`;
`chmod -R u+rwX .`;
`chmod -R go+rX .`;
`chmod 444 */AUTHORS`;
`chmod 444 */COPYING`;
`chmod 444 */INSTALL*`;
`chmod 444 */README*`;
`mv ${tarball}/NEWS ${tarball}/NEWS.${VersionName_}`;
`chmod 444 */NEWS*`;
`chmod 444 */TODO*`;

`tar cvf - $tarball | xz --best --stdout > ${tarball}.tar.xz`;
`cp ${tarball}.tar.xz $PWD`;
chdir $PWD;
`./GetVersion.pl -D >LATEST_SRC_DESCRIPTION`;
`echo src/${VersionName}/${tarball}.tar.xz >LATEST_SRC`;
print "Deleting $TMP\n";
`rm -Rf $TMP`;
print "Setting mode for ${tarball}.tar.xz\n";

if(@ARGV && $ARGV[0] eq "commit") {
    print "\nSending files to sourceforge...\n";
    system("scp LATEST_SRC* $ENV{USER}\@web.sourceforge.net:/home/project-web/ugp3/htdocs");
    system("scp ${tarball}.tar.xz $ENV{USER},ugp3\@frs.sourceforge.net:/home/pfs/project/u/ug/ugp3/src/${VersionName}/");
}
