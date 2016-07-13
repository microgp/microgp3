#!/usr/bin/perl --         # -*- perl -*-
#############################################################################
#             # Giovanni Squillero, Ph.D.                                   #
#   #####     # Politecnico di Torino - Dip. Automatica e Informatica       #
#  ######     # Cso Duca degli Abruzzi 24 / I-10129 TORINO / ITALY          #
#  ###   \    #                                                             #
#   ##G  c\   # Tel: +39-011-564.7186                                       #
#   #     _\  # Fax: +39-011-564.7099                                       #
#   |   _/    #                                                             #
#   |  _/     # email: giovanni.squillero@polito.it                         #
#             # www  : http://www.cad.polito.it/staff/squillero/            #
#############################################################################

undef $/;
$_ = <STDIN>;
#s/\| ugp3 team.*?\| +\|\n//sg;
#s|Copyright \(c\) 2002-2011 Giovanni Squillero|http://ugp3.sourceforge.net/              |sg;
#s|http://www.cad.polito.it/ugp3/              |Copyright (c) 2002-2016 Giovanni Squillero  |sg;
s|\(c\) 2002-201\d|(c) 2002-2016|sg;
print;
