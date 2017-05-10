#!/usr/bin/perl -w-

print "fitness program starts here, input file $ARGV[0] \n";
open INFILE, $ARGV[0] or die "Error:: cannot open file fitness $ARGV[0] $!";
my @values = split('\.', $ARGV[0]);
my $grp_name = $values[0];
print "\n";

my @vector;
my @individuals;
my @grp_array;
my $leng;

$cnt = 0;
while (<INFILE>)
{
   chomp;
   $my_program = $_;

   my @tmp = split ('\.', $my_program);

   my @tmp2 = split ($grp_name,$tmp[0]);
   my $ind_name = $tmp2[1];

   open F, "$my_program" or die "Can't open $my_program: $!";
   my $ind_line = <F>;
   $leng = length($ind_line);
   my $n = ($ind_line =~ tr/1/1/);
   push(@vector, $n);
   close F;

   #my @char_array = split(//,"$ind_line");
   #push(@individuals, "@char_array");
   push(@individuals, $ind_line);

   $cnt++;
}
close INFILE;

while($leng>0){
  push(@grp_array, 0);
  $leng--;
}

foreach my $row (@individuals){
  my $index=0;
  #print "---primo for. index: $index\n";
  my @char_array = split(//,"$row");
  foreach my $val(@char_array){
  #print "---sec for. val: $val\n";
    if($val == 1){
      #print "---sec for. Inserisco 1 a index: $index\n";
      $grp_array[$index] = 1;
    }
    $index++;
  }
}

my $grp_str = "@grp_array";
$grp_str =~ s/(.)\s/$1/seg; ## Simple Regex

my $m = ($grp_str =~ tr/1/1/);
unshift(@vector, $m);

open OUTFILE, ">$ENV{UGP3_FITNESS_FILE}" or die "Can't create $ENV{UGP3_FITNESS_FILE}: $!";
#open OUTFILE, ">test_fitness.out" or die "Can't create $ENV{UGP3_FITNESS_FILE}: $!";
foreach my $val (@vector){
  print OUTFILE "$val ";
}
close OUTFILE;
# ------------------------------------------------------------------------------------------------------------
#open OUT, ">$ENV{UGP3_FITNESS_FILE}" or die "Can't create $ENV{UGP3_FITNESS_FILE}: $!";
#@timeData = localtime(time);
#foreach $file (@ARGV) {
#    open F, $file or die "Can't open $file: $!";
#    $_ = <F>;
#    $n = tr/1/1/;

#    ($tag) = $file =~ m|_(\w+)\.|o;
#    print OUT sprintf "%d %s\@%02d:%02d:%02d\n", $n, $tag, $timeData[2], $timeData[1], $timeData[0];
#}
#close OUT;
