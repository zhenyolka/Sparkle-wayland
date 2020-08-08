#ifndef WERE_CLASS_WRAPPER_H
#define WERE_CLASS_WRAPPER_H

#include "were_capability_rc_simple.h"
#include <utility>

template <typename T>
class were_class_wrapper : public were_capability_rc_simple, public T
{
public:

    template <typename ...Args>
    explicit were_class_wrapper(Args &&...args) : T(std::forward<Args>(args)...)
    {
    }

    were_class_wrapper(const were_class_wrapper &other) : T(other)
    {
    }
};

#endif // WERE_CLASS_WRAPPER_H
