#!/usr/bin/perl
# Tamas K Lengyel 2012
# tamas.k.lengyel@gmail.com
# 
# This script parses the honeybrid.log file and calculates overall statistics on connection attempts.
# To run: perl stats.pl /var/log/honeybrid/honeybrid.log 1
#
# To install Tree::Binary::Search on Debian run "perl -MCPAN -e shell" and inside the shell "install Tree::Binary::Search"
# Set logging of Honeybrid to log_csv
#INPUT:
# ARGV[0] = location of honeybrid.log
# ARGV[1] = number of uplinks

    use warnings;
    use Text::CSV;
    use Tree::Binary::Search;

    if ($#ARGV != 1 ) {
	print "usage: perl stats.pl </path/to/honeybrid.log> <# of uplinks>\n";
	print "for example: perl stats.pl /var/log/honeybrid.lig 1\n";
	exit;
   }

    my $uplinks=$ARGV[1];
    my @linkstat;

    my $ports = Tree::Binary::Search->new();
    $ports->useNumericComparison(); 
    my $ips = Tree::Binary::Search->new();
    $ips->useStringComparison();

    my $file = $ARGV[0];

    my $connections=0;
    my $dropped=0;
    my $tcp=0;
    my $udp=0;
    my $csv = Text::CSV->new();
    open (CSV, "<", $file) or die $!;
    while (<CSV>) {
        next if ($. == 1);
        if ($csv->parse($_)) {
            my @columns = $csv->fields();
	    if($columns[2] != 0) {
		$connections++;
		my $port = $columns[7];
		my $ip = $columns[4];
		my $action = $columns[10];
		my $link = $columns[2];
		my $proto = $columns[3];
		if($ports->exists($port)) {
			my $count = $ports->select($port);
			$count++;
			$ports->update($port,$count);
		} else {
			$ports->insert($port, 1)
		}
	
		if(! $ips->exists($ip)) {
			$ips->insert($ip,0);	
		}

		if($action eq "DROP") {
			$dropped++;
		}

		$linkstat[$link]++;

		if($proto eq "TCP") {
			$tcp++;
		} else {
			$udp++;
		}
	    }
        } else {
            my $err = $csv->error_input;
            print "Failed to parse line: $err";
        }
    }
    close CSV;

    print $ips->size() ." unique IP address\n";
    print "$connections connections\n\tDropped:\t$dropped\n\tTCP:\t\t$tcp\n\tUDP:\t\t$udp\n";

    for($loopc=1;$loopc<=$uplinks;$loopc++) {
	print "\tUplink $loopc:\t". $linkstat[$loopc] ."\n"; 
    }

    print "Port statistics\n";
    while($ports->size() > 1) {
		my $key=$ports->min_key();
		my @portstats;
		$count = $ports->select($key);
		print "\t$key:\t$count\n";
		$ports->delete($key);
	}
	$key=$ports->min_key();
        $count = $ports->select($key);
        print "\t$key:\t$count\n";
