#include "sparkle_android_surface.h"
#include "sparkle_surface.h"
#include "sparkle_callback.h"
#include "sparkle_view.h"
#include "sparkle_android.h"

#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"

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

        if (window != nullptr)
        {
            int w_width = ANativeWindow_getWidth(window);
            int w_height = ANativeWindow_getHeight(window);
            int w_format = ANativeWindow_getFormat(window);

#if X_DEBUG
            fprintf(stdout, "surface changed %p %dx%d %p (view %dx%d)\n", window, w_width, w_height, w_format,
                this_wop->view_->width(), this_wop->view_->height());
#endif

            if (w_width != this_wop->view_->width() || w_height != this_wop->view_->height())
                throw were_exception(WE_SIMPLE);

            if (ANativeWindow_setBuffersGeometry(window, w_width, w_height, WINDOW_FORMAT) != 0)
                throw were_exception(WE_SIMPLE);

            this_wop->commit(true);
        }
        else
        {
#if X_DEBUG
            fprintf(stdout, "surface changed %p\n", window);
#endif
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
            wl_resource_destroy(this_wop->callback_); // XXX
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
#if X_DEBUG
            fprintf(stdout, "resize view %dx%d -> %dx%d\n", view_->width(), view_->height(), width, height);
#endif
            view_->set_size(width, height);
            return;
        }

        if ((format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888) && window_ != nullptr)
        {
            if (full)
                damage_.add(0, 0, width, height);


            int w_width = ANativeWindow_getWidth(window_);
            int w_height = ANativeWindow_getHeight(window_);
            int w_format = ANativeWindow_getFormat(window_);

            ANativeWindow_Buffer buffer;

            ARect rect;
            rect.left = damage_.x1();
            rect.top = damage_.y1();
            rect.right = damage_.x2();
            rect.bottom = damage_.y2();

#if X_DEBUG
            fprintf(stdout, "locking win %dx%d-%d region %d %d %d %d\n", w_width, w_height, w_format
                rect.left, rect.right, rect.top, rect.bottom);
#endif

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
                std::memcpy
                (
                    destination + destination_stride_bytes * y + offset,
                    source + source_stride_bytes * y + offset,
                    length
                );
            }


            ANativeWindow_unlockAndPost(window_);

            damage_.clear();
        }
        else
        {
        }
    }
    else
    {
    }

    if (callback_ != nullptr)
    {
        wl_callback_send_done(callback_, sparkle::current_msecs());
        wl_resource_destroy(callback_); // XXX
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
    });

    were::connect(view_, &sparkle_view::key_up, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_release(code);
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
