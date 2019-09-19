#include "sparkle_platform_surface.h"
#include "sparkle_platform.h"
#include "sparkle.h"
#include "sparkle_settings.h"
#include "sparkle_surface.h"
#include "were_surface.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"


sparkle_platform_surface::~sparkle_platform_surface()
{
    ws_.collapse();
}

sparkle_platform_surface::sparkle_platform_surface(were_object_pointer<sparkle_platform> platform, were_object_pointer<sparkle_surface> surface) :
    surface_(surface)
{
    MAKE_THIS_WOP

    ws_ = were_object_pointer<were_surface>(new were_surface(platform->platform_surface_provider()));

    no_damage_ = platform->sparkle1()->settings()->get_bool("no_damage", false);

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

#if 0
        if (view_->width() != width || view_->height() != height)
        {
            view_->set_size(width, height);
        }
#else
        if (false)
        {
        }
#endif
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
#if 0
                if (upload_mode_ == 0)
                    upload_0(destination, data, stride, destination_stride, x1, y1, x2, y2);
                else if (upload_mode_ == 1)
                    upload_1(destination, data, stride, destination_stride, x1, y1, x2, y2);
                else if (upload_mode_ == 2)
                    upload_2(destination, data, stride, destination_stride, x1, y1, x2, y2);
#else
                std::memcpy(destination, data, 1280 * 720 * 4);
#endif


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
