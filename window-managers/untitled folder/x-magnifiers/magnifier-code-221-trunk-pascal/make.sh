
#
# Detects and parses the architecture
#

ARCH=$(uname -m)

case "$ARCH" in

 "i686") ARCH="i386";;

 "i586") ARCH="i386";;

 "i486") ARCH="i386";;

esac

echo "Target architecture: $ARCH"

#
# Detects and parses the OS
#

OS="linux"

echo "Target operating system: $OS"


fpc -S2cgi -O1 -gl -vewnhi -l -Fu/usr/lib/lazarus/lcl/units/$ARCH-$OS/ -Fu/usr/lib/lazarus/lcl/units/$ARCH-$OS/ -Fu/usr/lib/lazarus/lcl/units/$ARCH-$OS/gtk2/ -Fu/usr/lib/lazarus/packager/units/$ARCH-$OS/ -Fu. -o./magnifier -dLCL -dLCLgtk2 magnifier.dpr
