#!/usr/bin/perl
use File::stat;

# determine which set of commands we should use
our $platform = "linux";
our $exe = "7kaa.exe";
print "This build script defaults to Linux, to build this game " .
		"for windows, please use 'build.pl windows'\n\n";

# read in the arguments, 
$argAmt = $#ARGV + 1;

if($argAmt == 1 && ($ARGV[0] eq "windows")) {
	# set our platform to windows
	$platform = "windows";
}

print "Building for $platform...\n\n";

require "files.pl";
require "opts.pl";

if($platform eq "windows") {
	# remove ovideo.cpp from files.pl
	# this causes compile problems in windows
	for($count = 0; $count < $#c_files; $count++) {
		if($c_files[$count] eq "OVIDEO") {
			splice(@c_files,$count,1);			
		}
	}
}

chdir "asm" or die "Missing asm dir";
foreach (@asm_files) {
  my $flag = 1;

  # check modification time
  if (-e "$_.o") {
    my $objstat = stat("$_.o");
    my $cppstat = stat("$_.asm");
    $cppstat->mtime >= $objstat->mtime or $flag = 0;
  }

  if ($flag) {
	# determine if we should use -coff for win32 or -elf for linux
    my $cmd = $platform eq "windows" ? "jwasm -q -coff -zt1 $_.asm" 
		: "jwasm -q -elf -zt1 $_.asm";
    print "$cmd\n";
    system $cmd and die "jwasm failed";
  }
}
chdir "..";

foreach (@c_files) {
  my $flag = 1;

  # check modification time
  if (-e "$_.o") {
    my $objstat = stat("$_.o");
    my $cppstat = stat("$_.cpp");
    $cppstat->mtime >= $objstat->mtime or $flag = 0;
  }

  if ($flag) {
	
    my $optlevel = $_ eq "OWORLD" ? "-O1" : "-O2";
	# this line is the same in both windows and linux
	# my $cmd = "g++ $optlevel -g -c @defines @includes $_.cpp";
	my $cmd = "g++ $optlevel -c @defines @includes $_.cpp";
    print "$cmd\n";
    system $cmd and die "g++ failed";
  }
}

@obj_files = map { "$_.o" } @c_files;
push ( @obj_files, map { "asm/$_.obj" } @asm_files );

#my $exe = "AM.exe";
my $flag = 0;
foreach (@obj_files) {
  (-e $_) or die "Missing file '$_' for linking";

  if (-e $exe) {
	my $exestat = stat($exe);
	my $objstat = stat($_);
	$objstat->mtime >= $exestat->mtime and $flag = 1;
  } else {
	$flag = 1;
  }
}

if ($flag) {
  
  my @libs = qw(
	gdi32 ddraw msvcrt ole32 dinput dplay dplayx dsound winmm
  );
  
  if($platform eq "windows") {
	  @libs = qw(
		gdi32 ddraw msvcrt ole32 dinput dxguid dplayx dsound winmm
	  );
  }

  my $iconObj = "iconres.o";
  
  if($platform eq "windows") {
	  # build the icon here
	  my $iconRC = "ico.rc";
	  my $cmd = "windres -i $iconRC -o $iconObj";
	  print $cmd,"\n";
	  system $cmd and die "windres failed to create icon object.";
  }
  
  @libs = map { "-l$_" } @libs;
  
  # added $iconObj var to compile the exe with the icon object
  # build seperately if you're using windows or linux
  # linux uses wineg++ and windows uses g++ (through mingw)
  # the windows build line also includes the $iconObj which links
  # the icon obj to the final exe
  my $cmd = $platform eq "windows" ? "g++ @obj_files $iconObj @libs -mwindows -o $exe" 
		: "wineg++ @obj_files @libs -o $exe"; ;
 
  print $cmd,"\n";
  system $cmd and die "g++ failed\n";

  system "copy $exe C:\\jeff\\7kaa\\install" and die "oops";
}

print "\nBuild Complete\n";
