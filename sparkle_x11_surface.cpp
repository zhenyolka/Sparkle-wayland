#include "sparkle_x11_surface.h"
#include "sparkle_x11.h"
#include "sparkle_surface.h"
#include "sparkle_callback.h"
#include <X11/Xutil.h> // XImage

#include <cstdio>

sparkle_x11_surface::~sparkle_x11_surface()
{
    XDestroyWindow(display_->get(), window_);
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

    were::connect(x11, &sparkle_x11::event, this_wop, [this_wop](XEvent event){this_wop->process(event);});

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
                were::emit(this_wop, &sparkle_x11_surface::pointer_button_press, event.xbutton.button);
            break;
        case ButtonRelease:
            if (event.xbutton.type == ButtonRelease)
                were::emit(this_wop, &sparkle_x11_surface::pointer_button_release, event.xbutton.button);
            break;
        case MotionNotify:
            were::emit(this_wop, &sparkle_x11_surface::pointer_motion, event.xbutton.x, event.xbutton.y);
            break;
        case KeyPress:
            were::emit(this_wop, &sparkle_x11_surface::key_press, event.xkey.keycode);
            break;
        case KeyRelease:
            were::emit(this_wop, &sparkle_x11_surface::key_release, event.xkey.keycode);
            break;
        case EnterNotify:
            were::emit(this_wop, &sparkle_x11_surface::pointer_enter);
            break;
        case LeaveNotify:
            were::emit(this_wop, &sparkle_x11_surface::pointer_leave);
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
}
