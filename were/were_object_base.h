#ifndef WERE_OBJECT_BASE_H
#define WERE_OBJECT_BASE_H

#include <functional>

template <typename T>
class were_object_pointer;

class were_thread;

class were_object_base
{
public:
    virtual ~were_object_base();
    were_object_base();
    virtual bool collapsed() const = 0;
    virtual void collapse() = 0;
    virtual void access() const = 0;
    virtual void reference() = 0;
    virtual void unreference() = 0;
    virtual int reference_count() const = 0;
    virtual were_object_pointer<were_thread> thread() const = 0;
    virtual void post(const std::function<void ()> &call) = 0;
};

#endif // WERE_OBJECT_BASE_H
