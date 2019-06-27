#ifndef SPARKLE_ANDROID_SURFACE_H
#define SPARKLE_ANDROID_SURFACE_H

#include "sparkle.h"

class sparkle_android;
class sparkle_surface;
class sparkle_view;

class ANativeWindow;

class sparkle_android_surface : public were_object_2
{
public:
    ~sparkle_android_surface();
    sparkle_android_surface(were_object_pointer<sparkle_android> android, were_object_pointer<sparkle_surface> surface);

    were_object_pointer<sparkle_surface> surface() const
    {
        return surface_;
    }

private:
    void commit();

private:
    were_object_pointer<sparkle_surface> surface_;
    were_object_pointer<sparkle_view> view_;
    struct wl_resource *buffer_; // XXX
    ANativeWindow *window_;
};

#endif // SPARKLE_ANDROID_SURFACE_H
