#!/usr/bin/perl
$in_list = 0;

print
    "<html>\n",
    "<head><title>ChangeLog</title></head>\n",
    "<body>\n";
while(<>) {
    s/\s+$//;
    s/^\s+//;
    s/</\&lt;/g;
    s/>/\&gt;/g;
    if(/^\d+-\d+-\d+ /) {
	print "</ul>\n</p>\n" if $in_list;
	$in_list = 1;
	print "<p>$_\n<ul>\n";
    } elsif(s/^\* //) {
	print "<li>$_\n";
    } else {
	print $_, "\n";
    }
}
print "</ul>\n</p>\n" if $in_list;
