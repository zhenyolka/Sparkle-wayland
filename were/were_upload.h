#ifndef WERE_UPLOAD_H
#define WERE_UPLOAD_H

typedef void (*uploader_type)(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, int x1, int y1, int x2, int y2);

namespace were_upload
{
    extern uploader_type uploader[3];
};

#endif // WERE_UPLOAD_H
