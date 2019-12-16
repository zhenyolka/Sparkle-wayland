#include "sparkle_surface.h"
#include "sparkle.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"


sparkle_surface::sparkle_surface(struct wl_client *client, int version, uint32_t id) :
    sparkle_wl_surface(client, version, id), buffer_(nullptr), callback_(nullptr)
{
    MAKE_THIS_WOP

    were_object::connect(this_wop, &sparkle_wl_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        if (this_wop->buffer_ != nullptr)
            wl_buffer_send_release(this_wop->buffer_);

        this_wop->buffer_ = buffer;

        struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(this_wop->buffer_);

        if (shm_buffer != nullptr)
        {
            uint32_t width = wl_shm_buffer_get_width(shm_buffer);
            uint32_t height = wl_shm_buffer_get_height(shm_buffer);
            int format = wl_shm_buffer_get_format(shm_buffer);
            uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);
            void *data = wl_shm_buffer_get_data(shm_buffer);

            if (format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888)
            {
                were_object::emit(this_wop, &were_surface::attach, data, width, height, stride, were_surface::format_ARGB8888);
            }
        }
    });

    were_object::connect(this_wop, &sparkle_wl_surface::damage, this_wop, [this_wop](int32_t x, int32_t y, int32_t width, int32_t height)
    {
        were_object::emit(this_wop, &were_surface::damage, x, y, width, height);
    });

    were_object::connect(this_wop, &sparkle_wl_surface::commit, this_wop, [this_wop]()
    {
        were_object::emit(this_wop, &were_surface::commit);

        if (this_wop->callback_ != nullptr)
        {
            wl_callback_send_done(this_wop->callback_, sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_);
            this_wop->callback_ = nullptr;
        }
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

void sparkle_surface::register_keyboard(were_object_pointer<sparkle_keyboard> keyboard)
{
    MAKE_THIS_WOP

    if (keyboard->client() != client())
        return;

    were_object::connect(this_wop, &were_surface::key_down, keyboard, [keyboard, this_wop](int code)
    {
        keyboard->key_press(code);
        keyboard.thread()->process_idle();
    });

    were_object::connect(this_wop, &were_surface::key_up, keyboard, [keyboard, this_wop](int code)
    {
        keyboard->key_release(code);
        keyboard.thread()->process_idle();
    });

    keyboard->enter(this_wop); // XXX2
}

void sparkle_surface::register_pointer(were_object_pointer<sparkle_pointer> pointer)
{
    MAKE_THIS_WOP

    if (pointer->client() != client())
        return;

    were_object::connect(this_wop, &were_surface::pointer_button_down, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were_object::connect(this_wop, &were_surface::pointer_button_up, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were_object::connect(this_wop, &were_surface::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were_object::connect(this_wop, &were_surface::pointer_enter, pointer, [pointer, this_wop]()
    {
        pointer->enter(this_wop);
    });

    were_object::connect(this_wop, &were_surface::pointer_leave, pointer, [pointer, this_wop]()
    {
        pointer->leave(this_wop);
    });

    pointer->enter(this_wop); // XXX2
}

void sparkle_surface::register_touch(were_object_pointer<sparkle_touch> touch)
{
    MAKE_THIS_WOP

    if (touch->client() != client())
        return;

    were_object::connect(this_wop, &were_surface::touch_down, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->down(this_wop, id, x, y);
    });

    were_object::connect(this_wop, &were_surface::touch_up, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->up(this_wop, id, x, y);
    });

    were_object::connect(this_wop, &were_surface::touch_motion, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->motion(this_wop, id, x, y);
    });
}
