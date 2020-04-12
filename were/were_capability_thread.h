#ifndef WERE_CAPABILITY_THREAD_H
#define WERE_CAPABILITY_THREAD_H

#include "were_object_pointer.h"

class were_thread;

namespace std
{
template <typename T>
class function;
}

class were_capability_thread
{
public:
    virtual were_object_pointer<were_thread> thread() const = 0;
    virtual void post(const std::function<void ()> &call) = 0;
};

#endif // WERE_CAPABILITY_THREAD_H
