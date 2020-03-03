#ifndef WERE_GLOBAL_H
#define WERE_GLOBAL_H

#include "were_registry.h"
#include "were_connect.h"


template <typename T>
were_object_pointer<T> &global()
{
    return were_registry<were_object_pointer<T>>::get();
}

template <typename T>
void global_clear()
{
    return were_registry<were_object_pointer<T>>::clear();
}

template <typename T>
void global_set(const were_object_pointer<T> &v)
{
    were_registry<were_object_pointer<T>>::set(v);

    were::connect(v, &were_object::destroyed, v, []()
    {
        global_clear<T>();
    });
}

template <typename T>
were_object_pointer<T> &t_l_global()
{
    return were_t_l_registry<were_object_pointer<T>>::get();
}

template <typename T>
void t_l_global_clear()
{
    return were_t_l_registry<were_object_pointer<T>>::clear();
}

template <typename T>
void t_l_global_set(const were_object_pointer<T> &v)
{
    were_t_l_registry<were_object_pointer<T>>::set(v);

    were::connect(v, &were_object::destroyed, v, []()
    {
        t_l_global_clear<T>();
    });
}


#endif // WERE_GLOBAL_H
