#!/system/bin/sh

#===============================================================================

SPARKLE_HOME="/data/data/com.sion.sparkle/"
CHROOT_HOME="/data/local/tmp/fedora/"
MY_USER="sion"

#===============================================================================

assert()
{
    STORED_EXIT_CODE=$?
    if [ ! $1 $2 $3 ]; then exit 1; fi
    return ${STORED_EXIT_CODE}
}

x_root()
{
    EUID=$(id -u)
    assert $? == 0
    test "${EUID}" == "0"
    return $?
}

run_as_root()
{
    su -c "/system/bin/sh $(realpath $0) $*"
    exit 0
}

x_mounted()
{
    mountpoint -q $1
    assert $? != 127
    return $?
}

x_running()
{
    PID=$(pidof -s $1)
    assert $? != 127
    return $?
}

x_exists()
{
    test -e $1
    return $?
}

#===============================================================================

user_install()
{
    x_root || run_as_root $*
    cp $0 ${SPARKLE_HOME}/user.sh
    chmod 0755 ${SPARKLE_HOME}/user.sh
    restorecon ${SPARKLE_HOME}/user.sh
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

    x_exists ${CHROOT_HOME}/usr
    assert $? == 0

    x_mounted ${CHROOT_HOME}/data ||
    {
        mkdir -p ${CHROOT_HOME}/data
        mount -o bind /data ${CHROOT_HOME}/data
        assert $? == 0
    }

    x_mounted ${CHROOT_HOME}/tmp ||
    {
        mkdir -p ${CHROOT_HOME}/tmp
        mount -t tmpfs tmpfs ${CHROOT_HOME}/tmp
        assert $? == 0
    }
    chcon "u:object_r:app_data_file:s0" ${CHROOT_HOME}/tmp

    x_exists ${CHROOT_HOME}/tmp/sparkle ||
    {
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "mkdir /tmp/sparkle"
        assert $? == 0
    }

    x_exists ${CHROOT_HOME}/tmp/sparkle/wayland-0 ||
    {
        ln -s ${SPARKLE_HOME}/wayland-0 ${CHROOT_HOME}/tmp/sparkle/wayland-0
        assert $? == 0
    }

    x_running Xwayland ||
    {
        #setenforce 0
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0 -noreset" &
        sleep 2
        #setenforce 1
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "DISPLAY=:0 /bin/sh ~/.xinitrc" &
        sleep 2
    }

    return 0
}

user_stop()
{
    return 0
}

#===============================================================================

set -x

user_$1 $*

#===============================================================================
