#!/usr/bin/perl
print <<HEAD;
<html>
<head>
<title>Release History</title>
<body>
<center><h1>Release History</h1></center>
HEAD

$ul = 0;

while(<>) {
    chomp;
    if(/^\s*$/) {
	print "<p>";
    } elsif(/^----------------------------------/) {
	print "</ul>\n" if $ul; $ul = 0;
	print "<hr>";
    } elsif(/^Changes in version/i) {
	print "<h2>", $_, "</h2>";
    } elsif(/^-/ || /^\s/) {
	print "<ul>\n" unless $ul; $ul = 1;
	s/^- /<li>/;
	print;
    } else {
	print "</ul>\n" if $ul; $ul = 0;
	print;
    }
    print "\n";
}

print <<TAIL;
</body>
</html>
TAIL
