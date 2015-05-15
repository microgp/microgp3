#!/usr/bin/perl -w-         # -*- perl -*-
##############################################################################
#                                                                            #
#   #####        Giovanni Squillero, Ph.D.                                   #
#  ######        Politecnico di Torino - Dip. Automatica e Informatica       #
#  ###   \       Cso Duca degli Abruzzi 24 / I-10129 TORINO / ITALY          #
#   ##G  c\                                                                  #
#   #     _\     Tel: +39-011-564.7092  /  Fax: +39-011-564.7099             #
#   |   _/                                                                   #
#   |  _/        email: giovanni.squillero@polito.it                         #
#                www  : http://www.cad.polito.it/staff/squillero/            #
#                                                                            #
#   It is easier for a camel to pass through the eye of a needle             #
#   if it is lightly greased.                                                #
#                                                    - Kehlog Albran         #
#                                                                            #
##############################################################################
# $Revision: 644 $
#     $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $ 
##############################################################################

use Getopt::Std;
getopts('ho:', \%opt);
die "Usage: $0 [-h] | [-o OutputBaseName] [ file1 [ ... fileN ] ]\nStopped" if $opt{h};

$opt{o} = 'Population' unless defined $opt{o};

while(<>) { $xml .= $_ }
study $xml;
@POP = $xml =~ m|(<population\s.*?</population>)|sgo;
die "Yeuch! I see no populations out there...\nAborted" unless @POP;
for($t=0; $t<=$#POP; ++$t) { 
    $file = "$opt{o}.$t.xml";
    open P, ">$file" or die "Can't create file \"$file\".\nAborted";
    print "Creating population $file...\n";
    print P "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    print P "$POP[$t]\n";
    close P;
}
