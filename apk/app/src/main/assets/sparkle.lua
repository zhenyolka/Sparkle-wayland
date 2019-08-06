local sparkle = {};

sparkle.create = function()
    local self = {};

    os.execute("busybox mkfifo -m 0600 shell_stdin > /dev/null 2>&1");
    os.execute("busybox mkfifo -m 0600 shell_stdout > /dev/null 2>&1");
    os.execute("su < shell_stdin > shell_stdout &");

    local stdin = io.open("shell_stdin", "w");
    if (stdin == nil) then
        error();
    end
    local stdout = io.open("shell_stdout", "r");
    if (stdout == nil) then
        error();
    end

    function self.destroy()
        stdout.close(stdout);
        stdin.close(stdin);
    end

    local function write(text)
        stdin.write(stdin, text);
        stdin.flush(stdin);
    end

    local function read_until_rc()
        local text = "";
        while (true) do
            local line = stdout.read(stdout, "*l");
            if (string.sub(line, 1, 3) == "RC=") then
                local rc = tonumber(string.sub(line, 4, -1));
                return rc, text;
            elseif (line == nil) then
                exit();
            else
                text = text .. line;
            end
        end
    end

    local function execute(command)
        write(command .. "\n");
        write("echo RC=$?\n");
        local rc, text = read_until_rc();
        return rc, text;
    end

    function self.critical(command)
        local rc, text = execute(command);
        print("critical \"" .. command .. "\" rc: " .. tostring(rc));
        if (rc == 0) then
            return text;
        else
            error();
        end
    end

    function self.critical_conditional(command)
        local rc, text = execute(command);
        print("critical_conditional \"" .. command .. "\" rc: " .. tostring(rc));
        if (rc == 0) then
            return true, text;
        elseif (rc == 1) then
            return false, text;
        else
            error();
        end
    end

    function self.optional(command)
        local rc, text = execute(command);
        print("optional \"" .. command .. "\" rc: " .. tostring(rc));
        return rc, text;
    end

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

    return self;
end


--sparkle.static_function()
--end


return sparkle;
