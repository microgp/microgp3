# Fake evaluator for an evolutionary algorithm
# by Alberto Tonda, 2014 <alberto.tonda@gmail.com>

use strict;

# some default values
my $numberOfValues = 1;
my $outputFile = "fitness.out";
my $rangeMin = 0;
my $rangeMax = 100;

# parse arguments
for(my $a = 0; $a <= $#ARGV; $a++)
{
	if( $ARGV[$a] eq "--values" && $a < $#ARGV )
	{
		$numberOfValues = $ARGV[$a+1];
	}
	elsif( $ARGV[$a] eq "--output" && $a < $#ARGV )
	{
		$outputFile = $ARGV[$a+1];
	}
}

# generate random numbers!
open OUTFILE, ">$outputFile" or die "Cannot write on file \"$outputFile\". Aborting...$!\n";
for(my $v = 0; $v < $numberOfValues; $v++)
{
	my $randomNumber = int(rand($rangeMax - $rangeMin)) + $rangeMin;
	print OUTFILE "$randomNumber ";
}
close OUTFILE;
