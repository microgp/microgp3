# PERL script to generate a given number of strings from a set of templates
# Alberto Tonda, 2012 <alberto.tonda@gmail.com>

use strict;

my $DEBUG = 1;

my $outputFile = "testSet.txt";
my @numberOfStrings = ( 200, 200 );
my @templates = ("11111111111111111111111111111111********************************", 
		 "********************************11111111111111111111111111111111" );
my $randomSeed = 42;
srand($randomSeed);


my @output;

# for each template
for(my $i = 0; $i <= $#templates; $i++)
{
	# array where each element is a character from a template
	my @templateTokens = split("", $templates[$i]);
	# number of strings to generate
	my $number = $numberOfStrings[$i];
	
	print "Current template is \"", $templates[$i], "\", split into ", ($#templateTokens+1), " tokens.\n" if $DEBUG;

	# generate strings
	for(my $j = 0; $j < $number; $j++)
	{
		my $string = "";
		for(my $t = 0; $t <= $#templateTokens; $t++)
		{
			if( $templateTokens[$t] eq "*" )
			{
				# throw random number and take decision
				my $random_number = rand();
				if( $random_number < 0.5 )
				{
					$string .= "0";
				}
				else
				{
					$string .= "1";
				}
			}
			else
			{
				$string .= $templateTokens[$t];
			}
		}
		# save string in output
		push(@output, $string);
	}
}

# write output
open OUTFILE, ">$outputFile" or die "Cannot write on \"$outputFile\" $!\n";
foreach(@output)
{
	print OUTFILE $_, "\n";
}
close OUTFILE;
