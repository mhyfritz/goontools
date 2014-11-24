my $re = qr/^((?!static)(\w+(\s+|\()){2,}(?<=\().+?\))\s*\n{/ms;

undef $/;
my $src = <>;

while ($src =~ /$re/g) {
    print "$1;\n";
}
