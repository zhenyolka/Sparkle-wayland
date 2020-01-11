#ifndef WERE_REGISTRY_H
#define WERE_REGISTRY_H

#include "were_optional.h"

template <typename T>
class were_registry
{
public:

    static void set(const T &value)
    {
        value_.set(value);
    }

    static void clear()
    {
        value_.clear();
    }

    static T &get()
    {
        return value_.operator*();
    }

    static bool lock()
    {
        return value_.lock();
    }

    static void unlock()
    {
        return value_.unlock();
    }


private:
    static were_optional<T> value_;
};

template <typename T>
were_optional<T> were_registry<T>::value_;

#endif // WERE_REGISTRY_H
