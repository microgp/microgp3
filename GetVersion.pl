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

push @ARGV, "--" unless @ARGV;

open H, "CMakeLists.txt" or die "Yeuch!";
undef $/;
$_ = <H>;
close H;

$VersionName = $1 if m/VERSION_NAME\s+"(\w+)"/;
$Version = $1 if m/VERSION\s+"(\d+)"/;
$SubVersion = $1 if m/SUBVERSION\s+"(\d+)"/;
$Revision = $1 if `./GetRevision.sh` =~ m/(\d+)/;

my ($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings);
@months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) 
    = localtime();
$year = 1900 + $yearOffset;

if($ARGV[0] eq '-v') {
    print "${Version}\n";
} elsif($ARGV[0] eq '-s') {
    print "${SubVersion}\n";
} elsif($ARGV[0] eq '-r') {
    print "${Revision}\n";
} elsif($ARGV[0] eq '-n') {
    print "${VersionName}\n";
} elsif($ARGV[0] eq '-f') {
    print "3.${Version}.${SubVersion}_${Revision}-${VersionName}\n";
} elsif($ARGV[0] eq '-F') {
    #print "v3.${Version}.${SubVersion}_${Revision} \"${VersionName}\" on $dayOfMonth-$months[$month]-$year\n";
    print "v3.${Version}.${SubVersion}_${Revision} on $dayOfMonth-$months[$month]-$year\n";
} elsif($ARGV[0] eq '-D') {
    print "v3.${Version}.${SubVersion} (rev ${Revision}) on $dayOfMonth-$months[$month]-$year\n";
} else {
    print "3.${Version}.${SubVersion}_${Revision}\n";
}
