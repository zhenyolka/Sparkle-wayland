
local sparkle = require("sparkle");
local s;

local xwayland_start = " \
    mkdir -p /tmp/sparkle \
    ln -s /data/data/com.sion.sparkle/files/wayland-0 /tmp/sparkle/wayland-0 \
    export XDG_RUNTIME_DIR=/tmp/sparkle \
    Xwayland :0 -noreset > /dev/null & \
    sleep 0.5 \
    export DISPLAY=:0 \
    /bin/sh ~/.xinitrc > /dev/null & \
";

--------------------------------------------------------------------------------

function start()
    prepare_generic_container({source="/dev/block/mmcblk0p36", point="/data/local/tmp/fedora"});
    start_service({process="sshd", user="root", command="/sbin/sshd"});
    start_service({process="Xwayland", user="sion", command=xwayland_start});
    setup_rndis();
end

function sparkle_initialize()
    s = sparkle.create("su");
end

function sparkle_finalize()
    s.destroy();
end

--------------------------------------------------------------------------------

function prepare_generic_container(args)

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
    s.check_mount({source="/data/data/com.sion.sparkle/files", options="bind", point="data/data/com.sion.sparkle/files"});

    s.optional("chcon u:object_r:app_data_file:s0 tmp");

end

--------------------------------------------------------------------------------

function start_service(args)

    if (not s.running(args.process)) then
        s.chroot({user=args.user, command=args.command});
    end

end

--------------------------------------------------------------------------------

function setup_rndis()

    local rndis_config = "rndis,adb";
    local current_config = s.critical("getprop sys.usb.config");

    if (current_config ~= rndis_config) then
        s.critical("setprop sys.usb.config " .. rndis_config);
        s.sleep(1);
        s.critical("busybox ip link set rndis0 up");
        s.critical("busybox ip addr add 192.168.10.2/24 dev rndis0");
        s.critical("busybox ip route add default via 192.168.10.1");
    end

end

--------------------------------------------------------------------------------
