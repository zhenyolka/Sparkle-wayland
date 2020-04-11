#ifndef WERE_OBJECT_POINTER_H
#define WERE_OBJECT_POINTER_H

#include "were_capability_rc.h"
#include "were_exception.h"
#include "were_registry.h"
#include <functional>




template <typename T>
class were_object_pointer
{
    template <typename T2>
    friend class were_object_pointer;

public:

    ~were_object_pointer() { reset(); }

    were_object_pointer(T *object__)
    {
        object_ = object__;

        if (object_ == nullptr)
            throw were_exception(WE_SIMPLE);

        capability<were_capability_rc>()->reference();
    }

    were_object_pointer(const were_object_pointer &other)
    {
        object_ = other.object_;

        capability<were_capability_rc>()->reference();
    }

    template <typename T2>
    were_object_pointer(const were_object_pointer<T2> &other)
    {
        object_ = other.object_;

        capability<were_capability_rc>()->reference();
    }

    were_object_pointer &operator=(const were_object_pointer &other)
    {
        reset();

        object_ = other.object_;

        capability<were_capability_rc>()->reference();

        return *this;
    }

    template <typename Capability>
    Capability *capability() const
    {
        return object_;
    }

    void reset()
    {
        capability<were_capability_rc>()->unreference();
    }

    void collapse()
    {
        object_->collapse();
    }

    T *access() const
    {
        return object_;
    }

    T *access_UNSAFE() const
    {
        return object_;
    }

    T *operator->() const { return access(); }
    bool operator==(const were_object_pointer &other) const { return object_ == other.object_; }
    bool operator!=(const were_object_pointer &other) const { return object_ != other.object_; }
    void increment_reference_count() { object_->reference(); }
    void decrement_reference_count() { object_->unreference(); }
    void post(const std::function<void ()> &call) const { object_->post(call); }
    bool operator<(const were_object_pointer &other) const { return object_ < other.object_; }

private:
    T *object_;
};


template <typename T>
were_object_pointer<T> make_wop(T *object__)
{
    return were_object_pointer<T>(object__);
}

#endif // WERE_OBJECT_POINTER_H
