#ifndef WERE1_UNIX_SOCKET_H
#define WERE1_UNIX_SOCKET_H

#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

int were1_unix_server_create(const char *path);
void were1_unix_server_shutdown(const char *path, int fd);
int were1_unix_server_accept(int fd);
void were1_unix_server_reject(int fd);

int were1_unix_socket_connect(const char *path);
void were1_unix_socket_shutdown(int fd);
int were1_unix_socket_send_all(int fd, const void *buffer, size_t length);
int were1_unix_socket_receive_all(int fd, void *buffer, size_t length);
int were1_unix_socket_bytes_available(int fd);
int were1_unix_socket_send_fds(int fd, const int *fds, int n);
int were1_unix_socket_receive_fds(int fd, int *fds, int n);

#ifdef  __cplusplus
}
#endif


#endif // WERE1_UNIX_SOCKET_H
