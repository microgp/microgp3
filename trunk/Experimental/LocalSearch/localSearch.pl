# Simple PERL script to generate files with random names, but with a given structure
# by Alberto Tonda, 2013 <alberto.tonda@gmail.com>

use strict;

my $DEBUG = 1;

my $outputPattern = "localSearch_offspring_";
my $minimum = 1;
my $maximum = 3;


# the number of individuals to generate, ranging from minimum to maximum
my $numberOfIndividuals = int(rand($maximum - $minimum)) + $minimum;
print "I am about to create $numberOfIndividuals individuals...\n";

# for each individual to generate, create a random string
my @chars = ("A".."Z", "a".."z", "0".."9");
for(my $i = 0; $i < $numberOfIndividuals; $i++)
{
	my $string;
	$string .= $chars[rand @chars] for 1..8;
	
	print "I am about to create a file called \"$outputPattern$string\"...\n" if $DEBUG;
	
	system("touch $outputPattern$string");
}
