# Simple PERL script to run a lot of experiments and gather results

use strict;

my $outputFileName = "results.txt";

my $success = 0;
my $failure = 0;

for(my $experiment = 0; $experiment < 100; $experiment++)
{
	# run experiment
	system("rm BEST* fitness.output");
	system("./launch.sh");
	
	# extract better result
	# system("./ugp3-extractor status.xml");
	
	# re-run better results (?)
	my @individuals = `ls BEST*`;
	my $optimal = 0;
	foreach(@individuals)
	{
		system("./evaluator --data data.txt --tree $_");
	
		# open and parse fitness file
		open INFILE, "fitness.output" or die "Cannot open fitness file! $!";
		my @lines = <INFILE>;
		close INFILE;

		my @tokens = split(' ', $lines[0]);
		if($tokens[0] == 10 && $tokens[1] == 10) { $optimal++; } 
	}

	if( $optimal )
	{
		system("echo \"$experiment) success!\" >> $outputFileName");
		$success++;
	}
	else
	{
		system("echo \"$experiment) failure...\" >> $outputFileName");
		$failure++;
	}
}

system("echo \"Final result: $success successes and $failure failures.\" >> $outputFileName");
