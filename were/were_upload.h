#ifndef WERE_UPLOAD_H
#define WERE_UPLOAD_H

#include <cstdint>

typedef void (*uploader_type)(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

namespace were_upload
{
    extern uploader_type uploader[3];
} // namespace were_upload

#endif // WERE_UPLOAD_H
