#!/usr/bin/perl
# CSCI-561 Fall 2014 grading agent
#
# USAGE: grading_agent.pl <directory>
# where <directory> is the directory that contains the student files.
#
# Note: Edit refdir below to point to where the reference files are.

use strict;
my $refdir = "test_cases"; # directory where the reference files are

# First build:
my $student = shift;
chdir($student);

print STDERR "#################### Start compilation for ${student} ####################\n";

system("make agent 1>&2");

if ($?)
{
    print STDERR "#################### Compilation failed for ${student} ####################\n";
    print "${student} 0.0\n";
    exit(0);
}

# Then run all the test input/output pairs:
my $count = 0; my $good = 0; my $idx = 1;
while (-f "${refdir}/input${idx}.txt")
{
    print STDERR "#################### Start Test ${idx} for ${student} ####################\n";
    system("/bin/cp -f ${refdir}/input${idx}.txt input.txt");
    if (-f "output.txt") { system("/bin/rm -f output.txt"); }

    # Run the student's program:
    system("make run 1>&2");
    
    # Compare outputs:
    open REF, "${refdir}/output${idx}.txt";
    open STU, "output.txt";
    my $correct = 1;
    while (<REF>)
    {
        print STDERR $_,"\n";
		my $ref = $_;    $ref =~ s/^\s+//; $ref =~ s/\s+$//; $ref =~ s/\s+/ /;
        my $stu = <STU>; $stu =~ s/^\s+//; $stu =~ s/\s+$//; $stu =~ s/\s+/ /;
        if ($stu ne $ref) { $correct = 0; last; }
    }
    
    # Check for extra garbage in student output:
    if ($correct) { while (<STU>) { if ($_ !~ m/\s*/) { $correct = 0; last; } } }

    close(REF); close(STU);
    my $result; if ($correct) { $result = "PASS"; } else { $result = "FAIL"; }
    print STDERR "#################### Test ${idx} for ${student}: ${result} ####################\n";
    
    # Accumulate score:
    $count ++; $good += $correct;
    
    # Next test pair:
    $idx ++;
}

my $score;
if ($good == $count) { $score = 100.0; }
else { $score = 50.0 - 50.0 * ($count - $good) / $count; }

print sprintf("${student} %.2f\n", $score);
