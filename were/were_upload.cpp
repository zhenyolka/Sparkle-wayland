#include "were_upload.h"
#include <cstdint>
#include <cstring>

struct sparkle_pixel
{
    uint8_t v1;
    uint8_t v2;
    uint8_t v3;
    uint8_t v4;
};

static void uploader_0(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, int x1, int y1, int x2, int y2)
{
    const char *source_c = reinterpret_cast<const char *>(source);
    char *destination_c = reinterpret_cast<char *>(destination);
    int offset = x1 * 4;
    int length = (x2 - x1) * 4;

    for (int y = y1; y < y2; ++y)
    {
        std::memcpy
        (
            (destination_c + destination_stride_bytes * y + offset),
            (source_c + source_stride_bytes * y + offset),
            length
        );
    }
}

static void uploader_1(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, int x1, int y1, int x2, int y2)
{
#if 0
    const uint32_t *source_i = reinterpret_cast<const uint32_t *>(source);
    uint32_t *destination_i = reinterpret_cast<uint32_t *>(destination);

    for (int y = y1; y < y2; ++y)
    {
        const uint32_t *source_i_l = &source_i[source_stride_bytes * y / 4];
        uint32_t *destination_i_l = &destination_i[destination_stride_bytes * y / 4];
        for (int x = x1; x < x2; ++x)
            destination_i_l[x] = source_i_l[x] | 0xFF000000;
    }
#endif
#if 1
    const uint64_t *source_i = reinterpret_cast<const uint64_t *>(source);
    uint64_t *destination_i = reinterpret_cast<uint64_t *>(destination);

    for (int y = y1; y < y2; ++y)
    {
        const uint64_t *source_i_l = &source_i[source_stride_bytes * y / 8];
        uint64_t *destination_i_l = &destination_i[destination_stride_bytes * y / 8];
        for (int x = x1 / 2; x < (x2 + 1) / 2; ++x)
            destination_i_l[x] = source_i_l[x] | 0xFF000000FF000000ULL;
    }
#endif
}

static void uploader_2(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, int x1, int y1, int x2, int y2)
{
#if 0
    const sparkle_pixel *source_p = reinterpret_cast<const sparkle_pixel *>(source);
    sparkle_pixel *destination_p = reinterpret_cast<sparkle_pixel *>(destination);
    register sparkle_pixel output;

    for (int y = y1; y < y2; ++y)
    {
        const sparkle_pixel *source_p_l = &source_p[source_stride_bytes * y / 4];
        sparkle_pixel *destination_p_l = &destination_p[destination_stride_bytes * y / 4];
        for (int x = x1; x < x2; ++x)
        {
            output.v1 = source_p_l[x].v3;
            output.v2 = source_p_l[x].v2;
            output.v3 = source_p_l[x].v1;
            destination_p_l[x] = output;
        }
    }
#endif
#if 0
    const uint32_t *source_i = reinterpret_cast<const uint32_t *>(source);
    uint32_t *destination_i = reinterpret_cast<uint32_t *>(destination);

    for (int y = y1; y < y2; ++y)
    {
        const uint32_t *source_i_l = &source_i[source_stride_bytes * y / 4];
        uint32_t *destination_i_l = &destination_i[destination_stride_bytes * y / 4];
        for (int x = x1; x < x2; ++x)
        {
            destination_i_l[x] =
            ((source_i_l[x] >> 16) & 0x000000FF) |
            ((source_i_l[x] << 0)  & 0x0000FF00) |
            ((source_i_l[x] << 16) & 0x00FF0000);
        }
    }
#endif
#if 1
    const uint64_t *source_i = reinterpret_cast<const uint64_t *>(source);
    uint64_t *destination_i = reinterpret_cast<uint64_t *>(destination);

    for (int y = y1; y < y2; ++y)
    {
        const uint64_t *source_i_l = &source_i[source_stride_bytes * y / 8];
        uint64_t *destination_i_l = &destination_i[destination_stride_bytes * y / 8];
        for (int x = x1 / 2; x < (x2 + 1) / 2; ++x)
        {
            destination_i_l[x] =
            ((source_i_l[x] >> 16) & 0x000000FF000000FFULL) |
            ((source_i_l[x] << 0)  & 0x0000FF000000FF00ULL) |
            ((source_i_l[x] << 16) & 0x00FF000000FF0000ULL);
        }
    }
#endif
}

namespace were_upload
{
    uploader_type uploader[3] = {uploader_0, uploader_1, uploader_2};
};
