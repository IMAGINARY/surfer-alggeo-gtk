#!/usr/bin/perl

#open(my $in,  "<",  "input.txt")  or die "Can't open input.txt: $!";
my $a  = <STDIN>;

$a =~ s/\^(\d+)/\^{$1}/g;

$a =~ s/\-/ \- /g;
$a =~ s/\+/ \+ /g;

$a =~ s/sqrt\(([^\(\)]+)\)/\\sqrt{$1}/g while $a =~ /sqrt\(([^\(\)]+)\)/;

$a =~ s/\)\*\(/\) \\cdot \(/g;

$a =~ s/\*/ /g;

$a =~ s/sin/\\sin/g;
$a =~ s/cos/\\cos/g;
$a =~ s/tan/\\tan/g;

$a =~ s/\(/\\left\(/g;
$a =~ s/\)/\\right\)/g;

$a =~ s/PI/\\pi/g;
print "\\ensuremath{";
print $a;
print "}";
