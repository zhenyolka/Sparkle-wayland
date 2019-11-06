#ifndef WERE_SURFACE_H
#define WERE_SURFACE_H

#include "were_object.h"

#define WERE_SURFACE_FORMAT_ARGB8888    1
#define WERE_SURFACE_FORMAT_ABGR8888    2

class were_platform_surface;
class were_platform_surface_provider;

class were_surface : public were_object
{
public:
    ~were_surface();
    were_surface(int width, int height, int format);

    int width() const;
    int height() const;
    void set_size(int width, int height);
    bool lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride);
    bool unlock_and_post();

signals:

    were_signal<void ()> expose;

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

private:
    were_object_pointer<were_platform_surface> platform_surface_;
};

#endif // WERE_SURFACE_H
