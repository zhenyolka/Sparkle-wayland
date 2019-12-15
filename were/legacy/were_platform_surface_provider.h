#ifndef WERE_PLATFORM_SURFACE_PROVIDER_H
#define WERE_PLATFORM_SURFACE_PROVIDER_H

#include "were_object.h"

class were_platform_surface;

class were_platform_surface_provider : public were_object
{
public:
    ~were_platform_surface_provider();
    were_platform_surface_provider();

    virtual int display_width() const = 0;
    virtual int display_height() const = 0;
    virtual were_object_pointer<were_platform_surface> create_surface(int width, int height, int format) = 0;

    static were_object_pointer<were_platform_surface_provider> &default_provider() { return default_provider_; }
    static void set_default_provider(were_object_pointer<were_platform_surface_provider> provider) { default_provider_ = provider; }

private:
    static were_object_pointer<were_platform_surface_provider> default_provider_;
};

#endif // WERE_PLATFORM_SURFACE_PROVIDER_H
