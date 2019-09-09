#include "sparkle_x11_surface.h"
#include "sparkle_x11.h"
#include "sparkle_surface.h"
#include "sparkle_keyboard.h"
#include "sparkle_pointer.h"
#include "sparkle_touch.h"
#include "were_debug.h"
#include <linux/input-event-codes.h>

#include <cstdio>

static const int button_map[6] = {0, BTN_LEFT, BTN_MIDDLE, BTN_RIGHT, BTN_GEAR_UP, BTN_GEAR_DOWN};

sparkle_x11_surface::~sparkle_x11_surface()
{
    were1_xcb_window_destroy(window_);
}

sparkle_x11_surface::sparkle_x11_surface(were_object_pointer<sparkle_x11> x11, were_object_pointer<sparkle_surface> surface) :
    surface_(surface)
{
    MAKE_THIS_WOP

    display_ = x11->display();

    int width = 1280;
    int height = 720;

    window_ = were1_xcb_window_create(display_->get(), width, height);

    were_object::connect(x11, &sparkle_x11::event1, this_wop, [this_wop](xcb_generic_event_t *event){this_wop->process(event);});

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

void sparkle_x11_surface::process(xcb_generic_event_t *event)
{
    MAKE_THIS_WOP

    switch (event->response_type & ~0x80)
    {
        case XCB_EXPOSE:
        {
            xcb_expose_event_t *ev = (xcb_expose_event_t *)event;
            if (ev->window == window_->window)
            {
                commit();
            }
            break;
        }
        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *ev = (xcb_button_press_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
                were_object::emit(this_wop, &sparkle_x11_surface::touch_down, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = true;
#else
                were_object::emit(this_wop, &sparkle_x11_surface::pointer_button_down, button_map[ev->detail]);
#endif
            }
            break;
        }
        case XCB_BUTTON_RELEASE:
        {
            xcb_button_release_event_t *ev = (xcb_button_release_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
                were_object::emit(this_wop, &sparkle_x11_surface::touch_up, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = false;
#else
                were_object::emit(this_wop, &sparkle_x11_surface::pointer_button_up, button_map[ev->detail]);
#endif
            }
            break;
        }
        case XCB_MOTION_NOTIFY:
        {
            xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
            if (touch_down_)
                were_object::emit(this_wop, &sparkle_x11_surface::touch_motion, TOUCH_ID, ev->event_x, ev->event_y);
#else
            were_object::emit(this_wop, &sparkle_x11_surface::pointer_motion, ev->event_x, ev->event_y);
#endif
            }
            break;
        }
    }

#if 0
    switch (event->response_type & ~0x80)
    {
\
        case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *ev = (xcb_button_press_event_t *)e;
            print_modifiers(ev->state);

            switch (ev->detail) {
                case 4:
                    printf ("Wheel Button up in window %ld, at coordinates (%d,%d)\n",
                            ev->event, ev->event_x, ev->event_y);
                    break;
                case 5:
                    printf ("Wheel Button down in window %ld, at coordinates (%d,%d)\n",
                            ev->event, ev->event_x, ev->event_y);
                    break;
                default:
                    printf ("Button %d pressed in window %ld, at coordinates (%d,%d)\n",
                            ev->detail, ev->event, ev->event_x, ev->event_y);
            }
            break;
        }
                case XCB_BUTTON_RELEASE: {
                    xcb_button_release_event_t *ev = (xcb_button_release_event_t *)e;
                    print_modifiers(ev->state);

                    printf ("Button %d released in window %ld, at coordinates (%d,%d)\n",
                            ev->detail, ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_MOTION_NOTIFY: {
                    xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t *)e;

                    printf ("Mouse moved in window %ld, at coordinates (%d,%d)\n",
                            ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_ENTER_NOTIFY: {
                    xcb_enter_notify_event_t *ev = (xcb_enter_notify_event_t *)e;

                    printf ("Mouse entered window %ld, at coordinates (%d,%d)\n",
                            ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_LEAVE_NOTIFY: {
                    xcb_leave_notify_event_t *ev = (xcb_leave_notify_event_t *)e;

                    printf ("Mouse left window %ld, at coordinates (%d,%d)\n",
                            ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_KEY_PRESS: {
                    xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
                    print_modifiers(ev->state);

                    printf ("Key pressed in window %ld\n",
                            ev->event);
                    break;
                }
                case XCB_KEY_RELEASE: {
                    xcb_key_release_event_t *ev = (xcb_key_release_event_t *)e;
                    print_modifiers(ev->state);

                    printf ("Key released in window %ld\n",
                            ev->event);
                    break;
                }
                default:
                    /* Unknown event type, ignore it */
                    printf("Unknown event: %d\n", e->response_type);
                    break;
    }

}
#endif

#if 0

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
#endif
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
            if (window_->data != NULL)
            {
                memcpy(window_->data, data, window_->width * window_->height * 4);
                were1_xcb_window_commit(window_);
            }
            else
            {
                window_->data = data;
                were1_xcb_window_commit(window_);
                window_->data = NULL;
            }


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
