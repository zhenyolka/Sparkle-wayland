#include "sparkle_android_surface.h"
#include "sparkle_surface.h"
#include "sparkle_callback.h"
#include "sparkle_view.h"
#include "sparkle_android.h"
#include <android/native_window_jni.h>

const int WINDOW_FORMAT = 5;

sparkle_android_surface::~sparkle_android_surface()
{
    view_->set_enabled(false);
    view_->collapse();
}

sparkle_android_surface::sparkle_android_surface(were_object_pointer<sparkle_android> android, were_object_pointer<sparkle_surface> surface) :
    surface_(surface), window_(nullptr)
{
    MAKE_THIS_WOP

    view_ = were_object_pointer<sparkle_view>(new sparkle_view(android->service()->env(), android->service())); // XXX
    view_->set_enabled(true);

    were::connect(view_, &sparkle_view::surface_changed, this_wop, [this_wop](ANativeWindow *window)
    {
        this_wop->window_ = window;
        this_wop->commit();
    });

    were::connect(surface, &sparkle_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        // XXX

        if (this_wop->buffer_ != nullptr)
            wl_buffer_send_release(this_wop->buffer_);

        this_wop->buffer_ = buffer;
    });

    were::connect(surface, &sparkle_surface::commit, this_wop, [this_wop]()
    {
        this_wop->commit();
    });

    were::connect(surface, &sparkle_surface::frame, this_wop, [this_wop](uint32_t callback)
    {
        were_object_pointer<sparkle_callback> callback__(new sparkle_callback(this_wop->surface_->client(), 1, callback));

        were::connect(this_wop->surface_, &sparkle_surface::commit, callback__, [callback__]()
        {
            callback__->send_done(sparkle::current_msecs());
            wl_resource_destroy(callback__->resource()); // FIXME
        });
    });

    buffer_ = nullptr;
}

void sparkle_android_surface::commit()
{
    if (buffer_ == nullptr)
        return;

    if (window_ == nullptr)
        return;

    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_);

    if (shm_buffer != nullptr)
    {
        uint32_t width = wl_shm_buffer_get_width(shm_buffer);
        uint32_t height = wl_shm_buffer_get_height(shm_buffer);
        int format = wl_shm_buffer_get_format(shm_buffer);
        uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);

        if ((format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888))
        {
            int w_width = ANativeWindow_getWidth(window_);
            int w_height = ANativeWindow_getHeight(window_);
            int w_format = ANativeWindow_getFormat(window_);
            ANativeWindow_setBuffersGeometry(window_, w_width, w_height, WINDOW_FORMAT); // XXX

            ANativeWindow_Buffer buffer;

            ARect rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = w_width;
            rect.bottom = w_height;

            ANativeWindow_lock(window_, &buffer, &rect);

            int rows = 0;
            if (height < buffer.height)
                rows = height;
            else
                rows = buffer.height;

            int line = 0;
            if (stride < buffer.stride*4)
                line = stride;
            else
                line = buffer.stride*4;

            for (int row = 0; row < rows; ++row)
                std::memcpy((char *)buffer.bits + buffer.stride * 4 * row, (char *)data + stride * row, line);

            ANativeWindow_unlockAndPost(window_);
        }
        else
        {
            // XXX Error
        }
    }
    else
    {
            // XXX Error
    }
}
