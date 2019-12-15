#ifndef WERE_PLATFORM_SURFACE_H
#define WERE_PLATFORM_SURFACE_H

#include "were_object.h"

class were_surface;

class were_platform_surface : public were_object
{
public:
    ~were_platform_surface();
    were_platform_surface();

    void set_callbacks(were_object_pointer<were_surface> callbacks) { callbacks_ = callbacks; }
    were_object_pointer<were_surface> callbacks() const { return callbacks_; }

    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual void set_size(int width, int height) = 0;
    virtual bool lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride) = 0;
    virtual bool unlock_and_post() = 0;

private:
    were_object_pointer<were_surface> callbacks_;
};

#endif // WERE_PLATFORM_SURFACE_H
