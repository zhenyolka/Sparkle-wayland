#include "were_x11_surface.h"
#include "were_x11_compositor.h"
#include "were_surface.h"


were_x11_surface::~were_x11_surface()
{
    were1_xcb_window_destroy(window_);
}

were_x11_surface::were_x11_surface(were_object_pointer<were_x11_compositor> compositor, were_object_pointer<were_surface> surface)
{
    MAKE_THIS_WOP

    display_ = compositor->display();

    window_ = were1_xcb_window_create(display_->get(), 1280, 720);

    were_object::connect(compositor, &were_x11_compositor::event1, this_wop, [this_wop](xcb_generic_event_t *event)
    {
        this_wop->process(event);
    });

    were_object::connect(surface, &were_surface::data, this_wop, [this_wop](void *data, int width, int height)
    {
        this_wop->data(data, width, height);
    });

#if TOUCH_MODE
    touch_down_ = false;
#endif
}

void were_x11_surface::data(void *data, int width, int height)
{
    if (window_->width != width || window_->height != height)
        were1_xcb_window_set_size(window_, width, height);

    std::memcpy(window_->data, data, width * height * 4);

    were1_xcb_window_commit(window_);
}

void were_x11_surface::process(xcb_generic_event_t *event)
{
    MAKE_THIS_WOP

    switch (event->response_type & ~0x80)
    {
        case XCB_EXPOSE:
        {
            xcb_expose_event_t *ev = (xcb_expose_event_t *)event;
            if (ev->window == window_->window)
            {
                //were_object::emit(callbacks(), &were_surface::expose);
            }
            break;
        }
        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *ev = (xcb_button_press_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
                were_object::emit(callbacks(), &were_surface::touch_down, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = true;
#else
                //were_object::emit(callbacks(), &were_surface::pointer_button_down, button_map[ev->detail]);
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
                were_object::emit(callbacks(), &were_surface::touch_up, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = false;
#else
                //were_object::emit(callbacks(), &were_surface::pointer_button_up, button_map[ev->detail]);
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
            //if (touch_down_)
                //were_object::emit(callbacks(), &were_surface::touch_motion, TOUCH_ID, ev->event_x, ev->event_y);
#else
            //were_object::emit(callbacks(), &were_surface::pointer_motion, ev->event_x, ev->event_y);
#endif
            }
            break;
        }
        case XCB_ENTER_NOTIFY:
        {
            xcb_enter_notify_event_t *ev = (xcb_enter_notify_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
#else
                // ev->event_x, ev->event_y
                //were_object::emit(callbacks(), &were_surface::pointer_enter);
#endif
            }
            break;
        }
        case XCB_LEAVE_NOTIFY:
        {
            xcb_leave_notify_event_t *ev = (xcb_leave_notify_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
#else
                //were_object::emit(callbacks(), &were_surface::pointer_leave);
#endif
            }
            break;
        }
        case XCB_KEY_PRESS:
        {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
            if (ev->event == window_->window)
            {
                //were_object::emit(callbacks(), &were_surface::key_down, ev->detail);
            }
            break;
        }
        case XCB_KEY_RELEASE:
        {
            xcb_key_release_event_t *ev = (xcb_key_release_event_t *)event;
            if (ev->event == window_->window)
            {
                //were_object::emit(callbacks(), &were_surface::key_up, ev->detail);
            }
            break;
        }
        default:
            break;

    }
}