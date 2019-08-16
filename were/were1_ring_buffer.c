#include "were1_ring_buffer.h"


void were1_ring_buffer_initialize(struct were1_ring_buffer *buffer, int size)
{
    buffer->size = size;
    buffer->write_position = 0;
    buffer->read_position = 0;
}

void were1_ring_buffer_finish(struct were1_ring_buffer *buffer)
{
}

int were1_ring_buffer_write_start(struct were1_ring_buffer *buffer, char **p, int size)
{
    int write_position = buffer->write_position % buffer->size;
    int max = buffer->size - write_position;
    if (size > max)
        size = max;

    *p = buffer->data + write_position;

    return size;
}

void were1_ring_buffer_write_finish(struct were1_ring_buffer *buffer, int size)
{
    buffer->write_position += size;
}

int were1_ring_buffer_read_start(struct were1_ring_buffer *buffer, char **p, int size)
{
    int read_position = buffer->read_position % buffer->size;
    int max = buffer->size - read_position;
    if (size > max)
        size = max;

    *p = buffer->data + read_position;

    return size;
}

void were1_ring_buffer_read_finish(struct were1_ring_buffer *buffer, int size)
{
    buffer->read_position += size;
}

int were1_ring_buffer_bytes_available(struct were1_ring_buffer *buffer)
{
    return buffer->write_position - buffer->read_position;
}
