#!/usr/bin/perl -w

open OUTFILE, ">output" or die "Error:: cannot open file fitness $!";

$selection = int(rand(2));
print OUTFILE "$selection";

close OUTFILE;

