#!/system/bin/sh

#===============================================================================

SPARKLE_HOME="/data/data/com.sion.sparkle/"
CHROOT_HOME="/data/local/tmp/fedora/"
MY_USER="sion"

#===============================================================================

x_root()
{
    test "$(id -u)" == "0" && return 0
    return 1
}

run_as_root()
{
    su -c "/system/bin/sh $(realpath $0) $*"
    exit
}

x_mounted()
{
    mountpoint -q $1 && return 0
    return 1
}

x_running()
{
    test "$(pidof -s $1)" != "" && return 0
    return 1
}

x_exists()
{
    test -e $1 && return 0
    return 1
}

#===============================================================================

user_install()
{
    x_root || run_as_root $*
    cp $0 ${SPARKLE_HOME}/user.sh
    chmod 0755 ${SPARKLE_HOME}/user.sh
    return 0
}

user_uninstall()
{
    x_root || run_as_root $*
    rm ${SPARKLE_HOME}/user.sh
    return 0
}

#===============================================================================

user_start()
{
    x_root || run_as_root $*

    sleep 2

    x_mounted ${CHROOT_HOME}/data ||
    (
        mkdir -p ${CHROOT_HOME}/data
        mount -o bind /data ${CHROOT_HOME}/data
    )

    x_mounted ${CHROOT_HOME}/tmp ||
    (
        mkdir -p ${CHROOT_HOME}/tmp
        mount -t tmpfs tmpfs ${CHROOT_HOME}/tmp
    )

    x_exists ${CHROOT_HOME}/tmp/sparkle ||
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "mkdir /tmp/sparkle"

    x_exists ${CHROOT_HOME}/tmp/sparkle/wayland-0 ||
        ln -s ${SPARKLE_HOME}/wayland-0 ${CHROOT_HOME}/tmp/sparkle/wayland-0

    x_running Xwayland ||
    (
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0" &
        sleep 2
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "DISPLAY=:0 /bin/sh ~/.xinitrc" &
        sleep 2
    )

    return 0
}

user_stop()
{
    return 0
}

#===============================================================================

set -e -x

user_$1 $*

#===============================================================================
