#!/usr/bin/perl  -- Peel

foreach(`rlog -l *.cc *.h makefile README`){
  $fileName=$1 if /Working file: (.*)/;
  printf("%-16s *** $_", $fileName) if /locked by/ || 
                        (/^branch: / && !/5.1.1$/) || (/^head: / && !/5.1$/);
}
