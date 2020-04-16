#ifndef WERE_CAPABILITY_COLLAPSE_H
#define WERE_CAPABILITY_COLLAPSE_H

#include "were_signal.h"

class were_capability_collapse
{
public:

    virtual bool collapsed() const = 0;
    virtual void collapse() = 0;

signals:
    were_signal<void ()> destroyed;
};

#endif // WERE_CAPABILITY_COLLAPSE_H
