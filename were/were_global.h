#ifndef WERE_GLOBAL_H
#define WERE_GLOBAL_H

#include "were_slot.h"

template <typename T>
class were_pointer;

template <typename T>
were_pointer<T> &slot()
{
    return were_slot<were_pointer<T>>::get();
}

template <typename T>
void slot_clear()
{
    return were_slot<were_pointer<T>>::clear();
}

template <typename T>
void slot_set(const were_pointer<T> &v)
{
    were_slot<were_pointer<T>>::set(v);
}

template <typename T>
were_pointer<T> &t_l_slot()
{
    return were_t_l_slot<were_pointer<T>>::get();
}

template <typename T>
void t_l_slot_clear()
{
    return were_t_l_slot<were_pointer<T>>::clear();
}

template <typename T>
void t_l_slot_set(const were_pointer<T> &v)
{
    were_t_l_slot<were_pointer<T>>::set(v);
}


#endif // WERE_GLOBAL_H
