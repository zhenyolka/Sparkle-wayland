#ifndef WERE1_RING_BUFFER_H
#define WERE1_RING_BUFFER_H

#include <stdint.h>

struct were1_ring_buffer
{
    int size;
    uint64_t write_position;
    uint64_t read_position;
    char data[0];
};

#ifdef  __cplusplus
extern "C" {
#endif

void were1_ring_buffer_initialize(struct were1_ring_buffer *buffer, int size);
void were1_ring_buffer_finish(struct were1_ring_buffer *buffer);
int were1_ring_buffer_write_start(struct were1_ring_buffer *buffer, char **p, int size);
void were1_ring_buffer_write_finish(struct were1_ring_buffer *buffer, int size);
int were1_ring_buffer_read_start(struct were1_ring_buffer *buffer, char **p, int size);
void were1_ring_buffer_read_finish(struct were1_ring_buffer *buffer, int size);
int were1_ring_buffer_bytes_available(struct were1_ring_buffer *buffer);
void were1_ring_buffer_reset(struct were1_ring_buffer *buffer);

#ifdef  __cplusplus
}
#endif

#endif // WERE1_RING_BUFFER_H
