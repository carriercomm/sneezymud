#!/usr/bin/perl  -- Peel

foreach(`rlog -l *.cc *.h makefile README`){
  $fileName=$1 if /Working file: (.*)/;
  print "$fileName *** $_" if /locked by/;
  print "$fileName *** $_" if /^branch: / && !/5.1.1$/;
  print "$fileName *** $_" if /^head: / && !/5.1$/;
}
