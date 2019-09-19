#include "were_x11_surface.h"
#include "were_x11_surface_provider.h"
#include "were_surface.h"
#include <linux/input-event-codes.h>


static const int button_map[6] = {0, BTN_LEFT, BTN_MIDDLE, BTN_RIGHT, BTN_GEAR_UP, BTN_GEAR_DOWN};


were_x11_surface::~were_x11_surface()
{
    were1_xcb_window_destroy(window_);
}

were_x11_surface::were_x11_surface(were_object_pointer<were_x11_surface_provider> x11_surface_provider)
{
    MAKE_THIS_WOP

    display_ = x11_surface_provider->display();

    width_ = 100;
    height_ = 100;

    window_ = were1_xcb_window_create(display_->get(), width_, height_);

    were_object::connect(x11_surface_provider, &were_x11_surface_provider::event1, this_wop, [this_wop](xcb_generic_event_t *event){this_wop->process(event);});

#if TOUCH_MODE
    touch_down_ = false;
#endif
}

void were_x11_surface::set_size(int width, int height)
{
    width_ = width;
    height_ = height;
    were1_xcb_window_set_size(window_, width_, height_);
}

bool were_x11_surface::lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride)
{
    lock_x1_ = *x1;
    lock_y1_ = *y1;
    lock_x2_ = *x2;
    lock_y2_ = *y2;

    *data = (char *)window_->data;
    *stride = window_->width * 4;

    return true;
}

bool were_x11_surface::unlock_and_post()
{
    //were1_xcb_window_commit(window_);
    were1_xcb_window_commit_with_damage(window_, lock_x1_, lock_y1_, lock_x2_, lock_y2_);

    return true;
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
                were_object::emit(callbacks(), &were_surface::expose);
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
                were_object::emit(callbacks(), &were_surface::pointer_button_down, button_map[ev->detail]);
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
                were_object::emit(callbacks(), &were_surface::pointer_button_up, button_map[ev->detail]);
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
                were_object::emit(callbacks(), &were_surface::touch_motion, TOUCH_ID, ev->event_x, ev->event_y);
#else
            were_object::emit(callbacks(), &were_surface::pointer_motion, ev->event_x, ev->event_y);
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
                were_object::emit(callbacks(), &were_surface::pointer_enter);
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
                were_object::emit(callbacks(), &were_surface::pointer_leave);
#endif
            }
            break;
        }
        case XCB_KEY_PRESS:
        {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
            if (ev->event == window_->window)
            {
                were_object::emit(callbacks(), &were_surface::key_down, ev->detail);
            }
            break;
        }
        case XCB_KEY_RELEASE:
        {
            xcb_key_release_event_t *ev = (xcb_key_release_event_t *)event;
            if (ev->event == window_->window)
            {
                were_object::emit(callbacks(), &were_surface::key_up, ev->detail);
            }
            break;
        }
        default:
            break;

    }
}
