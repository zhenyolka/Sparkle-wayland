#ifndef WERE_REGISTRY_H
#define WERE_REGISTRY_H

#include "were_exception.h"

template <typename T>
class were_registry
{
public:

    static void set(T *instance)
    {
        if (instance_)
            throw were_exception(WE_SIMPLE);

        instance_ = instance;
    }

    static T *get()
    {
        return instance_;
    }

    static void unset()
    {
        instance_ = nullptr;
    }

private:
    static T *instance_;
};

template <typename T>
T *were_registry<T>::instance_ = nullptr;

#endif // WERE_REGISTRY_H
