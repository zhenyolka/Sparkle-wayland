#ifndef WERE_CAPABILITY_THREAD_H
#define WERE_CAPABILITY_THREAD_H

#include "were_pointer.h"

class were_thread;

class were_capability_thread
{
public:
    virtual were_pointer<were_thread> thread() const = 0;
};

#endif // WERE_CAPABILITY_THREAD_H
