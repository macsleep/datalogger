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
my $url = 'http://10.0.0.1/api/logs';
my $timeout = 10;

my %opts;
if(!getopts('dy:', \%opts)) {
    printf("syntax: $0 -d\n");
    printf("  -d    : dry run, don't actually get any files\n");
    exit 1;
}

sub logs_get {
    my $path = @_ ? shift : "";

    my $request = HTTP::Request->new('GET', $url . $path);
    my $useragent = LWP::UserAgent->new();
    $useragent->timeout($timeout);
    my $response = $useragent->simple_request($request);
    $response->is_success or die(Dumper($response->status_line()));
    my @data = split(/\r\n/, $response->decoded_content);
    return @data;
}

foreach my $year (logs_get()) {
    foreach my $month (logs_get("/" . $year)) {
        foreach my $day (logs_get("/" . $year . "/" . $month)) {
            my ($file, $size) = split(/ /, $day);
            my $path = $year;
            $opts{'d'} or -e $path or -d $path or mkdir $path;
            $path = $year . "/" . $month;
            $opts{'d'} or -e $path or -d $path or mkdir $path;
            $path = $year . "/" . $month . "/" . $file;
            my $fsize = -f $path ? -s $path : 0;
            if($fsize < $size) {
                print $path . "\n";
                next if $opts{'d'};
                my $request = HTTP::Request->new('GET', $url . "/" . $path);
                my $useragent = LWP::UserAgent->new();
                $useragent->timeout($timeout);
                my $response = $useragent->simple_request($request, $path);
                $response->is_success or die(Dumper($response->status_line()));
            }
        }
    }
}

exit 0;
