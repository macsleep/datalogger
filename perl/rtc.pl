#!/usr/bin/perl

#
# 11.5.2025 JS
#
# This script can be used to read and write the RTC (Real Time Clock).
# The data logger time is set to UTC (Universal Time Coordinates) so
# you don't have to adjust for summer/winter time. If you set the time
# and date (-s option) the time and date of the computer where you run
# the perl script will be transferred to the data logger RTC.
#

use strict;
use warnings;
use Getopt::Std;
use Data::Dumper;
use HTTP::Request;
use LWP::UserAgent;

my $user = "admin";
my $password = "admin";
my $url = 'http://datalogger.local/api/rtc';

my %opts;
if(!getopts('s', \%opts)) {
        printf("syntax: $0 -s\n");
	printf("  -s  : write rtc\n");
        exit 1;
}

sub rtc_read {
	my $request = HTTP::Request->new('GET', $url);
	my $ua = LWP::UserAgent->new();
	$ua->timeout(3);
	my $response = $ua->request($request);
	$response->is_success or die(Dumper($response->status_line()));
	my $data = $response->decoded_content;
	$data =~ /epoch=([0-9]+)/i;
	return defined $1 ? $1 : $data;
}

sub rtc_write {
	my $epoch = @_ ? shift : 0;

	my $header = ['Content-Type' => 'application/x-www-form-urlencoded'];
	my $content = 'epoch=' . $epoch;
	my $request = HTTP::Request->new('PUT', $url, $header, $content);
	$request->authorization_basic($user, $password);
	my $ua = LWP::UserAgent->new();
	$ua->timeout(3);
	my $response = $ua->request($request);
	$response->is_success or die(Dumper($response->status_line()));
}

print "UTC: " . gmtime() . "\n";
print "RTC: " . gmtime(rtc_read()) . "\n";
defined($opts{'s'}) or exit(0);
print "writing UTC to RTC\n";
rtc_write(time());
print "RTC: " . gmtime(rtc_read()) . "\n";

exit 0;

