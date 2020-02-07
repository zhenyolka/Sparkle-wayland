
function critical()
{
    local rc

    eval "$1"
    rc=$?

    echo "critical: [$1] rc: ${rc}"

    if [ "${rc}" == "0" ]
    then
        return ${rc}
    else
        exit 1
    fi
}

function critical_conditional()
{
    local rc

    eval "$1"
    rc=$?

    echo "critical_conditional: [$1] rc: ${rc}"

    if [ "${rc}" == "0" -o "${rc}" == "1" ]
    then
        return ${rc}
    else
        exit 1
    fi
}

function optional()
{
    local rc

    eval "$1"
    rc=$?

    echo "optional: [$1] rc: ${rc}"

    return ${rc}
}

function is_root()
{
    local uid

    critical "uid=\$(id -u)"

    echo "uid: ${uid}"

    if [ "${uid}" == "0" ]
    then
        return 0
    else
        return 1
    fi
}

function restart_as_root()
{
    critical "su -c \"/system/bin/sh \$(realpath $0) ${ARGS}\""
    exit 1
}

function check_root()
{
    is_root || restart_as_root
}

function exists()
{
    critical_conditional "busybox test -e $1"
    return $?
}

function is_mounted()
{
    critical_conditional "busybox mountpoint -q $1"
    return $?
}

function check_mount()
{
    local "$@"
    local command

    is_mounted ${point?} ||
    {
        critical "busybox mkdir -p ${point?}"

        command="busybox mount"

        if [ "${type}" != "" ]
        then
            command="${command} -t ${type}"
        fi

        if [ "${options}" != "" ]
        then
            command="${command} -o ${options}"
        fi

        command="${command} ${source?} ${point?}"

        critical "${command}"
    }
}

function is_running()
{
    critical_conditional "busybox pidof -s $1"
    return $?
}

function sparkle_chroot()
{
    local "$@"

    critical "busybox chroot . /bin/su - ${user?} -c \"${command?}\""
}

ARGS="$@"

type busybox

