#!/usr/bin/perl
use warnings;
use strict;
use MIME::Base64;
use File::Slurp;
use Data::Dumper;
use JSON::Tiny;
use GD;
use GD::Image;

my $DIR = $ARGV[0];

for (my $font = 0; $font <= 8; $font++) {
	my $codepoints = [];
	for my $file (glob("$DIR/$font/*.png")) {
		next if $file !~ /\/([a-f0-9]+)\.png$/i;
		my $codepoint = hex($1);
		$codepoints->[$codepoint] = $file;
	}
	
	my $codepoints_data = "";
	my @ranges;
	my $range;
	
	for my $codepoint (keys @$codepoints) {
		my $file = $codepoints->[$codepoint];
		next if !defined $file;
		
		if (!$range || $codepoint - $range->{end} > 1) {
			$range = {
				start	=> $codepoint,
				end		=> $codepoint,
				chars	=> []
			};
			push @ranges, $range;
		}
		
		my $img = GD::Image->newFromPng($file, 1);
		
		my $row_size = int(($img->width + 7) / 8);
		my $total_bytes = $row_size * $img->height;
		
		my @bytes;
		for (my $i = 0; $i < $total_bytes; $i++) {
			push @bytes, 0;
		}
		
		for (my $y = 0; $y < $img->height; $y++) {
			for (my $x = 0; $x < $img->width; $x++) {
				my $index = $img->getPixel($x, $y) & 0xFFFFFF;
				
				my $current_bit = $y * ($row_size * 8) + $x;
				my $current_byte = int($current_bit / 8);
				my $current_shift = 7 - ($current_bit - $current_byte * 8);
				
				if ($index) {
					$bytes[$current_byte] |= (1 << $current_shift);
				}
			}
		}
		
		push @{$range->{chars}}, pack("CC", $img->width, $img->height).join("", map { chr($_) } @bytes);
		$range->{end} = $codepoint;
	}
	
	my $font_data = "";
	
	$font_data .= pack("v", scalar(@ranges));
	
	for my $range (@ranges) {
		$font_data .= pack("vvv", scalar(@{$range->{chars}}), $range->{start}, $range->{end});
		$font_data .= join("", @{$range->{chars}});
	}
	
	print "$font: ".length($font_data)." bytes\n";
	
	open F, ">$DIR/$font.font";
	print F $font_data;
	close F;
}
