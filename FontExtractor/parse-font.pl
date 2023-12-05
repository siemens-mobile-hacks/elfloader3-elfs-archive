#!/usr/bin/perl
use warnings;
use strict;
use MIME::Base64;
use File::Slurp;
use Data::Dumper;
use JSON::Tiny;
use GD;

my $font = -1;

my @fonts_info;
my @fonts;
my $prev_pixels;

my $file = $ARGV[0];

my $out_dir = $file;
$out_dir =~ s/\.txt//g;

my $icons = [];

for my $line (split(/\r\n|\n/, scalar(read_file($file)))) {
	if ($line =~ /FONT:(\d+)/) {
		$font = $1;
		$fonts[$font] = [];
		$fonts_info[$font] = {
			w	=> 0,
			h	=> 0,
		};
	}
	
	if ($line =~ /^([a-f0-9]+),(\d+),(\d+):(.*?)$/si) {
		my $codepoint = hex($1);
		my $w = int($2);
		my $h = int($3);
		my $pixels = decode_base64($4);
		die "broken b64: $4" if !defined $pixels;
		$fonts[$font]->[$codepoint] = [$codepoint, $w, $h, $pixels];
		$prev_pixels = $pixels;
		
		$fonts_info[$font]->{h} = $h if !$fonts_info[$font]->{h};
		$fonts_info[$font]->{w} = $w if $w > $fonts_info[$font]->{w};
		die sprintf("U+%04X: invalid height!") if $fonts_info[$font]->{h} != $h;
	}
	
	if ($line =~ /^([a-f0-9]+),(\d+),(\d+)&$/si) {
		my $codepoint = hex($1);
		my $w = int($2);
		my $h = int($3);
		die "broken prev_pixels" if !defined $prev_pixels;
		$fonts[$font]->[$codepoint] = [$codepoint, $w, $h, $prev_pixels];
		
		$fonts_info[$font]->{h} = $h if !$fonts_info[$font]->{h};
		$fonts_info[$font]->{w} = $w if $w > $fonts_info[$font]->{w};
		die sprintf("U+%04X: invalid height!") if $fonts_info[$font]->{h} != $h;
	}
	
	if ($line =~ /^([a-f0-9]+),(\d+),(\d+)#(\d+)$/si) {
		my $codepoint = hex($1);
		my $w = int($2);
		my $h = int($3);
		my $pic = int($4);
		$icons->[$codepoint - 0xE100] = $pic if $codepoint >= 0xE100 && $codepoint <= 0xE1D0;
	#	$fonts[$font]->[$codepoint] = [$codepoint, $w, $h, { pic => $pic }];
	}
}

open F, ">$out_dir/E1XX.json";
print F JSON::Tiny::to_json($icons)."\n";
close F;

for my $font (keys @fonts) {
	my $font_data = "";
	
	system("mkdir -p '$out_dir/$font'");
	
	my $fi = $fonts_info[$font];
	
	print "Font $font ".$fi->{w}."x".$fi->{h}."\n";
	
	my $chars = $fonts[$font];
	
	next if !exists $chars->[0xFFFF];
	my $undef_char = $chars->[0xFFFF]->[3];
	
	for my $ch (@$chars) {
		next if !defined $ch;
		
		my ($codepoint, $w, $h, $pixels) = @$ch;
		
		next if $undef_char eq $pixels && $codepoint != 0xFFFF;
		next if !$w || !$h;
		
		if (length($pixels) < $w * $h * 2) {
			printf("%04X: broken %d < %d\n", $codepoint, length($pixels), $w * $h * 2);
			next;
		}
		
		my $is_empty = 1;
		
		my $im = GD::Image->new($w, $h, 1);
		for (my $y = 0; $y < $h; $y++) {
			for (my $x = 0; $x < $w; $x++) {
				my ($color) = unpack("v", substr($pixels, ($y * $w + $x) * 2, 2));
				
				$is_empty = 0 if $color != 0x0000 && $color != 0xF81F;
				
				if ($color == 0xFFFF) {
					my $c = $im->colorAllocate(0xFF, 0xFF, 0xFF);
					$im->setPixel($x, $y, $c);
				} else {
					my $c = $im->colorAllocate(0x00, 0x00, 0x00);
					$im->setPixel($x, $y, $c);
				}
				
				# my $r = int((($color >> 11) & 0x1F) * 0xFF / 0x1F);
				# my $g = int((($color >> 5) & 0x3F) * 0xFF / 0x3F);
				# my $b = int(($color & 0x1F) * 0xFF / 0x1F);
				
				# my $c = $im->colorAllocate($r, $g, $b);
				# $im->setPixel($x, $y, $c);
			}
		}
		
		# if (!$is_empty) {
			open F, ">$out_dir/$font/".sprintf("%04x", $codepoint).".png";
			print F $im->png;
			close F;
		# }
	}
}
