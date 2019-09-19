#include "were_surface.h"
#include "were_platform_surface.h"
#include "were_platform_surface_provider.h"


were_surface::~were_surface()
{
    platform_surface_.collapse();
}

were_surface::were_surface(were_object_pointer<were_platform_surface_provider> platform_surface_provider, int width, int height, int format)
{
    MAKE_THIS_WOP

    platform_surface_ = platform_surface_provider->create_surface(width, height, format);
    platform_surface_->set_callbacks(this_wop);
    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->platform_surface_->set_callbacks(were_object_pointer<were_surface>());
    });
}

int were_surface::width() const
{
    return platform_surface_->width();
}

int were_surface::height() const
{
    return platform_surface_->height();
}

void were_surface::set_size(int width, int height)
{
    platform_surface_->set_size(width, height);
}

bool were_surface::lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride)
{
    return platform_surface_->lock(data, x1, y1, x2, y2, stride);
}

bool were_surface::unlock_and_post()
{
    return platform_surface_->unlock_and_post();
}
