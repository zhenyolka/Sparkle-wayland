#include "sparkle_surface.h"
#include "sparkle.h"
#include <cstdio>


sparkle_surface::sparkle_surface(struct wl_client *client, int version, uint32_t id) :
    sparkle_wl_surface(client, version, id), buffer_(nullptr), callback_(nullptr)
{
    MAKE_THIS_WOP

    were_object::connect(this_wop, &sparkle_wl_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        if (this_wop->buffer_ != nullptr)
            wl_buffer_send_release(this_wop->buffer_);

        this_wop->buffer_ = buffer;
    });

    were_object::connect(this_wop, &sparkle_wl_surface::commit, this_wop, [this_wop]()
    {
        this_wop->commit();
    });

    were_object::connect(this_wop, &sparkle_wl_surface::frame, this_wop, [this_wop](uint32_t callback)
    {
        if (this_wop->callback_ != nullptr)
        {
            wl_callback_send_done(this_wop->callback_, sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_);
            this_wop->callback_ = nullptr;
        }

        this_wop->callback_ = wl_resource_create(this_wop->client(), &wl_callback_interface, 1, callback);
    });
}

void sparkle_surface::commit()
{
    MAKE_THIS_WOP

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

        were_object::emit(this_wop, &were_surface::data, data, width, height);
    }

    if (callback_ != nullptr)
    {
        wl_callback_send_done(callback_, sparkle::current_msecs());
        wl_resource_destroy(callback_);
        callback_ = nullptr;
    }
}
