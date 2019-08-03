
package.path = package.path .. ";/data/data/com.sion.sparkle/?.lua"
local sparkle = require "sparkle"


function start()

    -- os.execute("/data/data/com.sion.sparkle/user.sh start");

    local shell = sparkle.shell.open();

    sparkle.check_mount(shell, "/data/local/tmp/fedora", "/dev/block/mmcblk0p36", nil, "noatime");
    sparkle.shell.critical(shell, "cd /data/local/tmp/fedora");
    if (not sparkle.exists(shell, "usr")) then
        error();
    end

    sparkle.check_mount(shell, "dev",       "/dev",     nil,        "bind");
    sparkle.check_mount(shell, "dev/pts",   "devpts",   "devpts",   nil);
    sparkle.check_mount(shell, "proc",      "proc",     "proc",     nil);
    sparkle.check_mount(shell, "sys",       "sysfs",    "sysfs",    nil);
    sparkle.check_mount(shell, "tmp",       "tmpfs",    "tmpfs",    nil);
    sparkle.check_mount(shell, "dev/shm",   "tmpfs",    "tmpfs",    nil);
    sparkle.check_mount(shell, "data",      "/data",    nil,        "bind");

    sparkle.shell.optional(shell, "chcon u:object_r:app_data_file:s0 tmp");

    if (not sparkle.exists(shell, "tmp/sparkle")) then
        sparkle.chroot(shell, "sion", "mkdir /tmp/sparkle");
    end

    if (not sparkle.exists(shell, "tmp/sparkle/wayland-0")) then
        sparkle.shell.critical(shell, "busybox ln -s /data/data/com.sion.sparkle/wayland-0 tmp/sparkle/wayland-0");
    end

    if (not sparkle.is_running(shell, "sshd")) then
        sparkle.chroot(shell, "root", "/usr/sbin/sshd");
        sparkle.sleep(shell, 1);
    end

    if (not sparkle.is_running(shell, "Xwayland")) then
        sparkle.chroot(shell, "sion", "XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0 -noreset &");
        sparkle.sleep(shell, 1);
        sparkle.chroot(shell, "sion", "DISPLAY=:0 /bin/sh ~/.xinitrc &");
        sparkle.sleep(shell, 1);
    end

    sparkle.shell.close(shell);

end
