my $re = qr/^((?!static)([\w*]+(\s+|\()){2,}(?<=\().*?\))\s*\n{/ms;

undef $/;
my $src = <>;

print "$1;\n" while $src =~ /$re/g;
