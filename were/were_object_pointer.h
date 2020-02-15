#ifndef WERE_OBJECT_POINTER_H
#define WERE_OBJECT_POINTER_H

#include "were_object_base.h"
#include "were_exception.h"
#include "were_registry.h"


template <typename T>
were_object_pointer<T> make_wop(T *object__)
{
    return were_object_pointer<T>(object__);
}

template <typename T>
were_object_pointer<T> &t_l_global()
{
    return were_t_l_registry<were_object_pointer<T>>::get();
}

template <typename T>
void t_l_global_set(const were_object_pointer<T> &v)
{
    return were_t_l_registry<were_object_pointer<T>>::set(v);
}

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
        pointer_ = object__;

        if (object_ == nullptr)
            throw were_exception(WE_SIMPLE);

        object_->reference();
    }

    were_object_pointer(const were_object_pointer &other)
    {
        object_ = other.object_;
        pointer_ = other.pointer_;

        object_->reference();
    }

    template <typename T2>
    were_object_pointer(const were_object_pointer<T2> &other)
    {
        object_ = other.object_;
        pointer_ = other.pointer_;

        object_->reference();
    }

    were_object_pointer &operator=(const were_object_pointer &other)
    {
        reset();

        object_ = other.object_;
        pointer_ = other.pointer_;

        object_->reference();

        return *this;
    }

    void reset()
    {
        object_->unreference();
    }

    void collapse()
    {
        object_->collapse();
    }

    T *access() const
    {
        object_->access();

        return pointer_;
    }

    T *access_UNSAFE() const
    {
        return pointer_;
    }

    T *operator->() const { return access(); }
    bool operator==(const were_object_pointer &other) const { return pointer_ == other.pointer_; }
    bool operator!=(const were_object_pointer &other) const { return pointer_ != other.pointer_; }
    void increment_reference_count() { object_->reference(); }
    void decrement_reference_count() { object_->unreference(); }
    //int reference_count() const { return object_->reference_count(); }
    //were_object_pointer<were_object> were() const;
    //operator were_object_pointer<were_object>();
    were_object_pointer<were_thread> thread() const { return object_->thread(); }
    void post(const std::function<void ()> &call) const { object_->post(call); }
    bool operator<(const were_object_pointer &other) const { return pointer_ < other.pointer_; }

private:
    were_object_base *object_;
    T *pointer_;
};

#endif // WERE_OBJECT_POINTER_H
