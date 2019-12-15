#ifndef WERE_SURFACE_H
#define WERE_SURFACE_H

#include "were_object.h"

class were_surface : virtual public were_object
{
public:

signals:
    were_signal<void (void *data, int width, int height)> data;
};

#endif // WERE_SURFACE_H
