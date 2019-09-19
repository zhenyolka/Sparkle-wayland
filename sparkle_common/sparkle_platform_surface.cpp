#include "sparkle_platform_surface.h"
#include "sparkle_platform.h"
#include "sparkle.h"
#include "sparkle_settings.h"
#include "sparkle_surface.h"
#include "were_surface.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"


/* ================================================================================================================== */

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
        const uint32_t *source_i_l = &source_i[source_stride_bytes / 4 * y];
        uint32_t *destination_i_l = &destination_i[destination_stride_bytes / 4 * y];
        for (int x = x1; x < x2; ++x)
            destination_i_l[x] = source_i_l[x] | 0xFF000000;
    }
#endif
#if 1
    const uint64_t *source_i = reinterpret_cast<const uint64_t *>(source);
    uint64_t *destination_i = reinterpret_cast<uint64_t *>(destination);

    for (int y = y1; y < y2; ++y)
    {
        const uint64_t *source_i_l = &source_i[source_stride_bytes / 8 * y];
        uint64_t *destination_i_l = &destination_i[destination_stride_bytes / 8 * y];
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
        const sparkle_pixel *source_p_l = &source_p[source_stride_bytes / 4 * y];
        sparkle_pixel *destination_p_l = &destination_p[destination_stride_bytes / 4 * y];
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
        const uint32_t *source_i_l = &source_i[source_stride_bytes / 4 * y];
        uint32_t *destination_i_l = &destination_i[destination_stride_bytes / 4 * y];
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
        const uint64_t *source_i_l = &source_i[source_stride_bytes / 8 * y];
        uint64_t *destination_i_l = &destination_i[destination_stride_bytes / 8 * y];
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

static void (*uploaders[])(void *destination, const void *source,
    int source_stride_bytes, int destination_stride_bytes, int x1, int y1, int x2, int y2) =
{
    uploader_0,
    uploader_1,
    uploader_2,
};

/* ================================================================================================================== */

sparkle_platform_surface::~sparkle_platform_surface()
{
    ws_.collapse();
}

sparkle_platform_surface::sparkle_platform_surface(were_object_pointer<sparkle_platform> platform, were_object_pointer<sparkle_surface> surface) :
    surface_(surface)
{
    MAKE_THIS_WOP

    no_damage_ = platform->sparkle1()->settings()->get_bool("no_damage", false);
    upload_mode_ = platform->sparkle1()->settings()->get_int("upload_mode", 0);

    int format;
    if (upload_mode_ == 2)
        format = WERE_SURFACE_FORMAT_ABGR8888;
    else
        format = WERE_SURFACE_FORMAT_ARGB8888;

    ws_ = were_object_pointer<were_surface>(new were_surface(platform->platform_surface_provider(), 100, 100, format));
    were_object::connect(ws_, &were_surface::expose, this_wop, [this_wop]()
    {
        this_wop->commit(true);
    });

    were_object::connect(surface_, &sparkle_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        if (this_wop->buffer_ != nullptr)
            wl_buffer_send_release(this_wop->buffer_);

        this_wop->buffer_ = buffer;
    });

    were_object::connect(surface_, &sparkle_surface::commit, this_wop, [this_wop]()
    {
        this_wop->commit();
    });

    were_object::connect(surface_, &sparkle_surface::frame, this_wop, [this_wop](uint32_t callback)
    {
        if (this_wop->callback_ != nullptr)
        {
            wl_callback_send_done(this_wop->callback_, sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_);
            this_wop->callback_ = nullptr;
        }

        this_wop->callback_ = wl_resource_create(this_wop->surface_->client(), &wl_callback_interface, 1, callback);
    });

    were_object::connect(surface_, &sparkle_surface::damage, this_wop, [this_wop](int32_t x, int32_t y, int32_t width, int32_t height)
    {
        this_wop->damage_.add(x, y, x + width, y + height);
    });

    buffer_ = nullptr;
    callback_ = nullptr;
}

void sparkle_platform_surface::commit(bool full)
{
    struct wl_shm_buffer *shm_buffer = nullptr;

    if (buffer_ != nullptr)
        shm_buffer = wl_shm_buffer_get(buffer_);

    if (shm_buffer != nullptr)
    {
        uint32_t width = wl_shm_buffer_get_width(shm_buffer);
        uint32_t height = wl_shm_buffer_get_height(shm_buffer);
        int format = wl_shm_buffer_get_format(shm_buffer);
        uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);

        if (ws_->width() != width || ws_->height() != height)
        {
            fprintf(stdout, "resizing %d %d\n", width, height);
            ws_->set_size(width, height);
        }
        else if ((format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888))
        {
            if (full || no_damage_)
                damage_.add(0, 0, width, height);

            damage_.limit(width, height);


            int x1 = damage_.x1();
            int y1 = damage_.y1();
            int x2 = damage_.x2();
            int y2 = damage_.y2();
            int destination_stride;
            char *destination;

            if (ws_->lock(&destination, &x1, &y1, &x2, &y2, &destination_stride))
            {
                uploaders[upload_mode_](destination, data, stride, destination_stride, x1, y1, x2, y2);

                ws_->unlock_and_post();

                damage_.clear();

                were_debug::instance().frame();
            }

        }
    }

    if (callback_ != nullptr)
    {
        wl_callback_send_done(callback_, sparkle::current_msecs());
        wl_resource_destroy(callback_);
        callback_ = nullptr;
    }
}

void sparkle_platform_surface::register_keyboard(were_object_pointer<sparkle_keyboard> keyboard)
{
    if (keyboard->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(ws_, &were_surface::key_down, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_press(code);
        keyboard.thread()->process_idle();
    });

    were_object::connect(ws_, &were_surface::key_up, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_release(code);
        keyboard.thread()->process_idle();
    });

    keyboard->enter(surface); // XXX2
}

void sparkle_platform_surface::register_pointer(were_object_pointer<sparkle_pointer> pointer)
{
    if (pointer->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(ws_, &were_surface::pointer_button_down, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were_object::connect(ws_, &were_surface::pointer_button_up, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were_object::connect(ws_, &were_surface::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were_object::connect(ws_, &were_surface::pointer_enter, pointer, [pointer, surface]()
    {
        pointer->enter(surface);
    });

    were_object::connect(ws_, &were_surface::pointer_leave, pointer, [pointer, surface]()
    {
        pointer->leave(surface);
    });

    pointer->enter(surface); // XXX2
}

void sparkle_platform_surface::register_touch(were_object_pointer<sparkle_touch> touch)
{
    if (touch->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(ws_, &were_surface::touch_down, touch, [touch, surface](int id, int x, int y)
    {
        touch->down(surface, id, x, y);
    });

    were_object::connect(ws_, &were_surface::touch_up, touch, [touch, surface](int id, int x, int y)
    {
        touch->up(surface, id, x, y);
    });

    were_object::connect(ws_, &were_surface::touch_motion, touch, [touch, surface](int id, int x, int y)
    {
        touch->motion(surface, id, x, y);
    });
}
