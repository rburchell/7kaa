
# Set $wine_prefix to the prefix where wine is installed.
# This is typically "/usr".
$wine_prefix = "/usr";

# This sets up the include paths.
# For Windows, the DirectX SDK must be available
# I will include a zip file that extracts to c:\dx7sdk\ 
# to take care of that.
our @includes = (
"include",
"$wine_prefix/include/wine/windows",
"$wine_prefix/include/wine/msvcrt"
);	

if($platform eq "windows") {
	@includes = (
	"include",
	"C:\\dx7sdk\\include"
	);
}

@includes = map { "-I$_" } @includes;

# The following sets flags used during compiling.
# The flags that are known to work:
# AMPLUS
# DISABLE_MULTI_PLAYER
# FRENCH
# GERMAN
# SPANISH
our @defines = qw(
AMPLUS
);
@defines = map { "-D$_" } @defines;

1;
