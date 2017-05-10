# PERL evaluator for the string coverage problem
# Alberto Tonda, 2012 <alberto.tonda@gmail.com>

use strict;

my $DEBUG = 0;

my $testFile = "testSet.txt";
my $outputFile = "fitness.out";

# open individual file
open INFILE, $ARGV[0] or die "Cannot open individual file \"" . $ARGV[0] . "\" $!\n";
my @input = <INFILE>;
close INFILE;

# open test file
open INFILE, "$testFile" or die "Cannot open test file \"$testFile\" $!\n";
my @testSet = <INFILE>;
close INFILE;

# iterate over all the templates in the individual file
my @individualFitness;
my %globalCoverage; # hash map of global coverage
foreach my $template (@input)
{
	chomp($template);
	print "Analyzing template \"$template\"...\n" if $DEBUG;

	if( length($template) > 1 )
	{
		my $fitnessRatio1 = 0;
		my @templateTokens = split("", $template);
		
		# compute ratio of 1s	
		foreach my $character (@templateTokens)
		{
			if( $character eq "1" ){ $fitnessRatio1 += 1; }
		}
		print "Template \"$template\" contains $fitnessRatio1 / ", length($template), " ones.\n" if $DEBUG;
		$fitnessRatio1 /= length($template);
		
		# compute matching (EXACT MATCHING)
		my $fitnessMatching = 0;
		for my $testString (@testSet)
		{
			chomp($testString);
			print "Now trying to match string \"$testString\"...\n" if $DEBUG;
			my @testTokens = split("",$testString);
			
			my $matching = 1;
			for(my $t = 0; $t <= $#testTokens && $matching == 1; $t++)
			{
				if( $templateTokens[$t] ne "*" && $templateTokens[$t] ne $testTokens[$t])
				{
					$matching = 0;
				}
			}
			$fitnessMatching += $matching;
			
			# if the string is matched, add it to the hash map of globally matched strings
			if( $matching == 1 )
			{
				$globalCoverage{ $testString } = "matched";
			}
		}
		print "Template \"$template\" matches $fitnessMatching / ", ($#testSet+1), " test strings.\n" if $DEBUG;
		$fitnessMatching /= ($#testSet + 1);
		
		# compute individual fitness
		my $fitness = $fitnessRatio1 * $fitnessMatching;
		push(@individualFitness, $fitness);
	}
}
my $sizeOfHash = keys(%globalCoverage);
print "A total of $sizeOfHash / ", ($#testSet+1) , " strings are globally covered.\n" if $DEBUG;

# global fitness
my $globalFitness = 0;
foreach(@individualFitness) { $globalFitness += $_; }
$globalFitness /= ($#individualFitness + 1);
$globalFitness *= keys( %globalCoverage ) / ($#testSet + 1);

open OUTFILE, ">$outputFile" or die "Cannot write on file \"$outputFile\" $!\n";
print OUTFILE "$globalFitness #";
for(my $i = 0; $i <= $#individualFitness; $i++)
{
	print OUTFILE "f$i=", $individualFitness[$i], "_";
}
print OUTFILE "$sizeOfHash/", ($#testSet+1), "\n";
close OUTFILE;
