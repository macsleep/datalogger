#!/usr/bin/perl

#
# 11.5.2025 JS
# 
# This script downloads the log files stored on the SD card
# of the data logger. It compares the size of local and remote
# log files and only downloads files that are larger (or don't
# yet exist). Log files are organised by year, month and day.
# For each year and month a directory is created. Each log file
# in the directory is named after the day of the month it was
# created. Example: 2025/03/21
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
if(!getopts('hdy:', \%opts) or $opts{'h'}) {
    printf("syntax: $0 -d -y <year>\n");
    printf("  -d  : dry run, don't actually get any files\n");
    printf("  -y  : only sync files for that year\n");
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

foreach my $year (logs_get("")) {
    next if $opts{'y'} and $opts{'y'} ne $year;
    foreach my $month (logs_get("/" . $year)) {
        foreach (logs_get("/" . $year . "/" . $month)) {
            my ($day, $size) = split / /;
            my $path = $year;
            $opts{'d'} or -e $path or -d $path or mkdir $path;
            $path = $year . "/" . $month;
            $opts{'d'} or -e $path or -d $path or mkdir $path;
            $path = $year . "/" . $month . "/" . $day;
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
