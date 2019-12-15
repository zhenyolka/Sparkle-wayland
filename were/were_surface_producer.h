#ifndef WERE_SURFACE_PRODUCER_H
#define WERE_SURFACE_PRODUCER_H

#include "were_object.h"

class were_surface;

class were_surface_producer : virtual public were_object
{
public:
    ~were_surface_producer();
    were_surface_producer();

signals:
    were_signal<void (were_object_pointer<were_surface> surface)> surface_created;
};

#endif // WERE_SURFACE_PRODUCER_H
