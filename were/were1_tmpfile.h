#ifndef WERE1_TMPFILE_H
#define WERE1_TMPFILE_H

#include <sys/types.h>

#ifdef  __cplusplus
extern "C" {
#endif

int were1_tmpfile_create(off_t size);
int were1_tmpfile_map(void **p, size_t length, int fd);
int were1_tmpfile_unmap(void **p, size_t length);

#ifdef  __cplusplus
}
#endif

#endif // WERE1_TMPFILE_H
