#include "sparkle_x11_surface.h"
#include "sparkle_x11.h"
#include "sparkle_surface.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"
#include "were_debug.h"
#include <X11/Xutil.h> // XImage
#include <linux/input-event-codes.h>

#include <cstdio>

static const int button_map[6] = {0, BTN_LEFT, BTN_MIDDLE, BTN_RIGHT, BTN_GEAR_UP, BTN_GEAR_DOWN};

sparkle_x11_surface::~sparkle_x11_surface()
{
    XDestroyWindow(display_->get(), window_);
    XPending(display_->get()); /* Go away, window! */
}

sparkle_x11_surface::sparkle_x11_surface(were_object_pointer<sparkle_x11> x11, were_object_pointer<sparkle_surface> surface) :
    surface_(surface)
{
    MAKE_THIS_WOP

    display_ = x11->display();

    int width = 1280;
    int height = 720;

    window_ = XCreateSimpleWindow(display_->get(), RootWindow(display_->get(), 0), 0, 0, width, height, 1, 0, 0);
    if (!window_)
        throw were_exception(WE_SIMPLE);

    XSelectInput(display_->get(), window_,
        PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask | KeyReleaseMask |
        EnterWindowMask | LeaveWindowMask);

    if (true)
        XMapWindow(display_->get(), window_);
    else
        XUnmapWindow(display_->get(), window_);

    were_object::connect(x11, &sparkle_x11::event1, this_wop, [this_wop](XEvent event){this_wop->process(event);});

    were_object::connect(surface, &sparkle_surface::attach, this_wop, [this_wop](struct wl_resource *buffer, int32_t x, int32_t y)
    {
        if (this_wop->buffer_ != nullptr)
            wl_buffer_send_release(this_wop->buffer_);

        this_wop->buffer_ = buffer;
    });

    were_object::connect(surface, &sparkle_surface::commit, this_wop, [this_wop]()
    {
        this_wop->commit();
    });

    were_object::connect(surface, &sparkle_surface::frame, this_wop, [this_wop](uint32_t callback)
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

    buffer_ = nullptr;
    callback_ = nullptr;

#if TOUCH_MODE
    touch_down_ = false;
#endif
}

void sparkle_x11_surface::process(XEvent event)
{
    MAKE_THIS_WOP

    if (event.xany.window != window_)
        return;

    switch(event.type)
    {
        case Expose:
            commit();
            break;
        case ButtonPress:
            if (event.xbutton.type == ButtonPress)
            {
#if TOUCH_MODE
                were_object::emit(this_wop, &sparkle_x11_surface::touch_down, TOUCH_ID, event.xbutton.x, event.xbutton.y);
                touch_down_ = true;
#else
                were_object::emit(this_wop, &sparkle_x11_surface::pointer_button_down, button_map[event.xbutton.button]);
#endif
            }
            break;
        case ButtonRelease:
            if (event.xbutton.type == ButtonRelease)
            {
#if TOUCH_MODE
                were_object::emit(this_wop, &sparkle_x11_surface::touch_up, TOUCH_ID, event.xbutton.x, event.xbutton.y);
                touch_down_ = false;
#else
                were_object::emit(this_wop, &sparkle_x11_surface::pointer_button_up, button_map[event.xbutton.button]);
#endif
            }
            break;
        case MotionNotify:
#if TOUCH_MODE
            if (touch_down_)
                were_object::emit(this_wop, &sparkle_x11_surface::touch_motion, TOUCH_ID, event.xbutton.x, event.xbutton.y);
#else
            were_object::emit(this_wop, &sparkle_x11_surface::pointer_motion, event.xbutton.x, event.xbutton.y);
#endif
            break;
        case KeyPress:
            were_object::emit(this_wop, &sparkle_x11_surface::key_down, event.xkey.keycode);
            break;
        case KeyRelease:
            were_object::emit(this_wop, &sparkle_x11_surface::key_up, event.xkey.keycode);
            break;
        case EnterNotify:
#if TOUCH_MODE
#else
            were_object::emit(this_wop, &sparkle_x11_surface::pointer_enter);
#endif
            break;
        case LeaveNotify:
#if TOUCH_MODE
#else
            were_object::emit(this_wop, &sparkle_x11_surface::pointer_leave);
#endif
            break;
        default:
            break;
    }
}

void sparkle_x11_surface::commit()
{
    if (buffer_ == nullptr)
        return;

    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer_);

    if (shm_buffer != nullptr)
    {
        uint32_t width = wl_shm_buffer_get_width(shm_buffer);
        uint32_t height = wl_shm_buffer_get_height(shm_buffer);
        int format = wl_shm_buffer_get_format(shm_buffer);
        uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);

        if ((format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888) && stride == width * 4)
        {
            XImage *image = XCreateImage(display_->get(), DefaultVisual(display_->get(), 0), 24, ZPixmap, 0, (char *)data, width, height, 32, 0);
            XPutImage(display_->get(), window_, DefaultGC(display_->get(), 0), image, 0, 0, 0, 0, image->width, image->height);
            image->data = nullptr;
            XDestroyImage(image);

            were_debug::instance().frame();
        }
        else
        {
            // Error
        }
    }
    else
    {
            // Error
    }

    if (callback_ != nullptr)
    {
        wl_callback_send_done(callback_, sparkle::current_msecs());
        wl_resource_destroy(callback_);
        callback_ = nullptr;
    }
}

void sparkle_x11_surface::register_keyboard(were_object_pointer<sparkle_keyboard> keyboard)
{
    MAKE_THIS_WOP

    if (keyboard->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(this_wop, &sparkle_x11_surface::key_down, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_press(code);
        keyboard.thread()->process_idle();
    });

    were_object::connect(this_wop, &sparkle_x11_surface::key_up, keyboard, [keyboard, surface](int code)
    {
        keyboard->key_release(code);
        keyboard.thread()->process_idle();
    });

    keyboard->enter(surface); // XXX2
}

void sparkle_x11_surface::register_pointer(were_object_pointer<sparkle_pointer> pointer)
{
    MAKE_THIS_WOP

    if (pointer->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(this_wop, &sparkle_x11_surface::pointer_button_down, pointer, [pointer](int button)
    {
        pointer->button_down(button);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::pointer_button_up, pointer, [pointer](int button)
    {
        pointer->button_up(button);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::pointer_motion, pointer, [pointer](int x, int y)
    {
        pointer->motion(x, y);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::pointer_enter, pointer, [pointer, surface]()
    {
        pointer->enter(surface);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::pointer_leave, pointer, [pointer, surface]()
    {
        pointer->leave(surface);
    });

    pointer->enter(surface); // XXX2
}

void sparkle_x11_surface::register_touch(were_object_pointer<sparkle_touch> touch)
{
    MAKE_THIS_WOP

    if (touch->client() != surface_->client())
        return;

    were_object_pointer<sparkle_surface> surface(surface_);

    were_object::connect(this_wop, &sparkle_x11_surface::touch_down, touch, [touch, surface](int id, int x, int y)
    {
        touch->down(surface, id, x, y);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::touch_up, touch, [touch, surface](int id, int x, int y)
    {
        touch->up(surface, id, x, y);
    });

    were_object::connect(this_wop, &sparkle_x11_surface::touch_motion, touch, [touch, surface](int id, int x, int y)
    {
        touch->motion(surface, id, x, y);
    });
}
