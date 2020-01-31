#ifndef WERE_SURFACE_H
#define WERE_SURFACE_H

#include "were_object.h"

class were_surface : virtual public were_object
{
public:

    enum class buffer_format { ARGB8888, ABGR8888 };

    virtual void *data() = 0;
    virtual int width() = 0;
    virtual int height() = 0;
    virtual int stride() = 0;
    virtual were_surface::buffer_format format() = 0;

signals:
    were_signal<void (int x, int y, int width, int height)> damage;
    were_signal<void ()> commit;

    were_signal<void (int code)> key_down;
    were_signal<void (int code)> key_up;

    were_signal<void (int button)> pointer_button_down;
    were_signal<void (int button)> pointer_button_up;
    were_signal<void (int x, int y)> pointer_motion;
    were_signal<void ()> pointer_enter;
    were_signal<void ()> pointer_leave;

    were_signal<void (int id, int x, int y)> touch_down;
    were_signal<void (int id, int x, int y)> touch_up;
    were_signal<void (int id, int x, int y)> touch_motion;
};

#endif // WERE_SURFACE_H
