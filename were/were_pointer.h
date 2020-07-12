#ifndef WERE_POINTER_H
#define WERE_POINTER_H

#include "were_capability_rc.h"
#include "were_capability_integrator.h"
#include "were_capability_sentinel.h"
#include "were_exception.h"


template <typename Second, typename First>
inline Second safe_cast(First object__)
{
    return object__;
}

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

        if (!(capability<were_capability_rc>()->reference_count() > 0))
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

        if constexpr (std::is_same<Capability, were_capability_rc>::value)
        {
            return const_cast<Capability *>(static_cast<const Capability *>(object_));
        }
        else
        {
            return object_;
        }
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


template <typename T, typename... Args>
were_pointer<T> were_new(Args &&...args)
{
    T *object__ = new T(std::forward<Args>(args)...);
    safe_cast<were_capability_rc *>(object__)->reference();
    were_pointer<T> result(object__);
    safe_cast<were_capability_rc *>(object__)->unreference();

    if constexpr (std::is_base_of<were_capability_integrator, T>::value)
    {
        safe_cast<were_capability_integrator *>(object__)->integrate();
    }

    return result;
}

#endif // WERE_POINTER_H
