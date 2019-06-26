#!/bin/sh

f_source()
{
    PACKAGE=${3?}

    source ./configure/${PACKAGE}
    package_source
    cd ${MOGRYPH_HOME}
}

f_configure()
{
    PACKAGE=${3?}

    source ./configure/${PACKAGE}
    package_configure
    cd ${MOGRYPH_HOME}
}

f_make()
{
    PACKAGE=${3?}

    source ./configure/${PACKAGE}
    package_make
    cd ${MOGRYPH_HOME}
}

f_install()
{
    PACKAGE=${3?}

    source ./configure/${PACKAGE}
    package_install
    cd ${MOGRYPH_HOME}

    find ${MOGRYPH_INSTALL_DIR} -name "*.la" -exec rm {} \;
    find ${MOGRYPH_TOOLCHAIN}/sysroot/ -name "*.la" -exec rm {} \;
}

set -e

export MOGRYPH_ARCH=${1?}
COMMAND=${2?}

if [ "${MOGRYPH_ARCH}" == "arm" ]
then
    export MOGRYPH_TOOLCHAIN="/home/sion/android/toolchains/arm"
    export MOGRYPH_HOST="arm-linux-androideabi"
    export MOGRYPH_INSTALL_DIR="/home/sion/projects/sparkle-w/foreign/install/armeabi-v7a"
elif [ "${MOGRYPH_ARCH}" == "arm64" ]
then
    export MOGRYPH_TOOLCHAIN="/home/sion/android/toolchains/arm64"
    export MOGRYPH_HOST="aarch64-linux-android"
    export MOGRYPH_INSTALL_DIR="/home/sion/projects/sparkle-w/foreign/install/arm64-v8a"
elif [ "${MOGRYPH_ARCH}" == "x86" ]
then
    export MOGRYPH_TOOLCHAIN="/home/sion/android/toolchains/x86"
    export MOGRYPH_HOST="i686-linux-android"
    export MOGRYPH_INSTALL_DIR="/home/sion/projects/sparkle-w/foreign/install/x86"
elif [ "${MOGRYPH_ARCH}" == "x86_64" ]
then
    export MOGRYPH_TOOLCHAIN="/home/sion/android/toolchains/x86_64"
    export MOGRYPH_HOST="x86_64-linux-android"
    export MOGRYPH_INSTALL_DIR="/home/sion/projects/sparkle-w/foreign/install/x86_64"
else
    exit 1
fi

export MOGRYPH_BUILD="x86_64-redhat-linux"
export MOGRYPH_PREFIX="/usr"
export PATH="${PATH}:${MOGRYPH_TOOLCHAIN}/bin"
export PKG_CONFIG_PATH=""
export PKG_CONFIG_LIBDIR="${MOGRYPH_TOOLCHAIN}/sysroot/usr/lib/pkgconfig:${MOGRYPH_TOOLCHAIN}/sysroot/usr/share/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="${MOGRYPH_TOOLCHAIN}/sysroot"
export MOGRYPH_HOME=$(pwd)

if [ "${COMMAND}" == "source" ]
then
    f_source "$@"
elif [ "${COMMAND}" == "configure" ]
then
    f_configure "$@"
elif [ "${COMMAND}" == "make" ]
then
    f_make "$@"
elif [ "${COMMAND}" == "install" ]
then
    f_install "$@"
elif [ "${COMMAND}" == "auto" ]
then
    f_configure "$@"
    f_make "$@"
    f_install "$@"
else
    exit 1
fi

