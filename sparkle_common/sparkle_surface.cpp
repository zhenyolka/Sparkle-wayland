#include "sparkle_surface.h"
#include "sparkle.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"
#include "generated/sparkle_wl_buffer.h"
#include "generated/sparkle_wl_callback.h"
#include <cstdio>

sparkle_surface::~sparkle_surface()
{
    if (buffer_.has_value())
    {
        buffer_.value()->collapse();
        buffer_.reset();
    }

    if (callback_.has_value())
    {
        callback_.value()->collapse();
        callback_.reset();
    }
}

sparkle_surface::sparkle_surface(struct wl_client *client, int version, uint32_t id) :
    sparkle_wl_surface(client, version, id)
{
    auto this_wop = were_pointer(this);

    were::connect(this_wop, &sparkle_wl_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        if (this_wop->buffer_.has_value())
        {
            this_wop->buffer_.value()->send_release();
            this_wop->buffer_.value()->collapse();
            this_wop->buffer_.reset();
        }

        this_wop->buffer_ = were_pointer<sparkle_wl_buffer>(new sparkle_wl_buffer(buffer));
    });

    were::connect(this_wop, &sparkle_wl_surface::damage, this_wop, [this_wop](int32_t x, int32_t y, int32_t width, int32_t height)
    {
        were::emit(this_wop, &were_surface::damage, x, y, width, height);
    });

    were::connect(this_wop, &sparkle_wl_surface::damage_buffer, this_wop, [this_wop](int32_t x, int32_t y, int32_t width, int32_t height)
    {
        were::emit(this_wop, &were_surface::damage, x, y, width, height);
    });

    were::connect(this_wop, &sparkle_wl_surface::commit, this_wop, [this_wop]()
    {
        were::emit(this_wop, &were_surface::commit);

        if (this_wop->callback_.has_value())
        {
            this_wop->callback_.value()->send_done(sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_.value()->resource()); //XXX1
            this_wop->callback_.value()->collapse();
            this_wop->callback_.reset();
        }
    });

    were::connect(this_wop, &sparkle_wl_surface::frame, this_wop, [this_wop](uint32_t callback)
    {
        if (this_wop->callback_.has_value())
        {
            this_wop->callback_.value()->send_done(sparkle::current_msecs());
            wl_resource_destroy(this_wop->callback_.value()->resource()); //XXX1
            this_wop->callback_.value()->collapse();
            this_wop->callback_.reset();
        }

        this_wop->callback_ = were_pointer<sparkle_wl_callback>(new sparkle_wl_callback(this_wop->client(), 1, callback));
    });
}

void *sparkle_surface::data()
{
    if (!buffer_.has_value())
        return nullptr;

    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_.value()->resource());

    if (shm_buffer == nullptr)
        return nullptr;

    int format = wl_shm_buffer_get_format(shm_buffer);

    if (!(format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888))
        return nullptr;

    void *data = wl_shm_buffer_get_data(shm_buffer);

    return data;
}

int sparkle_surface::width()
{
    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_.value()->resource());
    uint32_t width = wl_shm_buffer_get_width(shm_buffer);

    return width;
}

int sparkle_surface::height()
{
    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_.value()->resource());
    uint32_t height = wl_shm_buffer_get_height(shm_buffer);

    return height;
}

int sparkle_surface::stride()
{
    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_.value()->resource());
    uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);

    return stride;
}

were_surface::buffer_format sparkle_surface::format()
{
    return were_surface::buffer_format::ARGB8888;
}

void sparkle_surface::register_keyboard(were_pointer<sparkle_keyboard> keyboard)
{
    auto this_wop = were_pointer(this);

    if (keyboard->client() != client())
        return;

    were::connect(this_wop, &were_surface::key_down, keyboard, [keyboard, this_wop](int code)
    {
        keyboard->key_press(code);
        were::emit(keyboard->thread(), &were_thread::idle); // XXX2
    });

    were::connect(this_wop, &were_surface::key_up, keyboard, [keyboard, this_wop](int code)
    {
        keyboard->key_release(code);
        were::emit(keyboard->thread(), &were_thread::idle); // XXX2
    });

    keyboard->enter(this_wop); // XXX2
}

void sparkle_surface::register_pointer(were_pointer<sparkle_pointer> pointer)
{
    auto this_wop = were_pointer(this);

    if (pointer->client() != client())
        return;

    were::connect(this_wop, &were_surface::pointer_button_down, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were::connect(this_wop, &were_surface::pointer_button_up, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were::connect(this_wop, &were_surface::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were::connect(this_wop, &were_surface::pointer_enter, pointer, [pointer, this_wop]()
    {
        pointer->enter(this_wop);
    });

    were::connect(this_wop, &were_surface::pointer_leave, pointer, [pointer, this_wop]()
    {
        pointer->leave(this_wop);
    });

    pointer->enter(this_wop); // XXX2
}

void sparkle_surface::register_touch(were_pointer<sparkle_touch> touch)
{
    auto this_wop = were_pointer(this);

    if (touch->client() != client())
        return;

    were::connect(this_wop, &were_surface::touch_down, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->down(this_wop, id, x, y);
    });

    were::connect(this_wop, &were_surface::touch_up, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->up(this_wop, id, x, y);
    });

    were::connect(this_wop, &were_surface::touch_motion, touch, [touch, this_wop](int id, int x, int y)
    {
        touch->motion(this_wop, id, x, y);
    });
}
