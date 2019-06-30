#!/system/bin/sh

SPARKLE_HOME="/data/data/com.sion.sparkle/"
CHROOT_HOME="/data/local/tmp/fedora/"
MY_USER="sion"

as_root()
{
    if [ "${EUID}" != "0" ]
    then
        REAL_PATH=$(realpath $0)
        su -c "/system/bin/sh ${REAL_PATH} $*"
        exit
    fi
}

user_install()
{
    as_root $*
    cp $0 ${SPARKLE_HOME}/user.sh
    chmod 666 ${SPARKLE_HOME}/user.sh
}

user_uninstall()
{
    as_root $*
    rm ${SPARKLE_HOME}/user.sh
}

user_start()
{
    echo "user start ${EUID}"
    as_root $*

    sleep 2

    mkdir -p ${CHROOT_HOME}/data

    if ! mountpoint -q ${CHROOT_HOME}/data
    then
        mount -o bind /data ${CHROOT_HOME}/data
    fi

    mkdir -p ${CHROOT_HOME}/tmp

    if ! mountpoint -q ${CHROOT_HOME}/tmp
    then
        mount -t tmpfs tmpfs ${CHROOT_HOME}/tmp
    fi

    if [ ! -d "${CHROOT_HOME}/tmp/sparkle" ]
    then
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "mkdir /tmp/sparkle"
    fi

    if [ ! -e "${CHROOT_HOME}/tmp/sparkle/wayland-0" ]
    then
        ln -s ${SPARKLE_HOME}/wayland-0 ${CHROOT_HOME}/tmp/sparkle/wayland-0
    fi

    XWAYLAND_PID=$(pidof -s Xwayland || true)
    if [ "${XWAYLAND_PID}" == "" ]
    then
        setenforce 0
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0" &
        sleep 2
        #setenforce 1
        chroot ${CHROOT_HOME} /bin/su - ${MY_USER} -c "DISPLAY=:0 /bin/sh ~/.xinitrc" &
        sleep 2
    fi
}

user_stop()
{
    echo "user stop ${EUID}"
    as_root $*
}

set -e
set -x

EUID=$(id -u)

user_$1 $*

echo "Ok."

