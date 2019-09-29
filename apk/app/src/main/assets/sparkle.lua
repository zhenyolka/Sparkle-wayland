
local home_dir = "/data/data/com.sion.sparkle/files";

--------------------------------------------------------------------------------

local shell = {};

shell.id = 0;

function shell.new_id()
    shell.id = shell.id + 1;
    return shell.id - 1;
end

function shell.open(path, mode)
    local handle = io.open(path, mode);
    if (handle == nil) then
        error();
    end
    return handle;
end

function shell.create(shell_command)

    local self = {};

    ----------------------------------------------------------------------------

    local id_ = shell.new_id();
    local in_pipe_path_ = home_dir .. "/shell-" .. tostring(id_) .. "-in";
    local out_pipe_path_ = home_dir .. "/shell-" .. tostring(id_) .. "-out";
    local result_pipe_path_ = home_dir .. "/shell-" .. tostring(id_) .. "-result";

    os.execute("busybox mkfifo -m 0600 " .. in_pipe_path_);
    os.execute("busybox mkfifo -m 0600 " .. out_pipe_path_);
    os.execute("busybox mkfifo -m 0600 " .. result_pipe_path_);

    os.execute(shell_command .. " < " .. in_pipe_path_ .. " > /dev/null &");

    local in_pipe_ = shell.open(in_pipe_path_, "w");

    ----------------------------------------------------------------------------

    function self.destroy()

        in_pipe_.close(in_pipe_);

        os.execute("busybox unlink " .. result_pipe_path_);
        os.execute("busybox unlink " .. out_pipe_path_);
        os.execute("busybox unlink " .. in_pipe_path_);

    end

    ----------------------------------------------------------------------------

    local function write_(text)
        in_pipe_.write(in_pipe_, text);
        in_pipe_.flush(in_pipe_);
    end

    local function read_()
        local out_pipe = shell.open(out_pipe_path_, "r");
        local result_pipe = shell.open(result_pipe_path_, "r");
        local out_text = out_pipe.read(out_pipe, "*a");
        local result_text = result_pipe.read(result_pipe, "*a");
        if (out_text == nil or result_text == nil) then
            error();
        end
        result_pipe.close(result_pipe);
        out_pipe.close(out_pipe);
        return tonumber(result_text), string.sub(out_text, 1, -2);
    end

    ----------------------------------------------------------------------------

    function self.execute(command)
        write_("exec 1> " .. out_pipe_path_ .. "\n");
        write_(command .. "\n");
        write_("echo $? > " .. result_pipe_path_ .. "\n");
        write_("exec 1>&-\n");
        local rc, text = read_();
        return rc, text;
    end

    function self.execute1(command)
        write_("exec 1> /dev/null\n");
        write_(command .. "\n");
        write_("exec 1>&-\n");
    end

    function self.critical(command)
        local rc, text = self.execute(command);
        print("critical [" .. command .. "] rc: " .. tostring(rc));
        if (rc == 0) then
            return text;
        else
            error();
        end
    end

    function self.critical_conditional(command)
        local rc, text = self.execute(command);
        print("critical_conditional [" .. command .. "] rc: " .. tostring(rc));
        if (rc == 0) then
            return true, text;
        elseif (rc == 1) then
            return false, text;
        else
            error();
        end
    end

    function self.optional(command)
        local rc, text = self.execute(command);
        print("optional [" .. command .. "] rc: " .. tostring(rc));
        return rc, text;
    end

    ----------------------------------------------------------------------------

    return self;

end

--------------------------------------------------------------------------------

local sparkle = {};

function sparkle.create(shell_command)

    local self = shell.create(shell_command)

    ----------------------------------------------------------------------------

    function self.sleep(time)
        self.critical("busybox sleep " .. tostring(time));
    end

    function self.mounted(point)
        return self.critical_conditional("busybox mountpoint " .. point);
    end

    function self.exists(path)
        return self.critical_conditional("busybox test -e " .. path);
    end

    function self.running(name)
        return self.critical_conditional("busybox pidof -s " .. name);
    end

    function self.mount(args)
        local command = "busybox mount ";
        command = command .. args.source;
        if (args.type ~= nil) then
            command = command .. " -t " .. args.type;
        end
        if (args.options ~= nil) then
            command = command .. " -o " .. args.options;
        end
        command = command .. " " .. args.point;
        self.critical(command);
    end

    function self.check_mount(args)
        if (not self.mounted(args.point)) then
            self.critical("busybox mkdir -p " .. args.point);
            self.mount(args);
        end
    end

    function self.chroot(args)
        local command = "busybox chroot ";
        if (args.path == nil) then
            command = command .. ".";
        else
            command = command .. args.path;
        end
        command = command .. " /bin/su -";
        if (args.user ~= nil) then
            command = command .. " " .. args.user;
        end
        command = command .. " -c \"" .. args.command .. "\"";
        self.critical(command);
    end

    ----------------------------------------------------------------------------

    return self;

end


return sparkle;

