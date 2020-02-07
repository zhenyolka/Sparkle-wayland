
source /data/data/com.sion.sparkle/files/sparkle.sh

function user_start()
{
    #check_generic_container source="/dev/block/mmcblk0p36" point="/data/local/tmp/fedora" user="sion"
    #check_service user="root" process_name="sshd" command="/sbin/sshd"
    #check_service user="sion" process_name="Xwayland" command="${xwayland_command?}"
}

function check_generic_container()
{
    local "$@"

    check_root

    exists "${point?}/bin/su" || check_mount options=noatime source=${source?} point=${point?}

    critical "cd ${point?}"

    check_mount options=bind source=/dev point=dev
    check_mount type=devpts source=devpts point=dev/pts
    check_mount type=proc source=proc point=proc
    check_mount type=sysfs source=sysfs point=sys
    check_mount type=tmpfs source=tmpfs point=tmp
    check_mount type=tmpfs source=tmpfs point=dev/shm
    check_mount options=bind source=/data point=data

    optional "chcon u:object_r:app_data_file:s0 tmp"
}

function check_service()
{
    local "$@"

    is_running "${process_name?}" ||
    {
        sparkle_chroot user="${user?}" command="${command?}"
    }
}

xwayland_command=\
"
    mkdir -p /tmp/sparkle
    ln -s /data/data/com.sion.sparkle/files/wayland-0 /tmp/sparkle/wayland-0
    XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0 -noreset &
    sleep 1
    DISPLAY=:0 /bin/sh ~/.xinitrc &
    sleep 1
"

user_start
