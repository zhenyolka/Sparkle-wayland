#!/usr/bin/lua

local sparkle = require "sparkle";

function start()
    --os.execute("./user.sh");
    start_generic_container({source="/dev/block/mmcblk0p36", point="/data/local/tmp/fedora", user="sion"});
    --start_sparkle_container({source="/dev/block/mmcblk0p36", point="/data/local/tmp/fedora", user="sion"});
end

function start_generic_container(args)
    local s = sparkle.create();

    if (not s.exists(args.point .. "/bin/su")) then
        s.check_mount({source=args.source, options="noatime", point=args.point});
    end

    s.critical("cd " .. args.point);


    s.check_mount({source="/dev", options="bind", point="dev"});
    s.check_mount({source="devpts", type="devpts", point="dev/pts"});
    s.check_mount({source="proc", type="proc", point="proc"});
    s.check_mount({source="sysfs", type="sysfs", point="sys"});
    s.check_mount({source="tmpfs", type="tmpfs", point="tmp"});
    s.check_mount({source="tmpfs", type="tmpfs", point="dev/shm"});
    s.check_mount({source="/data", options="bind", point="data"});


    s.optional("chcon u:object_r:app_data_file:s0 tmp");

    if (not s.exists("tmp/sparkle")) then
        s.chroot({user=args.user, command="mkdir /tmp/sparkle"});
    end

    if (not s.exists("tmp/sparkle/wayland-0")) then
        s.critical("busybox ln -s /data/data/com.sion.sparkle/wayland-0 tmp/sparkle/wayland-0");
    end

    --if (not s.running("sshd")) then
    --    s.chroot({command="/usr/sbin/sshd"});
    --    s.sleep(1);
    --end

    if (not s.running("Xwayland")) then
        s.chroot({user=args.user, command="XDG_RUNTIME_DIR=/tmp/sparkle Xwayland :0 -noreset &"});
        s.sleep(1);
        s.chroot({user=args.user, command="DISPLAY=:0 /bin/sh ~/.xinitrc &"});
        s.sleep(1);
    end

    s.destroy();
end

function start_sparkle_container(args)
    local s = sparkle.create();

    if (not s.exists(args.point .. "/bin/su")) then
        s.check_mount({source=args.source, options="noatime", point=args.point});
    end

    s.critical("cd " .. args.point);

    s.critical("./init");

    s.destroy();
end
