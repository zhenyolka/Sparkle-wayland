#ifndef WERE_CAPABILITY_COLLAPSE_H
#define WERE_CAPABILITY_COLLAPSE_H

#include "were_signal.h"
#include "were_capability_rc.h"
#include "were_capability_integrator.h"

class were_capability_collapse : virtual public were_capability_rc,
                                 virtual public were_capability_integrator
{
public:
    were_capability_collapse();

signals:
    were_signal<void ()> destroyed;
};

#endif // WERE_CAPABILITY_COLLAPSE_H
