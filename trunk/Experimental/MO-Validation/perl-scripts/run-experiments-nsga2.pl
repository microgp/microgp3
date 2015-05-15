# PERL script to run several experiments
# Alberto Tonda, 2012 <alberto.tonda@gmail.com>

use Cwd 'abs_path'; # absolute paths
use strict;

my $DEBUG = 1;

# sub to compute average and stdev
sub averageAndStandardDeviation
{
        # data
        my @data = @_; 
    
        if($#data + 1 == 0)
        {   
                my @ret = (0.0,0.0);
                return @ret;
        }   

        # compute average!
        my $average = 0;
        for(my $i = 0; $i <= $#data; $i++)
        {   
                $average += $data[$i];
        }   
        $average /= ($#data + 1); 

        # compute standard deviation
        my $standardDeviation = 0;
        for(my $i = 0; $i <= $#data; $i++)
        {   
                $standardDeviation += abs($average - $data[$i]);
        }   
        $standardDeviation /= ($#data + 1); 

        # return results
        my @ret = ($average, $standardDeviation);

        return @ret;
}

# read and parse fitness values
sub readAndParseFitnessValues
{
	# fitness file is the first argument
	my $fitnessFile = @_[0];
	
	# open file
	open INFILE, $fitnessFile or die "Cannot open \"$fitnessFile\" to read fitness values. Aborting...$!\n";
	my @fitness = <INFILE>;
	close INFILE;
	
	# array and varibles used to store return values
	my @ret;
	my $loglikelihood = 0;
	my $dimension = 0;
	
	# every information should be on the first line
	my @tokens = split("[\s|\=|\_|\#]+", $fitness[0]);

	if($DEBUG)
	{
		foreach(@tokens)
		{
			print "Token: $_\n";
		}
	}

	for(my $t = 0; $t <= $#tokens; $t++)
	{
		chomp($tokens[$t]);
		if( $tokens[$t] eq "loglikelihood" )
		{
			$loglikelihood = $tokens[$t+1];
		}
		elsif( $tokens[$t] eq "aicPenalty" )
		{
			$dimension = $tokens[$t+1];
		}
	}
	
	# push results into return array
	push(@ret, $loglikelihood);
	push(@ret, $dimension);

	# return results
	return @ret;
}	

# experiment-dependent variables
# TODO: the fitness arrays could be combined into a single array of arrays; in this way, the code could be generic (maybe)
my $fitnessFile = "fitness.output";
my @loglikelihood;
my @dimension;

my $evaluationCommandLine = "";

# options
my $opDirectory = "--directory";
my $opNumber = "--number";
my $opPopulation = "--population";

# print usage and exit
sub printUsage
{
	print "PERL script to run many experiments and collect the results. Usage:\n";
	print "\tperl run-experiments.pl $opNumber <number> $opDirectory <output directory to be created> [$opPopulation <name of the population file>]\n";
}

	##############################################################
	############################ MAIN ############################
	############################################################## 

# first check
if( $#ARGV <= 0 )
{
	printUsage();
	exit(0);
}

# arguments to obtain
my $directory = "";
my $number = 0;
my $population = "";

# parse command line
for(my $a = 0; $a <= $#ARGV; $a++)
{
	if( $ARGV[$a] eq $opNumber && $a < $#ARGV )
	{
		$number = $ARGV[$a+1];
	}
	elsif( $ARGV[$a] eq $opDirectory && $a < $#ARGV )
	{
		$directory = $ARGV[$a+1];
	}
	elsif( $ARGV[$a] eq $opPopulation && $a < $#ARGV )
	{
		$population = $ARGV[$a+1];
	}
	elsif( $ARGV[$a] =~ /^--/ )
	{
		print STDERR "Warning: argument \"" . $ARGV[$a] . "\" not recognized...\n";
	}
}

# check if everything is ok
if( $number <= 0 || !$directory )
{
	print STDERR "Error: options not specified or invalid.\n";
	printUsage();
	exit(0);
}

# create target output directory
system("mkdir $directory");
my $directoryFullpath = abs_path(".") . "/" . $directory;

print "Directory with full path \"$directoryFullpath\"\n" if $DEBUG;

# start the loop
for(my $experiment = 0; $experiment < $number; $experiment++)
{
	# create temporary directory
	my $tempDirectory = "temp-" . $experiment;
	system("mkdir $tempDirectory");

	# copy all necessary files
	# TODO: change this part into a subroutine
	system("cp *.xml $tempDirectory"); 
	system("cp zdt* $tempDirectory"); 
	system("cp nsga-ii $tempDirectory");

	# change directory
	# TODO: change this part into a subroutine
	chdir($tempDirectory);

	# run experiment
	system("./nsga-ii --minimize");
	
	# clean some files and run the evaluator one last time to store the fitness value
	# TODO: change this part into a subroutine (look into XML population file and get the evaluator string, appending the BEST_*)
	system("rm fitness*");
	
	# now, since this is nsga-ii, we have a single file with all the points called "nsga2.output.csv"
	# copy results
	# TODO: change this part into a subroutine
	system("cp nsga2-output.csv $directoryFullpath/nsga2-$experiment.csv");

	# exit directory
	chdir("..");

	# remove temporary directory
	system("rm -r $tempDirectory");
}

# collect all fitness values in a single output file
open OUTFILE, ">$directoryFullpath/summary.txt" or die "Cannot write on file \"$directoryFullpath/summary.txt $!\n";
	print OUTFILE "objective1,objective2\n";

my @fitnessFiles = `ls $directoryFullpath/nsga2-*`;
foreach(@fitnessFiles)
{
	open INFILE, "$_" or die "Cannot read file \"$_\". Aborting...$!\n";
	my @content = <INFILE>;
	close INFILE;
	
	chomp($content[0]);
	my @tokens = split(" ", $content[0]);
	
	print OUTFILE $tokens[0], " ", $tokens[1], "\n";
}
close OUTFILE;

# compute average and standard deviation
#my ($avgLL,$stdLL) = averageAndStandardDeviation(@loglikelihood);
#my ($avgDIM,$stdDIM) = averageAndStandardDeviation(@dimension);

#open OUTFILE, ">$directoryFullpath/summary.txt" or die "Cannot write on file \"$directoryFullpath/summary.txt $!\n";
#	print OUTFILE "Statistics on ", $#loglikelihood + 1 , " experiments.\n";
#	print OUTFILE "The evaluation was \"$evaluationCommandLine\"\n\n";
#	print OUTFILE "Loglikelihood: avg=$avgLL (std=$stdLL)\n";
#	print OUTFILE "Dimension: avg=$avgDIM (std=$stdDIM)\n\n";
#	for(my $e = 0; $e <= $#loglikelihood; $e++)
#	{
#		print OUTFILE "Experiment #$e:\tll=" . $loglikelihood[$e] . "\tdim=" . $dimension[$e] . "\n";	
#	}
#close OUTFILE;
