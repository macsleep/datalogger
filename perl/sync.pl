#!/usr/bin/perl

#
# 11.5.2025 JS get data logger log files via http REST api
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

my %opts;
if(!getopts('d', \%opts)) {
        printf("syntax: $0 -d\n");
        printf("  -d  : dry run, don't actually get any files\n");
        exit 1;
}

sub logs_list {
	my $request = HTTP::Request->new('GET', $url);
	my $useragent = LWP::UserAgent->new();
	$useragent->timeout(3);
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
	$useragent->timeout(3);
	my $response = $useragent->simple_request($request, $destination);
	$response->is_success or die(Dumper($response->status_line()));
}

foreach (@{logs_list()}) {
	my ($log, $size) = split;
	my $file = substr($log, 4, 4);
	my $directory = substr($log, 0, 4);
	-e $directory or -d $directory or mkdir $directory;
	my $destination = $directory . "/" . $file;
	my $fsize = -f $destination ? -s $destination : 0;
	if($fsize < $size) {
		print $destination . "\n";
		$opts{'d'} or logs_get($log, $destination);
	}
}

exit 0;
