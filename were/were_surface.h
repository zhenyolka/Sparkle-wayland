#ifndef WERE_SURFACE_H
#define WERE_SURFACE_H

#include "were_object.h"

class were_surface : virtual public were_object
{
public:
    enum buffer_format {format_ARGB8888, format_ABGR8888};
signals:
    were_signal<void (void *data, int width, int height, int stride, were_surface::buffer_format format)> attach;
    were_signal<void (int x, int y, int width, int height)> damage;
    were_signal<void ()> commit;
};

#endif // WERE_SURFACE_H
