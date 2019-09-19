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
    virtual were_object_pointer<were_platform_surface> create_surface() = 0;
};

#endif // WERE_PLATFORM_SURFACE_PROVIDER_H
