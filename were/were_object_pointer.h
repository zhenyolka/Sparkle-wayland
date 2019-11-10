#ifndef WERE_OBJECT_POINTER_H
#define WERE_OBJECT_POINTER_H

#include "were_object_base.h"
#include "were_exception.h"

#define MAKE_THIS_WOP \
were_object_pointer<std::remove_pointer<decltype(this)>::type> this_wop(this);

template <typename T>
class were_object_pointer
{
    template <typename T2>
    friend class were_object_pointer;

public:

    ~were_object_pointer() { reset(); }

    were_object_pointer()
    {
        object_ = nullptr;
        pointer_ = nullptr;
    }

    were_object_pointer(T *object__)
    {
        object_ = object__;
        pointer_ = object__;

        if (object_ != nullptr)
            object_->reference();
    }

    were_object_pointer(const were_object_pointer &other)
    {
        object_ = other.object_;
        pointer_ = other.pointer_;

        if (object_ != nullptr)
            object_->reference();
    }

    template <typename T2>
    were_object_pointer(const were_object_pointer<T2> &other)
    {
        object_ = other.object_;
        pointer_ = other.pointer_;

        if (object_ != nullptr)
            object_->reference();
    }

    were_object_pointer &operator=(const were_object_pointer &other)
    {
        reset();

        object_ = other.object_;
        pointer_ = other.pointer_;

        if (object_ != nullptr)
            object_->reference();

        return *this;
    }

    void reset()
    {
        if (object_ != nullptr)
            object_->unreference();

        object_ = nullptr;
        pointer_ = nullptr;
    }

    void collapse()
    {
        if (object_ != nullptr)
            object_->collapse();

        reset(); // XXX1
    }

    T *access() const
    {
        if (pointer_ == nullptr)
            throw were_exception(WE_SIMPLE);

        object_->access();

        return pointer_;
    }

    T *access_UNSAFE() const
    {
        if (pointer_ == nullptr)
            throw were_exception(WE_SIMPLE);

        return pointer_;
    }

    T *operator->() const { return access(); }
    operator bool() const { return pointer_ != nullptr; }
    bool operator==(const were_object_pointer &other) const { return pointer_ == other.pointer_; }
    bool operator!=(const were_object_pointer &other) const { return pointer_ != other.pointer_; }
    void increment_reference_count() { object_->reference(); }
    void decrement_reference_count() { object_->unreference(); }
    //int reference_count() const { return object_->reference_count(); }
    //were_object_pointer<were_object> were() const;
    //operator were_object_pointer<were_object>();
    were_object_pointer<were_thread> thread() const { return object_->thread(); }
    void post(const std::function<void ()> &call) const { object_->post(call); }

private:
    were_object_base *object_;
    T *pointer_;
};

#endif // WERE_OBJECT_POINTER_H
