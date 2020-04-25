#ifndef WERE_POINTER_H
#define WERE_POINTER_H

#include "were_capability_rc.h"
#include "were_capability_sentinel.h"
#include "were_exception.h"



template <typename T>
class were_pointer
{
    template <typename T2>
    friend class were_pointer;

public:

    ~were_pointer() { reset(); }

    explicit were_pointer(T *object__)
    {
        object_ = object__;

        if (object_ == nullptr)
            throw were_exception(WE_SIMPLE);

        capability<were_capability_rc>()->reference();
    }

    were_pointer(const were_pointer &other)
    {
        object_ = other.object_;

        capability<were_capability_rc>()->reference();
    }

    template <typename T2>
    were_pointer(const were_pointer<T2> &other)
    {
        object_ = other.object_;

        capability<were_capability_rc>()->reference();
    }

    were_pointer &operator=(const were_pointer &other)
    {
        reset();

        object_ = other.object_;

        capability<were_capability_rc>()->reference();

        return *this;
    }

    template <typename Capability>
    Capability *capability() const
    {
        // TODO: Sentinel

        return object_;
    }

    T *access() const
    {
        if constexpr (std::is_base_of<were_capability_sentinel, T>::value)
        {
            if (!capability<were_capability_sentinel>()->sentinel())
                throw were_exception(WE_SIMPLE);
        }

        return object_;
    }

    T *access_UNSAFE() const
    {
        return object_;
    }

    T *operator->() const { return access(); }
    T &operator*() const { return *access(); }
    bool operator==(const were_pointer &other) const { return object_ == other.object_; }
    bool operator!=(const were_pointer &other) const { return object_ != other.object_; }
    void increment_reference_count() { object_->reference(); }
    void decrement_reference_count() { object_->unreference(); }
    bool operator<(const were_pointer &other) const { return object_ < other.object_; }

private:

    void reset()
    {
        capability<were_capability_rc>()->unreference();
    }


private:
    T *object_;
};


#endif // WERE_POINTER_H
