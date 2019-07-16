#include "sparkle_android_surface.h"
#include "sparkle_surface.h"
#include "sparkle_callback.h"
#include "sparkle_view.h"
#include "sparkle_android.h"

#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"

#include <android/native_window_jni.h>


//#define SPARKLE_KOT
//#define SPARKLE_KOT_CONVERT
//#define SPARKLE_KOT_MASK


#ifdef SPARKLE_KOT_CONVERT
const int WINDOW_FORMAT = WINDOW_FORMAT_RGBX_8888;
#else
const int WINDOW_FORMAT = 5;
#endif


sparkle_android_surface::~sparkle_android_surface()
{
    if (window_ != nullptr)
        ANativeWindow_release(window_);

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
        if (this_wop->window_ != nullptr)
            ANativeWindow_release(this_wop->window_);

        this_wop->window_ = window;

        if (window != nullptr)
        {
            ANativeWindow_acquire(this_wop->window_); // XXX Move to sparkle_view

            int w_width = ANativeWindow_getWidth(window);
            int w_height = ANativeWindow_getHeight(window);
            int w_format = ANativeWindow_getFormat(window);

            if (w_width != this_wop->view_->width() || w_height != this_wop->view_->height())
                throw were_exception(WE_SIMPLE);

            if (ANativeWindow_setBuffersGeometry(window, w_width, w_height, WINDOW_FORMAT) != 0)
                throw were_exception(WE_SIMPLE);

            this_wop->commit(true);
        }
        else
        {
        }
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
        if (this_wop->callback_ != nullptr)
        {
            fprintf(stdout, "callback_ != nullptr\n");
            wl_callback_send_done(this_wop->callback_, sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_);
            this_wop->callback_ = nullptr;
        }

        this_wop->callback_ = wl_resource_create(this_wop->surface_->client(), &wl_callback_interface, 1, callback);
    });

    were::connect(surface, &sparkle_surface::damage, this_wop, [this_wop](int32_t x, int32_t y, int32_t width, int32_t height)
    {
        this_wop->damage_.add(x, y, x + width, y + height);
    });

    buffer_ = nullptr;
    callback_ = nullptr;
}

#ifdef SPARKLE_KOT_CONVERT
struct sparkle_pixel
{
    uint8_t v1;
    uint8_t v2;
    uint8_t v3;
    uint8_t v4;
};
inline static void memcpy_kot(struct sparkle_pixel *destination, const struct sparkle_pixel *source, size_t n)
{
    register sparkle_pixel output;

    for (int i = 0; i < n; ++i)
    {
        output.v1 = source[i].v3;
        output.v2 = source[i].v2;
        output.v3 = source[i].v1;
        destination[i] = output;
    }
}
#endif

#ifdef SPARKLE_KOT_MASK
inline static void memcpy_kot(uint32_t *destination, const uint32_t *source, size_t n)
{
    for (int i = 0; i < n; ++i)
        destination[i] = source[i] & 0x00FFFFFF;
}
#endif

void sparkle_android_surface::commit(bool full)
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

        if (view_->width() != width || view_->height() != height)
        {
            view_->set_size(width, height);
        }
        else if ((format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888) && window_ != nullptr)
        {
            if (full)
                damage_.add(0, 0, width, height);

            damage_.limit(width, height); // XXX

            ANativeWindow_Buffer buffer;

            ARect rect;
            rect.left = damage_.x1();
            rect.top = damage_.y1();
            rect.right = damage_.x2();
            rect.bottom = damage_.y2();


            // XXX ANativeWindow_acquire
            if (ANativeWindow_lock(window_, &buffer, &rect) != 0)
                throw were_exception(WE_SIMPLE);

            if (buffer.width != width || buffer.height != height)
                throw were_exception(WE_SIMPLE);



            char *source = reinterpret_cast<char *>(data);
            char *destination = reinterpret_cast<char *>(buffer.bits);
            int source_stride_bytes = stride;
            int destination_stride_bytes = buffer.stride * 4;
            int offset = damage_.x1() * 4;
            int length = damage_.width() * 4;

            for (int y = damage_.y1(); y < damage_.y2(); ++y)
            {

#ifndef SPARKLE_KOT
                std::memcpy
                (
                    (destination + destination_stride_bytes * y + offset),
                    (source + source_stride_bytes * y + offset),
                    length
                );
#endif

#ifdef SPARKLE_KOT_CONVERT
                memcpy_kot
                (
                    (sparkle_pixel *)(destination + destination_stride_bytes * y + offset),
                    (sparkle_pixel *)(source + source_stride_bytes * y + offset),
                    damage_.width()
                );
#endif

#ifdef SPARKLE_KOT_MASK
                memcpy_kot
                (
                    (uint32_t *)(destination + destination_stride_bytes * y + offset),
                    (uint32_t *)(source + source_stride_bytes * y + offset),
                    damage_.width()
                );
#endif
            }


            ANativeWindow_unlockAndPost(window_);

            damage_.clear();
        }
    }

    if (callback_ != nullptr)
    {
        wl_callback_send_done(callback_, sparkle::current_msecs());
        wl_resource_destroy(callback_);
        callback_ = nullptr;
    }
}

void sparkle_android_surface::register_keyboard(were_object_pointer<sparkle_keyboard> keyboard)
{
    were_object_pointer<sparkle_surface> surface(surface_);

    //if (wl_resource_get_client(keyboard->resource()) != wl_resource_get_client(surface_->resource()))
    //    return; // XXX

    were::connect(view_, &sparkle_view::key_down, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_press(code);
#ifdef SPARKLE_KOT
        keyboard->thread()->idle();
#endif
    });

    were::connect(view_, &sparkle_view::key_up, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_release(code);
#ifdef SPARKLE_KOT
        keyboard->thread()->idle();
#endif
    });

    keyboard->enter(surface); // XXX

    fprintf(stdout, "keyboard registered\n");
}

void sparkle_android_surface::register_pointer(were_object_pointer<sparkle_pointer> pointer)
{
    were_object_pointer<sparkle_surface> surface(surface_);

    //if (wl_resource_get_client(keyboard->resource()) != wl_resource_get_client(surface_->resource()))
    //    return; // XXX

    were::connect(view_, &sparkle_view::pointer_button_down, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were::connect(view_, &sparkle_view::pointer_button_up, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were::connect(view_, &sparkle_view::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were::connect(view_, &sparkle_view::pointer_enter, pointer, [pointer, surface]()
    {
        pointer->enter(surface);
    });

    were::connect(view_, &sparkle_view::pointer_leave, pointer, [pointer, surface]()
    {
        pointer->leave(surface);
    });

    pointer->enter(surface); // XXX

    fprintf(stdout, "pointer registered\n");
}

void sparkle_android_surface::register_touch(were_object_pointer<sparkle_touch> touch)
{
    were_object_pointer<sparkle_surface> surface(surface_);

    were::connect(view_, &sparkle_view::touch_down, touch, [touch, surface](int id, int x, int y)
    {
        touch->down(surface, id, x, y);
    });

    were::connect(view_, &sparkle_view::touch_up, touch, [touch, surface](int id, int x, int y)
    {
        touch->up(surface, id, x, y);
    });

    were::connect(view_, &sparkle_view::touch_motion, touch, [touch, surface](int id, int x, int y)
    {
        touch->motion(surface, id, x, y);
    });

    fprintf(stdout, "touch registered\n");
}
