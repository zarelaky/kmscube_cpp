
# load ROOTFS varible
. rootfs.sh

TC=/opt/tc/gcc-linaro-arm-linux-gnueabihf/
PATH=$TC/bin:$PATH

SYSROOT=$ROOTFS
export LDFLAGS="-L$SYSROOT/usr/lib -lasound"
export CFLAGS="-I$SYSROOT/usr/include"
export CXXFLAGS="-I$SYSROOT/usr/include"
PKG_CONFIG_PATH=$SYSROOT/usr/lib/pkgconfig
PKG_CONFIG_LIBDIR=$PKG_CONFIG_PATH
PKG_CONFIG_SYSROOT_DIR=$SYSROOT
PKG_CONFIG_ALLOW_SYSTEM_CFLAGS=YES
PKG_CONFIG_ALLOW_SYSTEM_LIBS=YES
export PATH SYSROOT PKG_CONFIG_PATH PKG_CONFIG_SYSROOT_DIR PKG_CONFIG_LIBDIR PKG_CONFIG_ALLOW_SYSTEM_CFLAGS PKG_CONFIG_ALLOW_SYSTEM_LIBS

