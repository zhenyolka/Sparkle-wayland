
local sparkle = {};

sparkle.shell = {};

function sparkle.shell.open()

    local shell = {};

    os.execute("busybox mkfifo -m 0600 shell_stdin > /dev/null 2>&1");
    os.execute("busybox mkfifo -m 0600 shell_stdout > /dev/null 2>&1");
    os.execute("su < shell_stdin > shell_stdout &");

    shell.stdin = io.open("shell_stdin", "w");
    shell.stdout = io.open("shell_stdout", "r");

    return shell;
end

function sparkle.shell.close(shell)
    sparkle.shell.write(shell, "exit\n");
    shell.stdout.close(shell.stdout);
    shell.stdin.close(shell.stdin);
end

function sparkle.shell.write(shell, text)
    shell.stdin.write(shell.stdin, text);
    shell.stdin.flush(shell.stdin);
end

function sparkle.shell.read(shell)
    local text = "";
    while (true) do
        local line = shell.stdout.read(shell.stdout, "*l");
        if (string.sub(line, 1, 3) == "RC=") then
            local rc = tonumber(string.sub(line, 4, -1));
            return rc, text;
        else
            text = text .. line;
        end
    end
end

function sparkle.shell.execute(shell, command)
    sparkle.shell.write(shell, command .. "\n");
    sparkle.shell.write(shell, "echo RC=$?\n");
    local rc, text = sparkle.shell.read(shell);

    return rc, text;
end

function sparkle.shell.critical(shell, command)
    local rc, text = sparkle.shell.execute(shell, command);
    print("critical \"" .. command .. "\" rc: " .. tostring(rc));
    if (rc == 0) then
        return text;
    else
        error();
    end
end

function sparkle.shell.critical_conditional(shell, command)
    local rc, text = sparkle.shell.execute(shell, command);
    print("critical_conditional \"" .. command .. "\" rc: " .. tostring(rc));
    if (rc == 0) then
        return true, text;
    elseif (rc == 1) then
        return false, text;
    else
        error();
    end
end

function sparkle.shell.optional(shell, command)
    local rc, text = sparkle.shell.execute(shell, command);
    print("optional \"" .. command .. "\" rc: " .. tostring(rc));
    return rc, text;
end

function sparkle.sleep(shell, time)
    sparkle.shell.critical(shell, "busybox sleep " .. tostring(time));
end

function sparkle.is_mounted(shell, p)
    return sparkle.shell.critical_conditional(shell, "busybox mountpoint " .. p);
end

function sparkle.exists(shell, path)
    return sparkle.shell.critical_conditional(shell, "busybox test -e " .. path);
end

function sparkle.mount(shell, p, s, t, o)
    local command = "busybox mount ";
    command = command .. s;
    if (t ~= nil) then
        command = command .. " -t " .. t;
    end
    if (o ~= nil) then
        command = command .. " -o " .. o;
    end
    command = command .. " " .. p;
    sparkle.shell.critical(shell, command);
end

function sparkle.is_running(shell, name)
    return sparkle.shell.critical_conditional(shell, "busybox pidof -s " .. name);
end

function sparkle.check_mount(shell, p, s, t, o)
    if (not sparkle.is_mounted(shell, p)) then
        sparkle.shell.critical(shell, "busybox mkdir -p " .. p);
        sparkle.mount(shell, p, s, t, o);
    end
end

function sparkle.chroot(shell, user, command)
    local c = "busybox chroot . /bin/su - " .. user .. " -c \"" .. command .. "\"";
    sparkle.shell.critical(shell, c);
end

return sparkle;
