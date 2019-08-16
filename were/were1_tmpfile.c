#include "were1_tmpfile.h"
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>


int were1_tmpfile_create(off_t size)
{
    static const char template[] = "/were-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path)
    {
#if 0
        errno = ENOENT;
        return -1;
#else
        path = "/tmp";
#endif
    }

    name = malloc(strlen(path) + sizeof(template));
    if (!name)
        return -1;

    strcpy(name, path);
    strcat(name, template);

    fd = mkstemp(name);
    if (fd == -1)
    {
        free(name);
        return -1;
    }

    unlink(name);
    free(name);

    if (ftruncate(fd, size) == -1)
    {
        close(fd);
        return -1;
    }

    return fd;
}

int were1_tmpfile_map(void **p, size_t length, int fd)
{
    void *p__ = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p__ == MAP_FAILED)
        return -1;

    *p = p__;

    return 0;
}

int were1_tmpfile_unmap(void **p, size_t length)
{
    munmap(*p, length);

    return 0;
}
