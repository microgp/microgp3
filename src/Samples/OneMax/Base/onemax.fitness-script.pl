#!/usr/bin/perl -w-
#########################################################################
#                                                                       #
# This file is part of MicroGP v3 (ugp3)                                #
# http://ugp3.sourceforge.net/                                          #
#                                                                       #
#########################################################################
#                                                                       #
# This program is free software; you can redistribute it and/or modify  #
# it under the terms of the GNU General Public License as published by  #
# the Free Software Foundation, either version 3 of the License, or (at #
# your option) any later version.                                       #
#                                                                       #
# This program is distributed in the hope that it will be useful, but   #
# WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      #
# General Public License for more details                               #
#                                                                       #
#########################################################################
# $Revision: 643 $
# $Date: 2015-02-23 14:49:36 +0100 (Mon, 23 Feb 2015) $
#########################################################################


# From v3.1.2_1142 fitness scripts can use (again) environment variables:
# 
# $UGP3_FITNESS_FILE : the file created by the evaluator
# $UGP3_OFFSPRING    : the individuals to be evaluated (space separated list)
# $UGP3_GENERATION   : generation number
# $UGP3_VERSION      : current ugp3 version. eg. 3.1.2_1142
# $UGP3_TAGLINE      : full ugp3 tagline. eg. ugp3 (MicroGP++) v3.1.2_1142 "Bluebell"


open OUT, ">$ENV{UGP3_FITNESS_FILE}" or die "Can't create $ENV{UGP3_FITNESS_FILE}: $!";
@timeData = localtime(time);
foreach $file (@ARGV) {
    open F, $file or die "Can't open $file: $!";
    $_ = <F>;
    $n = tr/1/1/;

    ($tag) = $file =~ m|_(\w+)\.|o;
    print OUT sprintf "%d %s\@%02d:%02d:%02d\n", $n, $tag, $timeData[2], $timeData[1], $timeData[0];
}
close OUT;
