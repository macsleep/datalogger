#!/usr/bin/perl

#
# 11.5.2025 JS
# 
# This script downloads the log files stored on the SD card of the
# data logger. It compares the size of local and remote log files
# and only downloads files that are larger (or don't yet exist).
# Log files are organised by year. For each year a directory is
# created. Each log file in the directory is named after the month
# and day of the month it was created (e.g. 0403 would be the 3.
# of April). 
#

use strict;
use warnings;
use Getopt::Std;
use Data::Dumper;
use HTTP::Request;
use LWP::UserAgent;

my $user = "admin";
my $password = "admin";
my $url = 'http://datalogger.local/api/logs';
my $timeout = 10;

my %opts;
if(!getopts('dy:', \%opts)) {
        printf("syntax: $0 -d -y <year>\n");
        printf("  -d        : dry run, don't actually get any files\n");
        printf("  -y <year> : only get files for a certain year\n");
        exit 1;
}
defined $opts{'y'} and $url .= "?year=" . $opts{'y'};

sub logs_list {
	my $request = HTTP::Request->new('GET', $url);
	my $useragent = LWP::UserAgent->new();
	$useragent->timeout($timeout);
	my $response = $useragent->simple_request($request);
	$response->is_success or die(Dumper($response->status_line()));
	my @data = split(/\r\n/, $response->decoded_content);
	return \@data;
}

sub logs_get {
	my $log = @_ ? shift : "0";
	my $destination = @_ ? shift : "0";

	my $request = HTTP::Request->new('GET', $url . "/" . $log);
	my $useragent = LWP::UserAgent->new();
	$useragent->timeout($timeout);
	my $response = $useragent->simple_request($request, $destination);
	$response->is_success or die(Dumper($response->status_line()));
}

foreach (@{logs_list()}) {
	my ($log, $size) = split;
	my $file = substr($log, 4, 4);
	my $directory = substr($log, 0, 4);
	$opts{'d'} or -e $directory or -d $directory or mkdir $directory;
	my $destination = $directory . "/" . $file;
	my $fsize = -f $destination ? -s $destination : 0;
	if($fsize < $size) {
		print $destination . "\n";
		$opts{'d'} or logs_get($log, $destination);
	}
}

exit 0;
