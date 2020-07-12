#ifndef WERE_CAPABILITY_COLLAPSE_H
#define WERE_CAPABILITY_COLLAPSE_H

#include "were_signal.h"

class were_capability_collapse
{
public:
    were_capability_collapse();

    void collapse();

signals:
    were_signal<void ()> destroyed;
};

#endif // WERE_CAPABILITY_COLLAPSE_H
