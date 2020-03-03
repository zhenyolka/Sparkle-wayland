#include "were_x11_surface.h"
#include "were_x11_compositor.h"
#include "were_surface.h"
#include "were_debug.h"
#include "were_upload.h"
#include <linux/input-event-codes.h>



static const int button_map[6] = {0, BTN_LEFT, BTN_MIDDLE, BTN_RIGHT, BTN_GEAR_UP, BTN_GEAR_DOWN};


were_x11_surface::~were_x11_surface()
{
    were1_xcb_window_destroy(window_);
}

were_x11_surface::were_x11_surface(were_object_pointer<were_x11_compositor> compositor, were_object_pointer<were_surface> surface) :
    display_(compositor->display()),
    surface_(surface)
{
    auto this_wop = make_wop(this);

    window_ = were1_xcb_window_create(display_->get(), 1280, 720);

    were::connect(compositor, &were_x11_compositor::event1, this_wop, [this_wop](xcb_generic_event_t *event)
    {
        this_wop->process(event);
    });

    were::connect(surface, &were_surface::damage, this_wop, [this_wop](int x, int y, int width, int height)
    {
        this_wop->damage_.expand(x, y, x + width, y + height);
    });

    were::connect(surface, &were_surface::commit, this_wop, [this_wop]()
    {
        this_wop->update(false);
    });

#if TOUCH_MODE
    touch_down_ = false;
#endif
}

void were_x11_surface::process(xcb_generic_event_t *event)
{
    auto this_wop = make_wop(this);

    switch (event->response_type & ~0x80)
    {
        case XCB_EXPOSE:
        {
            xcb_expose_event_t *ev = (xcb_expose_event_t *)event;
            if (ev->window == window_->window)
            {
                update(true);
            }
            break;
        }
        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *ev = (xcb_button_press_event_t *)event;
            if (ev->event == window_->window)
            {
#if TOUCH_MODE
                were::emit(surface_, &were_surface::touch_down, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = true;
#else
                were::emit(surface_, &were_surface::pointer_button_down, button_map[ev->detail]);
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
                were::emit(surface_, &were_surface::touch_up, TOUCH_ID, ev->event_x, ev->event_y);
                touch_down_ = false;
#else
                were::emit(surface_, &were_surface::pointer_button_up, button_map[ev->detail]);
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
                were::emit(surface_, &were_surface::touch_motion, TOUCH_ID, ev->event_x, ev->event_y);
#else
            were::emit(surface_, &were_surface::pointer_motion, ev->event_x, ev->event_y);
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
                were::emit(surface_, &were_surface::pointer_enter);
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
                were::emit(surface_, &were_surface::pointer_leave);
#endif
            }
            break;
        }
        case XCB_KEY_PRESS:
        {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
            if (ev->event == window_->window)
            {
                were::emit(surface_, &were_surface::key_down, ev->detail);
            }
            break;
        }
        case XCB_KEY_RELEASE:
        {
            xcb_key_release_event_t *ev = (xcb_key_release_event_t *)event;
            if (ev->event == window_->window)
            {
                were::emit(surface_, &were_surface::key_up, ev->detail);
            }
            break;
        }
        default:
            break;

    }
}

void were_x11_surface::update(bool full)
{
    void *data = surface_->data();

    if (data == nullptr)
        return;

    if (window_->width != surface_->width() || window_->height != surface_->height())
        were1_xcb_window_set_size(window_, surface_->width(), surface_->height());

    if (full)
        damage_.expand(0, 0, surface_->width(), surface_->height());

    damage_.limit(surface_->width(), surface_->height());

    //std::memcpy(window_->data, data, window_->width * window_->height * 4);
    were_upload::uploader[0](window_->data, data,
                             surface_->stride(), surface_->width() * 4,
                             damage_.x1(), damage_.y1(), damage_.x2(), damage_.y2());

    were1_xcb_window_commit_with_damage(window_, damage_.x1(), damage_.y1(), damage_.x2(), damage_.y2());

    damage_.reset();
}
